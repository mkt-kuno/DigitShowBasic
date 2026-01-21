# Timer callbacks (TIMERPROC) vs WM_TIMER in DigitShowDST

このメモは、`DigitShowDST` リポジトリでのタイマー実装（WM_TIMER と SetTimer の TIMERPROC）に関する議論、考慮点、実装上の勧告をまとめたものです。

## 概要
- Windows の `SetTimer` は大きく分けて 2 種類の使い方がある：
  - `SetTimer(hWnd, id, elapse, NULL)` → WM_TIMER メッセージがウィンドウキューにポストされ、UI スレッドのメッセージループで処理される。
  - `SetTimer(hWnd, id, elapse, TimerProc)` → システムが指定の `TIMERPROC` 関数を呼び出す。呼び出し経路は WM_TIMER と異なり、再入や割り込みが発生しやすい。

## WM_TIMER の特徴
- メッセージループによる協調的な実行で、メイン（UI）スレッド上で処理が行われるため UI 操作が安全。
- 他のメッセージにより遅延する可能性があり、精度はメッセージ処理に依存する。
- ウィンドウ破棄後の `WM_TIMER` ポスト済みメッセージは残る可能性がある（処理タイミング次第でウィンドウ破棄後にハンドラが走ることがある）。

## TIMERPROC の特徴
- コールバック型であるため、WM_TIMER より呼び出しのタイミングが異なる。
- 再入、競合、API のスレッド安全性（= スレッドセーフでない API では危険）に注意が必要。
- `TIMERPROC` は `PostMessage` を使って UI スレッドへ処理を返送する（`PostMessage(hWnd, WM_APP, ...)`）ことで UI へのアクセスを安全化できる。
- `TIMERPROC` 自体は速く呼ばれることがあるが、やはり UI 操作はメインスレッドに留めるのが無難。

## KillTimer の挙動（重要）
- `KillTimer` は以降のタイマー通知を止めるが、すでに発生した TimerProc の呼び出しや、既にキューにポストされた `WM_TIMER` メッセージを強制的に抹消するわけではない。
- したがって、`KillTimer` 後でも既にコールバック中／ポスト済みメッセージが実行され、`this` ポインタ等を参照するとダングリング参照でクラッシュする可能性がある。

## Atomic での保護は万能ではない
- `std::atomic` は個々の変数の読み書きでデータ競合を防ぐが、複数変数にまたがる整合性、ライフタイム問題、非再入 API (pDoc や board_control 呼び出し) を守るものではない。
- つまり atomic を使ってるだけでは、ウィンドウ破棄タイミングでのコールバック実行や複合処理の安全性は担保できない。

## 推奨アプローチ（このリポジトリ向け）
1. UI が絡む処理（`Control_DA`, `SaveToFile`, `AD_INPUT` など）は必ずメイン（UI）スレッドで呼び出す。
   - 方法：`TimerProc` では `PostMessage(hWnd, WM_APP + N, ...)` を行い、`ON_MESSAGE` ハンドラで実行する。
2. メインスレッドで動かすことで、既存の `GetDocument()` / UI 操作を壊さずに済む。
3. `OnDestroy()` / デストラクタで `KillTimer` するだけでなく、`m_shuttingDown` のような `std::atomic<bool>` フラグをセットして、コールバックやハンドラが起動直後に早期 `return` できるようにする。
   - `OnDestroy()` 例:
     ```cpp
     void CDigitShowDSTView::OnDestroy() {
         m_shuttingDown.store(true, std::memory_order_release);
         KillTimer(timer::kTimerId_Control);
         KillTimer(timer::kTimerId_Log);
         KillTimer(timer::kTimerId_UI);
         CFormView::OnDestroy();
     }
     ```
   - ハンドラ側での guard:
     ```cpp
     LRESULT CDigitShowDSTView::OnLogTick(WPARAM w, LPARAM l) {
         if (m_shuttingDown.load(std::memory_order_acquire))
             return 0;
         // ... 処理
     }
     ```
4. もし TimerProc の中で全てを完結したい（UI を触らない、よりリアルタイム性を求める）場合は、まず `board_control` や Document の API が **スレッドセーフであること** を確認／改修する。
   - 事前に mutex/lock を導入するか、専用のワーカースレッドに処理を委譲してシリアライズするのが安全。

## 実装チェックリスト（移行／リファクタ化時）
- [ ] 影響範囲 (Control/Log/AD I/O の関数) を洗い出して thread-safe か確認
- [ ] `m_shuttingDown`（atomic）を導入して、ウィンドウ破棄時にコールバックの早期 return を保証
- [ ] `OnDestroy()` 内で KillTimer を呼ぶ
- [ ] TimerProc には最小限の処理だけをおき、UI 処理は `PostMessage` で main thread に回す
- [ ] `OnDestroy()` / destructor の中で `PostMessage` の race を避けるための guard（atomic）を設置
- [ ] 可能なら `CreateTimerQueueTimer` / `DeleteTimerQueueEx` などの API を検討し、コールバック終了を待てるようにする

## 参考: DigitShowDST の変更案
- `DigitShowDSTView.cpp` で、`timer::kTimerId_Control` と `timer::kTimerId_Log` を `SetTimer(..., StaticControlTimerProc / StaticLogTimerProc)` へ差し替え。
- これらの静的 TimerProc は `PostMessage(hwnd, WM_CONTROL_TICK / WM_LOG_TICK, 0, 0)` を行い、メインスレッドで `OnControlTick` / `OnLogTick` が実行される。
- `OnTimer` から Control および Log のケースは削除され、UI 更新 (UI タイマ) のみ `OnTimer` が担当する。

## 最後に（結論）
- 実行コンテキスト（スレッド）を明確にすることが最重要。UI を触る処理は `PostMessage`→メインスレッドで実行し、独立処理は TimerProc 内で行ってもよいが、スレッドセーフネスを事前に保証すること。
- KillTimer 単体では race を防げないため、必ずシャットダウンフラグや同期手段を組み合わせてください。

---

（必要なら、このファイルを `knowledge/` の他の文書に要約として追加して PR 提案できます。どのファイルに統合するか良ければ指示ください。）

# Monotonic Clock / std::chrono リファクタリング (現行ブランチ版)

このドキュメントは `copilot/replace-time-calculation` ブランチで実装された時間管理とロギング改善について、最新コードに合わせて再整理したものです。旧版記述との差異（未実装要素を含む）を明示します。

## 概要
従来の `_ftime64_s` / `timeb` による秒差分計算を廃止し、`std::chrono` の型安全な duration / time_point に全面移行する第一段階が完了しました。Timer2 (制御) のデルタ計測は単調増加な `steady_clock` ベースになり、防御的な過大デルタのクリッピングと `spdlog` による詳細ログを導入しています。その他のタイマー (UI, ログ) は段階的移行計画中で、現時点では一部 `system_clock` を使用しています。

## 追加・変更点一覧

### 1. 依存追加: spdlog
`vcpkg.json` に `"spdlog"` を追加。ビルド時に vcpkg が自動取得します。

### 2. ロギング基盤: `Logging.hpp`
回転ファイルロガー (`logs/digitshowdst.log`, 5 MiB ×3) を提供する `DSBLogger()` と利便マクロ (`DSB_LOG_DEBUG/INFO/WARN/ERROR`) を追加。失敗時はデフォルトロガーへフォールバック。

### 3. 時間型リファクタリング: `Control.hpp` / `chrono_alias.hpp`
`chrono_alias.hpp` で `seconds_d` / `minutes_d` などリテラルに基づくエイリアスを定義し、`Control.hpp` では以下を採用:
```cpp
inline std::chrono::seconds_d CtrlStepTime{0.0};      // 1制御ティックの経過秒
inline std::chrono::minutes_d TotalStepTime{0.0};     // 現行ステップ累積時間 (分)
```
Timer間隔は `Board.hpp` で `std::chrono::milliseconds` 型 (`TimeInterval_1/2/3`) に変更済み。

### 4. 制御タイマーのデルタ計測: `DigitShowDSTView.cpp` (Timer2 case)
`steady_clock` を使用し、ティック毎の `delta` を計算。過大 (>5s) の場合はクリップして WARN ログを出力。
```cpp
static steady_clock::time_point last_tick = steady_clock::now();
auto now = steady_clock::now();
auto delta = now - last_tick; last_tick = now;
if (delta > 5s) { DSB_LOG_WARN("Large timer delta observed: {}s — clamped to 5.0", duration<double>(delta).count()); delta = 5s; }
control::CtrlStepTime = duration_cast<seconds_d>(delta);
DSB_LOG_DEBUG("Control tick: delta={:.6f}s, CtrlStepTime={:.6f}s", duration<double>(delta).count(), control::CtrlStepTime.count());
```

### 5. ステップ累積時間の扱い
現行コードでは (旧ドキュメントが記述していた) `Control_DA()` 中での `TotalStepTime += CtrlStepTime` 直接加算処理はまだ存在しません。`TotalStepTime` の増分ロジックは今後 creep / relaxation 等のモード内へ統合予定で、現状は比較・リセットのみ行われる実装となっています。従って旧版に記載されていた「Accumulation の毎 tick ログ出力」は未実装です。

### 6. 時間比較の型安全化
各制御関数 (例: `Relaxation_Constant_Pressure`, `Creep_Constant_Pressure` など) で:
```cpp
auto target_time = std::chrono::minutes_d{CFPARA[CURNUM][X]};
if (TotalStepTime >= target_time) { /* 次ステップへ */ TotalStepTime = {}; }
```
のように CFPARA の「分」指定値を `minutes_d` へ明示変換し、duration 同士の比較を実現。
**Note: The Wait control pattern has been removed from the codebase (as of PR #XXX).**

### 7. API 境界でのみ `.count()` 利用
`SetTimer()` 呼び出しや MFC ダイアログ表示など、レガシー API が整数ミリ秒を要求する場所でのみ `TimeInterval_X.count()` を使用。内部計算は duration 型のまま保持し単位誤用を防止。

### 8. FIFO / サンプリング関連: `SamplingSettings.cpp` / `DigitShowDSTView.cpp`
サンプリングクロック (µs) と保存時間 (秒) を `microseconds_d` / `seconds` 等へマッピング。メモリ計算は `CalculateSamplingTimes()` のヘルパで行い、明示的に変換を集中。

### 9. ログ例 (現行)
`Control tick` のみが実装済み。旧ドキュメントにあった `Control_DA tick: ... TotalStepTime=...` 形式は未実装。
```
[2025-11-10 12:34:56.789] [digitshowdst] [debug] Control tick: delta=0.500123s, CtrlStepTime=0.500123s
[2025-11-10 12:34:57.289] [digitshowdst] [warn ] Large timer delta observed: 6.203414s — clamped to 5.0
```

### 10. 防御的クリッピング理由
期待インターバル ≈500ms に対して 5s 超はサスペンド / デバッグ停止 / 重大負荷を示唆。機械制御への過大時間差伝播を回避する目的でクリップと警告ログを出力。必要なら定数を `DigitShowDSTView.cpp` 内で変更して調整可。

## 型安全化の利点
1. 異なる単位 (秒 vs 分) の加算はコンパイル段階で検出。
2. コード中の単位が型として自己記述的 (`seconds_d`, `minutes_d`)。
3. 変換コストはテンプレートによる最適化でランタイムオーバーヘッドなし。

## 未実装 / 差異ポイント
| 項目 | 旧記述 | 現行状態 | 今後の方針 |
|------|--------|----------|------------|
| TotalStepTime 累積 | `CtrlStepTime` を毎 tick 加算 | 比較とリセットのみ。加算ロジック不在 | creep/relax 等での正確な時間集計を Control dispatcher 再設計時に導入予定 |
| `Control_DA tick` ログ | あり | なし | dispatcher 内部へ統合後に再追加 |
| 他タイマーの `steady_clock` 化 | 計画 | UI: `system_clock` 表示用途, ログ: `system_clock` | Timer1/Timer3 も steady_clock へ移行 (計測 vs 表示で clock 分離) |
| timeb 構造体削除 | 予定 | 未削除 (安全な最小変更) | クリーニングフェーズで除去 |

## テスト指針 (現行コード準拠)
1. Wait パターン (CFNUM 対応): `CFPARA[0]` を 0.2 分に設定し、`Wait_Control()` のステップ遷移ログが約 12 秒後に出ることを確認。
2. サスペンド再開: アプリ実行中に一時休止→再開し 5s 超デルタが WARN ログに出るか確認。
3. 異常大量 CPU 負荷下で delta 分布をログ解析し、500ms ± 数ms の安定性を確認。

## ビルド手順 (再掲)
```powershell
msbuild .\DigitShowDST.vcxproj /t:Build /p:Configuration=Release
```
初回または依存更新時は:
```powershell
.\vcpkg\bootstrap-vcpkg.bat
vcpkg integrate install
```

## トラブルシュート (現行差分適用)
| 症状 | 原因候補 | 対処 |
|------|----------|------|
| ログが生成されない | 権限/ディレクトリ/例外フォールバック | `logs/` 作成権限確認・stderr へ WARN 出力確認 |
| デルタが異常に不安定 | システム過負荷 / Timer 精度限界 | ログで分布収集し閾値再設定 |
| spdlog 未取得 | vcpkg 統合不足 | `vcpkg integrate install` 実行 |

## 今後の改善候補
1. `TotalStepTime` の正規累積とステップ状態遷移を dispatcher に一本化
2. Timer1/Timer3 の `steady_clock` 移行 + 表示用フォーマット分離
3. ログレベル動的切替 (環境変数 / 設定ダイアログ)
4. 時間統計 (平均デルタ・最大/最小・クリップ件数) の定期集計
5. 未使用 old timeb 構造体の削除とヘッダクリーンアップ
6. doctest による duration ユニットテスト導入 (例: creep 時間計算)

## 付録: 使用中 chrono エイリアス
`chrono_alias.hpp`:
```cpp
using seconds_d  = decltype(1.0s);      // duration<double>
using minutes_d  = decltype(1.0min);    // duration<double,std::ratio<60>>
using microseconds_d = decltype(1.0us); // duration<double>
```

## まとめ
本ブランチでは制御タイマーの単調クロック化と主要時間変数の型安全化を達成しました。旧ドキュメント内の一部「累積/ログ」記述は未実装のため差分として整理済みです。次段階では制御 dispatcher 再編と他タイマーの統一的 chrono 化、そして時間関連のテスト・統計機能追加を予定しています。

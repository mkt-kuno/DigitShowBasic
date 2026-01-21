# データファイル形式（TSV / SQLite）

**Status:** Active — reflects TSV and SQLite dual logging (2025-11-02)

DigitShowDST は「Start Save → Stop Save」（通常保存）や FIFO モードの「Write Data」で TSV を生成しつつ、PR #18 以降は同じペイロードを SQLite にも書き出せるようになりました。本書は実装（`DigitShowDSTView.cpp` / `DigitShowDSTDoc.cpp`）を基に、それぞれのフォーマットと出力経路を整理したものです。

## 形式の変遷

### 新形式（2025-10-28以降）
- **拡張子**: `.tsv` (物理量), `_vlt.tsv` (電圧), `_out.tsv` (パラメータ)
- **時刻記録**: Unix time（ミリ秒単位、1970-01-01 00:00:00 UTCからの経過時間）
- **ヘッダ**: `UnixTime(ms)` 列
- **メリット**:
  - データの絶対時刻が記録され、トレーサビリティ向上
  - 複数ファイルの時系列比較・統合が容易
  - TSV形式で外部ツール連携が改善
- **SQLiteとの関係**: TSV は互換性維持のため常に生成。SQLite ログを有効化しても削除されない。

### 旧形式（2025-10-27以前）
- **拡張子**: `.dat` (物理量), `.vlt` (電圧), `.out` (パラメータ)
- **時刻記録**: 経過時間（秒単位、保存開始からの相対時間）
- **ヘッダ**: `Time(s)` 列
- **互換性**: Python解析ツール（`scripts/load_data.py`）は新旧両形式に対応

### SQLite ロギング（試験運用）
- **導入**: PR #18。GUI の「Start Saving」で `{logs|run_logs}/<basename>.sqlite` を生成（設定により出力先が変わる計画）。
- **テーブル**: 現状は `samples`（物理量）と `voltages`（電圧）を実装。`derived`（計算パラメータ）はロードマップに掲載済み。
- **列の基本**: `UnixTimeMs`, `StepIndex`, `Channel00`... と TSV 相当値を格納。型は `INTEGER`（時刻）、`REAL`（値）。
- **ステータス**: Timer 3 のストリームに限定。FIFO 書き出しは未対応。スキーマは変動の可能性あり。
- **ツール連携**: Python 側は TSV を第一候補とし、SQLite 取り込みは別タスクで実装予定。

### ヘッダ更新（2025-12-30）
- **目的**: TSVファイルのヘッダ名を UI 表示ラベルと実際のデータ内容に一致させる
- **変更内容**:
  - 物理量ファイル (`.tsv` / `.dat`): 最初の8列のヘッダを UI ラベルに合わせて変更
  - パラメータファイル (`_out.tsv` / `.out`): 全ての列名を実際のデータ内容を反映する名前に変更
- **後方互換性**:
  - **既存のTSVファイル**: 旧ヘッダのファイルも引き続き有効
  - **Python解析スクリプト**: `scripts/` 内のスクリプトは**位置インデックス**を使用しているため、コード変更不要
  - **列名に依存するコード**: 旧ヘッダ名を使用している場合は新ヘッダ名に更新が必要
- **移行パス**: 新規に作成されるファイルは新ヘッダを使用。既存ファイルは変更不要

---

## 保存処理の入口

- **通常保存**: `CDigitShowDSTView::OnBUTTONStartSave()` でファイル作成・ヘッダ出力 → タイマー3で `CDigitShowDSTDoc::SaveToFile()` が周回書き込み → `OnBUTTONStopSave()` で最終1行書き込みしてクローズ
- **FIFO書き出し**: `OnBUTTONWriteData()` でヘッダ出力後に `CDigitShowDSTDoc::SaveToFile2()` が一括書き出し（メインファイルと電圧ファイルのみ）
- **SQLite**: `SaveToFile()` 内で TSV と同タイミングで行挿入。`OnBUTTONStopSave()` で保留トランザクションをクローズする暫定設計。

## 共通仕様

- **区切り**: タブ区切り（"\t"）
- **改行**: Windows CRLF（実装は "\n" 出力だが、テキストモードにより CRLF で保存）
- **文字コード**: UTF-8 with BOM（プレーンテキスト、数値と英字のみで互換性は高い）
- **時刻列**: 先頭列
  - **新形式**: `UnixTime(ms)` - Unix time（ミリ秒）を `%lld` で出力
    - 実装: `GetUnixTimeMs()` 関数（`std::chrono` 使用）
    - 通常保存: 各行で現在時刻を取得
    - FIFO書き出し: `FifoStartTimeMs + (SavingClock / 1000.0 * i)` で計算
  - **旧形式**: `Time(s)` - 経過時間（秒）を `%.3lf` で出力（小数第3位まで）
    - 通常保存: `SequentTime2`（保存開始からの経過秒）
    - FIFO書き出し: `SavingClock/1e6 * i`（サンプルインデックスから計算）
  - **SQLite**: `UnixTimeMs` を `INTEGER`（64bit）で保存。GUI 時刻取得と同じ `GetUnixTimeMs()` を使用。

---

## .tsv / .dat（物理量）

- **目的**: A/D 生電圧を校正係数（`Cal_a/b/c`）で物理量に換算した値を記録
- **拡張子**:
  - 新形式: `.tsv`
  - 旧形式: `.dat`
- **生成タイミング**:
  - 通常保存: 常時（Start/Stop 保存時）
  - FIFO書き出し: あり（`SaveToFile2()`）
- **ファイル名**: ユーザー選択のベース名 + 拡張子
- **列構成**（ヘッダは固定で16チャンネル想定）
  1. **UnixTime(ms) / Time(s)**（新形式/旧形式）
  2. Shear_load_(N) - せん断荷重（旧名: Load_(N)）
  3. Vertical_load_(N) - 鉛直荷重（旧名: Cell_P.(kPa)）
  4. Shear_disp._(mm) - せん断変位（旧名: Disp.(mm)）
  5. V-front-disp._(mm) - 前面鉛直変位（旧名: P.W.P(kPa)）
  6. V-rear-disp._(mm) - 背面鉛直変位（旧名: SP.Vol.(mm3)）
  7. Front_friction_(N) - 前面摩擦力（旧名: CH05_(V)）
  8. Rear_friction_(N) - 背面摩擦力（旧名: V-LDT1_(mm)）
  9. CH08 - チャンネル8（旧名: CH07_(V)）
  10. V-LDT2_(mm)
  11. CH09_(V)
  12. CG1_(mm)
  13. CH11_(V)
  14. CG2_(mm)
  15. CH13_(V)
  16. CG3_(mm)
  17. CH15_(V)
- **データ行**: 時刻列 + 各列は `double` を `%lf` で出力（小数点は "."）。`Phyout[k]` の順序で A/D の「1チャネルおき」配線（偶数インデックスのみ）に対応
- **注意**:
  - 実データのチャンネル数は A/D 構成（`AdChannels[i]/2` の合計）に依存。実機は16ch相当（CH00～CH15）を想定
  - ヘッダの物理名は歴史的に混在（例: 一部は "CHxx_(V)" のまま電圧、他は mm・mm3 等の物理量）
  - FIFO書き出し（`OnBUTTONWriteData`）ではヘッダ名が一部異なる（旧表記）。例:
    - `E_Cell_P.(kPa)`（通常保存では `P.W.P(kPa)`）
    - `SP.Vol.(cm3)`（通常保存では `SP.Vol.(mm3)`）
    - `LDT-V1(mm)` / `LDT-V2(mm)`（通常保存では `V-LDT1_(mm)` / `V-LDT2_(mm)`）

**実装参照**:
- ヘッダ出力: `DigitShowDSTView.cpp` → `OnBUTTONStartSave()` / `OnBUTTONWriteData()`
- データ出力: `DigitShowDSTDoc.cpp` → `SaveToFile()`（通常）/ `SaveToFile2()`（FIFO）

---

## _vlt.tsv / .vlt（生電圧）

- **目的**: A/D 各チャンネルの生電圧（V）をそのまま記録
- **拡張子**:
  - 新形式: `_vlt.tsv`
  - 旧形式: `.vlt`
- **生成タイミング**:
  - 通常保存: 常時
  - FIFO書き出し: あり
- **ファイル名**: メインファイルと同名で拡張子違い
  - 新形式: `{basename}_vlt.tsv`
  - 旧形式: `{basename}.vlt`
- **列構成**（16ch固定ヘッダ）
  1. **UnixTime(ms) / Time(s)**（新形式/旧形式）
  2. CH00_(V)
  3. CH01_(V)
  4. CH02_(V)
  5. CH03_(V)
  6. CH04_(V)
  7. CH05_(V)
  8. CH06_(V)
  9. CH07_(V)
  10. CH08_(V)
  11. CH09_(V)
  12. CH10_(V)
  13. CH11_(V)
  14. CH12_(V)
  15. CH13_(V)
  16. CH14_(V)
  17. CH15_(V)
- **データ行**: 時刻列 + `Vout[k]`（通常）または FIFO バッファから `BinaryToVolt(...)` で変換した `Vtmp`

**実装参照**:
- ヘッダ出力: `DigitShowDSTView.cpp` → `OnBUTTONStartSave()` / `OnBUTTONWriteData()`
- データ出力: `DigitShowDSTDoc.cpp` → `SaveToFile()` / `SaveToFile2()`

---

## _out.tsv / .out（計算パラメータ）

- **目的**: 表示用に計算された派生パラメータ（`param[0..15]`）を記録
  - これらのパラメータは `variables::physical::latest_physical_input`、`variables::physical::latest_physical_output` のスナップショット
    および生D/A電圧（`DAVout[]`）から生成され、従来の `CalParam[]` 配列には依存しません。
- **拡張子**:
  - 新形式: `_out.tsv`
  - 旧形式: `.out`
- **生成タイミング**: 通常保存のみ（Start/Stop 保存）。FIFO の `Write Data` では出力されない
- **ファイル名**: メインファイルと同名で拡張子違い
  - 新形式: `{basename}_out.tsv`
  - 旧形式: `{basename}.out`
- **列構成**（先頭は時刻 + 16列）
  1. **UnixTime(ms) / Time(s)**（新形式/旧形式）
  2. Tau_(kPa) - せん断応力 τ（旧名: s(a)_(kPa)）
  3. Shear_disp._(mm) - せん断変位（旧名: s(r)_(kPa)）
  4. Sigma_(kPa) - 鉛直応力 σ（旧名: s'(a)(kPa)）
  5. V-ave-disp._(mm) - 平均鉛直変位（旧名: s'(r)(kPa)）
  6. ev_diff/2_(mm) - 鉛直変位差の半分（傾き）（旧名: Pore_(kPa)）
  7. Front_friction_(N) - 前面摩擦力（旧名: p____(kPa)）
  8. Rear_friction_(N) - 背面摩擦力（旧名: q____(kPa)）
  9. RPM - モーター回転数（旧名: p'___(kPa)）
  10. Front_EP_(kPa) - 前面EP（旧名: e(a)_(%)_）
  11. Rear_EP_(kPa) - 背面EP（旧名: e(r)_(%)_）
  12. RPM_(V) - モーター回転数D/A生値（旧名: e(vol)_(%)_）
  13. Front_EP_(V) - 前面EPのD/A生値（旧名: AvLDT(%)_）
  14. Rear_EP_(V) - 背面EPのD/A生値（旧名: DA(%)inCOMP）
  15. Loop_count - ループ回数（旧名: DA(%)inEXT）
  16. Control_No - 制御ステップ番号（旧名: Cont_No__）
  17. Step_time_(s) - ステップ経過時間（秒単位）（旧名: Step_time(s)）
- **実データの中身**（コード上の割り当て）
  - このファイルの列はコード中の `SaveToFile()` 実装にしたがって記録されます。以前のヘッダ名とは異なり、現在のヘッダは実際のデータ内容を正確に反映しています。
  - 実際の並び（2列目=Index0）と意味:
    - 2: `param[0]` = τ（せん断応力）※ `latest_physical_input.shear_stress_kpa()`
    - 3: `param[1]` = せん断変位（mm）※ `latest_physical_input.shear_displacement_mm`
    - 4: `param[2]` = σ（鉛直応力, kPa）※ `latest_physical_input.vertical_stress_kpa()`
    - 5: `param[3]` = 平均鉛直変位（mm）※ `latest_physical_input.normal_displacement_mm()`
    - 6: `param[4]` = ev_diff/2（mm）※ `latest_physical_input.tilt_mm()`
    - 7: `param[5]` = 前面摩擦（N）※ `latest_physical_input.front_friction_force_N`
    - 8: `param[6]` = 背面摩擦（N）※ `latest_physical_input.rear_friction_force_N`
    - 9: `param[7]` = Motor RPM（回転数）※ `latest_physical_output.motor_rpm`
       - 注意: `ev_f` または `ev_r` が有限でない場合は 0.0 を設定
    - 10: `param[8]` = 前面 EP (kPa) ※ `latest_physical_output.front_ep_kpa`
    - 11: `param[9]` = 背面 EP (kPa) ※ `latest_physical_output.rear_ep_kpa`
    - 12: `param[10]` = RPM(V)（D/A 生値）※ `DAVout[CH_MotorSpeed]`
    - 13: `param[11]` = 前面 EP(V)（D/A 生値）※ `DAVout[CH_EP_Cell_f]`
    - 14: `param[12]` = 背面 EP(V)（D/A 生値）※ `DAVout[CH_EP_Cell_r]`
    - 15: `param[13]` = ループ回数（サイクリック載荷のカウンタ）
    - 16: `param[14]` = 制御ステップ番号（`control::current_step_index`）
    - 17: `param[15]` = クリープ/緩和の経過時間（`control::step_elapsed` 由来）
       - 注意: `std::chrono::seconds_d{control::step_elapsed}.count()` で秒単位で出力される
- **データ行**: 時刻列 + `param[0..15]` を `%lf` でタブ区切り出力 (`param[*]` は latest_physical_* と DAVout に対応)

**実装参照**:
- ヘッダ出力: `DigitShowDSTView.cpp` → `OnBUTTONStartSave()`
- データ出力: `DigitShowDSTDoc.cpp` → `SaveToFile()`

---

## Python解析ツールでの読み込み

`scripts/load_data.py` は新旧両形式に自動対応します：

```python
from scripts.load_data import read_all_files

# ファイル読み込み（拡張子なしのベース名を指定）
dat, vlt, out = read_all_files("2025-10-28_test")

# 新形式の場合: インデックスは pd.DatetimeIndex (Timestamp)
# 旧形式の場合: インデックスは Time(s) のまま
print(dat.index)  # 新形式: DatetimeIndex、旧形式: Index
```

**自動判別ロジック**:
- ヘッダに `UnixTime(ms)` があれば新形式 → `pd.to_datetime(df['UnixTime(ms)'], unit='ms')` で datetime インデックスに変換
- ヘッダに `Time(s)` があれば旧形式 → そのままインデックスに設定
- SQLite 版の読み込みは未実装。テーブル仕様が固まった段階で `scripts/load_data.py` に拡張予定。

---

## チャネル対応とキャリブレーションの要点

- A/D 読み取りは「1チャネルおき」（偶数インデックス）を使用
  - 例: `AdData0[AdChannels[0] * j + 2 * i]`
  - `Vout[k]` は `BinaryToVolt(...)` で電圧へ変換後、`Phyout[k]` に対して `Cal_a[k] * V^2 + Cal_b[k] * V + Cal_c[k]` で物理量へ
- 出力列は `NUMAD` 個のボード分を逐次結合（ボード0→ボード1 の順）
- 実運用は 16ch 相当（CH00～CH15）を前提にヘッダ固定。構成変更時はヘッダ不一致が起き得るため注意

---

## 典型的な利用シナリオ

### 通常保存（Start/Stop）
- **新形式**: `.tsv`, `_vlt.tsv`, `_out.tsv` を同名で併産
- **旧形式**: `.dat`, `.vlt`, `.out` を同名で併産
- `Intercept Save` は停止せずに現時点の1行を追加
- SQLite: オプション有効時は同時に追記（別スレッドでのフラッシュは今後の改善点）。

### FIFO 書き出し（Write Data）
- 高速連続取得を一時バッファに溜めてから電圧ファイルとメインファイルのみ一括書き出し
- パラメータファイル（`_out.tsv` / `.out`）は作成されない
- SQLite: 現状未対応。バッファ挿入を追加する際は一括トランザクションで投入する計画。

---

## トラブルシュート・既知の注意点

- `.out` / `_out.tsv` のヘッダ名は歴史的理由で実値と一致していない箇所がある（上記「実データの中身」を参照）
- `Step_time(s)` は一部制御で分単位で積算される可能性がある
- ヘッダは16ch固定だが、実チャンネル数を変更すると列数不一致になる可能性がある
- 数値フォーマットはロケール非依存の `printf` 系（小数点は「.」）。CSVとして扱う場合は区切りがタブである点に注意
- **新形式から旧形式への変換**: Unix timeからの経過時間計算が必要な場合は、Python上で最初の行のタイムスタンプを基準に差分を計算
- SQLite はスキーマが固まりきっていないため、早期採用する場合はバージョンカラムの確認とマイグレーション手順を別途用意すること。

---

## 参考情報

- **Unix time変換**: `pd.to_datetime(unix_ms, unit='ms')` でミリ秒単位のUnix timeをdatetimeに変換
- **タイムゾーン**: Unix timeはUTC基準。日本時間（JST）への変換は `df.index.tz_localize('UTC').tz_convert('Asia/Tokyo')`
- **後方互換性**: Python解析ツールは自動判別するため、新旧混在環境でも問題なく使用可能。SQLite 版は TSV を補完する位置付けであり、解析用 API が整うまで一次ソースとして TSV を参照する。

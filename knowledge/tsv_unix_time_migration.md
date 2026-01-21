# TSV形式・Unix Time記録への移行設計

**作成日**: 2025-10-28
**目的**: ログファイルの拡張子統一（TSV化）とUnix time（ms）記録への移行

---

## 1. 現状の課題

### 1.1 時刻記録の問題
- **現状**: 経過時間（ms）のみ記録（start savingからの相対時間）
- **課題**:
  - データ取得の絶対時刻が不明
  - 複数ファイルの時系列比較が困難
  - トレーサビリティ・再現性に欠ける
  - start saving時刻を別途記録しないとデータの日時が不明

### 1.2 ファイル形式の問題
- **現状**: `.dat`, `.vlt`, `.out` など拡張子が混在
- **課題**:
  - ファイル管理が煩雑
  - 外部ツールでの解析時に拡張子判別が必要
  - TSV（タブ区切り）なのに `.dat` 拡張子で分かりにくい

---

## 2. 改訂仕様

### 2.1 時刻記録方式
- **変更内容**: 各サンプリング行に「Unix time（1970-01-01 00:00:00 UTCからのミリ秒）」を記録
- **実装方法**:
  - Windows API `GetSystemTimeAsFileTime()` → FILETIME → Unix time変換
  - または `std::chrono::system_clock::now()` を使用
- **出力列**: 1列目に `UnixTime(ms)` を配置
- **旧形式との互換性**: 経過時間列は廃止（Unix timeから計算可能）

### 2.2 ファイル拡張子・命名規則
| 旧形式 | 新形式 | 内容 |
|--------|--------|------|
| `{filename}.dat` | `{filename}.tsv` | 物理量（主データ） |
| `{filename}.vlt` | `{filename}_vlt.tsv` | 電圧・生データ |
| `{filename}.out` | `{filename}_out.tsv` | パラメータ・制御情報 |

### 2.3 ヘッダー行の変更
**旧形式（.dat）**:
```
Time(ms)	Vertical_Stress(kPa)	...
```

**新形式（.tsv）**:
```
UnixTime(ms)	Vertical_Stress(kPa)	...
```

---

## 3. 実装設計（C++側）

### 3.1 現在の時刻取得実装

**使用中のAPI**: `_ftime64_s()` + `struct __timeb64`

**現在のコード**（`DigitShowDSTView.cpp`）:
```cpp
// ヘッダー: DigitShowDSTView.h
struct __timeb64 StartTime2, NowTime2;
struct __timeb64 StepTime0, StepTime1;

// Timer 3（データ保存）での使用例
if (_ftime64_s(&NowTime2) != 0) {
    return;
}
SequentTime2 = double(NowTime2.time - StartTime2.time)
             + double((NowTime2.millitm - StartTime2.millitm) / 1000.0);
```

**`__timeb64` 構造体の内容**:
- `time_t time` - Unix time（秒単位）
- `unsigned short millitm` - ミリ秒部分（0～999）
- `short timezone` - タイムゾーン
- `short dstflag` - 夏時間フラグ

**現在の問題点**:
- `_ftime64_s()` は非推奨（deprecated）なAPI
- `StartTime2` との差分計算で経過時間のみ記録
- 絶対時刻（Unix time）は取得できているが、保存していない

**std::chronoへの置き換えの可否**:
✅ **問題なし** - `std::chrono` は C++11以降の標準ライブラリで、より安全・ポータブル
- Visual Studio 2022では完全サポート
- `_ftime64_s()` より精度が高い（マイクロ秒・ナノ秒まで対応可能）
- 非推奨APIからの移行推奨

### 3.2 修正対象ファイル
- `DigitShowDSTDoc.cpp` - 保存処理の主要ロジック
- `DigitShowDSTView.cpp` - Timer 3 (`OnTimer()`) でのファイル保存トリガー、`_ftime64_s()` の置き換え
- `DigitShowDSTView.h` - `struct __timeb64` 変数の削除または置き換え
- `DigitShowDST.h` / `DigitShowDSTDoc.h` - 必要に応じてメンバ変数追加

### 3.3 Unix time取得関数の追加

**推奨実装**: `std::chrono`（C++11標準）

```cpp
// DigitShowDSTDoc.cpp または適切な場所に追加
#include <chrono>

// Unix time（ms）を取得
long long GetUnixTimeMs()
{
    using namespace std::chrono;
    auto now = system_clock::now();
    auto ms = duration_cast<milliseconds>(now.time_since_epoch());
    return ms.count();
}
```

**メリット**:
- C++標準ライブラリで、非推奨APIではない
- クロスプラットフォーム対応
- 型安全で精度が高い
- Visual Studio 2022で完全サポート

**代替案（Windows API使用）** - 既存コードとの親和性が高い場合:
```cpp
#include <windows.h>

long long GetUnixTimeMs()
{
    FILETIME ft;
    GetSystemTimeAsFileTime(&ft);

    // FILETIME（100ns単位、1601-01-01起点）→ Unix time（ms単位、1970-01-01起点）
    ULARGE_INTEGER uli;
    uli.LowPart = ft.dwLowDateTime;
    uli.HighPart = ft.dwHighDateTime;

    // 1601-01-01 → 1970-01-01 の差分（100ns単位）: 116444736000000000
    const long long EPOCH_DIFF = 116444736000000000LL;
    long long unix_time_100ns = uli.QuadPart - EPOCH_DIFF;
    long long unix_time_ms = unix_time_100ns / 10000; // 100ns → ms

    return unix_time_ms;
}
```

### 3.4 `_ftime64_s()` からの移行

**旧コード**（`DigitShowDSTView.cpp`）:
```cpp
// ヘッダー
struct __timeb64 StartTime2, NowTime2;

// 保存開始時
if (_ftime64_s(&StartTime2) != 0) { return; }

// データ保存時（Timer 3）
if (_ftime64_s(&NowTime2) != 0) { return; }
SequentTime2 = double(NowTime2.time - StartTime2.time)
             + double((NowTime2.millitm - StartTime2.millitm) / 1000.0);
```

**新コード案**:
```cpp
// ヘッダー: long long StartTimeMs を追加（または削除して都度 GetUnixTimeMs() を呼ぶ）
long long StartTimeMs;

// 保存開始時
StartTimeMs = GetUnixTimeMs();

// データ保存時（Timer 3）
long long nowTimeMs = GetUnixTimeMs();
SequentTime2 = (nowTimeMs - StartTimeMs) / 1000.0; // 秒単位の経過時間（互換性用、必要なら）

// ファイル保存時は nowTimeMs をそのまま出力
fprintf(FileSaveData0, "%lld\t", nowTimeMs);
```

**注意点**:
- `struct __timeb64` 型の変数（`StartTime2`, `NowTime2`, `StepTime0`, `StepTime1`）は削除または `long long` 型に変更
- `SequentTime2`（経過時間）は互換性のため残してもよいが、ファイル保存では Unix time を優先

### 3.5 ファイル保存処理の修正箇所

### 3.5 ファイル保存処理の修正箇所

#### 3.5.1 ファイル名生成ロジック
**場所**: `DigitShowDSTDoc.cpp` - `SaveToFile()` または関連関数

**旧コード例**:
```cpp
CString filename = m_FileNameData;  // 例: "2025-10-28_test"
CString datFile = filename + ".dat";
CString vltFile = filename + ".vlt";
CString outFile = filename + ".out";
```

**新コード案**:
```cpp
CString filename = m_FileNameData;  // 例: "2025-10-28_test"
CString tsvFile = filename + ".tsv";        // 物理量
CString vltFile = filename + "_vlt.tsv";    // 電圧
CString outFile = filename + "_out.tsv";    // パラメータ
```

#### 3.5.2 ヘッダー書き込み
**場所**: `DigitShowDSTDoc.cpp` - ヘッダー出力部

**旧コード例（.dat）**:
```cpp
fprintf(fp_data, "Time(ms)\tVertical_Stress(kPa)\t...\n");
```

**新コード案（.tsv）**:
```cpp
fprintf(fp_data, "UnixTime(ms)\tVertical_Stress(kPa)\t...\n");
```

#### 3.5.3 データ行の書き込み
**場所**: `DigitShowDSTDoc.cpp` - `SaveToFile()` 関数

**現在のコード** (`DigitShowDSTDoc.cpp` Line 538-539):
```cpp
fprintf(FileSaveData0, "%.3lf	", SequentTime2);  // 電圧ファイル
fprintf(FileSaveData1, "%.3lf	", SequentTime2);  // 物理量ファイル
fprintf(FileSaveData2, "%.3lf	", SequentTime2);  // パラメータファイル (Line 552)
```

**新コード案**:
```cpp
// Unix time（ms）を出力
long long unixTimeMs = GetUnixTimeMs();
fprintf(FileSaveData0, "%lld\t", unixTimeMs);  // 電圧ファイル
fprintf(FileSaveData1, "%lld\t", unixTimeMs);  // 物理量ファイル
fprintf(FileSaveData2, "%lld\t", unixTimeMs);  // パラメータファイル
```

**注意点**:
- `SequentTime2` は経過時間（秒単位、double型）
- 新形式では Unix time（ms単位、long long型）を使用
- フォーマット指定子を `%.3lf` → `%lld` に変更

### 3.6 FIFO保存モードの対応
**現状**: FIFO配列（`Data_FIFO[][]`）に経過時間を格納
**変更**: FIFO配列にもUnix time（ms）を格納

**修正箇所**:
- FIFO書き込み部（サンプリング時）: `Data_FIFO[0][index] = GetUnixTimeMs();`
- FIFO読み出し部（ファイル保存時 `SaveToFile2()`）: そのままUnix time列として出力

**現在のコード** (`DigitShowDSTDoc.cpp` Line 565-566):
```cpp
fprintf(FileSaveData0, "%.3lf	", SavingClock / 1000000.0 * i);
fprintf(FileSaveData1, "%.3lf	", SavingClock / 1000000.0 * i);
```

**新コード案**:
```cpp
// FIFO配列に格納されたUnix timeを出力（または計算ベース）
// 方法1: FIFO配列に格納済みの場合
fprintf(FileSaveData0, "%lld\t", Data_FIFO[0][i]);
fprintf(FileSaveData1, "%lld\t", Data_FIFO[0][i]);

// 方法2: サンプリング開始時刻 + サンプリング間隔で計算
long long startTimeMs = /* 保存開始時のUnix time */;
long long sampleTimeMs = startTimeMs + (long long)(SavingClock / 1000.0 * i);
fprintf(FileSaveData0, "%lld\t", sampleTimeMs);
fprintf(FileSaveData1, "%lld\t", sampleTimeMs);
```

---

## 4. 実装手順

### Phase 1: Unix time取得関数の実装
1. `GetUnixTimeMs()` 関数を `DigitShowDSTDoc.cpp` に追加
2. 単体テストまたは簡易検証（デバッグ出力で確認）

### Phase 2: ファイル名生成ロジックの変更
1. 拡張子を `.dat` → `.tsv`, `.vlt` → `_vlt.tsv`, `.out` → `_out.tsv` に変更
2. ファイル名生成関数を修正
3. ビルド・動作確認

### Phase 3: ヘッダー・データ行の変更
1. ヘッダー行を `Time(ms)` → `UnixTime(ms)` に変更
2. データ行の1列目を経過時間 → Unix time（ms）に変更
3. 通常保存モード・FIFO保存モードの両方を修正

### Phase 4: FIFO配列の対応
1. FIFO配列への格納時にUnix time（ms）を記録
2. FIFO読み出し時の出力ロジック確認

### Phase 5: テスト・検証
1. 新形式でのファイル保存テスト
2. Python解析ツールでの読み込みテスト
3. Unix timeの正確性確認（実時間との照合）

---

## 5. Python解析ツール側の対応

### 5.1 新形式対応
**修正ファイル**: `scripts/load_data.py`

**変更内容**:
- 1列目が `UnixTime(ms)` の場合、Unix time → datetime変換
- 旧形式（`Time(ms)`）の場合、経過時間として扱う（互換性維持）

**実装例**:
```python
def load_dat_file(filepath: str) -> pd.DataFrame:
    """Load .dat or .tsv file (supports both old and new formats)"""
    df = pd.read_csv(filepath, sep='\t', encoding='utf-8')

    # 新形式: UnixTime(ms) → datetime変換
    if 'UnixTime(ms)' in df.columns:
        df['Timestamp'] = pd.to_datetime(df['UnixTime(ms)'], unit='ms')
        df = df.drop(columns=['UnixTime(ms)'])
    # 旧形式: Time(ms) → 経過時間として保持
    elif 'Time(ms)' in df.columns:
        df['ElapsedTime_ms'] = df['Time(ms)']
        df = df.drop(columns=['Time(ms)'])

    return df
```

### 5.2 拡張子対応
- `.dat` / `.tsv` の両方を読み込み可能に
- `_vlt.tsv` / `.vlt` の両方を読み込み可能に

---

## 6. 移行計画

### 6.1 後方互換性
- 旧形式（`.dat`, `.vlt`, `.out`）の読み込みサポートは維持
- Python解析ツールは新旧両形式を自動判別

### 6.2 移行期間
- 新形式での保存開始後も、旧データの解析は継続可能
- ドキュメント（`data_file_formats.md`）を更新し、新旧両形式を記載

### 6.3 リリース計画
1. C++側実装・テスト
2. Python解析ツール更新
3. ドキュメント更新（`data_file_formats.md`）
4. リリースノート作成（新形式の説明、移行ガイド）

---

## 7. 注意事項・リスク

### 7.1 タイムゾーン
- Unix time（UTC）で記録されるため、ローカル時刻への変換はPython側で実施
- 日本時間（JST）への変換: `pd.to_datetime(...).tz_localize('UTC').tz_convert('Asia/Tokyo')`

### 7.2 ファイルサイズ
- Unix time（13桁）は経過時間（最大7桁程度）より桁数が多い → ファイルサイズ微増
- 影響: 数MB程度のファイルで数十KB増加程度（許容範囲）

### 7.3 既存データとの比較
- 旧形式（経過時間のみ）と新形式（Unix time）のデータを直接比較する際は注意
- Python解析ツールで時刻軸を統一する処理が必要

---

## 8. 関連ドキュメント

- `knowledge/data_file_formats.md` - 現行のファイル形式仕様（更新予定）
- `scripts/load_data.py` - Python解析ツール（対応予定）
- `README_ANALYSIS.md` - Python解析ツールの使い方（更新予定）

---

## 9. 実装チェックリスト

### C++側
- [ ] `GetUnixTimeMs()` 関数の実装
- [ ] ファイル名生成ロジックの変更（`.tsv`, `_vlt.tsv`, `_out.tsv`）
- [ ] ヘッダー行の変更（`UnixTime(ms)`）
- [ ] データ行の1列目をUnix timeに変更
- [ ] FIFO保存モードの対応
- [ ] ビルド・動作確認
- [ ] 実データでのテスト

### Python側
- [ ] `load_data.py` の新形式対応
- [ ] 旧形式との互換性確認
- [ ] タイムゾーン変換機能の追加
- [ ] プロット関数での時刻軸対応
- [ ] ドキュメント更新

### ドキュメント
- [ ] `data_file_formats.md` の更新
- [ ] `README_ANALYSIS.md` の更新
- [ ] リリースノート作成

---

## 10. まとめ

この移行により、以下のメリットが得られます：

1. **トレーサビリティ向上**: データの絶対時刻が記録され、いつのデータか明確
2. **解析の容易化**: 複数ファイルの時系列比較・統合が可能
3. **ファイル管理の改善**: TSV形式統一で外部ツール連携が容易
4. **再現性の向上**: 実験日時が明確になり、実験ノートとの対応付けが簡単

実装は段階的に進め、各フェーズでテスト・検証を行うことで、安全に移行できます。

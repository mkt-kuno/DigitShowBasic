# DigitShowDST ログファイル解析ツール

Python製のデータ分析・可視化ツール。DigitShowDSTが出力する `.dat`（物理量）、`.vlt`（生電圧）、`.out`（計算パラメータ）を読み込んでグラフ化・統計解析します。

## セットアップ

### 1. uv のインストール（未導入の場合）

```powershell
# Windows (PowerShell)
irm https://astral.sh/uv/install.ps1 | iex
```

他のOSは [公式ドキュメント](https://docs.astral.sh/uv/getting-started/installation/) を参照。

### 2. 依存パッケージのインストール

```powershell
# プロジェクトルートで実行
uv sync
```

### 3. 環境の有効化

```powershell
# uv run でスクリプトを直接実行可能
uv run python scripts/plot_data.py <ファイルパス>

# または仮想環境をアクティベート
.venv\Scripts\Activate.ps1  # Windows PowerShell
```

## 使い方

### クイックスタート

```powershell
# 総合レポートを自動生成（全グラフ＋統計）
uv run python scripts/plot_data.py 2025-10-28_test

# 統計解析JSONを出力
uv run python scripts/analyze_data.py 2025-10-28_test output/report.json

# 使用例を実行（例5: サマリーレポート生成）
uv run python scripts/examples.py 5
```

### 各スクリプトの説明

#### `scripts/load_data.py`
データ読み込みユーティリティ。

```python
from scripts.load_data import read_dat_file, read_all_files

# 単一ファイル読み込み
df_dat = read_dat_file("2025-10-28_test.dat")
print(df_dat.head())

# .dat/.vlt/.out を一括読み込み
df_dat, df_vlt, df_out = read_all_files("2025-10-28_test")
```

#### `scripts/plot_data.py`
可視化ツール。

```python
from scripts.plot_data import create_summary_report

# 全グラフを自動生成（output ディレクトリに保存）
create_summary_report("2025-10-28_test", output_dir="output")
```

**生成されるグラフ**:
- `*_stress_strain.png` — 荷重-変位 & セル圧力の時系列
- `*_dat_timeseries.png` — 主要物理量（Load, Disp, Cell_P）の時系列
- `*_voltage_ch0-5.png` — 生電圧 CH00～CH05 の時系列
- `*_parameters.png` — 計算パラメータ（せん断応力、有効応力、ひずみ、制御ステップ）

#### `scripts/analyze_data.py`
統計解析ツール。

```python
from scripts.analyze_data import generate_analysis_report

# JSON レポートを生成
report = generate_analysis_report("2025-10-28_test", "report.json")
print(report['loading_analysis'])
```

**出力内容**:
- 基本統計量（mean, std, min, max）
- 載荷解析（最大荷重、剛性推定）
- 圧力解析（最大・平均圧力、変化率）

#### `scripts/analyze_displacement_velocity.py`
変位速度解析ツール（複数載荷区間の線形回帰）。

```powershell
# 手動で区間を選択（デフォルト）
uv run python scripts/analyze_displacement_velocity.py 2025-10-29_shear_test

# 自動検出モード（変位閾値 0.5 mm）
uv run python scripts/analyze_displacement_velocity.py 2025-10-29_shear_test --auto --threshold 0.5

# 電圧を指定して電圧-速度関係も解析
uv run python scripts/analyze_displacement_velocity.py motor_calibration --auto --voltages 2,4,6,8

# 結果をCSVに保存
uv run python scripts/analyze_displacement_velocity.py 2025-10-29_shear_test --output results.csv
```

**機能**:
- 三角形状の載荷区間を自動検出または手動選択（マウスでドラッグ）
- 各区間で線形回帰: 傾き a (mm/s), 切片 b (mm), 決定係数 R²
- 電圧を指定すると電圧-変位速度関係も線形回帰（モーターキャリブレーション用）
- グラフ出力: 変位時系列 + 回帰直線 + 統計バーチャート + 電圧-速度プロット

#### `scripts/examples.py`
8種類の使用例。

```powershell
# 使用可能な例を表示
uv run python scripts/examples.py

# 例5を実行（総合レポート生成）
uv run python scripts/examples.py 5
```

## ファイル形式の詳細

`knowledge/data_file_formats.md` を参照。

- **区切り**: タブ
- **時刻列**: `Time(s)`（先頭列、秒単位）
- **文字コード**: UTF-8互換（数値と英字のみ）

## カスタマイズ例

### 特定の列だけプロット

```python
from scripts.load_data import read_dat_file
from scripts.plot_data import plot_time_series

df = read_dat_file("2025-10-28_test.dat")
plot_time_series(df, columns=['Load_(N)', 'Cell_P.(kPa)'], save_path="custom.png")
```

### 電圧チャンネルを選択

```python
from scripts.load_data import read_vlt_file
from scripts.plot_data import plot_voltage_channels

df_vlt = read_vlt_file("2025-10-28_test.vlt")
plot_voltage_channels(df_vlt, channels=[0, 1, 5, 10], save_path="ch_0_1_5_10.png")
```

### Jupyter Notebookで対話的に分析

```powershell
# Jupyter をインストール（オプション）
uv pip install jupyter

# Notebook 起動
uv run jupyter notebook
```

Notebook内:
```python
from scripts.load_data import read_all_files
import matplotlib.pyplot as plt

df_dat, df_vlt, df_out = read_all_files("2025-10-28_test")

# 自由にプロット
plt.figure(figsize=(10, 6))
plt.plot(df_dat['Disp.(mm)'], df_dat['Load_(N)'])
plt.xlabel('Displacement (mm)')
plt.ylabel('Load (N)')
plt.title('Custom Plot')
plt.grid(True)
plt.show()
```

## トラブルシュート

### ファイルが見つからない

```python
from scripts.load_data import list_log_files
from pathlib import Path

# カレントディレクトリの .dat ファイルを一覧表示
files = list_log_files(Path("."))
print(files)
```

### グラフが表示されない

- `save_path` を指定して画像ファイルに保存
- または `plt.show()` が呼ばれるまで待つ（バックエンド依存）

### 列名が見つからない

ヘッダ名はFIFO書き出しと通常保存で異なる場合あり。実際のヘッダを確認:

```python
df = read_dat_file("test.dat")
print(df.columns.tolist())
```

## 開発者向け

### 新しい解析関数を追加

`scripts/analyze_data.py` に関数を追加し、`generate_analysis_report()` 内で呼び出す。

### 新しいプロット関数を追加

`scripts/plot_data.py` に関数を追加し、`create_summary_report()` 内で呼び出す。

### テストデータ

`2025-10-28_test.vlt` など、実際のログファイルをプロジェクトルートに配置してテスト。

## ライセンス

プロジェクト本体に準じる。

## 関連ドキュメント

- `knowledge/data_file_formats.md` — ログファイル形式の詳細
- `knowledge/control_specifications.md` — 制御スクリプト仕様
- `.github/copilot-instructions.md` — アーキテクチャ概要

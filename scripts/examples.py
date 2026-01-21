"""
DigitShowDST ログファイル解析ツールの使用例

各スクリプトの基本的な使い方を示すサンプルコード。
Jupyter Notebook や Python REPL で実行可能。
"""

from pathlib import Path
import sys

# scripts ディレクトリをパスに追加（必要に応じて）
# sys.path.append(str(Path(__file__).parent))

from load_data import read_dat_file, read_vlt_file, read_out_file, read_all_files, list_log_files
from plot_data import plot_stress_strain, plot_time_series, plot_voltage_channels, plot_parameters, create_summary_report
from analyze_data import calculate_basic_stats, analyze_loading_phase, analyze_pressure, generate_analysis_report


# =============================================================================
# 例1: 単一ファイルの読み込みと基本確認
# =============================================================================

def example1_basic_loading():
    """基本的なファイル読み込みと確認"""
    print("Example 1: Basic file loading")

    # ファイルパスを指定（例: ワークスペースルートからの相対パス）
    dat_file = Path("2025-10-28_test.dat")

    if dat_file.exists():
        df = read_dat_file(dat_file)
        print(f"\nLoaded {len(df)} rows")
        print(f"Columns: {list(df.columns)}")
        print(f"\nFirst 5 rows:")
        print(df.head())
        print(f"\nBasic statistics:")
        print(df.describe())
    else:
        print(f"File not found: {dat_file}")


# =============================================================================
# 例2: .dat/.vlt/.out の一括読み込み
# =============================================================================

def example2_load_all():
    """同名の3ファイルを一括読み込み"""
    print("Example 2: Load all files at once")

    base_path = "2025-10-28_test"  # 拡張子なし

    df_dat, df_vlt, df_out = read_all_files(base_path)

    if df_dat is not None:
        print(f"\n.dat: {len(df_dat)} rows, columns: {list(df_dat.columns)[:5]}...")

    if df_vlt is not None:
        print(f".vlt: {len(df_vlt)} rows, columns: {list(df_vlt.columns)[:5]}...")

    if df_out is not None:
        print(f".out: {len(df_out)} rows, columns: {list(df_out.columns)[:5]}...")


# =============================================================================
# 例3: 応力-ひずみ曲線のプロット
# =============================================================================

def example3_plot_stress_strain():
    """応力-ひずみ曲線をプロット（画面表示）"""
    print("Example 3: Plot stress-strain curve")

    base_path = "2025-10-28_test"
    df_dat, _, _ = read_all_files(base_path)

    if df_dat is not None:
        # 画面に表示（save_path を指定しない）
        plot_stress_strain(df_dat)

        # ファイルに保存する場合
        # plot_stress_strain(df_dat, save_path="stress_strain.png")


# =============================================================================
# 例4: 特定のチャンネルの時系列プロット
# =============================================================================

def example4_plot_channels():
    """電圧チャンネルの時系列プロット"""
    print("Example 4: Plot voltage channels")

    base_path = "2025-10-28_test"
    _, df_vlt, _ = read_all_files(base_path)

    if df_vlt is not None:
        # CH00, CH01, CH02 のみプロット
        plot_voltage_channels(df_vlt, channels=[0, 1, 2])


# =============================================================================
# 例5: 総合レポートの自動生成
# =============================================================================

def example5_summary_report():
    """すべてのグラフを自動生成"""
    print("Example 5: Generate summary report")

    base_path = "2025-10-28_test"
    output_dir = Path("output")

    # output ディレクトリに全グラフを保存
    create_summary_report(base_path, output_dir)
    print(f"\nAll plots saved to: {output_dir}")


# =============================================================================
# 例6: 統計解析とJSON出力
# =============================================================================

def example6_analysis():
    """統計解析を実行してJSONレポートを生成"""
    print("Example 6: Statistical analysis")

    base_path = "2025-10-28_test"
    output_json = Path("output/analysis_report.json")

    report = generate_analysis_report(base_path, output_json)

    # Pythonオブジェクトとしてアクセス可能
    if 'loading_analysis' in report:
        print(f"\nMax Load: {report['loading_analysis'].get('max_load', 'N/A')} N")
        print(f"Max Displacement: {report['loading_analysis'].get('max_displacement', 'N/A')} mm")


# =============================================================================
# 例7: 複数ファイルのバッチ処理
# =============================================================================

def example7_batch_processing():
    """ディレクトリ内の全ファイルをバッチ処理"""
    print("Example 7: Batch processing")

    # カレントディレクトリの全 .dat ファイルを検索
    log_files = list_log_files(Path("."))

    print(f"Found {len(log_files)} log file(s):")
    for log_file in log_files:
        print(f"  - {log_file.name}")

    # 各ファイルを処理
    output_dir = Path("batch_output")
    output_dir.mkdir(exist_ok=True)

    for log_file in log_files:
        print(f"\nProcessing: {log_file.name}")
        try:
            create_summary_report(log_file, output_dir / log_file.name)
        except Exception as e:
            print(f"  Error: {e}")


# =============================================================================
# 例8: カスタムプロット（pandas + matplotlib）
# =============================================================================

def example8_custom_plot():
    """カスタムプロットの例"""
    print("Example 8: Custom plotting")

    import matplotlib.pyplot as plt

    base_path = "2025-10-28_test"
    df_dat, _, df_out = read_all_files(base_path)

    if df_dat is not None and df_out is not None:
        fig, ax = plt.subplots(figsize=(10, 6))

        # 荷重とせん断応力を同時プロット（2軸）
        ax.plot(df_dat.index, df_dat['Load_(N)'], 'b-', label='Load (N)')
        ax.set_xlabel('Time (s)')
        ax.set_ylabel('Load (N)', color='b')
        ax.tick_params(axis='y', labelcolor='b')

        ax2 = ax.twinx()
        if 's(a)_(kPa)' in df_out.columns:
            ax2.plot(df_out.index, df_out['s(a)_(kPa)'], 'r-', label='Shear Stress (kPa)')
            ax2.set_ylabel('Shear Stress (kPa)', color='r')
            ax2.tick_params(axis='y', labelcolor='r')

        ax.set_title('Load and Shear Stress vs Time')
        ax.grid(True, alpha=0.3)
        plt.tight_layout()
        plt.show()


# =============================================================================
# メイン実行部（コマンドライン引数で例を選択）
# =============================================================================

if __name__ == '__main__':
    examples = {
        '1': ('Basic file loading', example1_basic_loading),
        '2': ('Load all files at once', example2_load_all),
        '3': ('Plot stress-strain curve', example3_plot_stress_strain),
        '4': ('Plot voltage channels', example4_plot_channels),
        '5': ('Generate summary report', example5_summary_report),
        '6': ('Statistical analysis', example6_analysis),
        '7': ('Batch processing', example7_batch_processing),
        '8': ('Custom plotting', example8_custom_plot),
    }

    if len(sys.argv) < 2:
        print("DigitShowDST Log Analysis Examples")
        print("=" * 50)
        print("\nUsage: python examples.py <example_number>")
        print("\nAvailable examples:")
        for num, (desc, _) in examples.items():
            print(f"  {num}: {desc}")
        print("\nExample: python examples.py 5")
        sys.exit(0)

    example_num = sys.argv[1]

    if example_num in examples:
        desc, func = examples[example_num]
        print(f"\nRunning Example {example_num}: {desc}")
        print("=" * 50)
        func()
    else:
        print(f"Error: Example '{example_num}' not found")
        print(f"Available examples: {', '.join(examples.keys())}")
        sys.exit(1)

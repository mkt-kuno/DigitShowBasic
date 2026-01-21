"""
せん断力載荷装置の変位速度解析

水平変位計のデータから複数の載荷区間を自動検出し、
それぞれの区間で線形回帰を実行して変位速度を求める。
"""

import pandas as pd
import numpy as np
import matplotlib.pyplot as plt
from pathlib import Path
from typing import List, Tuple, Dict
from scipy.stats import linregress
from load_data import read_all_files


def linear_regression_through_origin(
    x: np.ndarray, y: np.ndarray
) -> Tuple[float, float]:
    """
    原点(0,0)を通る線形回帰: y = a*x
    numpy.linalg.lstsqを使用

    Args:
        x: 説明変数
        y: 目的変数

    Returns:
        Tuple[slope, r_squared]: 傾き a と決定係数 R²
    """
    # x を列ベクトルに変換 (切片なしなので1列のみ)
    A = x.reshape(-1, 1)

    # 最小二乗法で傾きを求める
    result = np.linalg.lstsq(A, y, rcond=None)
    slope = result[0][0]  # 傾き

    # R²の計算
    y_pred = slope * x
    ss_res = np.sum((y - y_pred) ** 2)  # 残差平方和
    ss_tot = np.sum((y - np.mean(y)) ** 2)  # 全平方和
    r_squared = 1 - (ss_res / ss_tot)

    return slope, r_squared


def detect_loading_segments(
    displacement: pd.Series,
    threshold: float = 0.1,
    min_duration: float = 5.0,
    reset_threshold: float = 0.05,
) -> List[Tuple[pd.Timestamp, pd.Timestamp]]:
    """
    変位データから載荷区間（三角形の上昇部分）を自動検出

    各三角形サイクル（上昇→下降）を検出し、上昇部分のみを返す。
    変位がほぼゼロに戻った時点で次のサイクルとみなす。

    Args:
        displacement: 変位時系列データ
        threshold: 変位の最小値閾値 (mm)、これを超えたら有効な上昇とみなす
        min_duration: 最小載荷時間 (s)
        reset_threshold: リセット判定の変位閾値 (mm)、これ以下になったら次のサイクル開始

    Returns:
        List[Tuple[start_time, end_time]]: 検出された上昇区間のリスト
    """
    segments = []

    # 変位がリセット閾値以下の点を検出（サイクルの境界候補）
    is_reset = displacement.abs() <= reset_threshold

    # リセット点のインデックスを取得
    reset_indices = displacement.index[is_reset].tolist()

    if not reset_indices:
        # リセット点がない場合は全体を1つの区間として扱う
        if len(displacement) > 0:
            return [(displacement.index[0], displacement.index[-1])]
        return []

    # 連続するリセット点をグループ化して境界を特定
    cycle_boundaries = []
    prev_idx = None

    for idx in reset_indices:
        if prev_idx is None:
            cycle_boundaries.append(idx)
        else:
            # 時間差を計算
            if isinstance(idx, pd.Timestamp):
                gap = (idx - prev_idx).total_seconds()
            else:
                gap = idx - prev_idx

            # 十分な時間差があれば新しい境界
            if gap > 1.0:  # 1秒以上の間隔
                cycle_boundaries.append(idx)

        prev_idx = idx

    # 最初と最後の境界を追加
    if displacement.index[0] not in cycle_boundaries:
        cycle_boundaries.insert(0, displacement.index[0])
    if displacement.index[-1] not in cycle_boundaries:
        cycle_boundaries.append(displacement.index[-1])

    # 各サイクルの上昇部分を検出
    for i in range(len(cycle_boundaries) - 1):
        start_bound = cycle_boundaries[i]
        end_bound = cycle_boundaries[i + 1]

        # この境界間のデータを取得
        cycle_data = displacement.loc[start_bound:end_bound]

        if len(cycle_data) < 2:
            continue

        # ピーク（最大変位）を見つける
        peak_idx = cycle_data.idxmax()
        peak_value = cycle_data.max()

        # 閾値チェック
        if peak_value < threshold:
            continue

        # 上昇部分を抽出（開始からピークまで）
        rising_data = displacement.loc[start_bound:peak_idx]

        # 継続時間チェック
        if isinstance(rising_data.index, pd.DatetimeIndex):
            duration = (rising_data.index[-1] - rising_data.index[0]).total_seconds()
        else:
            duration = rising_data.index[-1] - rising_data.index[0]

        if duration >= min_duration and len(rising_data) >= 2:
            segments.append((rising_data.index[0], rising_data.index[-1]))

    return segments


def manual_segment_selection(
    displacement: pd.Series, n_segments: int = 4
) -> List[Tuple[pd.Timestamp, pd.Timestamp]]:
    """
    変位データをプロットし、ユーザーが手動で区間を選択

    Args:
        displacement: 変位時系列データ
        n_segments: 選択する区間数

    Returns:
        List[Tuple[start_time, end_time]]: 選択された区間のリスト
    """
    import matplotlib.pyplot as plt
    from matplotlib.widgets import SpanSelector

    segments = []

    fig, ax = plt.subplots(figsize=(12, 6))
    ax.plot(displacement.index, displacement.values, "b-", linewidth=1)
    ax.set_xlabel("Time")
    ax.set_ylabel("Displacement (mm)")
    ax.set_title(f"Select {n_segments} loading segments (click and drag)")
    ax.grid(True)

    def onselect(xmin, xmax):
        if len(segments) < n_segments:
            segments.append((xmin, xmax))
            # 選択された区間をハイライト
            ax.axvspan(xmin, xmax, alpha=0.3, color="red")
            plt.draw()

            if len(segments) == n_segments:
                print(
                    f"✓ {n_segments} segments selected. Close the window to continue."
                )

    # SpanSelector で区間選択
    SpanSelector(
        ax,
        onselect,
        "horizontal",
        useblit=True,
        props=dict(alpha=0.3, facecolor="red"),
        interactive=True,
        drag_from_anywhere=True,
    )

    plt.tight_layout()
    plt.show()

    return segments


def trim_segment_by_velocity(
    displacement: pd.Series, velocity_threshold_ratio: float = 0.3
) -> pd.Series:
    """
    変位速度が小さい両端部分をトリミング

    変位の微分（速度）を計算し、最大速度の一定割合以下の部分を両端から除去。
    モーター停止時の平坦部分を除外するために使用。
    下降部分（負の速度）も除外する。

    Args:
        displacement: 変位時系列データ
        velocity_threshold_ratio: 最大速度に対する閾値の割合（0.3 = 最大速度の30%）

    Returns:
        pd.Series: トリミングされた変位データ
    """
    if len(displacement) < 3:
        return displacement

    # 時間差を計算
    if isinstance(displacement.index, pd.DatetimeIndex):
        dt = displacement.index.to_series().diff().dt.total_seconds()
    else:
        dt = displacement.index.to_series().diff()

    # 変位速度を計算（符号付き）
    velocity = displacement.diff() / dt

    # 最大速度の一定割合を閾値とする（正の速度のみ）
    max_velocity = velocity.max()
    threshold = max_velocity * velocity_threshold_ratio

    # 閾値を超える正の速度を持つ区間を検出
    above_threshold = velocity > threshold

    if not above_threshold.any():
        # 閾値を超える点がない場合は元のデータを返す
        return displacement

    # 最初と最後の閾値超え点を見つける
    valid_indices = np.where(above_threshold)[0]

    if len(valid_indices) == 0:
        return displacement

    start_pos = valid_indices[0]
    end_pos = valid_indices[-1]

    # インデックスで抽出
    trimmed = displacement.iloc[start_pos : end_pos + 1]

    return trimmed


def perform_linear_regression(
    time_series: pd.Series,
    displacement: pd.Series,
    trim_edges: bool = True,
    velocity_threshold_ratio: float = 0.3,
) -> Dict[str, any]:
    """
    時刻と変位の線形回帰を実行

    Args:
        time_series: 時刻データ（秒単位に変換）
        displacement: 変位データ
        trim_edges: 両端の低速部分をトリミングするか
        velocity_threshold_ratio: トリミング時の速度閾値（最大速度に対する割合）

    Returns:
        Dict: 回帰結果（傾き a, 切片 b, 決定係数 R^2, 標準誤差など）+ トリミング後のデータ
    """
    # 両端をトリミング
    trimmed_displacement = displacement
    if trim_edges:
        trimmed_displacement = trim_segment_by_velocity(
            displacement, velocity_threshold_ratio
        )

    if len(trimmed_displacement) < 2:
        # データ点が不足している場合
        return {
            "slope_a": 0.0,
            "intercept_b": 0.0,
            "r_squared": 0.0,
            "p_value": 1.0,
            "std_error": 0.0,
            "n_points": len(trimmed_displacement),
            "n_points_trimmed": len(displacement) - len(trimmed_displacement),
            "trimmed_data": trimmed_displacement,
        }

    # 時刻を秒単位の数値に変換
    if isinstance(trimmed_displacement.index, pd.DatetimeIndex):
        # 相対時間（最初を0とする）
        t_values = (
            (trimmed_displacement.index - trimmed_displacement.index[0])
            .total_seconds()
            .values
        )
    else:
        # 既に数値（Time(s)）の場合
        t_values = (
            trimmed_displacement.index.values - trimmed_displacement.index.values[0]
        )

    y_values = trimmed_displacement.values

    # 線形回帰
    slope, intercept, r_value, p_value, std_err = linregress(t_values, y_values)

    return {
        "slope_a": slope,  # 傾き a (mm/s)
        "intercept_b": intercept,  # 切片 b (mm)
        "r_squared": r_value**2,  # 決定係数 R^2
        "p_value": p_value,  # p値
        "std_error": std_err,  # 標準誤差
        "n_points": len(t_values),  # データ点数（トリミング後）
        "n_points_trimmed": len(time_series) - len(t_values),  # トリミングされた点数
        "trimmed_data": trimmed_displacement,  # トリミング後のデータ
    }


def voltage_velocity_regression(
    results_df: pd.DataFrame, voltages: List[float]
) -> Dict[str, float]:
    """
    電圧と変位速度の線形回帰（原点を通る）

    電圧0のとき変位速度0という物理的制約を考慮し、
    切片を0に固定した線形回帰を実行: velocity = a * voltage

    Args:
        results_df: 各セグメントの回帰結果
        voltages: 各セグメントに対応する電圧値のリスト

    Returns:
        Dict: 電圧-変位速度関係の回帰結果（傾き a, 決定係数 R^2）
    """
    if len(voltages) != len(results_df):
        raise ValueError(
            f"Number of voltages ({len(voltages)}) must match number of segments ({len(results_df)})"
        )

    # 変位速度を取得
    velocities = results_df["slope_a_mm_per_s"].values

    # 原点を通る線形回帰
    slope, r_squared = linear_regression_through_origin(np.array(voltages), velocities)

    return {
        "voltage_slope_a": slope,  # 傾き a (mm/s/V)
        "voltage_intercept_b": 0.0,  # 切片 b = 0 (mm/s) - 原点を通る
        "voltage_r_squared": r_squared,  # 決定係数 R^2
        "voltage_p_value": None,  # p値（原点回帰では計算しない）
        "voltage_std_error": None,  # 標準誤差（原点回帰では計算しない）
    }


def analyze_displacement_velocity(
    base_path: str | Path,
    displacement_column: str = "Disp.(mm)",
    auto_detect: bool = False,
    n_segments: int = 4,
    threshold: float = 0.5,
    reset_threshold: float = 0.05,
    voltages: List[float] = None,
    trim_edges: bool = True,
    velocity_threshold_ratio: float = 0.3,
    output_csv: str | Path = None,
    plot: bool = True,
) -> Tuple[pd.DataFrame, Dict[str, float]]:
    """
    変位速度解析のメイン処理

    Args:
        base_path: ログファイルのベースパス
        displacement_column: 変位データのカラム名
        auto_detect: 自動検出モード（False の場合は手動選択）
        n_segments: 検出/選択する区間数
        threshold: 自動検出時の変位の最小値閾値 (mm)、これ以上の変位を持つ三角形を検出
        reset_threshold: リセット判定閾値 (mm)、これ以下になったら次のサイクル
        voltages: 各セグメントに対応する電圧値のリスト (V)。指定すると電圧-速度関係も解析
        trim_edges: 両端の低速部分をトリミングするか（デフォルト: True）
        velocity_threshold_ratio: トリミング時の速度閾値（最大速度に対する割合、デフォルト: 0.3）
        output_csv: 結果の CSV 出力先
        plot: プロット表示の有無

    Returns:
        Tuple[pd.DataFrame, Dict]: 各区間の回帰結果と電圧-速度関係の回帰結果
    """
    base_path = Path(base_path)
    if base_path.suffix:
        base_path = base_path.with_suffix("")

    print(f"Loading data from: {base_path.name}.*")
    df_dat, df_vlt, df_out = read_all_files(base_path)

    if df_dat is None:
        raise FileNotFoundError(f"No data file found for {base_path}")

    if displacement_column not in df_dat.columns:
        print(f"Available columns: {df_dat.columns.tolist()}")
        raise ValueError(f"Column '{displacement_column}' not found in data")

    displacement = df_dat[displacement_column]
    print(f"Displacement data: {len(displacement)} points")

    # 区間検出
    if auto_detect:
        print(
            f"Auto-detecting loading segments (threshold={threshold} mm, reset={reset_threshold} mm)..."
        )
        segments = detect_loading_segments(
            displacement,
            threshold=threshold,
            min_duration=5.0,
            reset_threshold=reset_threshold,
        )
        print(f"Detected {len(segments)} segments")

        if len(segments) < n_segments:
            print(
                f"Warning: Only {len(segments)} segments detected, expected {n_segments}"
            )
            print(
                "Consider adjusting --threshold or --reset-threshold, or use manual selection mode"
            )
        elif len(segments) > n_segments:
            print(f"Warning: {len(segments)} segments detected, expected {n_segments}")
            print(f"Using first {n_segments} segments")
            segments = segments[:n_segments]
    else:
        print("Manual segment selection mode...")
        segments = manual_segment_selection(displacement, n_segments)

    if not segments:
        raise ValueError("No segments detected or selected")

    # 各区間で線形回帰
    results = []
    trimmed_data_list = []  # トリミング後のデータを保存
    for i, (start, end) in enumerate(segments, 1):
        print(f"\nSegment {i}: {start} to {end}")

        # 区間データを抽出
        segment_data = displacement.loc[start:end]

        if len(segment_data) < 2:
            print(f"  ⚠ Insufficient data points: {len(segment_data)}")
            continue

        # 線形回帰（トリミングオプション付き）
        original_n_points = len(segment_data)
        reg_result = perform_linear_regression(
            segment_data,
            segment_data,
            trim_edges=trim_edges,
            velocity_threshold_ratio=velocity_threshold_ratio,
        )

        # トリミング後のデータを保存
        trimmed_data_list.append(reg_result["trimmed_data"])

        if trim_edges and reg_result["n_points_trimmed"] > 0:
            print(
                f"  ⚠ Trimmed {reg_result['n_points_trimmed']} edge points ({original_n_points} → {reg_result['n_points']} points)"
            )

        # 時間範囲を計算
        if isinstance(segment_data.index, pd.DatetimeIndex):
            duration = (segment_data.index[-1] - segment_data.index[0]).total_seconds()
            start_time_str = segment_data.index[0].strftime("%Y-%m-%d %H:%M:%S.%f")
            end_time_str = segment_data.index[-1].strftime("%Y-%m-%d %H:%M:%S.%f")
        else:
            duration = segment_data.index[-1] - segment_data.index[0]
            start_time_str = f"{segment_data.index[0]:.3f} s"
            end_time_str = f"{segment_data.index[-1]:.3f} s"

        # 変位範囲
        disp_start = segment_data.iloc[0]
        disp_end = segment_data.iloc[-1]
        disp_range = disp_end - disp_start

        result = {
            "segment": i,
            "start_time": start_time_str,
            "end_time": end_time_str,
            "duration_s": duration,
            "disp_start_mm": disp_start,
            "disp_end_mm": disp_end,
            "disp_range_mm": disp_range,
            "slope_a_mm_per_s": reg_result["slope_a"],
            "intercept_b_mm": reg_result["intercept_b"],
            "r_squared": reg_result["r_squared"],
            "p_value": reg_result["p_value"],
            "std_error": reg_result["std_error"],
            "n_points": reg_result["n_points"],
            "n_points_original": original_n_points,
            "n_points_trimmed": reg_result["n_points_trimmed"],
        }

        results.append(result)

        print(f"  Slope (a):      {reg_result['slope_a']:.6f} mm/s")
        print(f"  Intercept (b):  {reg_result['intercept_b']:.6f} mm")
        print(f"  R²:             {reg_result['r_squared']:.6f}")
        print(f"  Duration:       {duration:.2f} s")
        print(f"  Displacement:   {disp_range:.3f} mm")

    # DataFrame に変換
    results_df = pd.DataFrame(results)

    # 電圧情報を追加
    voltage_regression_result = None
    if voltages is not None:
        if len(voltages) == len(results_df):
            results_df["voltage_V"] = voltages

            # 電圧-変位速度の線形回帰
            print("\n" + "=" * 60)
            print("VOLTAGE-VELOCITY REGRESSION")
            print("=" * 60)
            voltage_regression_result = voltage_velocity_regression(
                results_df, voltages
            )

            print("Voltage vs Velocity relationship (through origin):")
            print(
                f"  Slope (a):      {voltage_regression_result['voltage_slope_a']:.9f} mm/s/V"
            )
            print(
                f"  Intercept (b):  {voltage_regression_result['voltage_intercept_b']:.9f} mm/s (fixed at 0)"
            )
            print(
                f"  R²:             {voltage_regression_result['voltage_r_squared']:.6f}"
            )
        else:
            print(
                f"\n⚠ Warning: Number of voltages ({len(voltages)}) does not match number of segments ({len(results_df)})"
            )
            print("   Voltage-velocity regression skipped.")

    # CSV 出力
    if output_csv:
        output_csv = Path(output_csv)
        results_df.to_csv(output_csv, index=False, encoding="utf-8-sig")
        print(f"\n✓ Results saved to: {output_csv}")

    # プロット
    if plot:
        plot_segments_and_regression(
            displacement,
            segments,
            results_df,
            base_path.name,
            voltages,
            voltage_regression_result,
            trimmed_data_list,
        )

    return results_df, voltage_regression_result


def plot_segments_and_regression(
    displacement: pd.Series,
    segments: List[Tuple],
    results_df: pd.DataFrame,
    title: str,
    voltages: List[float] = None,
    voltage_regression_result: Dict[str, float] = None,
    trimmed_data_list: List[pd.Series] = None,
):
    """
    変位データと線形回帰結果をプロット
    """
    # 電圧情報がある場合は2段プロット（変位+電圧-速度）、ない場合は1段プロット（変位のみ）
    if voltages is not None and len(voltages) == len(results_df):
        fig, (ax1, ax2) = plt.subplots(2, 1, figsize=(12, 10))
    else:
        fig, ax1 = plt.subplots(1, 1, figsize=(12, 6))
        ax2 = None

    # 上段: 全体の変位データ（散布図）- 青色の円マーカー
    ax1.scatter(
        displacement.index,
        displacement.values,
        s=3,
        c="blue",
        alpha=0.3,
        label="Raw data",
        marker="o",
    )

    colors = ["red", "green", "orange", "purple", "cyan", "magenta"]

    for i, (start, end) in enumerate(segments):
        color = colors[i % len(colors)]

        # トリミング後のデータのみ表示
        if trimmed_data_list is not None and i < len(trimmed_data_list):
            trimmed_data = trimmed_data_list[i]

            # トリミング後のデータポイントを各セグメントの色で円マーカー表示
            ax1.scatter(
                trimmed_data.index,
                trimmed_data.values,
                s=5,
                color=color,
                label=f"Segment {i + 1} (data)",
                marker="o",
            )

            # 回帰直線
            if i < len(results_df):
                result = results_df.iloc[i]
                slope = result["slope_a_mm_per_s"]
                intercept = result["intercept_b_mm"]

                if isinstance(trimmed_data.index, pd.DatetimeIndex):
                    t_values = (
                        trimmed_data.index - trimmed_data.index[0]
                    ).total_seconds()
                else:
                    t_values = trimmed_data.index - trimmed_data.index[0]

                y_fit = slope * t_values + intercept
                ax1.plot(
                    trimmed_data.index,
                    y_fit,
                    "--",
                    color=color,
                    linewidth=2,
                )

    ax1.set_xlabel("Time")
    ax1.set_ylabel("Displacement (mm)")
    ax1.set_title(f"Displacement vs Time - {title}")
    ax1.legend()
    ax1.grid(True)

    # 下段: 電圧-変位速度の関係
    if ax2 is not None and voltages is not None and len(voltages) == len(results_df):
        velocities = results_df["slope_a_mm_per_s"].values

        # データ点をプロット
        ax2.plot(
            voltages,
            velocities,
            "o",
            markersize=8,
            color="steelblue",
            label="Measured data",
        )

        # 回帰直線をプロット
        if voltage_regression_result is not None:
            slope = voltage_regression_result["voltage_slope_a"]
            r_squared = voltage_regression_result["voltage_r_squared"]

            # 原点を通る回帰直線
            v_range = np.linspace(0, max(voltages) + 0.5, 100)
            velocity_fit = slope * v_range
            ax2.plot(
                v_range,
                velocity_fit,
                "r--",
                linewidth=2,
                label=f"Linear fit: y = {slope:.6f}x (R² = {r_squared:.6f})",
            )

        ax2.set_xlabel("Voltage (V)")
        ax2.set_ylabel("Displacement Velocity (mm/s)")
        ax2.set_title("Voltage vs Displacement Velocity")
        ax2.legend()
        ax2.grid(True, alpha=0.3)

        # 各点にラベル表示
        for v, vel, i in zip(voltages, velocities, range(1, len(voltages) + 1)):
            ax2.annotate(
                f"Seg {i}\n({v}V, {vel:.4f}mm/s)",
                xy=(v, vel),
                xytext=(5, 5),
                textcoords="offset points",
                fontsize=8,
                alpha=0.7,
            )

    plt.tight_layout()
    plt.show()


if __name__ == "__main__":
    import sys

    if len(sys.argv) < 2:
        print("Usage: python analyze_displacement_velocity.py <base_path> [options]")
        print()
        print("Options:")
        print("  --auto                   自動検出モード（デフォルトは手動選択）")
        print("  --segments N             検出する区間数（デフォルト: 4）")
        print(
            "  --threshold T            自動検出時の変位最小値閾値 mm（デフォルト: 0.5）"
        )
        print(
            "  --reset-threshold T      リセット判定の変位閾値 mm（デフォルト: 0.05）"
        )
        print(
            "  --voltages V1,V2,V3,...  各セグメントの電圧値（カンマ区切り、例: 2,4,6,8）"
        )
        print(
            "  --column COL             変位データのカラム名（デフォルト: 'Disp.(mm)'）"
        )
        print(
            "  --trim-edges             両端の低速部分をトリミング（デフォルト: 有効）"
        )
        print("  --no-trim                両端をトリミングしない")
        print(
            "  --trim-ratio R           トリミング速度閾値（最大速度比、デフォルト: 0.3）"
        )
        print("  --output FILE.csv        結果を CSV に保存")
        print("  --no-plot                プロットを表示しない")
        print()
        print("Example:")
        print("  python analyze_displacement_velocity.py motor_calibration")
        print(
            "  python analyze_displacement_velocity.py motor_calibration --auto --threshold 0.5"
        )
        print(
            "  python analyze_displacement_velocity.py motor_calibration --auto --voltages 2,4,6,8"
        )
        print(
            "  python analyze_displacement_velocity.py motor_calibration --auto --no-trim"
        )
        sys.exit(1)

    base_path = sys.argv[1]

    # オプション解析
    auto_detect = "--auto" in sys.argv
    plot = "--no-plot" not in sys.argv

    n_segments = 4
    if "--segments" in sys.argv:
        idx = sys.argv.index("--segments")
        n_segments = int(sys.argv[idx + 1])

    threshold = 0.5
    if "--threshold" in sys.argv:
        idx = sys.argv.index("--threshold")
        threshold = float(sys.argv[idx + 1])

    reset_threshold = 0.05
    if "--reset-threshold" in sys.argv:
        idx = sys.argv.index("--reset-threshold")
        reset_threshold = float(sys.argv[idx + 1])

    voltages = None
    if "--voltages" in sys.argv:
        idx = sys.argv.index("--voltages")
        voltage_str = sys.argv[idx + 1]
        voltages = [float(v.strip()) for v in voltage_str.split(",")]
        print(f"Voltages specified: {voltages}")

    displacement_column = "Disp.(mm)"
    if "--column" in sys.argv:
        idx = sys.argv.index("--column")
        displacement_column = sys.argv[idx + 1]

    trim_edges = "--no-trim" not in sys.argv

    velocity_threshold_ratio = 0.3
    if "--trim-ratio" in sys.argv:
        idx = sys.argv.index("--trim-ratio")
        velocity_threshold_ratio = float(sys.argv[idx + 1])

    output_csv = None
    if "--output" in sys.argv:
        idx = sys.argv.index("--output")
        output_csv = sys.argv[idx + 1]

    # 実行
    try:
        results, voltage_reg = analyze_displacement_velocity(
            base_path=base_path,
            displacement_column=displacement_column,
            auto_detect=auto_detect,
            n_segments=n_segments,
            threshold=threshold,
            reset_threshold=reset_threshold,
            voltages=voltages,
            trim_edges=trim_edges,
            velocity_threshold_ratio=velocity_threshold_ratio,
            output_csv=output_csv,
            plot=plot,
        )

        print("\n" + "=" * 60)
        print("SEGMENT SUMMARY")
        print("=" * 60)
        print(results.to_string(index=False))

        if voltage_reg is not None:
            print("\n" + "=" * 60)
            print("VOLTAGE-VELOCITY REGRESSION SUMMARY")
            print("=" * 60)
            print(
                f"Equation: Velocity = {voltage_reg['voltage_slope_a']:.9f} * Voltage (through origin)"
            )
            print(f"R² = {voltage_reg['voltage_r_squared']:.6f}")

    except Exception as e:
        print(f"Error: {e}")
        import traceback

        traceback.print_exc()
        sys.exit(1)

"""
DigitShowDST せん断変位速度・RPM 時系列グラフ生成ツール

せん断変位から速度を計算し、RPMと併せて時系列プロットする。
制御ステップごとに分割表示可能。
"""

import matplotlib.pyplot as plt
import pandas as pd
import numpy as np
from pathlib import Path
from typing import Optional
from scipy.signal import savgol_filter

from load_data import read_all_files


def _get_times_ms_from_df(df: Optional[pd.DataFrame]) -> Optional[pd.Series]:
    """
    DataFrame から各行の時刻を Unix ミリ秒で返す。取得できなければ None を返す。

    Returns:
        pandas Series (same index as df) of int milliseconds since epoch, or None
    """
    if df is None or len(df.index) == 0:
        return None

    # DatetimeIndex
    if isinstance(df.index, pd.DatetimeIndex):
        try:
            times_ms = pd.to_datetime(df.index).astype("int64") // 1_000_000
            return pd.Series(times_ms, index=df.index)
        except Exception:
            pass

    # UnixTime(ms) 列
    if "UnixTime(ms)" in df.columns:
        try:
            s = pd.to_numeric(df["UnixTime(ms)"], errors="coerce")
            if s.isnull().all():
                raise Exception("cannot parse UnixTime(ms)")
            return pd.Series(s.astype("int64"), index=df.index)
        except Exception:
            try:
                times_ms = (
                    pd.to_datetime(df["UnixTime(ms)"], unit="ms").astype("int64")
                    // 1_000_000
                )
                return pd.Series(times_ms, index=df.index)
            except Exception:
                return None

    # 数値インデックス（秒とみなす -> ミリ秒に変換）
    try:
        idx_vals = pd.to_numeric(pd.Series(df.index), errors="coerce")
        if idx_vals.isnull().any():
            return None
        vals = (idx_vals.astype("float") * 1000).astype("int64")
        return pd.Series(vals.values, index=df.index)
    except Exception:
        return None


def _get_df_start_ts(df: Optional[pd.DataFrame]) -> Optional[pd.Timestamp]:
    """
    DataFrame から記録開始の UTC タイムスタンプを返す（pandas.Timestamp）。
    Returns None if 取得不可。
    """
    if df is None or len(df.index) == 0:
        return None
    if isinstance(df.index, pd.DatetimeIndex):
        try:
            return pd.to_datetime(df.index[0])
        except Exception:
            return None
    if "UnixTime(ms)" in df.columns:
        try:
            return pd.to_datetime(df["UnixTime(ms)"].iloc[0], unit="ms")
        except Exception:
            return None
    return None


def _compute_elapsed_seconds_and_start_iso(
    df: Optional[pd.DataFrame],
    timezone_str: Optional[str] = None,
) -> tuple[Optional[pd.Series], Optional[str]]:
    """
    DataFrame から経過秒配列と開始時刻(ISO8601)を推定して返す。

    ログデータは UTC で記録されていると想定し、指定タイムゾーンに変換して表示。
    """
    if df is None or len(df.index) == 0:
        return None, None

    if isinstance(df.index, pd.DatetimeIndex):
        start_ts_utc = pd.to_datetime(df.index[0])
        elapsed = (df.index - start_ts_utc).total_seconds()
        start_iso = _convert_utc_to_timezone_str(start_ts_utc, timezone_str)
        return pd.Series(elapsed, index=df.index), start_iso

    if "UnixTime(ms)" in df.columns:
        times_utc = pd.to_datetime(df["UnixTime(ms)"], unit="ms")
        start_ts_utc = pd.to_datetime(times_utc.iloc[0])
        elapsed = (times_utc - start_ts_utc).total_seconds()
        start_iso = _convert_utc_to_timezone_str(start_ts_utc, timezone_str)
        return pd.Series(elapsed, index=df.index), start_iso

    try:
        idx0 = float(df.index[0])
        elapsed = pd.Series([float(x) - idx0 for x in df.index], index=df.index)
        return elapsed, None
    except Exception:
        return None, None


def _convert_utc_to_timezone_str(
    utc_time: pd.Timestamp, timezone_str: Optional[str] = None
) -> str:
    """
    UTC 時刻を指定タイムゾーンに変換して ISO 8601 形式の文字列で返す。
    """
    utc_time = utc_time.floor("s")

    if timezone_str == "UTC":
        return utc_time.strftime("%Y-%m-%dT%H:%M:%SZ")
    elif timezone_str:
        try:
            import zoneinfo

            tzinfo = zoneinfo.ZoneInfo(timezone_str)
            utc_aware = utc_time.tz_localize("UTC")
            local_time = utc_aware.tz_convert(tzinfo)
            offset = local_time.utcoffset()
            if offset is not None:
                hours, remainder = divmod(int(offset.total_seconds()), 3600)
                mins = abs(remainder) // 60
                tz_str = f"{hours:+03d}:{mins:02d}"
            else:
                tz_str = "+00:00"
            return local_time.strftime("%Y-%m-%dT%H:%M:%S") + tz_str
        except Exception as e:
            print(f"警告: タイムゾーン変換エラー ({e})。システムロケールを使用します。")

    import datetime

    utc_aware = utc_time.tz_localize("UTC")
    local_tz = datetime.datetime.now().astimezone().tzinfo
    local_time = utc_aware.tz_convert(local_tz)
    offset = local_time.utcoffset()
    if offset is not None:
        hours, remainder = divmod(int(offset.total_seconds()), 3600)
        mins = abs(remainder) // 60
        tz_str = f"{hours:+03d}:{mins:02d}"
    else:
        tz_str = "+00:00"
    return local_time.strftime("%Y-%m-%dT%H:%M:%S") + tz_str


def _compute_control_change_events(
    df_out: Optional[pd.DataFrame],
    out_elapsed: Optional[pd.Series],
) -> list[tuple[float, int]]:
    """
    df_out の Cont_No__ から制御ステップ変化イベントを抽出。

    Returns:
        list of (elapsed_seconds, step_int)
    """
    if df_out is None or "Cont_No__" not in df_out.columns:
        return []

    out_times_ms = _get_times_ms_from_df(df_out)
    if out_times_ms is None:
        return []

    cont = pd.to_numeric(df_out["Cont_No__"], errors="coerce").round().astype("Int64")
    events_idx = []
    prev = None
    for i, val in enumerate(cont):
        if pd.isna(val):
            prev = val
            continue
        if prev is None or val != prev:
            events_idx.append(i)
        prev = val

    event_ms = out_times_ms.iloc[events_idx].tolist()
    event_steps = cont.iloc[events_idx].tolist()

    if out_elapsed is None:
        return []

    mapped: list[tuple[float, int]] = []
    for ms, step in zip(event_ms, event_steps):
        try:
            idx = (out_times_ms - ms).abs().idxmin()
            el = float(out_elapsed.loc[idx])
            mapped.append((el, int(step) if pd.notna(step) else -1))
        except Exception:
            continue
    return mapped


def _build_segments(
    elapsed_seconds: Optional[pd.Series],
    events: list[tuple[float, int]],
) -> Optional[list[tuple[float, float, int]]]:
    """
    変化イベント [(elapsed, step), ...] から (start, end, step) のセグメント配列を作成。
    """
    if elapsed_seconds is None or len(elapsed_seconds) == 0:
        return None
    if not events:
        return None

    events_sorted = sorted(events, key=lambda x: float(x[0]))

    try:
        t_end = float(pd.to_numeric(elapsed_seconds, errors="coerce").dropna().iloc[-1])
    except Exception:
        t_end = float(events_sorted[-1][0])

    segments: list[tuple[float, float, int]] = []
    for i, (st, step) in enumerate(events_sorted):
        st_f = float(st)
        if i + 1 < len(events_sorted):
            ed_f = float(events_sorted[i + 1][0])
        else:
            ed_f = t_end
        if ed_f < st_f:
            ed_f = st_f
        segments.append((st_f, ed_f, int(step)))
    return segments


def _label_segments(segments, df, timezone_str):
    """
    各セグメントに対応する開始時刻の ISO 表示を付加。
    """
    if not segments:
        return None
    start_ts = _get_df_start_ts(df)
    labeled = []
    for seg in segments:
        st, ed, step = seg
        start_iso = None
        if start_ts is not None:
            try:
                abs_start = start_ts + pd.Timedelta(seconds=float(st))
                start_iso = _convert_utc_to_timezone_str(abs_start, timezone_str)
            except Exception:
                start_iso = None
        labeled.append((st, ed, step, start_iso))
    return labeled


def _compute_shear_velocity(
    df_out: pd.DataFrame,
    elapsed_seconds: pd.Series,
) -> pd.Series:
    """
    せん断変位（パラメータ列 param[1]）から速度を計算する。

    Args:
        df_out: .out DataFrame (param[1] がせん断変位 mm)
        elapsed_seconds: 経過秒の Series

    Returns:
        せん断変位速度 (mm/min) の Series
    """
    # param[1] = せん断変位 (mm) は 0-based で 1列目
    disp_pos = 1
    if disp_pos >= len(df_out.columns):
        return pd.Series([np.nan] * len(df_out), index=df_out.index)

    disp = pd.to_numeric(df_out.iloc[:, disp_pos], errors="coerce")
    time_s = pd.to_numeric(elapsed_seconds, errors="coerce")

    # 中心差分で速度を計算（端点は前方/後方差分）
    velocity_mm_per_s = pd.Series([np.nan] * len(disp), index=disp.index)

    for i in range(len(disp)):
        if i == 0 and i + 1 < len(disp):
            # 前方差分
            dt = time_s.iloc[i + 1] - time_s.iloc[i]
            dd = disp.iloc[i + 1] - disp.iloc[i]
            if pd.notna(dt) and pd.notna(dd) and dt > 0:
                velocity_mm_per_s.iloc[i] = dd / dt
        elif i == len(disp) - 1 and i - 1 >= 0:
            # 後方差分
            dt = time_s.iloc[i] - time_s.iloc[i - 1]
            dd = disp.iloc[i] - disp.iloc[i - 1]
            if pd.notna(dt) and pd.notna(dd) and dt > 0:
                velocity_mm_per_s.iloc[i] = dd / dt
        elif 0 < i < len(disp) - 1:
            # 中心差分
            dt = time_s.iloc[i + 1] - time_s.iloc[i - 1]
            dd = disp.iloc[i + 1] - disp.iloc[i - 1]
            if pd.notna(dt) and pd.notna(dd) and dt > 0:
                velocity_mm_per_s.iloc[i] = dd / dt

    # mm/s → mm/min に変換
    velocity_mm_per_min = velocity_mm_per_s * 60.0

    return velocity_mm_per_min


def _apply_lowpass_filter(
    velocity: pd.Series,
    window_length: int = 51,
    polyorder: int = 3,
) -> pd.Series:
    """
    Savitzky-Golay フィルターを適用してノイズを除去する。

    Args:
        velocity: 元の速度データ
        window_length: フィルター窓のサイズ（奇数である必要がある）
        polyorder: 多項式の次数

    Returns:
        フィルター適用後の速度データ
    """
    # NaN を除外して有効なデータのみを抽出
    valid_mask = velocity.notna()
    if valid_mask.sum() < window_length:
        # データ点が少なすぎる場合はそのまま返す
        return velocity

    filtered = pd.Series([np.nan] * len(velocity), index=velocity.index)

    valid_values = velocity[valid_mask].values
    valid_indices = velocity[valid_mask].index

    # ウィンドウサイズをデータ点数に合わせて調整
    actual_window = min(window_length, len(valid_values))
    if actual_window % 2 == 0:
        actual_window -= 1
    if actual_window < polyorder + 2:
        actual_window = polyorder + 2
        if actual_window % 2 == 0:
            actual_window += 1

    if actual_window <= len(valid_values):
        try:
            filtered_values = savgol_filter(
                valid_values, actual_window, polyorder, mode="nearest"
            )
            filtered.loc[valid_indices] = filtered_values
        except Exception:
            # フィルター適用失敗時は元のデータを返す
            return velocity

    return filtered


def plot_velocity_and_rpm(
    base_path: str | Path,
    output_dir: Optional[str | Path] = None,
    timezone_str: Optional[str] = None,
):
    """
    せん断変位速度とモーターRPMの時系列グラフを生成。

    Args:
        base_path: ログファイルのベースパス（拡張子なし）
        output_dir: 出力先ディレクトリ（Noneの場合はbase_pathと同じディレクトリ）
        timezone_str: タイムゾーン文字列（例: "Asia/Tokyo"）
    """
    base_path = Path(base_path)
    if base_path.suffix:
        base_path = base_path.with_suffix("")

    if output_dir is None:
        output_dir = base_path.parent
    else:
        output_dir = Path(output_dir)
        output_dir.mkdir(parents=True, exist_ok=True)

    print(f"Reading files: {base_path.name}.*")
    df_dat, df_vlt, df_out = read_all_files(base_path)

    if df_out is None or df_out.empty:
        print("エラー: .out ファイルが見つかりません")
        return

    # 経過秒と開始時刻を計算
    out_elapsed, out_start = _compute_elapsed_seconds_and_start_iso(
        df_out, timezone_str
    )

    if out_elapsed is None:
        print("エラー: 時間情報を取得できません")
        return

    # 制御ステップ変化イベントを抽出
    events_out = _compute_control_change_events(df_out, out_elapsed)
    seg_out = _build_segments(out_elapsed, events_out)
    seg_out = _label_segments(seg_out, df_out, timezone_str)

    # せん断変位速度を計算
    print("計算中: せん断変位速度...")
    velocity = _compute_shear_velocity(df_out, out_elapsed)

    # RPMを取得（param[7]）
    rpm_pos = 7
    if rpm_pos < len(df_out.columns):
        rpm = pd.to_numeric(df_out.iloc[:, rpm_pos], errors="coerce")
    else:
        rpm = pd.Series([np.nan] * len(df_out), index=df_out.index)

    # せん断変位を取得（param[1]）
    disp_pos = 1
    if disp_pos < len(df_out.columns):
        displacement = pd.to_numeric(df_out.iloc[:, disp_pos], errors="coerce")
    else:
        displacement = pd.Series([np.nan] * len(df_out), index=df_out.index)

    # グラフ作成
    if seg_out:
        # ステップごとに分割（4段構成）
        n_cols = len(seg_out)
        fig, axes = plt.subplots(4, n_cols, figsize=(5 * n_cols, 13), squeeze=False)

        for j, seg in enumerate(seg_out):
            st, ed, step, start_iso = seg

            # セグメント範囲のデータを抽出
            mask = (out_elapsed >= st) & (out_elapsed <= ed)
            x_sub = out_elapsed.loc[mask] - st
            disp_sub = displacement.loc[mask]
            vel_sub = velocity.loc[mask]
            rpm_sub = rpm.loc[mask]

            # 第1段: 時間 - せん断変位
            ax0 = axes[0][j]
            if len(x_sub) > 0 and disp_sub.notna().any():
                valid = (~x_sub.isna()) & (~disp_sub.isna())
                ax0.plot(
                    x_sub[valid].values, disp_sub[valid].values, "b-", linewidth=1.2
                )
            else:
                ax0.text(
                    0.5,
                    0.5,
                    "No data",
                    ha="center",
                    va="center",
                    transform=ax0.transAxes,
                )
            ax0.set_xlabel("Time (s)")
            ax0.set_ylabel("Shear Displacement (mm)")
            if start_iso:
                ax0.set_title(f"Step {step}\n{start_iso}")
            else:
                ax0.set_title(f"Step {step}")
            ax0.set_xlim(left=0)
            ax0.grid(True, alpha=0.3)

            # 第2段: 時間 - せん断変位速度
            ax1 = axes[1][j]
            if len(x_sub) > 0 and vel_sub.notna().any():
                valid = (~x_sub.isna()) & (~vel_sub.isna())
                ax1.plot(
                    x_sub[valid].values,
                    vel_sub[valid].values,
                    "b-",
                    linewidth=0.8,
                    alpha=0.7,
                )
            else:
                ax1.text(
                    0.5,
                    0.5,
                    "No data",
                    ha="center",
                    va="center",
                    transform=ax1.transAxes,
                )
            ax1.set_xlabel("Time (s)")
            ax1.set_ylabel("Shear Velocity (mm/min)")
            ax1.set_xlim(left=0)
            ax1.grid(True, alpha=0.3)

            # 第3段: せん断変位速度のヒストグラム
            # 第3段: せん断変位速度のヒストグラム
            ax2 = axes[2][j]
            if vel_sub.notna().any():
                valid_vel = vel_sub.dropna()
                if len(valid_vel) > 0:
                    # 外れ値を除外するため、5-95パーセンタイルの範囲を計算
                    q05, q95 = valid_vel.quantile([0.05, 0.95])
                    filtered_vel = valid_vel[(valid_vel >= q05) & (valid_vel <= q95)]

                    if len(filtered_vel) > 0:
                        ax2.hist(
                            filtered_vel.values,
                            bins=30,
                            color="blue",
                            alpha=0.7,
                            edgecolor="black",
                        )
                        # 平均値、中央値、標準偏差を表示
                        mean_val = filtered_vel.mean()
                        median_val = filtered_vel.median()
                        std_val = filtered_vel.std()
                        ax2.axvline(
                            mean_val,
                            color="red",
                            linestyle="--",
                            linewidth=2,
                            label=f"Mean: {mean_val:.2f}",
                        )
                        ax2.axvline(
                            median_val,
                            color="green",
                            linestyle="-",
                            linewidth=2,
                            label=f"Median: {median_val:.2f}",
                        )
                        # ±1σ の範囲を表示
                        ax2.axvline(
                            mean_val - std_val,
                            color="gray",
                            linestyle=":",
                            linewidth=1.5,
                            alpha=0.7,
                        )
                        ax2.axvline(
                            mean_val + std_val,
                            color="gray",
                            linestyle=":",
                            linewidth=1.5,
                            alpha=0.7,
                            label=f"±1σ: {std_val:.2f}",
                        )
                        ax2.legend(fontsize=8)
                    else:
                        ax2.text(
                            0.5,
                            0.5,
                            "No data",
                            ha="center",
                            va="center",
                            transform=ax2.transAxes,
                        )
                else:
                    ax2.text(
                        0.5,
                        0.5,
                        "No data",
                        ha="center",
                        va="center",
                        transform=ax2.transAxes,
                    )
            else:
                ax2.text(
                    0.5,
                    0.5,
                    "No data",
                    ha="center",
                    va="center",
                    transform=ax2.transAxes,
                )
            ax2.set_xlabel("Shear Velocity (mm/min)")
            ax2.set_ylabel("Frequency")
            ax2.grid(True, alpha=0.3, axis="y")

            # 第4段: 時間 - モーターRPM
            ax3 = axes[3][j]
            if rpm_sub.notna().any():
                valid = (~x_sub.isna()) & (~rpm_sub.isna())
                ax3.plot(
                    x_sub[valid].values, rpm_sub[valid].values, "r-", linewidth=1.2
                )
            else:
                ax3.text(
                    0.5,
                    0.5,
                    "No data",
                    ha="center",
                    va="center",
                    transform=ax3.transAxes,
                )
            ax3.set_xlabel("Time (s)")
            ax3.set_ylabel("RPM")
            ax3.set_xlim(left=0)
            ax3.grid(True, alpha=0.3)

        plt.tight_layout()
    else:
        # 分割なし（4段構成）
        fig, axes = plt.subplots(4, 1, figsize=(10, 13))

        # 第1段: 時間 - せん断変位
        if displacement.notna().any():
            valid = (~out_elapsed.isna()) & (~displacement.isna())
            axes[0].plot(
                out_elapsed[valid].values,
                displacement[valid].values,
                "b-",
                linewidth=1.2,
            )
        else:
            axes[0].text(
                0.5,
                0.5,
                "No data",
                ha="center",
                va="center",
                transform=axes[0].transAxes,
            )
        axes[0].set_xlabel("Time (s)")
        axes[0].set_ylabel("Shear Displacement (mm)")
        axes[0].set_title("Shear Displacement vs Time")
        axes[0].set_xlim(left=0)
        axes[0].grid(True, alpha=0.3)

        # 第2段: 時間 - せん断変位速度
        if velocity.notna().any():
            valid = (~out_elapsed.isna()) & (~velocity.isna())
            axes[1].plot(
                out_elapsed[valid].values,
                velocity[valid].values,
                "b-",
                linewidth=0.8,
                alpha=0.7,
            )
        else:
            axes[1].text(
                0.5,
                0.5,
                "No data",
                ha="center",
                va="center",
                transform=axes[1].transAxes,
            )
        axes[1].set_xlabel("Time (s)")
        axes[1].set_ylabel("Shear Velocity (mm/min)")
        axes[1].set_title("Shear Velocity vs Time")
        axes[1].set_xlim(left=0)
        axes[1].grid(True, alpha=0.3)

        # 第3段: せん断変位速度のヒストグラム
        if velocity.notna().any():
            valid_vel = velocity.dropna()
            if len(valid_vel) > 0:
                # 外れ値を除外するため、5-95パーセンタイルの範囲を計算
                q05, q95 = valid_vel.quantile([0.05, 0.95])
                filtered_vel = valid_vel[(valid_vel >= q05) & (valid_vel <= q95)]

                if len(filtered_vel) > 0:
                    axes[2].hist(
                        filtered_vel.values,
                        bins=50,
                        color="blue",
                        alpha=0.7,
                        edgecolor="black",
                    )
                    # 平均値、中央値、標準偏差を表示
                    mean_val = filtered_vel.mean()
                    median_val = filtered_vel.median()
                    std_val = filtered_vel.std()
                    axes[2].axvline(
                        mean_val,
                        color="red",
                        linestyle="--",
                        linewidth=2,
                        label=f"Mean: {mean_val:.2f}",
                    )
                    axes[2].axvline(
                        median_val,
                        color="green",
                        linestyle="-",
                        linewidth=2,
                        label=f"Median: {median_val:.2f}",
                    )
                    # ±1σ の範囲を表示
                    axes[2].axvline(
                        mean_val - std_val,
                        color="gray",
                        linestyle=":",
                        linewidth=1.5,
                        alpha=0.7,
                    )
                    axes[2].axvline(
                        mean_val + std_val,
                        color="gray",
                        linestyle=":",
                        linewidth=1.5,
                        alpha=0.7,
                        label=f"±1σ: {std_val:.2f}",
                    )
                    axes[2].legend()
                else:
                    axes[2].text(
                        0.5,
                        0.5,
                        "No data",
                        ha="center",
                        va="center",
                        transform=axes[2].transAxes,
                    )
            else:
                axes[2].text(
                    0.5,
                    0.5,
                    "No data",
                    ha="center",
                    va="center",
                    transform=axes[2].transAxes,
                )
        else:
            axes[2].text(
                0.5,
                0.5,
                "No data",
                ha="center",
                va="center",
                transform=axes[2].transAxes,
            )
        axes[2].set_xlabel("Shear Velocity (mm/min)")
        axes[2].set_ylabel("Frequency")
        axes[2].set_title("Shear Velocity Distribution")
        axes[2].grid(True, alpha=0.3, axis="y")

        # 第4段: 時間 - モーターRPM
        if rpm.notna().any():
            valid = (~out_elapsed.isna()) & (~rpm.isna())
            axes[3].plot(
                out_elapsed[valid].values, rpm[valid].values, "r-", linewidth=1.2
            )
        else:
            axes[3].text(
                0.5,
                0.5,
                "No data",
                ha="center",
                va="center",
                transform=axes[3].transAxes,
            )
        axes[3].set_xlabel("Time (s)")
        axes[3].set_ylabel("RPM")
        axes[3].set_title("Motor RPM vs Time")
        axes[3].set_xlim(left=0)
        axes[3].grid(True, alpha=0.3)

        if out_start:
            fig.suptitle(f"Start at {out_start}", fontsize=9)
            plt.tight_layout(rect=[0, 0, 1, 0.97])
        else:
            plt.tight_layout()

    # 保存
    output_path = output_dir / f"{base_path.name}_velocity_rpm.png"
    plt.savefig(output_path, dpi=300, bbox_inches="tight")
    print(f"保存完了: {output_path}")
    plt.close()


if __name__ == "__main__":
    import argparse

    parser = argparse.ArgumentParser(
        description="せん断変位速度とモーターRPMの時系列グラフを生成"
    )
    parser.add_argument(
        "base_path",
        help="ログファイルのベースパス（拡張子なし）. 例: logs/2025-10-28_test",
    )
    parser.add_argument(
        "output_dir",
        nargs="?",
        default=None,
        help="出力先ディレクトリ（指定なしの場合はbase_pathと同じディレクトリ）",
    )
    parser.add_argument(
        "-tz",
        "--timezone",
        dest="timezone",
        default=None,
        help="タイムゾーン（例: 'Asia/Tokyo', 'UTC'）。指定なしの場合はシステムロケール",
    )

    args = parser.parse_args()

    plot_velocity_and_rpm(args.base_path, args.output_dir, timezone_str=args.timezone)

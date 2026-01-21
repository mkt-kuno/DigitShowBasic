"""
DigitShowDST ログファイル可視化ツール

.dat/.vlt/.out から主要なグラフを生成する。
"""

import matplotlib.pyplot as plt
import pandas as pd
from pathlib import Path
from typing import Optional


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
            # 既にミリ秒として格納されていれば Series を返す
            s = pd.to_numeric(df["UnixTime(ms)"], errors="coerce")
            if s.isnull().all():
                raise Exception("cannot parse UnixTime(ms)")
            return pd.Series(s.astype("int64"), index=df.index)
        except Exception:
            # 可能な変換
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
        # index を秒とみなして ms に変換
        vals = (idx_vals.astype("float") * 1000).astype("int64")
        return pd.Series(vals.values, index=df.index)
    except Exception:
        return None


def _compute_control_change_events(
    df_out: Optional[pd.DataFrame],
    df_dat: Optional[pd.DataFrame],
    df_vlt: Optional[pd.DataFrame],
    dat_elapsed: Optional[pd.Series],
    vlt_elapsed: Optional[pd.Series],
    out_elapsed: Optional[pd.Series],
):
    """
    df_out の Control_No から制御ステップ変化イベントを抽出し、各 DataFrame 用の経過秒リストに変換して返す。

    Returns:
        tuple: (events_dat, events_vlt, events_out)
        各 events_* は (elapsed_seconds, step_int) のリスト。elapsed_seconds が None のイベントは除外される。
    """
    if df_out is None or "Control_No" not in df_out.columns:
        return [], [], []

    out_times_ms = _get_times_ms_from_df(df_out)
    if out_times_ms is None:
        # 位置が不明 -> 変化は取れるが他のデータセットへのマッピング不可
        return [], [], []

    # 丸めて整数の制御ステップとして扱う
    cont = pd.to_numeric(df_out["Control_No"], errors="coerce").round().astype("Int64")
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

    # マッピング用の時間シリーズを取得
    dat_times_ms = _get_times_ms_from_df(df_dat)
    vlt_times_ms = _get_times_ms_from_df(df_vlt)

    def _map_events_to_elapsed(
        times_ms_series: Optional[pd.Series],
        elapsed_series: Optional[pd.Series],
    ) -> list[tuple[float, int]]:
        """
        out の変化イベント（event_ms, step）は既に df_out 時刻で求めてある。
        任意のデータセットの (times_ms_series, elapsed_series) に対応づけ、
        それぞれの elapsed 秒でイベント一覧 [(elapsed, step), ...] を返す。
        """
        if times_ms_series is None or elapsed_series is None:
            return []
        mapped: list[tuple[float, int]] = []
        for ms, step in zip(event_ms, event_steps):
            try:
                # もっとも近い時刻の行を探す
                idx = (times_ms_series - ms).abs().idxmin()
                el = float(elapsed_series.loc[idx])
                mapped.append((el, int(step) if pd.notna(step) else -1))
            except Exception:
                continue
        return mapped

    events_dat = _map_events_to_elapsed(dat_times_ms, dat_elapsed)
    events_vlt = _map_events_to_elapsed(vlt_times_ms, vlt_elapsed)
    events_out = _map_events_to_elapsed(out_times_ms, out_elapsed)

    return events_dat, events_vlt, events_out


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

    優先順:
    1) DatetimeIndex（UTC と想定）
    2) 'UnixTime(ms)' 列（UTC と想定）
    3) 数値 index を秒とみなす（開始を 0 にシフト）
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


def _build_segments(
    elapsed_seconds: Optional[pd.Series],
    events: list[tuple[float, int]],
) -> Optional[list[tuple[float, float, int]]]:
    """
    変化イベント [(elapsed, step), ...] から (start, end, step) のセグメント配列を作成。

    end は次イベントの elapsed、最後の end は elapsed_seconds の最大値（なければ start）。
    """
    if elapsed_seconds is None or len(elapsed_seconds) == 0:
        return None
    if not events:
        return None

    # 時刻で昇順ソート
    events_sorted = sorted(events, key=lambda x: float(x[0]))

    # 記録の最終時刻
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
        # ガード: end < start にならないように
        if ed_f < st_f:
            ed_f = st_f
        segments.append((st_f, ed_f, int(step)))
    return segments


def plot_stress_strain(
    save_path: Optional[str | Path] = None,
    control_segments: Optional[list[tuple[float, float, int]]] = None,
    df_out: Optional[pd.DataFrame] = None,
    out_elapsed_seconds: Optional[pd.Series] = None,
):
    """
    .out（計算パラメータ）だけを使って stress-strain を描画する。
    上段: tau (kPa) vs Shear Displacement (mm)
    下段: sigma (kPa) vs Vertical Average Displacement (mm)
    """
    if df_out is None or df_out.empty:
        fig, ax = plt.subplots(1, 1, figsize=(6, 4))
        ax.text(
            0.5, 0.5, ".out not found", ha="center", va="center", transform=ax.transAxes
        )
        if save_path:
            plt.savefig(save_path, dpi=300, bbox_inches="tight")
            print(f"Saved: {save_path}")
        else:
            plt.show()
        return

    # 列選択（plot_parameters と同じ方式：位置ベース）
    # _out.tsv の列構成: [UnixTime(ms), param[0], param[1], ..., param[15], Control_No]
    # param[0] = tau (1-based で 2列目)
    # param[1] = shear disp. (1-based で 3列目)
    # param[2] = sigma (1-based で 4列目)
    # param[3] = V-ave-disp. (1-based で 5列目)

    tau_pos = 0  # param[0] = tau は 0-based で 0列目
    disp_pos = 1  # param[1] = shear disp. は 0-based で 1列目
    sigma_pos = 2  # param[2] = sigma は 0-based で 2列目
    v_ave_pos = 3  # param[3] = V-ave-disp. は 0-based で 3列目

    tau_s = None
    sigma_s = None
    disp_s = None
    v_ave_s = None

    if tau_pos < len(df_out.columns):
        tau_s = pd.to_numeric(df_out.iloc[:, tau_pos], errors="coerce")
    if sigma_pos < len(df_out.columns):
        sigma_s = pd.to_numeric(df_out.iloc[:, sigma_pos], errors="coerce")
    if disp_pos < len(df_out.columns):
        disp_s = pd.to_numeric(df_out.iloc[:, disp_pos], errors="coerce")
    if v_ave_pos < len(df_out.columns):
        v_ave_s = pd.to_numeric(df_out.iloc[:, v_ave_pos], errors="coerce")

    if control_segments:
        n_cols = len(control_segments)
        fig, axes = plt.subplots(2, n_cols, figsize=(5.0 * n_cols, 7.0), squeeze=False)
        for j, seg in enumerate(control_segments):
            if len(seg) >= 3:
                st, ed, step = seg[0], seg[1], seg[2]
            else:
                st, ed, step = seg
            start_iso = seg[3] if len(seg) > 3 else None

            if out_elapsed_seconds is not None:
                mask = (out_elapsed_seconds >= st) & (out_elapsed_seconds <= ed)
                x_shear_sub = disp_s.loc[mask] if disp_s is not None else None
                x_v_ave_sub = v_ave_s.loc[mask] if v_ave_s is not None else None
                tau_sub = tau_s.loc[mask] if tau_s is not None else None
                sig_sub = sigma_s.loc[mask] if sigma_s is not None else None
            else:
                x_shear_sub = disp_s
                x_v_ave_sub = v_ave_s
                tau_sub = tau_s
                sig_sub = sigma_s

            for row, x_sub, y_sub, x_label, y_label in [
                (0, x_shear_sub, tau_sub, "Shear Displacement (mm)", "tau (kPa)"),
                (
                    1,
                    x_v_ave_sub,
                    sig_sub,
                    "Vertical Ave. Displacement (mm)",
                    "sigma (kPa)",
                ),
            ]:
                ax = axes[row][j]
                if (
                    x_sub is not None
                    and y_sub is not None
                    and len(x_sub) > 0
                    and len(y_sub) > 0
                ):
                    xv = pd.to_numeric(x_sub, errors="coerce")
                    yv = pd.to_numeric(y_sub, errors="coerce")
                    valid = (~xv.isna()) & (~yv.isna())
                    if valid.any():
                        ax.plot(xv[valid].values, yv[valid].values, "b-", linewidth=1.2)
                    else:
                        ax.text(
                            0.5,
                            0.5,
                            "No data",
                            ha="center",
                            va="center",
                            transform=ax.transAxes,
                        )
                else:
                    ax.text(
                        0.5,
                        0.5,
                        "No data",
                        ha="center",
                        va="center",
                        transform=ax.transAxes,
                    )
                ax.set_xlabel(x_label)
                ax.set_ylabel(y_label)
                if row == 0:
                    if start_iso:
                        ax.set_title(f"Step {step}\n{start_iso}")
                    else:
                        ax.set_title(f"Step {step}")
                ax.grid(True, alpha=0.3)
        plt.tight_layout()
    else:
        fig, axes = plt.subplots(2, 1, figsize=(8, 7))
        for ax, x_all, y_all, x_label, y_label in [
            (axes[0], disp_s, tau_s, "Shear Displacement (mm)", "tau (kPa)"),
            (
                axes[1],
                v_ave_s,
                sigma_s,
                "Vertical Ave. Displacement (mm)",
                "sigma (kPa)",
            ),
        ]:
            if x_all is not None and y_all is not None:
                xv = pd.to_numeric(x_all, errors="coerce")
                yv = pd.to_numeric(y_all, errors="coerce")
                m = min(len(xv), len(yv))
                xv = xv.iloc[:m]
                yv = yv.iloc[:m]
                valid = (~xv.isna()) & (~yv.isna())
                if valid.any():
                    ax.plot(xv[valid].values, yv[valid].values, "b-", linewidth=1.2)
                else:
                    ax.text(
                        0.5,
                        0.5,
                        "No data",
                        ha="center",
                        va="center",
                        transform=ax.transAxes,
                    )
            else:
                ax.text(
                    0.5,
                    0.5,
                    "No data",
                    ha="center",
                    va="center",
                    transform=ax.transAxes,
                )
            ax.set_xlabel(x_label)
            ax.set_ylabel(y_label)
            ax.grid(True, alpha=0.3)
        plt.tight_layout()

    if save_path:
        plt.savefig(save_path, dpi=300, bbox_inches="tight")
        print(f"Saved: {save_path}")
    else:
        plt.show()


def plot_time_series(
    df_dat: pd.DataFrame,
    columns: list[str],
    save_path: Optional[str | Path] = None,
    elapsed_seconds: Optional[pd.Series] = None,
    start_time_iso: Optional[str] = None,
    control_segments: Optional[list[tuple[float, float, int]]] = None,
):
    """
    指定された列の時系列プロット（control_events を受け取るオーバーロード）。
    """
    # control_segments が与えられていれば、行 = 列ごとのグラフ種類、列 = ステップ
    if control_segments:
        n_rows = len(columns)
        n_cols = len(control_segments)
        fig, axes = plt.subplots(
            n_rows, n_cols, figsize=(5 * n_cols, 3 * n_rows), squeeze=False
        )
        for i, col in enumerate(columns):
            for j, seg in enumerate(control_segments):
                if len(seg) >= 3:
                    st, ed, step = seg[0], seg[1], seg[2]
                else:
                    st, ed, step = seg
                start_iso = seg[3] if len(seg) > 3 else None
                ax = axes[i][j]
                if elapsed_seconds is not None:
                    mask = (elapsed_seconds >= st) & (elapsed_seconds <= ed)
                    sub = df_dat.loc[mask]
                    x_sub = elapsed_seconds.loc[mask] - st
                else:
                    sub = df_dat
                    x_sub = df_dat.index

                if col in sub.columns and len(sub) > 0:
                    ax.plot(x_sub, sub[col], linewidth=1.2)
                    ax.set_xlim(left=0)
                    ax.set_xlabel("Time (s)")
                    ax.set_ylabel(col)
                else:
                    ax.text(
                        0.5,
                        0.5,
                        "No data",
                        ha="center",
                        va="center",
                        transform=ax.transAxes,
                    )
                # 列見出しは上段のみで表示（重複を避ける）
                if i == 0:
                    if start_iso:
                        ax.set_title(f"Step {step}\n{start_iso}")
                    else:
                        ax.set_title(f"Step {step}")
                ax.grid(True, alpha=0.3)
        plt.tight_layout()
    else:
        n_cols = len(columns)
        fig, axes = plt.subplots(n_cols, 1, figsize=(10, 3 * n_cols))
        if n_cols == 1:
            axes = [axes]
        for ax, col in zip(axes, columns):
            if col in df_dat.columns:
                x = elapsed_seconds if elapsed_seconds is not None else df_dat.index
                ax.plot(x, df_dat[col], linewidth=1.5)
                ax.set_xlabel("Time (s)", fontsize=11)
                ax.set_ylabel(col, fontsize=11)
                ax.set_title(f"{col} vs Time", fontsize=12, fontweight="bold")
                ax.set_xlim(left=0)
                ax.grid(True, alpha=0.3)
            else:
                ax.text(
                    0.5,
                    0.5,
                    f'Column "{col}" not found',
                    ha="center",
                    va="center",
                    transform=ax.transAxes,
                )

        # セグメント表示では上部の記録開始時刻は表示しない
        if control_segments is None:
            if start_time_iso:
                fig.suptitle(f"Start at {start_time_iso}", fontsize=9)
                plt.tight_layout(rect=[0, 0, 1, 0.97])
            else:
                plt.tight_layout()
        else:
            plt.tight_layout()

    if save_path:
        plt.savefig(save_path, dpi=300, bbox_inches="tight")
        print(f"Saved: {save_path}")
    else:
        plt.show()


def plot_voltage_channels(
    df_vlt: pd.DataFrame,
    channels: Optional[list[int]] = None,
    save_path: Optional[str | Path] = None,
    elapsed_seconds: Optional[pd.Series] = None,
    start_time_iso: Optional[str] = None,
    control_events: Optional[list[tuple[float, int]]] = None,
):
    """
    生電圧チャンネルの時系列プロット

    Args:
        df_vlt: .vlt ファイルから読み込んだ DataFrame
        channels: プロットするチャンネル番号のリスト（例: [0, 1, 2]）。None の場合は全チャンネル
        save_path: 保存先パス（Noneの場合は表示のみ）
    """
    if channels is None:
        # 主要チャンネルをプロット（CH00～CH06）
        channels = list(range(7))

    # control_events が与えられていれば、行 = チャンネル、列 = ステップ
    # ここでは control_events をセグメントに変換する呼び出し元 create_summary_report が行うため、
    # control_events は (start,end,step) のリスト相当を想定する
    control_segments = None
    if control_events:
        # もし (elapsed,step) の形式で渡された場合にも対応しておく
        if len(control_events) > 0 and len(control_events[0]) == 2:
            # 変換不可 -> segmentation は実行時に行うためここは無視
            control_segments = None
        else:
            control_segments = control_events

    if control_segments:
        n_rows = len(channels)
        n_cols = len(control_segments)
        fig, axes = plt.subplots(
            n_rows, n_cols, figsize=(4 * n_cols, 2.5 * n_rows), squeeze=False
        )
        for i, ch in enumerate(channels):
            col_name = f"CH{ch:02d}_(V)"
            for j, seg in enumerate(control_segments):
                # seg may be (start,end,step) or (start,end,step,start_iso)
                if len(seg) >= 3:
                    st, ed, step = seg[0], seg[1], seg[2]
                else:
                    st, ed, step = seg
                start_iso = seg[3] if len(seg) > 3 else None
                ax = axes[i][j]
                if col_name in df_vlt.columns:
                    if elapsed_seconds is not None:
                        mask = (elapsed_seconds >= st) & (elapsed_seconds <= ed)
                        sub = df_vlt.loc[mask]
                        x_sub = elapsed_seconds.loc[mask] - st
                    else:
                        sub = df_vlt
                        x_sub = df_vlt.index

                    if len(sub) > 0:
                        ax.plot(x_sub, sub[col_name], linewidth=1.0)
                    else:
                        ax.text(
                            0.5,
                            0.5,
                            "No data",
                            ha="center",
                            va="center",
                            transform=ax.transAxes,
                        )
                else:
                    ax.text(
                        0.5,
                        0.5,
                        f"Channel {col_name} not found",
                        ha="center",
                        va="center",
                        transform=ax.transAxes,
                    )
                ax.set_xlabel("Time (s)")
                ax.set_ylabel(f"CH{ch:02d} (V)")
                # 列ごとのステップ表記は上段にだけ表示して開始時刻を含める
                if i == 0:
                    if start_iso:
                        ax.set_title(f"Step {step}\n{start_iso}")
                    else:
                        ax.set_title(f"Step {step}")
                # 時間軸は各セグメントで 0 始まりに
                try:
                    ax.set_xlim(left=0)
                except Exception:
                    pass
                ax.grid(True, alpha=0.3)
        plt.tight_layout()
    else:
        # 従来の表示（セグメントなし）
        n_channels = len(channels)
        fig, axes = plt.subplots(n_channels, 1, figsize=(10, 2.5 * n_channels))
        if n_channels == 1:
            axes = [axes]

        for ax, ch in zip(axes, channels):
            col_name = f"CH{ch:02d}_(V)"
            if col_name in df_vlt.columns:
                x = elapsed_seconds if elapsed_seconds is not None else df_vlt.index
                ax.plot(x, df_vlt[col_name], linewidth=1.0)
                ax.set_xlabel("Time (s)", fontsize=10)
                ax.set_ylabel(f"CH{ch:02d} (V)")
                ax.set_xlim(left=0)
                ax.grid(True, alpha=0.3)
            else:
                ax.text(
                    0.5,
                    0.5,
                    f"Channel {col_name} not found",
                    ha="center",
                    va="center",
                    transform=ax.transAxes,
                )

        if start_time_iso:
            fig.suptitle(f"Start at {start_time_iso}", fontsize=9)
            plt.tight_layout(rect=[0, 0, 1, 0.97])
        else:
            plt.tight_layout()

    if save_path:
        plt.savefig(save_path, dpi=300, bbox_inches="tight")
        print(f"Saved: {save_path}")
    else:
        plt.show()


def plot_parameters(
    df_out: pd.DataFrame,
    df_dat: Optional[pd.DataFrame] = None,
    save_path: Optional[str | Path] = None,
    elapsed_seconds: Optional[pd.Series] = None,
    start_time_iso: Optional[str] = None,
    control_segments: Optional[list[tuple[float, float, int]]] = None,
):
    """
    .out ファイルから主要パラメータをプロット。control_segments が与えられれば
    列方向にステップごとの分割を行い、行方向にパラメータ種別を並べる。

    df_dat が与えられた場合、前面/背面の鉛直変位も追加でプロットする。
    """
    # We will plot a selection of calculated parameter columns by their positional mapping in the _out.tsv
    # Per spec, column layout is: [Time, param[0], param[1], ..., param[15]]
    # param mapping after fix:
    # [0]=tau, [1]=shear disp., [2]=sigma, [3]=V-ave-disp., [4]=ev_diff/2,
    # [5]=Front friction, [6]=Rear friction, [7]=RPM,
    # [8]=Front EP(kPa), [9]=Rear EP(kPa), [10]=RPM(V), [11]=Front EP(V), [12]=Rear EP(V)
    # User-requested parameter indices: 0,1,2,3,4,5,6,7,8,9 (excluding voltage readings 10,11,12)
    # Additionally, if df_dat is provided, add front/rear vertical displacements after ev_diff/2
    # Base positions: 0=tau, 1=shear disp., 2=sigma, 3=V-ave-disp., 4=ev_diff/2
    requested_positions = [0, 1, 2, 3, 4]

    # Add front/rear vertical displacements if df_dat is available (after ev_diff/2)
    # These come from the .tsv (physical) file, not from _out.tsv
    add_front_rear_disp = df_dat is not None and not df_dat.empty
    if add_front_rear_disp:
        requested_positions.extend([100, 101])  # Use special indices for df_dat columns

    # Add remaining parameters
    requested_positions.extend([5, 6, 7, 8, 9])

    # Build column names based on positions if available in df_out.columns
    col_names = []
    col_sources = []  # Track whether column comes from df_out or df_dat
    for pos in requested_positions:
        if pos == 100:  # Front vertical disp from df_dat
            col_names.append("V-front-disp._(mm)")
            col_sources.append("dat")
        elif pos == 101:  # Rear vertical disp from df_dat
            col_names.append("V-rear-disp._(mm)")
            col_sources.append("dat")
        elif pos < len(df_out.columns):
            col_names.append(df_out.columns[pos])
            col_sources.append("out")
        else:
            col_names.append(None)
            col_sources.append("out")

    # Semantic labels per documentation (use these instead of possibly-misleading header names)
    semantic_map = {
        0: "tau (kPa)",
        1: "Shear disp. (mm)",
        2: "sigma (kPa)",
        3: "V-ave-disp. (mm)",
        4: "ev_diff/2 (mm)",
        5: "Front friction (N)",
        6: "Rear friction (N)",
        7: "RPM",
        8: "Front EP (kPa)",
        9: "Rear EP (kPa)",
        10: "RPM(V)",
        11: "Front EP(V)",
        12: "Rear EP(V)",
        100: "V-front-disp. (mm)",
        101: "V-rear-disp. (mm)",
    }
    friendly_labels = [
        semantic_map.get(pos, (col if col is not None else f"col{pos}"))
        for col, pos in zip(col_names, requested_positions)
    ]

    if control_segments:
        n_rows = len(col_names)
        n_cols = len(control_segments)
        fig, axes = plt.subplots(
            n_rows, n_cols, figsize=(4 * n_cols, 2.5 * n_rows), squeeze=False
        )
        for j, seg in enumerate(control_segments):
            if len(seg) >= 3:
                st, ed, step = seg[0], seg[1], seg[2]
            else:
                st, ed, step = seg
            start_iso = seg[3] if len(seg) > 3 else None
            # subset by elapsed_seconds if available
            if elapsed_seconds is not None:
                mask = (elapsed_seconds >= st) & (elapsed_seconds <= ed)
                sub = df_out.loc[mask]
            else:
                sub = df_out

            for i, (cname, col_source) in enumerate(zip(col_names, col_sources)):
                ax = axes[i][j]

                # Determine which DataFrame to use (df_out or df_dat)
                source_df = df_dat if col_source == "dat" else df_out

                if (
                    source_df is not None
                    and cname is not None
                    and cname in source_df.columns
                    and len(source_df) > 0
                ):
                    # Align x and y by index to avoid length mismatch
                    if elapsed_seconds is not None:
                        # select by boolean mask to preserve duplicates/index order
                        try:
                            x_sub = elapsed_seconds.loc[mask]
                        except Exception:
                            # fallback: align by position
                            x_sub = pd.Series(elapsed_seconds.values, index=sub.index)
                        if st is not None:
                            x_sub = x_sub - st
                    else:
                        # use DataFrame index (may be numeric)
                        x_sub = pd.Series(list(sub.index), index=sub.index)

                    y_sub = source_df[cname]
                    # drop any pairs with NaN in either axis
                    valid_mask = (~x_sub.isna()) & (
                        ~pd.to_numeric(y_sub, errors="coerce").isna()
                    )
                    x_plot = x_sub.loc[valid_mask]
                    y_plot = y_sub.loc[valid_mask]
                    if len(x_plot) > 0 and len(y_plot) > 0:
                        ax.plot(x_plot.values, y_plot.values, linewidth=1.0)
                    else:
                        ax.text(
                            0.5,
                            0.5,
                            "No data",
                            ha="center",
                            va="center",
                            transform=ax.transAxes,
                        )
                else:
                    ax.text(
                        0.5,
                        0.5,
                        "No data",
                        ha="center",
                        va="center",
                        transform=ax.transAxes,
                    )
                # labels
                if i == 0:
                    if start_iso:
                        ax.set_title(f"Step {step}\n{start_iso}")
                    else:
                        ax.set_title(f"Step {step}")
                ax.set_xlabel("Time (s)")
                try:
                    ax.set_xlim(left=0)
                except Exception:
                    pass
                label = friendly_labels[i]
                ax.set_ylabel(label)
                ax.grid(True, alpha=0.3)
        plt.tight_layout()
    else:
        # Non-segmented: one column per parameter (vertical stack)
        n_rows = len(col_names)
        fig, axes = plt.subplots(n_rows, 1, figsize=(10, 2.5 * n_rows))
        if n_rows == 1:
            axes = [axes]
        for ax, cname, col_source, pos, flabel in zip(
            axes, col_names, col_sources, requested_positions, friendly_labels
        ):
            # Determine which DataFrame to use (df_out or df_dat)
            source_df = df_dat if col_source == "dat" else df_out

            if (
                source_df is not None
                and cname is not None
                and cname in source_df.columns
            ):
                # Align x to source_df by position when possible to avoid reindex on duplicates
                if elapsed_seconds is not None and len(elapsed_seconds) == len(
                    source_df
                ):
                    x = pd.Series(elapsed_seconds.values, index=source_df.index)
                elif elapsed_seconds is not None:
                    # lengths differ: try to select by index but protect against duplicate-label reindex
                    try:
                        x = elapsed_seconds.reindex(source_df.index)
                    except ValueError:
                        x = pd.Series(list(source_df.index), index=source_df.index)
                else:
                    x = pd.Series(list(source_df.index), index=source_df.index)

                y = source_df[cname]
                valid_mask = (~x.isna()) & (~pd.to_numeric(y, errors="coerce").isna())
                x_plot = x.loc[valid_mask]
                y_plot = y.loc[valid_mask]
                if len(x_plot) > 0 and len(y_plot) > 0:
                    ax.plot(x_plot.values, y_plot.values, linewidth=1.2)
                    ax.set_xlabel("Time (s)", fontsize=10)
                    ax.set_ylabel(flabel, fontsize=10)
                    ax.set_title(f"{flabel} (column {pos})", fontsize=11)
                    try:
                        ax.set_xlim(left=0)
                    except Exception:
                        pass
                    ax.grid(True, alpha=0.3)
                else:
                    ax.text(
                        0.5,
                        0.5,
                        "No data",
                        ha="center",
                        va="center",
                        transform=ax.transAxes,
                    )
            else:
                ax.text(
                    0.5,
                    0.5,
                    f"Column at pos {pos} not found",
                    ha="center",
                    va="center",
                    transform=ax.transAxes,
                )

        if start_time_iso:
            fig.suptitle(f"Start at {start_time_iso}", fontsize=9)
            plt.tight_layout(rect=[0, 0, 1, 0.97])
        else:
            plt.tight_layout()

    if save_path:
        plt.savefig(save_path, dpi=300, bbox_inches="tight")
        print(f"Saved: {save_path}")
    else:
        plt.show()


def create_summary_report(
    base_path: str | Path,
    output_dir: Optional[str | Path] = None,
    timezone_str: Optional[str] = None,
):
    """
    .dat/.vlt/.out から総合レポート画像を生成

    Args:
        base_path: ログファイルのベースパス（拡張子なし）
        output_dir: 出力先ディレクトリ（Noneの場合はbase_pathと同じディレクトリ）
        timezone_str: タイムゾーン文字列（例: "Asia/Tokyo"）。None の場合はシステムロケール
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

    # 開始時刻と経過秒の推定（dat -> vlt -> out の優先順）
    dat_elapsed, dat_start = _compute_elapsed_seconds_and_start_iso(
        df_dat, timezone_str
    )
    vlt_elapsed, vlt_start = _compute_elapsed_seconds_and_start_iso(
        df_vlt, timezone_str
    )
    out_elapsed, out_start = _compute_elapsed_seconds_and_start_iso(
        df_out, timezone_str
    )

    start_time_iso = dat_start or vlt_start or out_start

    # 開始時刻をログ出力（タイムゾーン確認用）
    if start_time_iso:
        print(f"記録開始時刻: {start_time_iso}")
        if timezone_str:
            print(f"指定タイムゾーン: {timezone_str}")
    else:
        print("日時情報が見つかりません（タイムゾーン不明）")

    # 制御ステップ変化イベントを抽出して各データセット向け経過秒へマッピング
    events_dat, events_vlt, events_out = _compute_control_change_events(
        df_out, df_dat, df_vlt, dat_elapsed, vlt_elapsed, out_elapsed
    )

    # 各データセット用にセグメント (start,end,step) を構築
    seg_dat = _build_segments(dat_elapsed, events_dat)
    seg_vlt = _build_segments(vlt_elapsed, events_vlt)
    seg_out = _build_segments(out_elapsed, events_out)

    # 各セグメントに対応する開始時刻の ISO 表示を付加 (start_iso)
    def _label_segments(segments, df):
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

    seg_dat = _label_segments(seg_dat, df_dat)
    seg_vlt = _label_segments(seg_vlt, df_vlt)
    seg_out = _label_segments(seg_out, df_out)

    # stress-strain は .out のみから作図
    if df_out is not None:
        print("Plotting stress-strain (from _out.tsv/.out) ...")
        plot_stress_strain(
            output_dir / f"{base_path.name}_stress_strain.png",
            control_segments=seg_out,
            df_out=df_out,
            out_elapsed_seconds=out_elapsed,
        )

    # .vlt からプロット（CH00～CH06のみ）
    if df_vlt is not None:
        print("Plotting .vlt (voltage channels)...")
        plot_voltage_channels(
            df_vlt,
            channels=[0, 1, 2, 3, 4, 5, 6],
            save_path=output_dir / f"{base_path.name}_voltage_ch0-6.png",
            elapsed_seconds=vlt_elapsed,
            start_time_iso=start_time_iso,
            control_events=seg_vlt,
        )

    # .out からプロット
    if df_out is not None:
        print("Plotting .out (calculated parameters)...")
        plot_parameters(
            df_out,
            df_dat=df_dat,
            save_path=output_dir / f"{base_path.name}_parameters.png",
            elapsed_seconds=out_elapsed,
            start_time_iso=start_time_iso,
            control_segments=seg_out,
        )

    print(f"\nSummary report generated in: {output_dir}")


if __name__ == "__main__":
    import argparse

    parser = argparse.ArgumentParser(
        description="DigitShowDST ログファイルから グラフを生成"
    )
    parser.add_argument(
        "base_path",
        help="ログファイルのベースパス（拡張子なし）. 例: 2025-10-28_test または 2025-10-28_test.dat",
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

    create_summary_report(args.base_path, args.output_dir, timezone_str=args.timezone)

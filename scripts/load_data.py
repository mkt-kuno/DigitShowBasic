"""
DigitShowDST ログファイル読み込みユーティリティ

新形式: .tsv (物理量), _vlt.tsv (生電圧), _out.tsv (計算パラメータ)
旧形式: .dat (物理量), .vlt (生電圧), .out (計算パラメータ)

両形式に対応し、Unix time（新形式）または経過時間（旧形式）を自動判別。
"""

import pandas as pd
from pathlib import Path
from typing import Optional, Tuple


def read_dat_file(filepath: str | Path) -> pd.DataFrame:
    """
    .dat/.tsv ファイル（物理量）を読み込む

    Args:
        filepath: .dat または .tsv ファイルのパス

    Returns:
        pandas.DataFrame: Timestamp（新形式）または Time(s)（旧形式）を含む物理量データ

    Notes:
        - タブ区切り
        - 先頭行はヘッダ
        - 新形式（UnixTime(ms)）: datetime インデックスに変換
        - 旧形式（Time(s)）: そのままインデックスに設定
    """
    filepath = Path(filepath)
    if not filepath.exists():
        raise FileNotFoundError(f"File not found: {filepath}")

    df = pd.read_csv(
        filepath,
        sep="\t",
        encoding="utf-8",
        skipinitialspace=True,
    )

    # 新形式: UnixTime(ms) → datetime インデックス
    if "UnixTime(ms)" in df.columns:
        df["Timestamp"] = pd.to_datetime(df["UnixTime(ms)"], unit="ms")
        df.set_index("Timestamp", inplace=True)
        df.drop(columns=["UnixTime(ms)"], inplace=True)
    # 旧形式: Time(s) → そのままインデックス
    elif "Time(s)" in df.columns:
        df.set_index("Time(s)", inplace=True)

    return df


def read_vlt_file(filepath: str | Path) -> pd.DataFrame:
    """
    .vlt/_vlt.tsv ファイル（生電圧）を読み込む

    Args:
        filepath: .vlt または _vlt.tsv ファイルのパス

    Returns:
        pandas.DataFrame: Timestamp（新形式）または Time(s)（旧形式）を含む生電圧データ

    Notes:
        - タブ区切り
        - 先頭行はヘッダ (CH00_(V), CH01_(V), ...)
        - 新形式（UnixTime(ms)）: datetime インデックスに変換
        - 旧形式（Time(s)）: そのままインデックスに設定
    """
    filepath = Path(filepath)
    if not filepath.exists():
        raise FileNotFoundError(f"File not found: {filepath}")

    df = pd.read_csv(
        filepath,
        sep="\t",
        encoding="utf-8",
        skipinitialspace=True,
    )

    # 新形式: UnixTime(ms) → datetime インデックス
    if "UnixTime(ms)" in df.columns:
        df["Timestamp"] = pd.to_datetime(df["UnixTime(ms)"], unit="ms")
        df.set_index("Timestamp", inplace=True)
        df.drop(columns=["UnixTime(ms)"], inplace=True)
    # 旧形式: Time(s) → そのままインデックス
    elif "Time(s)" in df.columns:
        df.set_index("Time(s)", inplace=True)

    return df


def read_out_file(filepath: str | Path) -> pd.DataFrame:
    """
    .out/_out.tsv ファイル（計算パラメータ）を読み込む

    Args:
        filepath: .out または _out.tsv ファイルのパス

    Returns:
        pandas.DataFrame: Timestamp（新形式）または Time(s)（旧形式）を含む計算パラメータデータ

    Notes:
        - タブ区切り
        - 先頭行はヘッダ (s(a)_(kPa), s(r)_(kPa), ...)
        - 新形式（UnixTime(ms)）: datetime インデックスに変換
        - 旧形式（Time(s)）: そのままインデックスに設定
        - ヘッダ名と実データの意味が一部齟齬あり（data_file_formats.md 参照）
    """
    filepath = Path(filepath)
    if not filepath.exists():
        raise FileNotFoundError(f"File not found: {filepath}")

    df = pd.read_csv(
        filepath,
        sep="\t",
        encoding="utf-8",
        skipinitialspace=True,
    )

    # 新形式: UnixTime(ms) → datetime インデックス
    if "UnixTime(ms)" in df.columns:
        df["Timestamp"] = pd.to_datetime(df["UnixTime(ms)"], unit="ms")
        df.set_index("Timestamp", inplace=True)
        df.drop(columns=["UnixTime(ms)"], inplace=True)
    # 旧形式: Time(s) → そのままインデックス
    elif "Time(s)" in df.columns:
        df.set_index("Time(s)", inplace=True)

    return df


def read_all_files(
    base_path: str | Path,
) -> Tuple[Optional[pd.DataFrame], Optional[pd.DataFrame], Optional[pd.DataFrame]]:
    """
    同名の .dat/.vlt/.out（旧形式）または .tsv/_vlt.tsv/_out.tsv（新形式）を一括で読み込む

    Args:
        base_path: ファイルパス（拡張子なし、または .dat/.vlt/.out/.tsv いずれか）

    Returns:
        Tuple[df_dat, df_vlt, df_out]: 各 DataFrame（存在しない場合は None）

    Example:
        >>> # 新形式の場合
        >>> dat, vlt, out = read_all_files("2025-10-28_test")  # .tsv, _vlt.tsv, _out.tsv を読み込み
        >>> # 旧形式の場合
        >>> dat, vlt, out = read_all_files("2025-10-27_old")  # .dat, .vlt, .out を読み込み
        >>> print(dat.head())
    """
    base_path = Path(base_path)

    # 拡張子を除去してベース名を取得
    if base_path.suffix in [".dat", ".vlt", ".out", ".tsv"]:
        base_path = base_path.with_suffix("")

    # 新形式を優先的にチェック
    tsv_path = base_path.with_suffix(".tsv")
    vlt_tsv_path = Path(str(base_path) + "_vlt.tsv")
    out_tsv_path = Path(str(base_path) + "_out.tsv")

    # 旧形式
    dat_path = base_path.with_suffix(".dat")
    vlt_path = base_path.with_suffix(".vlt")
    out_path = base_path.with_suffix(".out")

    # 新形式が存在すればそれを優先
    df_dat = None
    if tsv_path.exists():
        df_dat = read_dat_file(tsv_path)
    elif dat_path.exists():
        df_dat = read_dat_file(dat_path)

    df_vlt = None
    if vlt_tsv_path.exists():
        df_vlt = read_vlt_file(vlt_tsv_path)
    elif vlt_path.exists():
        df_vlt = read_vlt_file(vlt_path)

    df_out = None
    if out_tsv_path.exists():
        df_out = read_out_file(out_tsv_path)
    elif out_path.exists():
        df_out = read_out_file(out_path)

    return df_dat, df_vlt, df_out


def list_log_files(directory: str | Path) -> list[Path]:
    """
    指定ディレクトリ内の .dat/.tsv ファイルを一覧表示

    Args:
        directory: 検索対象ディレクトリ

    Returns:
        list[Path]: .dat/.tsv ファイルのパスリスト（拡張子なしのベース名）

    Notes:
        - .tsv ファイルを優先的に検出
        - _vlt.tsv, _out.tsv は除外（メインファイルのみ）
    """
    directory = Path(directory)

    # 新形式 (.tsv) を検索（_vlt.tsv, _out.tsv は除外）
    tsv_files = [
        f
        for f in sorted(directory.glob("*.tsv"))
        if not (f.stem.endswith("_vlt") or f.stem.endswith("_out"))
    ]

    # 旧形式 (.dat) を検索
    dat_files = sorted(directory.glob("*.dat"))

    # 重複排除: .tsv が存在する場合は .dat を除外
    tsv_stems = {f.stem for f in tsv_files}
    dat_files_unique = [f for f in dat_files if f.stem not in tsv_stems]

    # 結合して返す（拡張子なしのベース名）
    all_files = tsv_files + dat_files_unique
    return [f.with_suffix("") for f in sorted(all_files)]

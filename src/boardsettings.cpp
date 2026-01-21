/*
 * DigitShowDST - Direct Shear Test Machine Control Software
 * Copyright (C) 2025 Makoto KUNO, Takuto ISHII
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

/**
 * @file BoardSettings.cpp
 * @brief Implementation of board settings dialog
 *
 * インプリメンテーション ファイル
 */
#include "StdAfx.h"

#include "Board.hpp"
#include "resource.h"

#include "BoardSettings.h"
#include "Constants.h"
#include <spdlog/spdlog.h>

/////////////////////////////////////////////////////////////////////////////
// CBoardSettings ダイアログ

CBoardSettings::CBoardSettings(CWnd *pParent /*=NULL*/)
    : CDialog(CBoardSettings::IDD, pParent), m_ADMethod0(_T("")), m_ADMethod1(_T("")), m_ADResolution0(_T("")),
      m_ADResolution1(_T("")), m_ADRange0(_T("")), m_ADRange1(_T("")), m_ADMaxChannel0(_T("")), m_ADMaxChannel1(_T("")),
      m_DAMaxChannel0(_T("")), m_DARange0(_T("")), m_DAResolution0(_T(""))
{
    //{{AFX_DATA_INIT(CBoardSettings)

    //}}AFX_DATA_INIT
}

void CBoardSettings::DoDataExchange(CDataExchange *pDX)
{
    CDialog::DoDataExchange(pDX);
    //{{AFX_DATA_MAP(CBoardSettings)
    DDX_Text(pDX, IDC_EDIT_ADMethod0, m_ADMethod0);
    DDX_Text(pDX, IDC_EDIT_ADMethod1, m_ADMethod1);
    DDX_Text(pDX, IDC_EDIT_ADResolution0, m_ADResolution0);
    DDX_Text(pDX, IDC_EDIT_ADResolution1, m_ADResolution1);
    DDX_Text(pDX, IDC_EDIT_ADRange0, m_ADRange0);
    DDX_Text(pDX, IDC_EDIT_ADRange1, m_ADRange1);
    DDX_Text(pDX, IDC_EDIT_ADMaxChannel0, m_ADMaxChannel0);
    DDX_Text(pDX, IDC_EDIT_ADMaxChannel1, m_ADMaxChannel1);
    DDX_Text(pDX, IDC_EDIT_DAMaxChannel0, m_DAMaxChannel0);
    DDX_Text(pDX, IDC_EDIT_DARange0, m_DARange0);
    DDX_Text(pDX, IDC_EDIT_DAResolution0, m_DAResolution0);
    //}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP_IGNORE_UNUSED_LOCAL_TYPEDEF(CBoardSettings, CDialog)
//{{AFX_MSG_MAP(CBoardSettings)
//}}AFX_MSG_MAP
END_MESSAGE_MAP_IGNORE_UNUSED_LOCAL_TYPEDEF()

/////////////////////////////////////////////////////////////////////////////
// CBoardSettings メッセージ ハンドラ
BOOL CBoardSettings::OnInitDialog()
{
    using namespace board;
    CDialog::OnInitDialog();

    spdlog::debug("BoardSettings dialog initializing");

    // TODO: この位置に初期化の補足処理を追加してください
    if constexpr (dsb::NUMAD > 0)
    {
        if (AdInputMethod[0] == 0)
            m_ADMethod0 = "Single Input";
        if (AdInputMethod[0] == 1)
            m_ADMethod0 = "Differential Input";
        if (AdResolution[0] == 12)
            m_ADResolution0 = "12 bit";
        if (AdResolution[0] == 16)
            m_ADResolution0 = "16 bit";
    }
    if constexpr (dsb::NUMAD > 1)
    {
        if (AdInputMethod[1] == 0)
            m_ADMethod1 = "Single Input";
        if (AdInputMethod[1] == 1)
            m_ADMethod1 = "Differential Input";
        if (AdResolution[1] == 12)
            m_ADResolution1 = "12 bit";
        if (AdResolution[1] == 16)
            m_ADResolution1 = "16 bit";
    }

    if constexpr (dsb::NUMAD > 0)
    {
        if (AdRange[0] == 0)
            m_ADRange0 = "-10V   +10V";
        if (AdRange[0] == 1)
            m_ADRange0 = "-5V   +5V";
        if (AdRange[0] == 50)
            m_ADRange0 = "0V   +10V";
        if (AdRange[0] == 51)
            m_ADRange0 = "0V   +5V";
    }
    if constexpr (dsb::NUMAD > 1)
    {
        if (AdRange[1] == 0)
            m_ADRange1 = "-10V   +10V";
        if (AdRange[1] == 1)
            m_ADRange1 = "-5V   +5V";
        if (AdRange[1] == 50)
            m_ADRange1 = "0V   +10V";
        if (AdRange[1] == 51)
            m_ADRange1 = "0V   +5V";
    }

    if constexpr (dsb::NUMAD > 0)
        m_ADMaxChannel0.Format(_T("%3d"), AdChannels[0] / 2);
    if constexpr (dsb::NUMAD > 1)
        m_ADMaxChannel1.Format(_T("%3d"), AdChannels[1] / 2);

    if constexpr (dsb::NUMDA > 0)
    {
        if (DaResolution[0] == 12)
            m_DAResolution0 = "12 bit";
        if (DaResolution[0] == 16)
            m_DAResolution0 = "16 bit";
    }

    if constexpr (dsb::NUMDA > 0)
    {
        if (DaRange[0] == 0)
            m_DARange0 = "-10V   +10V";
        if (DaRange[0] == 1)
            m_DARange0 = "-5V   +5V";
        if (DaRange[0] == 50)
            m_DARange0 = "0V   +10V";
        if (DaRange[0] == 51)
            m_DARange0 = "0V   +5V";
        m_DAMaxChannel0.Format(_T("%3d"), DaChannels[0]);
    }
    UpdateData(FALSE);
    return TRUE; // コントロールにフォーカスを設定しないとき、戻り値は TRUE となります
                 // 例外: OCX プロパティ ページの戻り値は FALSE となります
}

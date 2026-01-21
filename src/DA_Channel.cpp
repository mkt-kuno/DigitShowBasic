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
 * @file DA_Channel.cpp
 * @brief Implementation of D/A channel calibration dialog
 *
 * インプリメンテーション ファイル
 */
#include "StdAfx.h"

#include "DA_Channel.h"
#include "Variables.hpp"
#include "resource.h"

using namespace variables;

/////////////////////////////////////////////////////////////////////////////
// CDA_Channel ダイアログ

CDA_Channel::CDA_Channel(CWnd *pParent /*=NULL*/) : CDialog(CDA_Channel::IDD, pParent)
{
    // Initialize arrays with current DA_Cal values
    for (size_t i = 0; i < CHANNELS_DA; ++i)
    {
        m_DA_Cala[i] = DA_Cal_a[i];
        m_DA_Calb[i] = DA_Cal_b[i];
    }
}

void CDA_Channel::DoDataExchange(CDataExchange *pDX)
{
    CDialog::DoDataExchange(pDX);
    //{{AFX_DATA_MAP(CDA_Channel)

    // Resource ID arrays for DDX_Text loops
    static constexpr std::array<int, CHANNELS_DA> IDS_CALA{{IDC_EDIT_DA_Cala00, IDC_EDIT_DA_Cala01, IDC_EDIT_DA_Cala02,
                                                            IDC_EDIT_DA_Cala03, IDC_EDIT_DA_Cala04, IDC_EDIT_DA_Cala05,
                                                            IDC_EDIT_DA_Cala06, IDC_EDIT_DA_Cala07}};

    static constexpr std::array<int, CHANNELS_DA> IDS_CALB{{IDC_EDIT_DA_Calb00, IDC_EDIT_DA_Calb01, IDC_EDIT_DA_Calb02,
                                                            IDC_EDIT_DA_Calb03, IDC_EDIT_DA_Calb04, IDC_EDIT_DA_Calb05,
                                                            IDC_EDIT_DA_Calb06, IDC_EDIT_DA_Calb07}};

    // Use loops for array-based DDX
    for (size_t i = 0; i < CHANNELS_DA; ++i)
    {
        DDX_Text(pDX, IDS_CALA[i], m_DA_Cala[i]);
        DDX_Text(pDX, IDS_CALB[i], m_DA_Calb[i]);
    }
    //}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP_IGNORE_UNUSED_LOCAL_TYPEDEF(CDA_Channel, CDialog)
//{{AFX_MSG_MAP(CDA_Channel)
//}}AFX_MSG_MAP
END_MESSAGE_MAP_IGNORE_UNUSED_LOCAL_TYPEDEF()

/////////////////////////////////////////////////////////////////////////////
// CDA_Channel メッセージ ハンドラ

void CDA_Channel::OnOK()
{
    // TODO: この位置にその他の検証用のコードを追加してください
    UpdateData(TRUE);
    for (size_t i = 0; i < CHANNELS_DA; ++i)
    {
        DA_Cal_a[i] = m_DA_Cala[i];
        DA_Cal_b[i] = m_DA_Calb[i];
    }
    CDialog::OnOK();
}

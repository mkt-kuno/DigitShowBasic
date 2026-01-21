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
 * @file DA_Vout.cpp
 * @brief Implementation of D/A voltage output dialog
 *
 * インプリメンテーション ファイル
 */

#include "StdAfx.h"
#include "resource.h"

#include "DA_Vout.h"
#include "Variables.hpp"
#include <ranges>
#include <spdlog/spdlog.h>

using namespace variables;

/////////////////////////////////////////////////////////////////////////////
// CDA_Vout ダイアログ

CDA_Vout::CDA_Vout(CWnd *pParent /*=NULL*/) : CDialog(CDA_Vout::IDD, pParent), m_DAVout{DAVout}
{
}

void CDA_Vout::DoDataExchange(CDataExchange *pDX)
{
    CDialog::DoDataExchange(pDX);
    //{{AFX_DATA_MAP(CDA_Vout)

    // Resource ID array for DDX_Text loop (only channels 1-8 are used)
    static constexpr std::array<int, 8> IDS_DAVOUT = {{IDC_EDIT_DAVout01, IDC_EDIT_DAVout02, IDC_EDIT_DAVout03,
                                                       IDC_EDIT_DAVout04, IDC_EDIT_DAVout05, IDC_EDIT_DAVout06,
                                                       IDC_EDIT_DAVout07, IDC_EDIT_DAVout08}};

    // Use loop for array-based DDX
    for (const auto &&[id, value] : std::views::zip(IDS_DAVOUT, m_DAVout))
    {
        DDX_Text(pDX, id, value);
    }
    //}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP_IGNORE_UNUSED_LOCAL_TYPEDEF(CDA_Vout, CDialog)
//{{AFX_MSG_MAP(CDA_Vout)
ON_BN_CLICKED(IDC_BUTTON_DA_Vout, &CDA_Vout::OnBUTTONDAVout)
ON_BN_CLICKED(IDC_BUTTON_Reflesh, &CDA_Vout::OnBUTTONReflesh)
//}}AFX_MSG_MAP
END_MESSAGE_MAP_IGNORE_UNUSED_LOCAL_TYPEDEF()

/////////////////////////////////////////////////////////////////////////////
// CDA_Vout メッセージ ハンドラ

void CDA_Vout::OnBUTTONDAVout()
{
    // TODO: この位置にコントロール通知ハンドラ用のコードを追加してください
    UpdateData(TRUE);
    if (const auto result = variables::set_output_voltages(m_DAVout); !result)
    {
        spdlog::warn("set_output_voltages failed: {}", result.error());
    }
}

void CDA_Vout::OnBUTTONReflesh()
{
    // TODO: この位置にコントロール通知ハンドラ用のコードを追加してください
    m_DAVout = DAVout;
    UpdateData(FALSE);
}

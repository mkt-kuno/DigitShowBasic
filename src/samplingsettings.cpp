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
 * @file SamplingSettings.cpp
 * @brief Implementation of sampling settings dialog
 *
 * インプリメンテーション ファイル
 */
#include "StdAfx.h"

#include "Board.hpp"
#include "Constants.h"
#include "SamplingSettings.h"
#include "resource.h"
#include "timer.hpp"

/////////////////////////////////////////////////////////////////////////////
// CSamplingSettings ダイアログ

CSamplingSettings::CSamplingSettings(CWnd *pParent /*=NULL*/) : CDialog(CSamplingSettings::IDD, pParent)
{
    //{{AFX_DATA_INIT(CSamplingSettings)

    //}}AFX_DATA_INIT
}

void CSamplingSettings::DoDataExchange(CDataExchange *pDX)
{
    CDialog::DoDataExchange(pDX);
    //{{AFX_DATA_MAP(CSamplingSettings)
    DDX_Text(pDX, IDC_EDIT_TimeInterval1, m_TimeInterval1);
    DDX_Text(pDX, IDC_EDIT_TimeInterval2, m_TimeInterval2);
    DDX_Text(pDX, IDC_EDIT_TimeInterval3, m_TimeInterval3);
    DDX_Text(pDX, IDC_EDIT_AvSmplNum, m_AvSmplNum);
    DDX_Text(pDX, IDC_EDIT_Channels, m_Channels);
    //}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP_IGNORE_UNUSED_LOCAL_TYPEDEF(CSamplingSettings, CDialog)
//{{AFX_MSG_MAP(CSamplingSettings)
ON_BN_CLICKED(IDC_BUTTON_Check, &CSamplingSettings::OnBUTTONCheck)
//}}AFX_MSG_MAP
END_MESSAGE_MAP_IGNORE_UNUSED_LOCAL_TYPEDEF()

/////////////////////////////////////////////////////////////////////////////
// CSamplingSettings メッセージ ハンドラ

BOOL CSamplingSettings::OnInitDialog()
{
    using namespace board;
    CDialog::OnInitDialog();

    // TODO: この位置に初期化の補足処理を追加してください
    m_TimeInterval1 = timer::TimeInterval_1.count();
    m_TimeInterval2 = timer::TimeInterval_2.count();
    m_TimeInterval3 = timer::TimeInterval_3.count();
    //
    m_Channels = AdMaxCH;
    m_AvSmplNum = AvSmplNum;
    UpdateData(FALSE);

    GetDlgItem(IDOK)->EnableWindow(FALSE);

    return TRUE; // コントロールにフォーカスを設定しないとき、戻り値は TRUE となります
                 // 例外: OCX プロパティ ページの戻り値は FALSE となります
}

void CSamplingSettings::OnBUTTONCheck()
{
    GetDlgItem(IDOK)->EnableWindow(TRUE);
}

void CSamplingSettings::OnOK()
{
    CDialog::OnOK();
}

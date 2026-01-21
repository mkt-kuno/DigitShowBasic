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
 * @file DA_Pout.cpp
 * @brief Implementation of D/A physical output dialog
 *
 * インプリメンテーション ファイル
 */
#include "StdAfx.h"

#include "DA_Pout.h"
#include "Variables.hpp"
#include "control/measurement.hpp"
#include "resource.h"
#include <spdlog/spdlog.h>

using namespace variables;

/////////////////////////////////////////////////////////////////////////////
// CDA_Pout ダイアログ

CDA_Pout::CDA_Pout(CWnd *pParent /*=NULL*/) : CDialog(CDA_Pout::IDD, pParent)
{
    //{{AFX_DATA_INIT(CDA_Pout)

    //}}AFX_DATA_INIT

    // Initialize arrays with current values
    for (size_t i = 0; i < CHANNELS_DA_POUT; ++i)
    {
        m_DACala[i] = DA_Cal_a[i];
        m_DACalb[i] = DA_Cal_b[i];
        m_DAVout[i] = DAVout[i];
    }
}

void CDA_Pout::DoDataExchange(CDataExchange *pDX)
{
    CDialog::DoDataExchange(pDX);
    //{{AFX_DATA_MAP(CDA_Pout)

    // Resource ID arrays for DDX_Text loops
    static constexpr int IDS_DACALA[CHANNELS_DA_POUT] = {IDC_EDIT_DACala00, IDC_EDIT_DACala01, IDC_EDIT_DACala02,
                                                         IDC_EDIT_DACala03, IDC_EDIT_DACala04, IDC_EDIT_DACala05,
                                                         IDC_EDIT_DACala06, IDC_EDIT_DACala07};

    static constexpr int IDS_DACALB[CHANNELS_DA_POUT] = {IDC_EDIT_DACalb00, IDC_EDIT_DACalb01, IDC_EDIT_DACalb02,
                                                         IDC_EDIT_DACalb03, IDC_EDIT_DACalb04, IDC_EDIT_DACalb05,
                                                         IDC_EDIT_DACalb06, IDC_EDIT_DACalb07};

    static constexpr int IDS_DAPVALUE[CHANNELS_DA_POUT] = {
        IDC_EDIT_DAPvalue00, IDC_EDIT_DAPvalue01, IDC_EDIT_DAPvalue02, IDC_EDIT_DAPvalue03,
        IDC_EDIT_DAPvalue04, IDC_EDIT_DAPvalue05, IDC_EDIT_DAPvalue06, IDC_EDIT_DAPvalue07};

    static constexpr int IDS_DAVOUT[CHANNELS_DA_POUT] = {IDC_EDIT_DAVout00, IDC_EDIT_DAVout01, IDC_EDIT_DAVout02,
                                                         IDC_EDIT_DAVout03, IDC_EDIT_DAVout04, IDC_EDIT_DAVout05,
                                                         IDC_EDIT_DAVout06, IDC_EDIT_DAVout07};

    // Use loops for array-based DDX
    for (size_t i = 0; i < CHANNELS_DA_POUT; ++i)
    {
        DDX_Text(pDX, IDS_DACALA[i], m_DACala[i]);
        DDX_Text(pDX, IDS_DACALB[i], m_DACalb[i]);
        DDX_Text(pDX, IDS_DAPVALUE[i], m_DAPvalue[i]);
        DDX_Text(pDX, IDS_DAVOUT[i], m_DAVout[i]);
    }
    //}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP_IGNORE_UNUSED_LOCAL_TYPEDEF(CDA_Pout, CDialog)
//{{AFX_MSG_MAP(CDA_Pout)
ON_BN_CLICKED(IDC_BUTTON_DAOutput, &CDA_Pout::OnBUTTONDAOutput)
ON_BN_CLICKED(IDC_BUTTON_Calculation00, &CDA_Pout::OnBUTTONCalculation00)
ON_BN_CLICKED(IDC_BUTTON_Calculation01, &CDA_Pout::OnBUTTONCalculation01)
ON_BN_CLICKED(IDC_BUTTON_Calculation02, &CDA_Pout::OnBUTTONCalculation02)
ON_BN_CLICKED(IDC_BUTTON_Calculation03, &CDA_Pout::OnBUTTONCalculation03)
ON_BN_CLICKED(IDC_BUTTON_Calculation04, &CDA_Pout::OnBUTTONCalculation04)
ON_BN_CLICKED(IDC_BUTTON_Calculation05, &CDA_Pout::OnBUTTONCalculation05)
ON_BN_CLICKED(IDC_BUTTON_Calculation06, &CDA_Pout::OnBUTTONCalculation06)
ON_BN_CLICKED(IDC_BUTTON_Calculation07, &CDA_Pout::OnBUTTONCalculation07)
//}}AFX_MSG_MAP
END_MESSAGE_MAP_IGNORE_UNUSED_LOCAL_TYPEDEF()

/////////////////////////////////////////////////////////////////////////////
// CDA_Pout メッセージ ハンドラ

void CDA_Pout::OnBUTTONDAOutput()
{
    // TODO: この位置にコントロール通知ハンドラ用のコードを追加してください
    UpdateData(TRUE);
    if (const auto result = variables::set_output_voltages(m_DAVout); !result)
    {
        spdlog::warn("set_output_voltages failed: {}", result.error());
    }
}

void CDA_Pout::OnBUTTONCalculation00()
{
    // TODO: この位置にコントロール通知ハンドラ用のコードを追加してください
    UpdateData(TRUE);
    m_DAVout[0] = static_cast<float>(control::toVoltage(m_DAPvalue[0], DA_Cal_a[0], DA_Cal_b[0]));
    UpdateData(FALSE);
}

void CDA_Pout::OnBUTTONCalculation01()
{
    // TODO: この位置にコントロール通知ハンドラ用のコードを追加してください
    UpdateData(TRUE);
    m_DAVout[1] = static_cast<float>(control::toVoltage(m_DAPvalue[1], DA_Cal_a[1], DA_Cal_b[1]));
    UpdateData(FALSE);
}

void CDA_Pout::OnBUTTONCalculation02()
{
    // TODO: この位置にコントロール通知ハンドラ用のコードを追加してください
    UpdateData(TRUE);
    m_DAVout[2] = static_cast<float>(control::toVoltage(m_DAPvalue[2], DA_Cal_a[2], DA_Cal_b[2]));
    UpdateData(FALSE);
}

void CDA_Pout::OnBUTTONCalculation03()
{
    // TODO: この位置にコントロール通知ハンドラ用のコードを追加してください
    UpdateData(TRUE);
    m_DAVout[3] = static_cast<float>(control::toVoltage(m_DAPvalue[3], DA_Cal_a[3], DA_Cal_b[3]));
    UpdateData(FALSE);
}

void CDA_Pout::OnBUTTONCalculation04()
{
    // TODO: この位置にコントロール通知ハンドラ用のコードを追加してください
    UpdateData(TRUE);
    m_DAVout[4] = static_cast<float>(control::toVoltage(m_DAPvalue[4], DA_Cal_a[4], DA_Cal_b[4]));
    UpdateData(FALSE);
}

void CDA_Pout::OnBUTTONCalculation05()
{
    // TODO: この位置にコントロール通知ハンドラ用のコードを追加してください
    UpdateData(TRUE);
    m_DAVout[5] = static_cast<float>(control::toVoltage(m_DAPvalue[5], DA_Cal_a[5], DA_Cal_b[5]));
    UpdateData(FALSE);
}

void CDA_Pout::OnBUTTONCalculation06()
{
    // TODO: この位置にコントロール通知ハンドラ用のコードを追加してください
    UpdateData(TRUE);
    m_DAVout[6] = static_cast<float>(control::toVoltage(m_DAPvalue[6], DA_Cal_a[6], DA_Cal_b[6]));
    UpdateData(FALSE);
}

void CDA_Pout::OnBUTTONCalculation07()
{
    // TODO: この位置にコントロール通知ハンドラ用のコードを追加してください
    UpdateData(TRUE);
    m_DAVout[7] = static_cast<float>(control::toVoltage(m_DAPvalue[7], DA_Cal_a[7], DA_Cal_b[7]));
    UpdateData(FALSE);
}

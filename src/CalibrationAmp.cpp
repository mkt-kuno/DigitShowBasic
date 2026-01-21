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
 * @file CalibrationAmp.cpp
 * @brief Implementation of amplifier calibration dialog
 *
 * インプリメンテーション ファイル
 */
#include "StdAfx.h"

#include "CalibrationAmp.h"
#include "Variables.hpp"
#include "resource.h"
#include "ui_helpers.hpp"

using namespace variables;

/////////////////////////////////////////////////////////////////////////////
// CCalibrationAmp ダイアログ

CCalibrationAmp::CCalibrationAmp(CWnd *pParent /*=NULL*/) : CDialog(CCalibrationAmp::IDD, pParent), m_AmpNo(AmpID)
{
}

void CCalibrationAmp::DoDataExchange(CDataExchange *pDX)
{
    CDialog::DoDataExchange(pDX);
    //{{AFX_DATA_MAP(CCalibrationAmp)
    DDX_Text(pDX, IDC_EDIT_AmpNO, m_AmpNo);
    DDX_Text(pDX, IDC_EDIT_AmpPB, m_AmpPB);
    DDX_Text(pDX, IDC_EDIT_AmpVB, m_AmpVB);
    DDX_Text(pDX, IDC_EDIT_AmpVO, m_AmpVO);
    DDX_Text(pDX, IDC_EDIT_AmpPO, m_AmpPO);
    //}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP_IGNORE_UNUSED_LOCAL_TYPEDEF(CCalibrationAmp, CDialog)
//{{AFX_MSG_MAP(CCalibrationAmp)
ON_BN_CLICKED(IDC_BUTTON_AmpBase, &CCalibrationAmp::OnBUTTONAmpBase)
ON_BN_CLICKED(IDC_BUTTON_AmpOffset, &CCalibrationAmp::OnBUTTONAmpOffset)
ON_BN_CLICKED(IDC_BUTTON_AmpUpdate, &CCalibrationAmp::OnBUTTONAmpUpdate)
//}}AFX_MSG_MAP
END_MESSAGE_MAP_IGNORE_UNUSED_LOCAL_TYPEDEF()

/////////////////////////////////////////////////////////////////////////////
// CCalibrationAmp メッセージ ハンドラ

BOOL CCalibrationAmp::OnInitDialog()
{
    CDialog::OnInitDialog();

    // Initialize status control
    SetDlgItemText(IDC_STATIC_STATUS, _T(""));

    m_AmpPB = AmpPB[AmpID];
    m_AmpPO = AmpPO[AmpID];

    UpdateData(FALSE);

    return TRUE;
}

void CCalibrationAmp::OnBUTTONAmpBase()
{
    // TODO: この位置にコントロール通知ハンドラ用のコードを追加してください
    UpdateData(TRUE);
    m_AmpVB = Vout[AmpID];
    UpdateData(FALSE);
}

void CCalibrationAmp::OnBUTTONAmpOffset()
{
    // TODO: この位置にコントロール通知ハンドラ用のコードを追加してください
    UpdateData(TRUE);
    m_AmpVO = Vout[AmpID];
    UpdateData(FALSE);
}

void CCalibrationAmp::OnBUTTONAmpUpdate()
{
    // TODO: この位置にコントロール通知ハンドラ用のコードを追加してください
    UpdateData(TRUE);
    if (m_AmpVO - m_AmpVB == 0.0)
    {
        AfxMessageBox(_T("Can not get calibration factors!"), MB_ICONEXCLAMATION | MB_OK);
    }
    else
    {
        Cal_b[AmpID] = (m_AmpPO - m_AmpPB) / (m_AmpVO - m_AmpVB);
        Cal_c[AmpID] = m_AmpPB - Cal_b[AmpID] * m_AmpVB;
        AmpPB[AmpID] = m_AmpPB;
        AmpPO[AmpID] = m_AmpPO;
        set_status_text_and_fit_dialog(*this, IDC_STATIC_STATUS, "Calibration factors computed.");
    }
}

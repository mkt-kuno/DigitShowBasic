/*
 * DigitShowBasic - Triaxial Test Machine Control Software
 * Copyright (C) 2025 Makoto KUNO
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY;
 without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#include "stdafx.h"
#include "DigitShowBasic.h"
#include "CalibrationAmp.h"
#include "DigitShowContext.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CCalibrationAmp::CCalibrationAmp(CWnd* pParent)
    : CDialog(CCalibrationAmp::IDD, pParent)
{
    DigitShowContext* ctx = GetContext();
    m_AmpNo = ctx->AmpID;
    m_AmpPB = 0.0f;
    m_AmpVB = 0.0f;
    m_AmpVO = 0.0f;
    m_AmpPO = 0.0f;
}

void CCalibrationAmp::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    DDX_Text(pDX, IDC_EDIT_AmpNO, m_AmpNo);
    DDX_Text(pDX, IDC_EDIT_AmpPB, m_AmpPB);
    DDX_Text(pDX, IDC_EDIT_AmpVB, m_AmpVB);
    DDX_Text(pDX, IDC_EDIT_AmpVO, m_AmpVO);
    DDX_Text(pDX, IDC_EDIT_AmpPO, m_AmpPO);
}

BEGIN_MESSAGE_MAP(CCalibrationAmp, CDialog)
    ON_BN_CLICKED(IDC_BUTTON_AmpBase, OnBUTTONAmpBase)
    ON_BN_CLICKED(IDC_BUTTON_AmpOffset, OnBUTTONAmpOffset)
    ON_BN_CLICKED(IDC_BUTTON_AmpUpdate, OnBUTTONAmpUpdate)
END_MESSAGE_MAP()

void CCalibrationAmp::OnBUTTONAmpBase()
{
    UpdateData(TRUE);
    DigitShowContext* ctx = GetContext();
    m_AmpVB = ctx->Vout[ctx->AmpID];
    UpdateData(FALSE);
}

void CCalibrationAmp::OnBUTTONAmpOffset()
{
    UpdateData(TRUE);
    DigitShowContext* ctx = GetContext();
    m_AmpVO = ctx->Vout[ctx->AmpID];
    UpdateData(FALSE);
}

void CCalibrationAmp::OnBUTTONAmpUpdate()
{
    UpdateData(TRUE);
    if (m_AmpVO - m_AmpVB == 0.0) {
        AfxMessageBox("Can not get calibration factors!", MB_ICONEXCLAMATION | MB_OK);
    }
    else {
        DigitShowContext* ctx = GetContext();
        ctx->cal.b[ctx->AmpID] = (m_AmpPO - m_AmpPB) / (m_AmpVO - m_AmpVB);
        ctx->cal.c[ctx->AmpID] = m_AmpPB - ctx->cal.b[ctx->AmpID] * m_AmpVB;
        AfxMessageBox("Get calibration factors!", MB_ICONEXCLAMATION | MB_OK);
    }
}

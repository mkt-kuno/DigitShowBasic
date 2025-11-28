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
#include "TransAdjustment.h"
#include "DigitShowBasicDoc.h"
#include "DigitShowContext.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CTransAdjustment::CTransAdjustment(CWnd* pParent)
    : CDialog(CTransAdjustment::IDD, pParent)
{
    m_FinalDisp = 0.0;
    m_InitialDisp = 0.0;
    m_FinalBullet = 0.0;
    m_InitialBullet = 0.0;
}

void CTransAdjustment::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    DDX_Text(pDX, IDC_EDIT_FinalDisp, m_FinalDisp);
    DDX_Text(pDX, IDC_EDIT_InitialDisp, m_InitialDisp);
    DDX_Text(pDX, IDC_EDIT_FinalBullet, m_FinalBullet);
    DDX_Text(pDX, IDC_EDIT_InitialBullet, m_InitialBullet);
}

BEGIN_MESSAGE_MAP(CTransAdjustment, CDialog)
    ON_BN_CLICKED(IDC_BUTTON_InitialDisp, OnBUTTONInitialDisp)
    ON_BN_CLICKED(IDC_BUTTON_EndDisp, OnBUTTONEndDisp)
    ON_BN_CLICKED(IDC_BUTTON_InitialBullet, OnBUTTONInitialBullet)
    ON_BN_CLICKED(IDC_BUTTON_EndBullet, OnBUTTONEndBullet)
    ON_BN_CLICKED(IDC_BUTTON_UpdateDisp, OnBUTTONUpdateDisp)
    ON_BN_CLICKED(IDC_BUTTON_UpdateBullet, OnBUTTONUpdateBullet)
END_MESSAGE_MAP()

void CTransAdjustment::OnBUTTONInitialDisp()
{
    DigitShowContext* ctx = GetContext();
    m_InitialDisp = ctx->Phyout[1];
    UpdateData(FALSE);
    CButton* myBTN1 = (CButton*)GetDlgItem(IDC_BUTTON_UpdateDisp);
    myBTN1->EnableWindow(TRUE);
}

void CTransAdjustment::OnBUTTONEndDisp()
{
    DigitShowContext* ctx = GetContext();
    m_FinalDisp = ctx->Phyout[1];
    UpdateData(FALSE);
    CButton* myBTN1 = (CButton*)GetDlgItem(IDC_BUTTON_UpdateDisp);
    myBTN1->EnableWindow(TRUE);
}

void CTransAdjustment::OnBUTTONUpdateDisp()
{
    UpdateData(TRUE);
    DigitShowContext* ctx = GetContext();
    ctx->cal.c[1] = ctx->cal.c[1] + (m_InitialDisp - m_FinalDisp);
    CButton* myBTN1 = (CButton*)GetDlgItem(IDC_BUTTON_UpdateDisp);
    myBTN1->EnableWindow(FALSE);
}

void CTransAdjustment::OnBUTTONInitialBullet()
{
    DigitShowContext* ctx = GetContext();
    m_InitialBullet = ctx->Phyout[4];
    UpdateData(FALSE);
    CButton* myBTN1 = (CButton*)GetDlgItem(IDC_BUTTON_UpdateBullet);
    myBTN1->EnableWindow(TRUE);
}

void CTransAdjustment::OnBUTTONEndBullet()
{
    DigitShowContext* ctx = GetContext();
    m_FinalBullet = ctx->Phyout[4];
    UpdateData(FALSE);
    CButton* myBTN1 = (CButton*)GetDlgItem(IDC_BUTTON_UpdateBullet);
    myBTN1->EnableWindow(TRUE);
}

void CTransAdjustment::OnBUTTONUpdateBullet()
{
    UpdateData(TRUE);
    DigitShowContext* ctx = GetContext();
    ctx->cal.c[4] = ctx->cal.c[4] + (m_InitialBullet - m_FinalBullet);
    CButton* myBTN1 = (CButton*)GetDlgItem(IDC_BUTTON_UpdateBullet);
    myBTN1->EnableWindow(FALSE);
}

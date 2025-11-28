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
#include "Control_LinearStressPath.h"
#include "DigitShowBasicDoc.h"
#include "DigitShowContext.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CControl_LinearStressPath::CControl_LinearStressPath(CWnd* pParent)
    : CDialog(CControl_LinearStressPath::IDD, pParent)
{
    DigitShowContext* ctx = GetContext();
    m_e_sigma1 = ctx->control[7].e_sigma[0];
    m_e_sigma2 = ctx->control[7].e_sigma[1];
    m_MotorSpeed = ctx->control[7].MotorSpeed;
    m_sigma_rate = ctx->control[7].sigmaRate[0];
    m_sigma1 = ctx->control[7].sigma[0];
    m_sigma2 = ctx->control[7].sigma[1];
}

void CControl_LinearStressPath::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    DDX_Text(pDX, IDC_EDIT_e_sigma1, m_e_sigma1);
    DDX_Text(pDX, IDC_EDIT_e_sigma2, m_e_sigma2);
    DDX_Text(pDX, IDC_EDIT_MotorSpeed, m_MotorSpeed);
    DDX_Text(pDX, IDC_EDIT_sigma_rate, m_sigma_rate);
    DDX_Text(pDX, IDC_EDIT_sigma1, m_sigma1);
    DDX_Text(pDX, IDC_EDIT_sigma2, m_sigma2);
}

BEGIN_MESSAGE_MAP(CControl_LinearStressPath, CDialog)
    ON_BN_CLICKED(IDC_BUTTON_Update, OnBUTTONUpdate)
END_MESSAGE_MAP()

void CControl_LinearStressPath::OnBUTTONUpdate()
{
    UpdateData(TRUE);
    DigitShowContext* ctx = GetContext();
    ctx->control[7].e_sigma[0] = m_e_sigma1;
    ctx->control[7].e_sigma[1] = m_e_sigma2;
    ctx->control[7].MotorSpeed = m_MotorSpeed;
    ctx->control[7].sigmaRate[0] = m_sigma_rate;
    ctx->control[7].sigma[0] = m_sigma1;
    ctx->control[7].sigma[1] = m_sigma2;
}

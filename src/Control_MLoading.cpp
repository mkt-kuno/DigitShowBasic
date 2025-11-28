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
#include "Control_MLoading.h"
#include "DigitShowBasicDoc.h"
#include "DigitShowContext.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CControl_MLoading::CControl_MLoading(CWnd* pParent)
    : CDialog(CControl_MLoading::IDD, pParent)
{
    DigitShowContext* ctx = GetContext();
    m_MotorCruch = ctx->control[3].MotorCruch;
    m_MotorSpeed = ctx->control[3].MotorSpeed;
    m_flag0 = ctx->control[3].flag[0];
    m_q = ctx->control[3].q;
}

void CControl_MLoading::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    DDX_Text(pDX, IDC_EDIT_Motor_Cruch, m_MotorCruch);
    DDV_MinMaxInt(pDX, m_MotorCruch, 0, 1);
    DDX_Text(pDX, IDC_EDIT_MotorSpeed, m_MotorSpeed);
    DDX_Text(pDX, IDC_EDIT_flag0, m_flag0);
    DDX_Text(pDX, IDC_EDIT_q, m_q);
}

BEGIN_MESSAGE_MAP(CControl_MLoading, CDialog)
    ON_BN_CLICKED(IDC_BUTTON_Update, OnBUTTONUpdate)
    ON_BN_CLICKED(IDC_BUTTON_Reflesh, OnBUTTONReflesh)
END_MESSAGE_MAP()

void CControl_MLoading::OnBUTTONUpdate()
{
    UpdateData(TRUE);
    DigitShowContext* ctx = GetContext();
    ctx->control[3].MotorCruch = m_MotorCruch;
    ctx->control[3].MotorSpeed = m_MotorSpeed;
    ctx->control[3].flag[0] = (m_flag0 != 0);
    ctx->control[3].q = m_q;
    ctx->control[4] = ctx->control[3];
}

void CControl_MLoading::OnBUTTONReflesh()
{
    DigitShowContext* ctx = GetContext();
    if (ctx->ControlID == 4) {
        ctx->control[3] = ctx->control[4];
    }
    m_MotorCruch = ctx->control[3].MotorCruch;
    m_MotorSpeed = ctx->control[3].MotorSpeed;
    m_flag0 = ctx->control[3].flag[0];
    m_q = ctx->control[3].q;
    UpdateData(FALSE);
}

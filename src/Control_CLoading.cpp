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
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#include "stdafx.h"
#include "DigitShowBasic.h"
#include "Control_CLoading.h"
#include "DigitShowBasicDoc.h"
#include "DigitShowContext.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CControl_CLoading::CControl_CLoading(CWnd* pParent)
    : CDialog(CControl_CLoading::IDD, pParent)
{
    DigitShowContext* ctx = GetContext();
    m_flag0 = ctx->control[5].flag[0];
    m_MotorSpeed = ctx->control[5].MotorSpeed;
    m_q_lower = ctx->control[5].sigma[0];
    m_q_upper = ctx->control[5].sigma[1];
    m_time0 = ctx->control[5].time[0];
    m_time1 = ctx->control[5].time[1];
    m_time2 = ctx->control[5].time[2];
}

void CControl_CLoading::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    DDX_Text(pDX, IDC_EDIT_flag0, m_flag0);
    DDX_Text(pDX, IDC_EDIT_MotorSpeed, m_MotorSpeed);
    DDX_Text(pDX, IDC_EDIT_q_lower, m_q_lower);
    DDX_Text(pDX, IDC_EDIT_q_upper, m_q_upper);
    DDX_Text(pDX, IDC_EDIT_time0, m_time0);
    DDX_Text(pDX, IDC_EDIT_time1, m_time1);
    DDX_Text(pDX, IDC_EDIT_time2, m_time2);
}

BEGIN_MESSAGE_MAP(CControl_CLoading, CDialog)
    ON_BN_CLICKED(IDC_BUTTON_Update, OnBUTTONUpdate)
    ON_BN_CLICKED(IDC_BUTTON_Reflesh, OnBUTTONReflesh)
END_MESSAGE_MAP()

void CControl_CLoading::OnBUTTONUpdate()
{
    UpdateData(TRUE);
    DigitShowContext* ctx = GetContext();
    ctx->control[5].flag[0] = (m_flag0 != 0);
    ctx->control[5].MotorSpeed = m_MotorSpeed;
    ctx->control[5].sigma[0] = m_q_lower;
    ctx->control[5].sigma[1] = m_q_upper;
    ctx->control[5].time[0] = m_time0;
    ctx->control[5].time[1] = m_time1;
    ctx->control[5].time[2] = m_time2;
    ctx->control[6] = ctx->control[5];
}

void CControl_CLoading::OnBUTTONReflesh()
{
    DigitShowContext* ctx = GetContext();
    if (ctx->ControlID == 6) {
        ctx->control[5] = ctx->control[6];
    }
    m_flag0 = ctx->control[5].flag[0];
    m_MotorSpeed = ctx->control[5].MotorSpeed;
    m_q_lower = ctx->control[5].sigma[0];
    m_q_upper = ctx->control[5].sigma[1];
    m_time0 = ctx->control[5].time[0];
    m_time1 = ctx->control[5].time[1];
    m_time2 = ctx->control[5].time[2];
    UpdateData(FALSE);
}

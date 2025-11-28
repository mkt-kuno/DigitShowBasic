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
#include "DA_Channel.h"
#include "DigitShowContext.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CDA_Channel::CDA_Channel(CWnd* pParent)
    : CDialog(CDA_Channel::IDD, pParent)
{
    DigitShowContext* ctx = GetContext();
    m_DA_Cala00 = ctx->cal.DA_a[0];
    m_DA_Cala01 = ctx->cal.DA_a[1];
    m_DA_Cala02 = ctx->cal.DA_a[2];
    m_DA_Cala03 = ctx->cal.DA_a[3];
    m_DA_Cala04 = ctx->cal.DA_a[4];
    m_DA_Cala05 = ctx->cal.DA_a[5];
    m_DA_Cala06 = ctx->cal.DA_a[6];
    m_DA_Cala07 = ctx->cal.DA_a[7];
    m_DA_Calb00 = ctx->cal.DA_b[0];
    m_DA_Calb01 = ctx->cal.DA_b[1];
    m_DA_Calb02 = ctx->cal.DA_b[2];
    m_DA_Calb03 = ctx->cal.DA_b[3];
    m_DA_Calb04 = ctx->cal.DA_b[4];
    m_DA_Calb05 = ctx->cal.DA_b[5];
    m_DA_Calb06 = ctx->cal.DA_b[6];
    m_DA_Calb07 = ctx->cal.DA_b[7];
}

void CDA_Channel::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    DDX_Text(pDX, IDC_EDIT_DA_Cala00, m_DA_Cala00);
    DDX_Text(pDX, IDC_EDIT_DA_Cala01, m_DA_Cala01);
    DDX_Text(pDX, IDC_EDIT_DA_Cala02, m_DA_Cala02);
    DDX_Text(pDX, IDC_EDIT_DA_Cala03, m_DA_Cala03);
    DDX_Text(pDX, IDC_EDIT_DA_Cala04, m_DA_Cala04);
    DDX_Text(pDX, IDC_EDIT_DA_Cala05, m_DA_Cala05);
    DDX_Text(pDX, IDC_EDIT_DA_Cala06, m_DA_Cala06);
    DDX_Text(pDX, IDC_EDIT_DA_Cala07, m_DA_Cala07);
    DDX_Text(pDX, IDC_EDIT_DA_Calb00, m_DA_Calb00);
    DDX_Text(pDX, IDC_EDIT_DA_Calb01, m_DA_Calb01);
    DDX_Text(pDX, IDC_EDIT_DA_Calb02, m_DA_Calb02);
    DDX_Text(pDX, IDC_EDIT_DA_Calb03, m_DA_Calb03);
    DDX_Text(pDX, IDC_EDIT_DA_Calb04, m_DA_Calb04);
    DDX_Text(pDX, IDC_EDIT_DA_Calb05, m_DA_Calb05);
    DDX_Text(pDX, IDC_EDIT_DA_Calb06, m_DA_Calb06);
    DDX_Text(pDX, IDC_EDIT_DA_Calb07, m_DA_Calb07);
}

BEGIN_MESSAGE_MAP(CDA_Channel, CDialog)
END_MESSAGE_MAP()

void CDA_Channel::OnOK()
{
    UpdateData(TRUE);
    DigitShowContext* ctx = GetContext();
    ctx->cal.DA_a[0] = m_DA_Cala00;
    ctx->cal.DA_a[1] = m_DA_Cala01;
    ctx->cal.DA_a[2] = m_DA_Cala02;
    ctx->cal.DA_a[3] = m_DA_Cala03;
    ctx->cal.DA_a[4] = m_DA_Cala04;
    ctx->cal.DA_a[5] = m_DA_Cala05;
    ctx->cal.DA_a[6] = m_DA_Cala06;
    ctx->cal.DA_a[7] = m_DA_Cala07;
    ctx->cal.DA_b[0] = m_DA_Calb00;
    ctx->cal.DA_b[1] = m_DA_Calb01;
    ctx->cal.DA_b[2] = m_DA_Calb02;
    ctx->cal.DA_b[3] = m_DA_Calb03;
    ctx->cal.DA_b[4] = m_DA_Calb04;
    ctx->cal.DA_b[5] = m_DA_Calb05;
    ctx->cal.DA_b[6] = m_DA_Calb06;
    ctx->cal.DA_b[7] = m_DA_Calb07;
    CDialog::OnOK();
}

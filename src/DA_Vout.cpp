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
#include "DA_Vout.h"
#include "DigitShowContext.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CDA_Vout::CDA_Vout(CWnd* pParent)
    : CDialog(CDA_Vout::IDD, pParent)
{
    DigitShowContext* ctx = GetContext();
    m_DAVout01 = ctx->DAVout[0];
    m_DAVout02 = ctx->DAVout[1];
    m_DAVout03 = ctx->DAVout[2];
    m_DAVout04 = ctx->DAVout[3];
    m_DAVout05 = ctx->DAVout[4];
    m_DAVout06 = ctx->DAVout[5];
    m_DAVout07 = ctx->DAVout[6];
    m_DAVout08 = ctx->DAVout[7];
}

void CDA_Vout::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    DDX_Text(pDX, IDC_EDIT_DAVout01, m_DAVout01);
    DDX_Text(pDX, IDC_EDIT_DAVout02, m_DAVout02);
    DDX_Text(pDX, IDC_EDIT_DAVout03, m_DAVout03);
    DDX_Text(pDX, IDC_EDIT_DAVout04, m_DAVout04);
    DDX_Text(pDX, IDC_EDIT_DAVout05, m_DAVout05);
    DDX_Text(pDX, IDC_EDIT_DAVout06, m_DAVout06);
    DDX_Text(pDX, IDC_EDIT_DAVout07, m_DAVout07);
    DDX_Text(pDX, IDC_EDIT_DAVout08, m_DAVout08);
}

BEGIN_MESSAGE_MAP(CDA_Vout, CDialog)
    ON_BN_CLICKED(IDC_BUTTON_DA_Vout, OnBUTTONDAVout)
    ON_BN_CLICKED(IDC_BUTTON_Reflesh, OnBUTTONReflesh)
END_MESSAGE_MAP()

void CDA_Vout::OnBUTTONDAVout()
{
    UpdateData(TRUE);
    DigitShowContext* ctx = GetContext();
    ctx->DAVout[0] = m_DAVout01;
    ctx->DAVout[1] = m_DAVout02;
    ctx->DAVout[2] = m_DAVout03;
    ctx->DAVout[3] = m_DAVout04;
    ctx->DAVout[4] = m_DAVout05;
    ctx->DAVout[5] = m_DAVout06;
    ctx->DAVout[6] = m_DAVout07;
    ctx->DAVout[7] = m_DAVout08;
    pDoc->DA_OUTPUT();
}

void CDA_Vout::OnBUTTONReflesh()
{
    DigitShowContext* ctx = GetContext();
    m_DAVout01 = ctx->DAVout[0];
    m_DAVout02 = ctx->DAVout[1];
    m_DAVout03 = ctx->DAVout[2];
    m_DAVout04 = ctx->DAVout[3];
    m_DAVout05 = ctx->DAVout[4];
    m_DAVout06 = ctx->DAVout[5];
    m_DAVout07 = ctx->DAVout[6];
    m_DAVout08 = ctx->DAVout[7];
    UpdateData(FALSE);
}

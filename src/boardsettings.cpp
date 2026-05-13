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
#include "DigitShowBasicDoc.h"
#include "DigitShowContext.h"
#include "BoardSettings.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CBoardSettings::CBoardSettings(CWnd* pParent)
    : CDialog(CBoardSettings::IDD, pParent)
{
    m_ADMethod0 = _T("");
    m_ADMethod1 = _T("");
    m_ADResolution0 = _T("");
    m_ADResolution1 = _T("");
    m_ADRange0 = _T("");
    m_ADRange1 = _T("");
    m_ADMaxChannel0 = _T("");
    m_ADMaxChannel1 = _T("");
    m_DAMaxChannel0 = _T("");
    m_DARange0 = _T("");
    m_DAResolution0 = _T("");
}

void CBoardSettings::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    DDX_Text(pDX, IDC_EDIT_ADMethod0, m_ADMethod0);
    DDX_Text(pDX, IDC_EDIT_ADMethod1, m_ADMethod1);
    DDX_Text(pDX, IDC_EDIT_ADResolution0, m_ADResolution0);
    DDX_Text(pDX, IDC_EDIT_ADResolution1, m_ADResolution1);
    DDX_Text(pDX, IDC_EDIT_ADRange0, m_ADRange0);
    DDX_Text(pDX, IDC_EDIT_ADRange1, m_ADRange1);
    DDX_Text(pDX, IDC_EDIT_ADMaxChannel0, m_ADMaxChannel0);
    DDX_Text(pDX, IDC_EDIT_ADMaxChannel1, m_ADMaxChannel1);
    DDX_Text(pDX, IDC_EDIT_DAMaxChannel0, m_DAMaxChannel0);
    DDX_Text(pDX, IDC_EDIT_DARange0, m_DARange0);
    DDX_Text(pDX, IDC_EDIT_DAResolution0, m_DAResolution0);
}


BEGIN_MESSAGE_MAP(CBoardSettings, CDialog)
    //{{AFX_MSG_MAP(CBoardSettings)
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CBoardSettings メッセージ ハンドラ
BOOL CBoardSettings::OnInitDialog() 
{
    CDialog::OnInitDialog();
    DigitShowContext* ctx = GetContext();

    if(ctx->flags.SetBoard && ctx->ad.InputMethod==0) m_ADMethod0 ="Single Input";
    if(ctx->flags.SetBoard && ctx->ad.InputMethod==1) m_ADMethod0 ="Differential Input";
    if(ctx->flags.SetBoard && ctx->ad.Resolution==12) m_ADResolution0 ="12 bit";
    if(ctx->flags.SetBoard && ctx->ad.Resolution==16) m_ADResolution0 ="16 bit";

    if(ctx->flags.SetBoard && ctx->ad.Range==0) m_ADRange0 ="-10V   +10V";
    if(ctx->flags.SetBoard && ctx->ad.Range==1) m_ADRange0 ="-5V   +5V";
    if(ctx->flags.SetBoard && ctx->ad.Range==50) m_ADRange0 ="0V   +10V";
    if(ctx->flags.SetBoard && ctx->ad.Range==51) m_ADRange0 ="0V   +5V";

    if(ctx->flags.SetBoard) m_ADMaxChannel0.Format("%3d",ctx->ad.Channels/2);
    
    if(ctx->flags.HasDA && ctx->da.Resolution==12) m_DAResolution0 ="12 bit";
    if(ctx->flags.HasDA && ctx->da.Resolution==16) m_DAResolution0 ="16 bit";

    if(ctx->flags.HasDA && ctx->da.Range==0) m_DARange0 ="-10V   +10V";
    if(ctx->flags.HasDA && ctx->da.Range==1) m_DARange0 ="-5V   +5V";
    if(ctx->flags.HasDA && ctx->da.Range==50) m_DARange0 ="0V   +10V";
    if(ctx->flags.HasDA && ctx->da.Range==51) m_DARange0 ="0V   +5V";
    if(ctx->flags.HasDA) m_DAMaxChannel0.Format("%3d",ctx->da.Channels);
    UpdateData(FALSE);    
    return TRUE;
    // コントロールにフォーカスを設定しないとき、戻り値は TRUE となります
                  // 例外: OCX プロパティ ページの戻り値は FALSE となります
}

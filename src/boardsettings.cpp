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

    if(ctx->NumAD >0 && ctx->ad.InputMethod[0]==0) m_ADMethod0 ="Single Input";
    if(ctx->NumAD >0 && ctx->ad.InputMethod[0]==1) m_ADMethod0 ="Differential Input";
    if(ctx->NumAD >1 && ctx->ad.InputMethod[1]==0) m_ADMethod1 ="Single Input";
    if(ctx->NumAD >1 && ctx->ad.InputMethod[1]==1) m_ADMethod1 ="Differential Input";
    if(ctx->NumAD >0 && ctx->ad.Resolution[0]==12) m_ADResolution0 ="12 bit";
    if(ctx->NumAD >0 && ctx->ad.Resolution[0]==16) m_ADResolution0 ="16 bit";
    if(ctx->NumAD >1 && ctx->ad.Resolution[1]==12) m_ADResolution1 ="12 bit";
    if(ctx->NumAD >1 && ctx->ad.Resolution[1]==16) m_ADResolution1 ="16 bit";

    if(ctx->NumAD >0 && ctx->ad.Range[0]==0) m_ADRange0 ="-10V   +10V";
    if(ctx->NumAD >0 && ctx->ad.Range[0]==1) m_ADRange0 ="-5V   +5V";
    if(ctx->NumAD >0 && ctx->ad.Range[0]==50) m_ADRange0 ="0V   +10V";
    if(ctx->NumAD >0 && ctx->ad.Range[0]==51) m_ADRange0 ="0V   +5V";
    if(ctx->NumAD >1 && ctx->ad.Range[1]==0) m_ADRange1 ="-10V   +10V";
    if(ctx->NumAD >1 && ctx->ad.Range[1]==1) m_ADRange1 ="-5V   +5V";
    if(ctx->NumAD >1 && ctx->ad.Range[1]==50) m_ADRange1 ="0V   +10V";
    if(ctx->NumAD >1 && ctx->ad.Range[1]==51) m_ADRange1 ="0V   +5V";

    if(ctx->NumAD >0) m_ADMaxChannel0.Format("%3d",ctx->ad.Channels[0]/2);
    if(ctx->NumAD >1) m_ADMaxChannel1.Format("%3d",ctx->ad.Channels[1]/2);
    
    if(ctx->NumDA >0 && ctx->da.Resolution[0]==12) m_DAResolution0 ="12 bit";
    if(ctx->NumDA >0 && ctx->da.Resolution[0]==16) m_DAResolution0 ="16 bit";

    if(ctx->NumDA >0 && ctx->da.Range[0]==0) m_DARange0 ="-10V   +10V";
    if(ctx->NumDA >0 && ctx->da.Range[0]==1) m_DARange0 ="-5V   +5V";
    if(ctx->NumDA >0 && ctx->da.Range[0]==50) m_DARange0 ="0V   +10V";
    if(ctx->NumDA >0 && ctx->da.Range[0]==51) m_DARange0 ="0V   +5V";
    if(ctx->NumDA >0) m_DAMaxChannel0.Format("%3d",ctx->da.Channels[0]);
    UpdateData(FALSE);    
    return TRUE;
    // コントロールにフォーカスを設定しないとき、戻り値は TRUE となります
                  // 例外: OCX プロパティ ページの戻り値は FALSE となります
}

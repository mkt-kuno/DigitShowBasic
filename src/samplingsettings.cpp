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
#include "SamplingSettings.h"
#include "DigitShowContext.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CSamplingSettings::CSamplingSettings(CWnd* pParent)
    : CDialog(CSamplingSettings::IDD, pParent)
{
    m_TimeInterval1 = 0;
    m_TimeInterval2 = 0;
    m_TimeInterval3 = 0;
    m_AllocatedMemory = _T("");
    m_AvSmplNum = 0;
    m_Channels = 0;
    m_EventSamplingTimes = 0;
    m_MemoryType = _T("");
    m_SamplingClock = 0.0f;
    m_SavingTime = 0;
    m_TotalSamplingTimes = 0;
}

void CSamplingSettings::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    DDX_Text(pDX, IDC_EDIT_TimeInterval1, m_TimeInterval1);
    DDX_Text(pDX, IDC_EDIT_TimeInterval2, m_TimeInterval2);
    DDX_Text(pDX, IDC_EDIT_TimeInterval3, m_TimeInterval3);
    DDX_Text(pDX, IDC_EDIT_AllocatedMemory, m_AllocatedMemory);
    DDX_Text(pDX, IDC_EDIT_AvSmplNum, m_AvSmplNum);
    DDX_Text(pDX, IDC_EDIT_Channels, m_Channels);
    DDX_Text(pDX, IDC_EDIT_EventSamplingTimes, m_EventSamplingTimes);
    DDX_Text(pDX, IDC_EDIT_MemoryType, m_MemoryType);
    DDX_Text(pDX, IDC_EDIT_SamplingClock, m_SamplingClock);
    DDX_Text(pDX, IDC_EDIT_SavingTime, m_SavingTime);
    DDX_Text(pDX, IDC_EDIT_TotalSamplingTimes, m_TotalSamplingTimes);
    //}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CSamplingSettings, CDialog)
    //{{AFX_MSG_MAP(CSamplingSettings)
    ON_BN_CLICKED(IDC_BUTTON_Check, OnBUTTONCheck)
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSamplingSettings メッセージ ハンドラ

BOOL CSamplingSettings::OnInitDialog() 
{
    CDialog::OnInitDialog();
    DigitShowContext* ctx = GetContext();

    m_TimeInterval1 = ctx->timeSettings.Interval1;
    m_TimeInterval2 = ctx->timeSettings.Interval2;
    m_TimeInterval3 = ctx->timeSettings.Interval3;
    m_AllocatedMemory.Format("%.1f", ctx->sampling.AllocatedMemory);
    m_Channels = ctx->AdMaxChannels;
    m_EventSamplingTimes = ctx->ad.SamplingTimes[0];
    m_AvSmplNum = ctx->sampling.AvSmplNum;
    if(ctx->ad.MemoryType[0]==0) m_MemoryType = _T("FIFO");
    if(ctx->ad.MemoryType[0]==1) m_MemoryType = _T("RING");
    m_SamplingClock = ctx->ad.SamplingClock[0]/1000.0f;
    m_SavingTime = ctx->sampling.SavingTime;
    m_TotalSamplingTimes = ctx->sampling.TotalSamplingTimes;
    UpdateData(FALSE);
    CButton* myBTN1 = (CButton*)GetDlgItem(IDC_BUTTON_Check);
    CButton* myBTN2 = (CButton*)GetDlgItem(IDOK);
    if(ctx->FlagFIFO==TRUE)    myBTN1->EnableWindow(FALSE);
    myBTN2->EnableWindow(FALSE);
    
    return TRUE;
    // コントロールにフォーカスを設定しないとき、戻り値は TRUE となります
                  // 例外: OCX プロパティ ページの戻り値は FALSE となります
}


void CSamplingSettings::OnBUTTONCheck() 
{

    UpdateData(TRUE);
    DigitShowContext* ctx = GetContext();
    m_TotalSamplingTimes = long(m_SavingTime*1000/m_SamplingClock);
    m_AllocatedMemory.Format("%.1f",4*ctx->AdMaxChannels*m_TotalSamplingTimes/1024.0f/1024.0f);
    m_EventSamplingTimes = long(ctx->timeSettings.Interval1/m_SamplingClock);
    UpdateData(FALSE);

    CButton* myBTN1 = (CButton*)GetDlgItem(IDOK);
    myBTN1->EnableWindow(TRUE);
    
}

void CSamplingSettings::OnOK() 
{

    UpdateData(TRUE);
    DigitShowContext* ctx = GetContext();
    ctx->ad.SamplingClock[0] = m_SamplingClock*1000.0f;
    ctx->sampling.SavingTime = m_SavingTime;
    ctx->ad.SamplingTimes[0] = m_EventSamplingTimes;
    ctx->sampling.TotalSamplingTimes = long(ctx->sampling.SavingTime*1000000/ctx->ad.SamplingClock[0]);
    ctx->sampling.AllocatedMemory = 4*ctx->AdMaxChannels*m_TotalSamplingTimes/1024.0f/1024.0f;
    m_AllocatedMemory.Format("%.1f", ctx->sampling.AllocatedMemory);
    m_TotalSamplingTimes = ctx->sampling.TotalSamplingTimes;
    UpdateData(FALSE);

    if(ctx->NumAD>1){
        ctx->ad.SamplingClock[1] = ctx->ad.SamplingClock[0];
        ctx->ad.SamplingTimes[1] = ctx->ad.SamplingTimes[0];
    }
    
    CDialog::OnOK();
}


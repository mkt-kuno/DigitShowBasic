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
#include "CalibrationFactor.h"

#include "CalibrationAmp.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CCalibrationFactor::CCalibrationFactor(CWnd* pParent /*=NULL*/)
    : CDialog(CCalibrationFactor::IDD, pParent)
{
    //{{AFX_DATA_INIT(CCalibrationFactor)
    m_CFP00 = _T("");
    m_CFP01 = _T("");
    m_CFP02 = _T("");
    m_CFP03 = _T("");
    m_CFP04 = _T("");
    m_CFP05 = _T("");
    m_CFP06 = _T("");
    m_CFP07 = _T("");
    m_CFP08 = _T("");
    m_CFP09 = _T("");
    m_CFP10 = _T("");
    m_CFP11 = _T("");
    m_CFP12 = _T("");
    m_CFP13 = _T("");
    m_CFP14 = _T("");
    m_CFP15 = _T("");
    m_CFA00 = 0.0;
    m_CFB00 = 0.0;
    m_CFC00 = 0.0;
    m_CFA01 = 0.0;
    m_CFB01 = 0.0;
    m_CFC01 = 0.0;
    m_CFA02 = 0.0;
    m_CFB02 = 0.0;
    m_CFC02 = 0.0;
    m_CFA03 = 0.0;
    m_CFB03 = 0.0;
    m_CFC03 = 0.0;
    m_CFA04 = 0.0;
    m_CFB04 = 0.0;
    m_CFC04 = 0.0;
    m_CFA05 = 0.0;
    m_CFB05 = 0.0;
    m_CFC05 = 0.0;
    m_CFA06 = 0.0;
    m_CFB06 = 0.0;
    m_CFC06 = 0.0;
    m_CFA07 = 0.0;
    m_CFB07 = 0.0;
    m_CFC07 = 0.0;
    m_CFA08 = 0.0;
    m_CFB08 = 0.0;
    m_CFC08 = 0.0;
    m_CFA09 = 0.0;
    m_CFB09 = 0.0;
    m_CFC09 = 0.0;
    m_CFA10 = 0.0;
    m_CFB10 = 0.0;
    m_CFC10 = 0.0;
    m_CFA11 = 0.0;
    m_CFB11 = 0.0;
    m_CFC11 = 0.0;
    m_CFA12 = 0.0;
    m_CFB12 = 0.0;
    m_CFC12 = 0.0;
    m_CFA13 = 0.0;
    m_CFB13 = 0.0;
    m_CFC13 = 0.0;
    m_CFA14 = 0.0;
    m_CFB14 = 0.0;
    m_CFC14 = 0.0;
    m_CFA15 = 0.0;
    m_CFB15 = 0.0;
    m_CFC15 = 0.0;
    m_Channels = _T("");
    m_C00 = _T("");
    m_C01 = _T("");
    m_C02 = _T("");
    m_C03 = _T("");
    m_C04 = _T("");
    m_C05 = _T("");
    m_C06 = _T("");
    m_C07 = _T("");
    m_C08 = _T("");
    m_C09 = _T("");
    m_C10 = _T("");
    m_C11 = _T("");
    m_C12 = _T("");
    m_C13 = _T("");
    m_C14 = _T("");
    m_C15 = _T("");
    //}}AFX_DATA_INIT
}

void CCalibrationFactor::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    //{{AFX_DATA_MAP(CCalibrationFactor)
    DDX_Text(pDX, IDC_EDIT_CFP00, m_CFP00);
    DDX_Text(pDX, IDC_EDIT_CFP01, m_CFP01);
    DDX_Text(pDX, IDC_EDIT_CFP02, m_CFP02);
    DDX_Text(pDX, IDC_EDIT_CFP03, m_CFP03);
    DDX_Text(pDX, IDC_EDIT_CFP04, m_CFP04);
    DDX_Text(pDX, IDC_EDIT_CFP05, m_CFP05);
    DDX_Text(pDX, IDC_EDIT_CFP06, m_CFP06);
    DDX_Text(pDX, IDC_EDIT_CFP07, m_CFP07);
    DDX_Text(pDX, IDC_EDIT_CFP08, m_CFP08);
    DDX_Text(pDX, IDC_EDIT_CFP09, m_CFP09);
    DDX_Text(pDX, IDC_EDIT_CFP10, m_CFP10);
    DDX_Text(pDX, IDC_EDIT_CFP11, m_CFP11);
    DDX_Text(pDX, IDC_EDIT_CFP12, m_CFP12);
    DDX_Text(pDX, IDC_EDIT_CFP13, m_CFP13);    
    DDX_Text(pDX, IDC_EDIT_CFP14, m_CFP14);
    DDX_Text(pDX, IDC_EDIT_CFP15, m_CFP15);
    DDX_Text(pDX, IDC_EDIT_CFA00, m_CFA00);
    DDX_Text(pDX, IDC_EDIT_CFA01, m_CFA01);
    DDX_Text(pDX, IDC_EDIT_CFA02, m_CFA02);
    DDX_Text(pDX, IDC_EDIT_CFA03, m_CFA03);
    DDX_Text(pDX, IDC_EDIT_CFA04, m_CFA04);
    DDX_Text(pDX, IDC_EDIT_CFA05, m_CFA05);
    DDX_Text(pDX, IDC_EDIT_CFA06, m_CFA06);
    DDX_Text(pDX, IDC_EDIT_CFA07, m_CFA07);
    DDX_Text(pDX, IDC_EDIT_CFA08, m_CFA08);
    DDX_Text(pDX, IDC_EDIT_CFA09, m_CFA09);
    DDX_Text(pDX, IDC_EDIT_CFA10, m_CFA10);
    DDX_Text(pDX, IDC_EDIT_CFA11, m_CFA11);
    DDX_Text(pDX, IDC_EDIT_CFA12, m_CFA12);
    DDX_Text(pDX, IDC_EDIT_CFA13, m_CFA13);
    DDX_Text(pDX, IDC_EDIT_CFA14, m_CFA14);
    DDX_Text(pDX, IDC_EDIT_CFA15, m_CFA15);
    DDX_Text(pDX, IDC_EDIT_CFB00, m_CFB00);
    DDX_Text(pDX, IDC_EDIT_CFB01, m_CFB01);
    DDX_Text(pDX, IDC_EDIT_CFB02, m_CFB02);
    DDX_Text(pDX, IDC_EDIT_CFB03, m_CFB03);
    DDX_Text(pDX, IDC_EDIT_CFB04, m_CFB04);
    DDX_Text(pDX, IDC_EDIT_CFB05, m_CFB05);
    DDX_Text(pDX, IDC_EDIT_CFB06, m_CFB06);
    DDX_Text(pDX, IDC_EDIT_CFB07, m_CFB07);
    DDX_Text(pDX, IDC_EDIT_CFB08, m_CFB08);
    DDX_Text(pDX, IDC_EDIT_CFB09, m_CFB09);
    DDX_Text(pDX, IDC_EDIT_CFB10, m_CFB10);
    DDX_Text(pDX, IDC_EDIT_CFB11, m_CFB11);
    DDX_Text(pDX, IDC_EDIT_CFB12, m_CFB12);
    DDX_Text(pDX, IDC_EDIT_CFB13, m_CFB13);
    DDX_Text(pDX, IDC_EDIT_CFB14, m_CFB14);
    DDX_Text(pDX, IDC_EDIT_CFB15, m_CFB15);
    DDX_Text(pDX, IDC_EDIT_CFC00, m_CFC00);
    DDX_Text(pDX, IDC_EDIT_CFC01, m_CFC01);
    DDX_Text(pDX, IDC_EDIT_CFC02, m_CFC02);
    DDX_Text(pDX, IDC_EDIT_CFC03, m_CFC03);
    DDX_Text(pDX, IDC_EDIT_CFC04, m_CFC04);
    DDX_Text(pDX, IDC_EDIT_CFC05, m_CFC05);
    DDX_Text(pDX, IDC_EDIT_CFC06, m_CFC06);
    DDX_Text(pDX, IDC_EDIT_CFC07, m_CFC07);
    DDX_Text(pDX, IDC_EDIT_CFC08, m_CFC08);
    DDX_Text(pDX, IDC_EDIT_CFC09, m_CFC09);
    DDX_Text(pDX, IDC_EDIT_CFC10, m_CFC10);
    DDX_Text(pDX, IDC_EDIT_CFC11, m_CFC11);
    DDX_Text(pDX, IDC_EDIT_CFC12, m_CFC12);
    DDX_Text(pDX, IDC_EDIT_CFC13, m_CFC13);
    DDX_Text(pDX, IDC_EDIT_CFC14, m_CFC14);
    DDX_Text(pDX, IDC_EDIT_CFC15, m_CFC15);
    DDX_Text(pDX, IDC_EDIT_Channels, m_Channels);
    DDX_Text(pDX, IDC_STATIC_C00, m_C00);
    DDX_Text(pDX, IDC_STATIC_C01, m_C01);
    DDX_Text(pDX, IDC_STATIC_C02, m_C02);
    DDX_Text(pDX, IDC_STATIC_C03, m_C03);
    DDX_Text(pDX, IDC_STATIC_C04, m_C04);
    DDX_Text(pDX, IDC_STATIC_C05, m_C05);
    DDX_Text(pDX, IDC_STATIC_C06, m_C06);
    DDX_Text(pDX, IDC_STATIC_C07, m_C07);
    DDX_Text(pDX, IDC_STATIC_C08, m_C08);
    DDX_Text(pDX, IDC_STATIC_C09, m_C09);
    DDX_Text(pDX, IDC_STATIC_C10, m_C10);
    DDX_Text(pDX, IDC_STATIC_C11, m_C11);
    DDX_Text(pDX, IDC_STATIC_C12, m_C12);
    DDX_Text(pDX, IDC_STATIC_C13, m_C13);
    DDX_Text(pDX, IDC_STATIC_C14, m_C14);
    DDX_Text(pDX, IDC_STATIC_C15, m_C15);
    //}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CCalibrationFactor, CDialog)
    //{{AFX_MSG_MAP(CCalibrationFactor)
    ON_BN_CLICKED(IDC_BUTTON_CFUpdate, OnBUTTONCFUpdate)
    ON_BN_CLICKED(IDC_BUTTON_Zero00, OnBUTTONZero00)
    ON_BN_CLICKED(IDC_BUTTON_Zero01, OnBUTTONZero01)
    ON_BN_CLICKED(IDC_BUTTON_Zero02, OnBUTTONZero02)
    ON_BN_CLICKED(IDC_BUTTON_Zero03, OnBUTTONZero03)
    ON_BN_CLICKED(IDC_BUTTON_Zero04, OnBUTTONZero04)
    ON_BN_CLICKED(IDC_BUTTON_Zero05, OnBUTTONZero05)
    ON_BN_CLICKED(IDC_BUTTON_Zero06, OnBUTTONZero06)
    ON_BN_CLICKED(IDC_BUTTON_Zero07, OnBUTTONZero07)
    ON_BN_CLICKED(IDC_BUTTON_Zero08, OnBUTTONZero08)
    ON_BN_CLICKED(IDC_BUTTON_Zero09, OnBUTTONZero09)
    ON_BN_CLICKED(IDC_BUTTON_Zero10, OnBUTTONZero10)
    ON_BN_CLICKED(IDC_BUTTON_Zero11, OnBUTTONZero11)
    ON_BN_CLICKED(IDC_BUTTON_Zero12, OnBUTTONZero12)
    ON_BN_CLICKED(IDC_BUTTON_Zero13, OnBUTTONZero13)
    ON_BN_CLICKED(IDC_BUTTON_Zero14, OnBUTTONZero14)
    ON_BN_CLICKED(IDC_BUTTON_Zero15, OnBUTTONZero15)
    ON_BN_CLICKED(IDC_BUTTON_Amp00, OnBUTTONAmp00)
    ON_BN_CLICKED(IDC_BUTTON_Amp01, OnBUTTONAmp01)
    ON_BN_CLICKED(IDC_BUTTON_Amp02, OnBUTTONAmp02)
    ON_BN_CLICKED(IDC_BUTTON_Amp03, OnBUTTONAmp03)
    ON_BN_CLICKED(IDC_BUTTON_Amp04, OnBUTTONAmp04)
    ON_BN_CLICKED(IDC_BUTTON_Amp05, OnBUTTONAmp05)
    ON_BN_CLICKED(IDC_BUTTON_Amp06, OnBUTTONAmp06)
    ON_BN_CLICKED(IDC_BUTTON_Amp07, OnBUTTONAmp07)
    ON_BN_CLICKED(IDC_BUTTON_Amp08, OnBUTTONAmp08)
    ON_BN_CLICKED(IDC_BUTTON_Amp09, OnBUTTONAmp09)
    ON_BN_CLICKED(IDC_BUTTON_Amp10, OnBUTTONAmp10)
    ON_BN_CLICKED(IDC_BUTTON_Amp11, OnBUTTONAmp11)
    ON_BN_CLICKED(IDC_BUTTON_Amp12, OnBUTTONAmp12)
    ON_BN_CLICKED(IDC_BUTTON_Amp13, OnBUTTONAmp13)
    ON_BN_CLICKED(IDC_BUTTON_Amp14, OnBUTTONAmp14)
    ON_BN_CLICKED(IDC_BUTTON_Amp15, OnBUTTONAmp15)
    ON_BN_CLICKED(IDC_BUTTON_CFLoadFile, OnBUTTONCFLoadFile)
    ON_BN_CLICKED(IDC_BUTTON_CFSave, OnBUTTONCFSave)
    ON_BN_CLICKED(IDC_BUTTON_ChannelChange, OnBUTTONChannelChange)
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCalibrationFactor メッセージ ハンドラ

BOOL CCalibrationFactor::OnInitDialog() 
{
    CDialog::OnInitDialog();
    m_Channels = _T("CH0-CH15");
    CF_Load();

    
    return TRUE;
    // コントロールにフォーカスを設定しないとき、戻り値は TRUE となります
                  // 例外: OCX プロパティ ページの戻り値は FALSE となります
}

void CCalibrationFactor::CF_Load()
{
    DigitShowContext* ctx = GetContext();
    pDoc->Cal_Physical();
    if(m_Channels=="CH0-CH15"){    
        m_CFA00 = ctx->cal.a[0];
        m_CFB00 = ctx->cal.b[0];
        m_CFC00 = ctx->cal.c[0];
        m_CFP00.Format("%11.5f",ctx->Phyout[0]);
        m_CFA01 = ctx->cal.a[1];
        m_CFB01 = ctx->cal.b[1];
        m_CFC01 = ctx->cal.c[1];
        m_CFP01.Format("%11.5f",ctx->Phyout[1]);
        m_CFA02 = ctx->cal.a[2];
        m_CFB02 = ctx->cal.b[2];
        m_CFC02 = ctx->cal.c[2];
        m_CFP02.Format("%11.5f",ctx->Phyout[2]);
        m_CFA03 = ctx->cal.a[3];
        m_CFB03 = ctx->cal.b[3];
        m_CFC03 = ctx->cal.c[3];
        m_CFP03.Format("%11.5f",ctx->Phyout[3]);
        m_CFA04 = ctx->cal.a[4];
        m_CFB04 = ctx->cal.b[4];
        m_CFC04 = ctx->cal.c[4];
        m_CFP04.Format("%11.5f",ctx->Phyout[4]);
        m_CFA05 = ctx->cal.a[5];
        m_CFB05 = ctx->cal.b[5];
        m_CFC05 = ctx->cal.c[5];
        m_CFP05.Format("%11.5f",ctx->Phyout[5]);
        m_CFA06 = ctx->cal.a[6];
        m_CFB06 = ctx->cal.b[6];
        m_CFC06 = ctx->cal.c[6];
        m_CFP06.Format("%11.5f",ctx->Phyout[6]);
        m_CFA07 = ctx->cal.a[7];
        m_CFB07 = ctx->cal.b[7];
        m_CFC07 = ctx->cal.c[7];
        m_CFP07.Format("%11.5f",ctx->Phyout[7]);
        m_CFA08 = ctx->cal.a[8];
        m_CFB08 = ctx->cal.b[8];
        m_CFC08 = ctx->cal.c[8];
        m_CFP08.Format("%11.5f",ctx->Phyout[8]);
        m_CFA09 = ctx->cal.a[9];
        m_CFB09 = ctx->cal.b[9];
        m_CFC09 = ctx->cal.c[9];
        m_CFP09.Format("%11.5f",ctx->Phyout[9]);
        m_CFA10 = ctx->cal.a[10];
        m_CFB10 = ctx->cal.b[10];
        m_CFC10 = ctx->cal.c[10];
        m_CFP10.Format("%11.5f",ctx->Phyout[10]);
        m_CFA11 = ctx->cal.a[11];
        m_CFB11 = ctx->cal.b[11];
        m_CFC11 = ctx->cal.c[11];
        m_CFP11.Format("%11.5f",ctx->Phyout[11]);
        m_CFA12 = ctx->cal.a[12];
        m_CFB12 = ctx->cal.b[12];
        m_CFC12 = ctx->cal.c[12];
        m_CFP12.Format("%11.5f",ctx->Phyout[12]);
        m_CFA13 = ctx->cal.a[13];
        m_CFB13 = ctx->cal.b[13];
        m_CFC13 = ctx->cal.c[13];
        m_CFP13.Format("%11.5f",ctx->Phyout[13]);
        m_CFA14 = ctx->cal.a[14];
        m_CFB14 = ctx->cal.b[14];
        m_CFC14 = ctx->cal.c[14];
        m_CFP14.Format("%11.5f",ctx->Phyout[14]);
        m_CFA15 = ctx->cal.a[15];
        m_CFB15 = ctx->cal.b[15];
        m_CFC15 = ctx->cal.c[15];
        m_CFP15.Format("%11.5f",ctx->Phyout[15]);
        m_C00 = _T("CH00, Load Cell");
        m_C01 = _T("CH01, Displacement");
        m_C02 = _T("CH02, Cell Pre.");
        m_C03 = _T("CH03, Effect.Cell Pre. ");
        m_C04 = _T("CH04, Drained Vol.");
        m_C05 = _T("CH05, LVDT1");
        m_C06 = _T("CH06, LVDT2");
        m_C07 = _T("CH07");
        m_C08 = _T("CH08");
        m_C09 = _T("CH09");
        m_C10 = _T("CH10");
        m_C11 = _T("CH11");
        m_C12 = _T("CH12");
        m_C13 = _T("CH13");
        m_C14 = _T("CH14");
        m_C15 = _T("CH15");
    }
    if(m_Channels=="CH16-CH31"){    
        m_CFA00 = ctx->cal.a[16];
        m_CFB00 = ctx->cal.b[16];
        m_CFC00 = ctx->cal.c[16];
        m_CFP00.Format("%11.5f",ctx->Phyout[16]);
        m_CFA01 = ctx->cal.a[17];
        m_CFB01 = ctx->cal.b[17];
        m_CFC01 = ctx->cal.c[17];
        m_CFP01.Format("%11.5f",ctx->Phyout[17]);
        m_CFA02 = ctx->cal.a[18];
        m_CFB02 = ctx->cal.b[18];
        m_CFC02 = ctx->cal.c[18];
        m_CFP02.Format("%11.5f",ctx->Phyout[18]);
        m_CFA03 = ctx->cal.a[19];
        m_CFB03 = ctx->cal.b[19];
        m_CFC03 = ctx->cal.c[19];
        m_CFP03.Format("%11.5f",ctx->Phyout[19]);
        m_CFA04 = ctx->cal.a[20];
        m_CFB04 = ctx->cal.b[20];
        m_CFC04 = ctx->cal.c[20];
        m_CFP04.Format("%11.5f",ctx->Phyout[20]);
        m_CFA05 = ctx->cal.a[21];
        m_CFB05 = ctx->cal.b[21];
        m_CFC05 = ctx->cal.c[21];
        m_CFP05.Format("%11.5f",ctx->Phyout[21]);
        m_CFA06 = ctx->cal.a[22];
        m_CFB06 = ctx->cal.b[22];
        m_CFC06 = ctx->cal.c[22];
        m_CFP06.Format("%11.5f",ctx->Phyout[22]);
        m_CFA07 = ctx->cal.a[23];
        m_CFB07 = ctx->cal.b[23];
        m_CFC07 = ctx->cal.c[23];
        m_CFP07.Format("%11.5f",ctx->Phyout[23]);
        m_CFA08 = ctx->cal.a[24];
        m_CFB08 = ctx->cal.b[24];
        m_CFC08 = ctx->cal.c[24];
        m_CFP08.Format("%11.5f",ctx->Phyout[24]);
        m_CFA09 = ctx->cal.a[25];
        m_CFB09 = ctx->cal.b[25];
        m_CFC09 = ctx->cal.c[25];
        m_CFP09.Format("%11.5f",ctx->Phyout[25]);
        m_CFA10 = ctx->cal.a[26];
        m_CFB10 = ctx->cal.b[26];
        m_CFC10 = ctx->cal.c[26];
        m_CFP10.Format("%11.5f",ctx->Phyout[26]);
        m_CFA11 = ctx->cal.a[27];
        m_CFB11 = ctx->cal.b[27];
        m_CFC11 = ctx->cal.c[27];
        m_CFP11.Format("%11.5f",ctx->Phyout[27]);
        m_CFA12 = ctx->cal.a[28];
        m_CFB12 = ctx->cal.b[28];
        m_CFC12 = ctx->cal.c[28];
        m_CFP12.Format("%11.5f",ctx->Phyout[28]);
        m_CFA13 = ctx->cal.a[29];
        m_CFB13 = ctx->cal.b[29];
        m_CFC13 = ctx->cal.c[29];
        m_CFP13.Format("%11.5f",ctx->Phyout[29]);
        m_CFA14 = ctx->cal.a[30];
        m_CFB14 = ctx->cal.b[30];
        m_CFC14 = ctx->cal.c[30];
        m_CFP14.Format("%11.5f",ctx->Phyout[30]);
        m_CFA15 = ctx->cal.a[31];
        m_CFB15 = ctx->cal.b[31];
        m_CFC15 = ctx->cal.c[31];
        m_CFP15.Format("%11.5f",ctx->Phyout[31]);
        m_C00 = _T("CH16");
        m_C01 = _T("CH17");
        m_C02 = _T("CH18");
        m_C03 = _T("CH19");
        m_C04 = _T("CH20");
        m_C05 = _T("CH21");
        m_C06 = _T("CH22");
        m_C07 = _T("CH23");
        m_C08 = _T("CH24");
        m_C09 = _T("CH25");
        m_C10 = _T("CH26");
        m_C11 = _T("CH27");
        m_C12 = _T("CH28");
        m_C13 = _T("CH29");
        m_C14 = _T("CH30");
        m_C15 = _T("CH31");
    }
    UpdateData(FALSE);
}

void CCalibrationFactor::OnBUTTONCFUpdate() 
{
    DigitShowContext* ctx = GetContext();
    UpdateData(TRUE);
    if(m_Channels=="CH0-CH15"){    
        ctx->cal.a[0] = m_CFA00;
        ctx->cal.b[0] = m_CFB00;
        ctx->cal.c[0] = m_CFC00;
        ctx->cal.a[1] = m_CFA01;
        ctx->cal.b[1] = m_CFB01;
        ctx->cal.c[1] = m_CFC01;
        ctx->cal.a[2] = m_CFA02;
        ctx->cal.b[2] = m_CFB02;
        ctx->cal.c[2] = m_CFC02;
        ctx->cal.a[3] = m_CFA03;
        ctx->cal.b[3] = m_CFB03;
        ctx->cal.c[3] = m_CFC03;
        ctx->cal.a[4] = m_CFA04;
        ctx->cal.b[4] = m_CFB04;
        ctx->cal.c[4] = m_CFC04;
        ctx->cal.a[5] = m_CFA05;
        ctx->cal.b[5] = m_CFB05;
        ctx->cal.c[5] = m_CFC05;
        ctx->cal.a[6] = m_CFA06;
        ctx->cal.b[6] = m_CFB06;
        ctx->cal.c[6] = m_CFC06;
        ctx->cal.a[7] = m_CFA07;
        ctx->cal.b[7] = m_CFB07;
        ctx->cal.c[7] = m_CFC07;
        ctx->cal.a[8] = m_CFA08;
        ctx->cal.b[8] = m_CFB08;
        ctx->cal.c[8] = m_CFC08;
        ctx->cal.a[9] = m_CFA09;
        ctx->cal.b[9] = m_CFB09;
        ctx->cal.c[9] = m_CFC09;
        ctx->cal.a[10] = m_CFA10;
        ctx->cal.b[10] = m_CFB10;
        ctx->cal.c[10] = m_CFC10;
        ctx->cal.a[11] = m_CFA11;
        ctx->cal.b[11] = m_CFB11;
        ctx->cal.c[11] = m_CFC11;
        ctx->cal.a[12] = m_CFA12;
        ctx->cal.b[12] = m_CFB12;
        ctx->cal.c[12] = m_CFC12;
        ctx->cal.a[13] = m_CFA13;
        ctx->cal.b[13] = m_CFB13;
        ctx->cal.c[13] = m_CFC13;
        ctx->cal.a[14] = m_CFA14;
        ctx->cal.b[14] = m_CFB14;
        ctx->cal.c[14] = m_CFC14;
        ctx->cal.a[15] = m_CFA15;
        ctx->cal.b[15] = m_CFB15;
        ctx->cal.c[15] = m_CFC15;
    }
    if(m_Channels=="CH16-CH31"){    
        ctx->cal.a[16] = m_CFA00;
        ctx->cal.b[16] = m_CFB00;
        ctx->cal.c[16] = m_CFC00;
        ctx->cal.a[17] = m_CFA01;
        ctx->cal.b[17] = m_CFB01;
        ctx->cal.c[17] = m_CFC01;
        ctx->cal.a[18] = m_CFA02;
        ctx->cal.b[18] = m_CFB02;
        ctx->cal.c[18] = m_CFC02;
        ctx->cal.a[19] = m_CFA03;
        ctx->cal.b[19] = m_CFB03;
        ctx->cal.c[19] = m_CFC03;
        ctx->cal.a[20] = m_CFA04;
        ctx->cal.b[20] = m_CFB04;
        ctx->cal.c[20] = m_CFC04;
        ctx->cal.a[21] = m_CFA05;
        ctx->cal.b[21] = m_CFB05;
        ctx->cal.c[21] = m_CFC05;
        ctx->cal.a[22] = m_CFA06;
        ctx->cal.b[22] = m_CFB06;
        ctx->cal.c[22] = m_CFC06;
        ctx->cal.a[23] = m_CFA07;
        ctx->cal.b[23] = m_CFB07;
        ctx->cal.c[23] = m_CFC07;
        ctx->cal.a[24] = m_CFA08;
        ctx->cal.b[24] = m_CFB08;
        ctx->cal.c[24] = m_CFC08;
        ctx->cal.a[25] = m_CFA09;
        ctx->cal.b[25] = m_CFB09;
        ctx->cal.c[25] = m_CFC09;
        ctx->cal.a[26] = m_CFA10;
        ctx->cal.b[26] = m_CFB10;
        ctx->cal.c[26] = m_CFC10;
        ctx->cal.a[27] = m_CFA11;
        ctx->cal.b[27] = m_CFB11;
        ctx->cal.c[27] = m_CFC11;
        ctx->cal.a[28] = m_CFA12;
        ctx->cal.b[28] = m_CFB12;
        ctx->cal.c[28] = m_CFC12;
        ctx->cal.a[29] = m_CFA13;
        ctx->cal.b[29] = m_CFB13;
        ctx->cal.c[29] = m_CFC13;
        ctx->cal.a[30] = m_CFA14;
        ctx->cal.b[30] = m_CFB14;
        ctx->cal.c[30] = m_CFC14;
        ctx->cal.a[31] = m_CFA15;
        ctx->cal.b[31] = m_CFB15;
        ctx->cal.c[31] = m_CFC15;
    }
}


void CCalibrationFactor::OnBUTTONZero00() 
{
    DigitShowContext* ctx = GetContext();
    OnBUTTONCFUpdate();
    if(m_Channels=="CH0-CH15")    ctx->cal.c[0] = ctx->cal.c[0]-ctx->Phyout[0];
    if(m_Channels=="CH16-CH31")    ctx->cal.c[16] = ctx->cal.c[16]-ctx->Phyout[16];
    CF_Load();
}

void CCalibrationFactor::OnBUTTONZero01() 
{
    DigitShowContext* ctx = GetContext();
    OnBUTTONCFUpdate();
    if(m_Channels=="CH0-CH15")    ctx->cal.c[1] = ctx->cal.c[1]-ctx->Phyout[1];
    if(m_Channels=="CH16-CH31")    ctx->cal.c[17] = ctx->cal.c[17]-ctx->Phyout[17];
    CF_Load();
}

void CCalibrationFactor::OnBUTTONZero02() 
{
    DigitShowContext* ctx = GetContext();
    OnBUTTONCFUpdate();
    if(m_Channels=="CH0-CH15")    ctx->cal.c[2] = ctx->cal.c[2]-ctx->Phyout[2];
    if(m_Channels=="CH16-CH31")    ctx->cal.c[18] = ctx->cal.c[18]-ctx->Phyout[18];
    CF_Load();
}

void CCalibrationFactor::OnBUTTONZero03() 
{
    DigitShowContext* ctx = GetContext();
    OnBUTTONCFUpdate();
    if(m_Channels=="CH0-CH15")    ctx->cal.c[3] = ctx->cal.c[3]-ctx->Phyout[3];
    if(m_Channels=="CH16-CH31")    ctx->cal.c[19] = ctx->cal.c[19]-ctx->Phyout[19];
    CF_Load();    
}

void CCalibrationFactor::OnBUTTONZero04() 
{
    DigitShowContext* ctx = GetContext();
    OnBUTTONCFUpdate();
    if(m_Channels=="CH0-CH15")    ctx->cal.c[4] = ctx->cal.c[4]-ctx->Phyout[4];
    if(m_Channels=="CH16-CH31")    ctx->cal.c[20] = ctx->cal.c[20]-ctx->Phyout[20];
    CF_Load();    
}

void CCalibrationFactor::OnBUTTONZero05() 
{
    DigitShowContext* ctx = GetContext();
    OnBUTTONCFUpdate();
    if(m_Channels=="CH0-CH15")    ctx->cal.c[5] = ctx->cal.c[5]-ctx->Phyout[5];
    if(m_Channels=="CH16-CH31")    ctx->cal.c[21] = ctx->cal.c[21]-ctx->Phyout[21];
    CF_Load();
}

void CCalibrationFactor::OnBUTTONZero06() 
{
    DigitShowContext* ctx = GetContext();
    OnBUTTONCFUpdate();
    if(m_Channels=="CH0-CH15")    ctx->cal.c[6] = ctx->cal.c[6]-ctx->Phyout[6];
    if(m_Channels=="CH16-CH31")    ctx->cal.c[22] = ctx->cal.c[22]-ctx->Phyout[22];
    CF_Load();
}

void CCalibrationFactor::OnBUTTONZero07() 
{
    DigitShowContext* ctx = GetContext();
    OnBUTTONCFUpdate();
    if(m_Channels=="CH0-CH15")    ctx->cal.c[7] = ctx->cal.c[7]-ctx->Phyout[7];
    if(m_Channels=="CH16-CH31")    ctx->cal.c[23] = ctx->cal.c[23]-ctx->Phyout[23];
    CF_Load();
}

void CCalibrationFactor::OnBUTTONZero08() 
{
    DigitShowContext* ctx = GetContext();
    OnBUTTONCFUpdate();
    if(m_Channels=="CH0-CH15")    ctx->cal.c[8] = ctx->cal.c[8]-ctx->Phyout[8];
    if(m_Channels=="CH16-CH31")    ctx->cal.c[24] = ctx->cal.c[24]-ctx->Phyout[24];
    CF_Load();
}

void CCalibrationFactor::OnBUTTONZero09() 
{
    DigitShowContext* ctx = GetContext();
    OnBUTTONCFUpdate();
    if(m_Channels=="CH0-CH15")    ctx->cal.c[9] = ctx->cal.c[9]-ctx->Phyout[9];
    if(m_Channels=="CH16-CH31")    ctx->cal.c[25] = ctx->cal.c[25]-ctx->Phyout[25];
    CF_Load();
}

void CCalibrationFactor::OnBUTTONZero10() 
{
    DigitShowContext* ctx = GetContext();
    OnBUTTONCFUpdate();
    if(m_Channels=="CH0-CH15")    ctx->cal.c[10] = ctx->cal.c[10]-ctx->Phyout[10];
    if(m_Channels=="CH16-CH31")    ctx->cal.c[26] = ctx->cal.c[26]-ctx->Phyout[26];
    CF_Load();
}

void CCalibrationFactor::OnBUTTONZero11() 
{
    DigitShowContext* ctx = GetContext();
    OnBUTTONCFUpdate();
    if(m_Channels=="CH0-CH15")    ctx->cal.c[11] = ctx->cal.c[11]-ctx->Phyout[11];
    if(m_Channels=="CH16-CH31")    ctx->cal.c[27] = ctx->cal.c[27]-ctx->Phyout[27];
    CF_Load();    
}

void CCalibrationFactor::OnBUTTONZero12() 
{
    DigitShowContext* ctx = GetContext();
    OnBUTTONCFUpdate();
    if(m_Channels=="CH0-CH15")    ctx->cal.c[12] = ctx->cal.c[12]-ctx->Phyout[12];
    if(m_Channels=="CH16-CH31")    ctx->cal.c[28] = ctx->cal.c[28]-ctx->Phyout[28];
    CF_Load();    
}

void CCalibrationFactor::OnBUTTONZero13() 
{
    DigitShowContext* ctx = GetContext();
    OnBUTTONCFUpdate();
    if(m_Channels=="CH0-CH15")    ctx->cal.c[13] = ctx->cal.c[13]-ctx->Phyout[13];
    if(m_Channels=="CH16-CH31")    ctx->cal.c[29] = ctx->cal.c[29]-ctx->Phyout[29];
    CF_Load();    
}

void CCalibrationFactor::OnBUTTONZero14() 
{
    DigitShowContext* ctx = GetContext();
    OnBUTTONCFUpdate();
    if(m_Channels=="CH0-CH15")    ctx->cal.c[14] = ctx->cal.c[14]-ctx->Phyout[14];
    if(m_Channels=="CH16-CH31")    ctx->cal.c[30] = ctx->cal.c[30]-ctx->Phyout[30];
    CF_Load();    
}

void CCalibrationFactor::OnBUTTONZero15() 
{
    DigitShowContext* ctx = GetContext();
    OnBUTTONCFUpdate();
    if(m_Channels=="CH0-CH15")    ctx->cal.c[15] = ctx->cal.c[15]-ctx->Phyout[15];
    if(m_Channels=="CH16-CH31")    ctx->cal.c[31] = ctx->cal.c[31]-ctx->Phyout[31];
    CF_Load();
}



void CCalibrationFactor::OnBUTTONAmp00() 
{
    DigitShowContext* ctx = GetContext();
    int    nResult;
    if(m_Channels=="CH0-CH15")    ctx->AmpID = 0;
    if(m_Channels=="CH16-CH31")    ctx->AmpID = 16;
    if( ctx->AmpID<=ctx->AdMaxChannels ){
        CCalibrationAmp CalibrationAmp;
        nResult = CalibrationAmp.DoModal();
        if(nResult==IDOK){
            CF_Load();
        }
    }    
}

void CCalibrationFactor::OnBUTTONAmp01() 
{
    DigitShowContext* ctx = GetContext();
    int    nResult;
    if(m_Channels=="CH0-CH15")    ctx->AmpID = 1;
    if(m_Channels=="CH16-CH31")    ctx->AmpID = 17;
    if( ctx->AmpID<=ctx->AdMaxChannels ){
        CCalibrationAmp CalibrationAmp;
        nResult = CalibrationAmp.DoModal();
        if(nResult==IDOK){
            CF_Load();
        }
    }
}

void CCalibrationFactor::OnBUTTONAmp02() 
{
    DigitShowContext* ctx = GetContext();
    int    nResult;
    if(m_Channels=="CH0-CH15")    ctx->AmpID = 2;
    if(m_Channels=="CH16-CH31")    ctx->AmpID = 18;
    if( ctx->AmpID<=ctx->AdMaxChannels ){
        CCalibrationAmp CalibrationAmp;
        nResult = CalibrationAmp.DoModal();
        if(nResult==IDOK){
            CF_Load();
        }
    }
}

void CCalibrationFactor::OnBUTTONAmp03() 
{
    DigitShowContext* ctx = GetContext();
    int    nResult;
    if(m_Channels=="CH0-CH15")    ctx->AmpID = 3;
    if(m_Channels=="CH16-CH31")    ctx->AmpID = 19;
    if( ctx->AmpID<=ctx->AdMaxChannels ){
        CCalibrationAmp CalibrationAmp;
        nResult = CalibrationAmp.DoModal();
        if(nResult==IDOK){
            CF_Load();
        }
    }    
}

void CCalibrationFactor::OnBUTTONAmp04() 
{
    DigitShowContext* ctx = GetContext();
    int    nResult;
    if(m_Channels=="CH0-CH15")    ctx->AmpID = 4;
    if(m_Channels=="CH16-CH31")    ctx->AmpID = 20;
    if( ctx->AmpID<=ctx->AdMaxChannels ){
        CCalibrationAmp CalibrationAmp;
        nResult = CalibrationAmp.DoModal();
        if(nResult==IDOK){
            CF_Load();
        }
    }    
}

void CCalibrationFactor::OnBUTTONAmp05() 
{
    DigitShowContext* ctx = GetContext();
    int    nResult;
    if(m_Channels=="CH0-CH15")    ctx->AmpID = 5;
    if(m_Channels=="CH16-CH31")    ctx->AmpID = 21;
    if( ctx->AmpID<=ctx->AdMaxChannels ){
        CCalibrationAmp CalibrationAmp;
        nResult = CalibrationAmp.DoModal();
        if(nResult==IDOK){
            CF_Load();
        }
    }    
}

void CCalibrationFactor::OnBUTTONAmp06() 
{
    DigitShowContext* ctx = GetContext();
    int    nResult;
    if(m_Channels=="CH0-CH15")    ctx->AmpID = 6;
    if(m_Channels=="CH16-CH31")    ctx->AmpID = 22;
    if( ctx->AmpID<=ctx->AdMaxChannels ){
        CCalibrationAmp CalibrationAmp;
        nResult = CalibrationAmp.DoModal();
        if(nResult==IDOK){
            CF_Load();
        }
    }    
}

void CCalibrationFactor::OnBUTTONAmp07() 
{
    DigitShowContext* ctx = GetContext();
    int    nResult;
    if(m_Channels=="CH0-CH15")    ctx->AmpID = 7;
    if(m_Channels=="CH16-CH31")    ctx->AmpID = 23;
    if( ctx->AmpID<=ctx->AdMaxChannels ){
        CCalibrationAmp CalibrationAmp;
        nResult = CalibrationAmp.DoModal();
        if(nResult==IDOK){
            CF_Load();
        }
    }
}

void CCalibrationFactor::OnBUTTONAmp08() 
{
    DigitShowContext* ctx = GetContext();
    int    nResult;
    if(m_Channels=="CH0-CH15")    ctx->AmpID = 8;
    if(m_Channels=="CH16-CH31")    ctx->AmpID = 24;
    if( ctx->AmpID<=ctx->AdMaxChannels ){
        CCalibrationAmp CalibrationAmp;
        nResult = CalibrationAmp.DoModal();
        if(nResult==IDOK){
            CF_Load();
        }
    }    
}

void CCalibrationFactor::OnBUTTONAmp09() 
{
    DigitShowContext* ctx = GetContext();
    int    nResult;
    if(m_Channels=="CH0-CH15")    ctx->AmpID = 9;
    if(m_Channels=="CH16-CH31")    ctx->AmpID = 25;
    if( ctx->AmpID<=ctx->AdMaxChannels ){
        CCalibrationAmp CalibrationAmp;
        nResult = CalibrationAmp.DoModal();
        if(nResult==IDOK){
            CF_Load();
        }
    }    
}

void CCalibrationFactor::OnBUTTONAmp10() 
{
    DigitShowContext* ctx = GetContext();
    int    nResult;
    if(m_Channels=="CH0-CH15")    ctx->AmpID = 10;
    if(m_Channels=="CH16-CH31")    ctx->AmpID = 26;
    if( ctx->AmpID<=ctx->AdMaxChannels ){
        CCalibrationAmp CalibrationAmp;
        nResult = CalibrationAmp.DoModal();
        if(nResult==IDOK){
            CF_Load();
        }
    }    
}

void CCalibrationFactor::OnBUTTONAmp11() 
{
    DigitShowContext* ctx = GetContext();
    int    nResult;
    if(m_Channels=="CH0-CH15")    ctx->AmpID = 11;
    if(m_Channels=="CH16-CH31")    ctx->AmpID = 27;
    if( ctx->AmpID<=ctx->AdMaxChannels ){
        CCalibrationAmp CalibrationAmp;
        nResult = CalibrationAmp.DoModal();
        if(nResult==IDOK){
            CF_Load();
        }
    }    
}

void CCalibrationFactor::OnBUTTONAmp12() 
{
    DigitShowContext* ctx = GetContext();
    int    nResult;
    if(m_Channels=="CH0-CH15")    ctx->AmpID = 12;
    if(m_Channels=="CH16-CH31")    ctx->AmpID = 28;
    if( ctx->AmpID<=ctx->AdMaxChannels ){
        CCalibrationAmp CalibrationAmp;
        nResult = CalibrationAmp.DoModal();
        if(nResult==IDOK){
            CF_Load();
        }
    }    
}

void CCalibrationFactor::OnBUTTONAmp13() 
{
    DigitShowContext* ctx = GetContext();
    int    nResult;
    if(m_Channels=="CH0-CH15")    ctx->AmpID = 13;
    if(m_Channels=="CH16-CH31")    ctx->AmpID = 29;
    if( ctx->AmpID<=ctx->AdMaxChannels ){
        CCalibrationAmp CalibrationAmp;
        nResult = CalibrationAmp.DoModal();
        if(nResult==IDOK){
            CF_Load();
        }
    }    
}

void CCalibrationFactor::OnBUTTONAmp14() 
{
    DigitShowContext* ctx = GetContext();
    int    nResult;
    if(m_Channels=="CH0-CH15")    ctx->AmpID = 14;
    if(m_Channels=="CH16-CH31")    ctx->AmpID = 30;
    if( ctx->AmpID<=ctx->AdMaxChannels ){
        CCalibrationAmp CalibrationAmp;
        nResult = CalibrationAmp.DoModal();
        if(nResult==IDOK){
            CF_Load();
        }
    }    
}

void CCalibrationFactor::OnBUTTONAmp15() 
{
    DigitShowContext* ctx = GetContext();
    int    nResult;
    if(m_Channels=="CH0-CH15")    ctx->AmpID = 15;
    if(m_Channels=="CH16-CH31")    ctx->AmpID = 31;
    if( ctx->AmpID<=ctx->AdMaxChannels ){
        CCalibrationAmp CalibrationAmp;
        nResult = CalibrationAmp.DoModal();
        if(nResult==IDOK){
            CF_Load();
        }
    }    
}

void CCalibrationFactor::OnBUTTONCFSave() 
{
    DigitShowContext* ctx = GetContext();
    int        i;
    CString    pFileName;
    FILE    *FileCalData;
    errno_t err; 

    CFileDialog CalSaveFile_dlg( FALSE, NULL, "*.cal",  OFN_CREATEPROMPT | OFN_OVERWRITEPROMPT,
        "Calibration Files(*.cal)|*.cal| All Files(*.*)|*.*| |",NULL);

    if (CalSaveFile_dlg.DoModal()==IDOK)
    {
        pFileName = CalSaveFile_dlg.GetPathName();    
        if((err = fopen_s(&FileCalData,(LPCSTR)pFileName , _T("w"))) == 0)
        {
            fprintf(FileCalData,"64 \n");
            for(i = 0;i<64;i++){
                fprintf(FileCalData,"%d    %lf    %lf    %lf\n",i,ctx->cal.a[i],ctx->cal.b[i],ctx->cal.c[i]);
            }
            fclose(FileCalData);
        }
    }    
}

void CCalibrationFactor::OnBUTTONCFLoadFile() 
{
    DigitShowContext* ctx = GetContext();
    int        i,j;
    CString    pFileName;
    FILE    *FileCalData;
    errno_t err; 

    CFileDialog CalLoadFile_dlg( TRUE, NULL, "*.cal",  OFN_FILEMUSTEXIST | OFN_HIDEREADONLY,
        "Calibration Files(*.cal)|*.cal| All Files(*.*)|*.*| |",NULL);

    if (CalLoadFile_dlg.DoModal()==IDOK)
    {
        pFileName = CalLoadFile_dlg.GetPathName();    
        if((err = fopen_s(&FileCalData,(LPCSTR)pFileName , _T("r"))) == 0)
        {
            fscanf_s(FileCalData,_T("%d"),&j);
            for(i = 0;i<64;i++){
                fscanf_s(FileCalData,_T("%d%lf%lf%lf"),&j,&ctx->cal.a[i],&ctx->cal.b[i],&ctx->cal.c[i]);
            }
            fclose(FileCalData);
            CF_Load();
        }
    }    
}

void CCalibrationFactor::OnBUTTONChannelChange() 
{
    DigitShowContext* ctx = GetContext();
    if(m_Channels=="CH0-CH15"){
        m_Channels ="CH16-CH31";
    }
    else if(m_Channels=="CH16-CH31"){
        m_Channels ="CH0-CH15";
    }
    CF_Load();
}

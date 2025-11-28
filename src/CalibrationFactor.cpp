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

// CalibrationFactor.cpp : インプリメンテーション ファイル
//

#include "stdafx.h"
#include "DigitShowBasic.h"
#include "CalibrationFactor.h"

#include "CalibrationAmp.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CCalibrationFactor ダイアログ
// CDigitShowBasicView
extern	float		Vout[64];	
extern	double		Phyout[64];				
extern	double		Cal_a[64],Cal_b[64],Cal_c[64];				
extern	int			AdMaxCH;
		int			AmpID;

CCalibrationFactor::CCalibrationFactor(CWnd* pParent /*=NULL*/)
	: CDialog(CCalibrationFactor::IDD, pParent)
{
	//{{AFX_DATA_INIT(CCalibrationFactor)
	m_CFP00 = _T("");	m_CFP01 = _T("");
	m_CFP02 = _T("");	m_CFP03 = _T("");
	m_CFP04 = _T("");	m_CFP05 = _T("");
	m_CFP06 = _T("");	m_CFP07 = _T("");
	m_CFP08 = _T("");	m_CFP09 = _T("");
	m_CFP10 = _T("");	m_CFP11 = _T("");
	m_CFP12 = _T("");	m_CFP13 = _T("");
	m_CFP14 = _T("");	m_CFP15 = _T("");
	m_CFA00=0.0;	m_CFB00=0.0;	m_CFC00=0.0;
	m_CFA01=0.0;	m_CFB01=0.0;	m_CFC01=0.0;
	m_CFA02=0.0;	m_CFB02=0.0;	m_CFC02=0.0;
	m_CFA03=0.0;	m_CFB03=0.0;	m_CFC03=0.0;
	m_CFA04=0.0;	m_CFB04=0.0;	m_CFC04=0.0;
	m_CFA05=0.0;	m_CFB05=0.0;	m_CFC05=0.0;
	m_CFA06=0.0;	m_CFB06=0.0;	m_CFC06=0.0;
	m_CFA07=0.0;	m_CFB07=0.0;	m_CFC07=0.0;
	m_CFA08=0.0;	m_CFB08=0.0;	m_CFC08=0.0;
	m_CFA09=0.0;	m_CFB09=0.0;	m_CFC09=0.0;
	m_CFA10=0.0;	m_CFB10=0.0;	m_CFC10=0.0;
	m_CFA11=0.0;	m_CFB11=0.0;	m_CFC11=0.0;
	m_CFA12=0.0;	m_CFB12=0.0;	m_CFC12=0.0;
	m_CFA13=0.0;	m_CFB13=0.0;	m_CFC13=0.0;
	m_CFA14=0.0;	m_CFB14=0.0;	m_CFC14=0.0;
	m_CFA15=0.0;	m_CFB15=0.0;	m_CFC15=0.0;
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
    m_Channels=_T("CH0-CH15");
	CF_Load();
	// TODO: この位置に初期化の補足処理を追加してください
	
	return TRUE;  // コントロールにフォーカスを設定しないとき、戻り値は TRUE となります
	              // 例外: OCX プロパティ ページの戻り値は FALSE となります
}

void CCalibrationFactor::CF_Load()
{
	pDoc->Cal_Physical();
	if(m_Channels=="CH0-CH15"){	
		m_CFA00=Cal_a[0];	m_CFB00=Cal_b[0];	m_CFC00=Cal_c[0];	m_CFP00.Format("%11.5f",Phyout[0]);	
		m_CFA01=Cal_a[1];	m_CFB01=Cal_b[1];	m_CFC01=Cal_c[1];	m_CFP01.Format("%11.5f",Phyout[1]);	
		m_CFA02=Cal_a[2];	m_CFB02=Cal_b[2];	m_CFC02=Cal_c[2];	m_CFP02.Format("%11.5f",Phyout[2]);	
		m_CFA03=Cal_a[3];	m_CFB03=Cal_b[3];	m_CFC03=Cal_c[3];	m_CFP03.Format("%11.5f",Phyout[3]);	
		m_CFA04=Cal_a[4];	m_CFB04=Cal_b[4];	m_CFC04=Cal_c[4];	m_CFP04.Format("%11.5f",Phyout[4]);	
		m_CFA05=Cal_a[5];	m_CFB05=Cal_b[5];	m_CFC05=Cal_c[5];	m_CFP05.Format("%11.5f",Phyout[5]);	
		m_CFA06=Cal_a[6];	m_CFB06=Cal_b[6];	m_CFC06=Cal_c[6];	m_CFP06.Format("%11.5f",Phyout[6]);	
		m_CFA07=Cal_a[7];	m_CFB07=Cal_b[7];	m_CFC07=Cal_c[7];	m_CFP07.Format("%11.5f",Phyout[7]);	
		m_CFA08=Cal_a[8];	m_CFB08=Cal_b[8];	m_CFC08=Cal_c[8];	m_CFP08.Format("%11.5f",Phyout[8]);	
		m_CFA09=Cal_a[9];	m_CFB09=Cal_b[9];	m_CFC09=Cal_c[9];	m_CFP09.Format("%11.5f",Phyout[9]);	
		m_CFA10=Cal_a[10];	m_CFB10=Cal_b[10];	m_CFC10=Cal_c[10];	m_CFP10.Format("%11.5f",Phyout[10]);	
		m_CFA11=Cal_a[11];	m_CFB11=Cal_b[11];	m_CFC11=Cal_c[11];	m_CFP11.Format("%11.5f",Phyout[11]);	
		m_CFA12=Cal_a[12];	m_CFB12=Cal_b[12];	m_CFC12=Cal_c[12];	m_CFP12.Format("%11.5f",Phyout[12]);	
		m_CFA13=Cal_a[13];	m_CFB13=Cal_b[13];	m_CFC13=Cal_c[13];	m_CFP13.Format("%11.5f",Phyout[13]);	
		m_CFA14=Cal_a[14];	m_CFB14=Cal_b[14];	m_CFC14=Cal_c[14];	m_CFP14.Format("%11.5f",Phyout[14]);	
		m_CFA15=Cal_a[15];	m_CFB15=Cal_b[15];	m_CFC15=Cal_c[15];	m_CFP15.Format("%11.5f",Phyout[15]);	
		m_C00 = _T("CH00, Load Cell");		m_C01 = _T("CH01, Displacement");
		m_C02 = _T("CH02, Cell Pre.");		m_C03 = _T("CH03, Effect.Cell Pre. ");
		m_C04 = _T("CH04, Drained Vol.");	m_C05 = _T("CH05, LVDT1");
		m_C06 = _T("CH06, LVDT2");			m_C07 = _T("CH07");
		m_C08 = _T("CH08");					m_C09 = _T("CH09");
		m_C10 = _T("CH10");					m_C11 = _T("CH11");
		m_C12 = _T("CH12");					m_C13 = _T("CH13");
		m_C14 = _T("CH14");					m_C15 = _T("CH15");
	}
	if(m_Channels=="CH16-CH31"){	
		m_CFA00=Cal_a[16];	m_CFB00=Cal_b[16];	m_CFC00=Cal_c[16];	m_CFP00.Format("%11.5f",Phyout[16]);	
		m_CFA01=Cal_a[17];	m_CFB01=Cal_b[17];	m_CFC01=Cal_c[17];	m_CFP01.Format("%11.5f",Phyout[17]);	
		m_CFA02=Cal_a[18];	m_CFB02=Cal_b[18];	m_CFC02=Cal_c[18];	m_CFP02.Format("%11.5f",Phyout[18]);	
		m_CFA03=Cal_a[19];	m_CFB03=Cal_b[19];	m_CFC03=Cal_c[19];	m_CFP03.Format("%11.5f",Phyout[19]);	
		m_CFA04=Cal_a[20];	m_CFB04=Cal_b[20];	m_CFC04=Cal_c[20];	m_CFP04.Format("%11.5f",Phyout[20]);	
		m_CFA05=Cal_a[21];	m_CFB05=Cal_b[21];	m_CFC05=Cal_c[21];	m_CFP05.Format("%11.5f",Phyout[21]);	
		m_CFA06=Cal_a[22];	m_CFB06=Cal_b[22];	m_CFC06=Cal_c[22];	m_CFP06.Format("%11.5f",Phyout[22]);	
		m_CFA07=Cal_a[23];	m_CFB07=Cal_b[23];	m_CFC07=Cal_c[23];	m_CFP07.Format("%11.5f",Phyout[23]);	
		m_CFA08=Cal_a[24];	m_CFB08=Cal_b[24];	m_CFC08=Cal_c[24];	m_CFP08.Format("%11.5f",Phyout[24]);	
		m_CFA09=Cal_a[25];	m_CFB09=Cal_b[25];	m_CFC09=Cal_c[25];	m_CFP09.Format("%11.5f",Phyout[25]);	
		m_CFA10=Cal_a[26];	m_CFB10=Cal_b[26];	m_CFC10=Cal_c[26];	m_CFP10.Format("%11.5f",Phyout[26]);	
		m_CFA11=Cal_a[27];	m_CFB11=Cal_b[27];	m_CFC11=Cal_c[27];	m_CFP11.Format("%11.5f",Phyout[27]);	
		m_CFA12=Cal_a[28];	m_CFB12=Cal_b[28];	m_CFC12=Cal_c[28];	m_CFP12.Format("%11.5f",Phyout[28]);	
		m_CFA13=Cal_a[29];	m_CFB13=Cal_b[29];	m_CFC13=Cal_c[29];	m_CFP13.Format("%11.5f",Phyout[29]);	
		m_CFA14=Cal_a[30];	m_CFB14=Cal_b[30];	m_CFC14=Cal_c[30];	m_CFP14.Format("%11.5f",Phyout[30]);	
		m_CFA15=Cal_a[31];	m_CFB15=Cal_b[31];	m_CFC15=Cal_c[31];	m_CFP15.Format("%11.5f",Phyout[31]);	
		m_C00 = _T("CH16");	m_C01 = _T("CH17");
		m_C02 = _T("CH18");	m_C03 = _T("CH19");
		m_C04 = _T("CH20");	m_C05 = _T("CH21");
		m_C06 = _T("CH22");	m_C07 = _T("CH23");
		m_C08 = _T("CH24");	m_C09 = _T("CH25");
		m_C10 = _T("CH26");	m_C11 = _T("CH27");
		m_C12 = _T("CH28");	m_C13 = _T("CH29");
		m_C14 = _T("CH30");	m_C15 = _T("CH31");
	}
	UpdateData(FALSE);
}

void CCalibrationFactor::OnBUTTONCFUpdate() 
{
	// TODO: この位置にコントロール通知ハンドラ用のコードを追加してください
	UpdateData(TRUE);
	if(m_Channels=="CH0-CH15"){	
		Cal_a[0]=m_CFA00;	Cal_b[0]=m_CFB00;	Cal_c[0]=m_CFC00;	
		Cal_a[1]=m_CFA01;	Cal_b[1]=m_CFB01;	Cal_c[1]=m_CFC01;	
		Cal_a[2]=m_CFA02;	Cal_b[2]=m_CFB02;	Cal_c[2]=m_CFC02;	
		Cal_a[3]=m_CFA03;	Cal_b[3]=m_CFB03;	Cal_c[3]=m_CFC03;	
		Cal_a[4]=m_CFA04;	Cal_b[4]=m_CFB04;	Cal_c[4]=m_CFC04;	
		Cal_a[5]=m_CFA05;	Cal_b[5]=m_CFB05;	Cal_c[5]=m_CFC05;	
		Cal_a[6]=m_CFA06;	Cal_b[6]=m_CFB06;	Cal_c[6]=m_CFC06;	
		Cal_a[7]=m_CFA07;	Cal_b[7]=m_CFB07;	Cal_c[7]=m_CFC07;	
		Cal_a[8]=m_CFA08;	Cal_b[8]=m_CFB08;	Cal_c[8]=m_CFC08;	
		Cal_a[9]=m_CFA09;	Cal_b[9]=m_CFB09;	Cal_c[9]=m_CFC09;	
		Cal_a[10]=m_CFA10;	Cal_b[10]=m_CFB10;	Cal_c[10]=m_CFC10;	
		Cal_a[11]=m_CFA11;	Cal_b[11]=m_CFB11;	Cal_c[11]=m_CFC11;	
		Cal_a[12]=m_CFA12;	Cal_b[12]=m_CFB12;	Cal_c[12]=m_CFC12;	
		Cal_a[13]=m_CFA13;	Cal_b[13]=m_CFB13;	Cal_c[13]=m_CFC13;	
		Cal_a[14]=m_CFA14;	Cal_b[14]=m_CFB14;	Cal_c[14]=m_CFC14;	
		Cal_a[15]=m_CFA15;	Cal_b[15]=m_CFB15;	Cal_c[15]=m_CFC15;	
	}
	if(m_Channels=="CH16-CH31"){	
		Cal_a[16]=m_CFA00;	Cal_b[16]=m_CFB00;	Cal_c[16]=m_CFC00;	
		Cal_a[17]=m_CFA01;	Cal_b[17]=m_CFB01;	Cal_c[17]=m_CFC01;	
		Cal_a[18]=m_CFA02;	Cal_b[18]=m_CFB02;	Cal_c[18]=m_CFC02;	
		Cal_a[19]=m_CFA03;	Cal_b[19]=m_CFB03;	Cal_c[19]=m_CFC03;	
		Cal_a[20]=m_CFA04;	Cal_b[20]=m_CFB04;	Cal_c[20]=m_CFC04;	
		Cal_a[21]=m_CFA05;	Cal_b[21]=m_CFB05;	Cal_c[21]=m_CFC05;	
		Cal_a[22]=m_CFA06;	Cal_b[22]=m_CFB06;	Cal_c[22]=m_CFC06;	
		Cal_a[23]=m_CFA07;	Cal_b[23]=m_CFB07;	Cal_c[23]=m_CFC07;	
		Cal_a[24]=m_CFA08;	Cal_b[24]=m_CFB08;	Cal_c[24]=m_CFC08;	
		Cal_a[25]=m_CFA09;	Cal_b[25]=m_CFB09;	Cal_c[25]=m_CFC09;	
		Cal_a[26]=m_CFA10;	Cal_b[26]=m_CFB10;	Cal_c[26]=m_CFC10;	
		Cal_a[27]=m_CFA11;	Cal_b[27]=m_CFB11;	Cal_c[27]=m_CFC11;	
		Cal_a[28]=m_CFA12;	Cal_b[28]=m_CFB12;	Cal_c[28]=m_CFC12;	
		Cal_a[29]=m_CFA13;	Cal_b[29]=m_CFB13;	Cal_c[29]=m_CFC13;	
		Cal_a[30]=m_CFA14;	Cal_b[30]=m_CFB14;	Cal_c[30]=m_CFC14;	
		Cal_a[31]=m_CFA15;	Cal_b[31]=m_CFB15;	Cal_c[31]=m_CFC15;
	}
}


void CCalibrationFactor::OnBUTTONZero00() 
{
	// TODO: この位置にコントロール通知ハンドラ用のコードを追加してください
	OnBUTTONCFUpdate();
	if(m_Channels=="CH0-CH15")	Cal_c[0]=Cal_c[0]-Phyout[0];
	if(m_Channels=="CH16-CH31")	Cal_c[16]=Cal_c[16]-Phyout[16];
	CF_Load();
}

void CCalibrationFactor::OnBUTTONZero01() 
{
	// TODO: この位置にコントロール通知ハンドラ用のコードを追加してください
	OnBUTTONCFUpdate();
	if(m_Channels=="CH0-CH15")	Cal_c[1]=Cal_c[1]-Phyout[1];
	if(m_Channels=="CH16-CH31")	Cal_c[17]=Cal_c[17]-Phyout[17];
	CF_Load();
}

void CCalibrationFactor::OnBUTTONZero02() 
{
	// TODO: この位置にコントロール通知ハンドラ用のコードを追加してください
	OnBUTTONCFUpdate();
	if(m_Channels=="CH0-CH15")	Cal_c[2]=Cal_c[2]-Phyout[2];
	if(m_Channels=="CH16-CH31")	Cal_c[18]=Cal_c[18]-Phyout[18];
	CF_Load();
}

void CCalibrationFactor::OnBUTTONZero03() 
{
	// TODO: この位置にコントロール通知ハンドラ用のコードを追加してください
	OnBUTTONCFUpdate();
	if(m_Channels=="CH0-CH15")	Cal_c[3]=Cal_c[3]-Phyout[3];
	if(m_Channels=="CH16-CH31")	Cal_c[19]=Cal_c[19]-Phyout[19];
	CF_Load();	
}

void CCalibrationFactor::OnBUTTONZero04() 
{
	// TODO: この位置にコントロール通知ハンドラ用のコードを追加してください
	OnBUTTONCFUpdate();
	if(m_Channels=="CH0-CH15")	Cal_c[4]=Cal_c[4]-Phyout[4];
	if(m_Channels=="CH16-CH31")	Cal_c[20]=Cal_c[20]-Phyout[20];
	CF_Load();	
}

void CCalibrationFactor::OnBUTTONZero05() 
{
	// TODO: この位置にコントロール通知ハンドラ用のコードを追加してください
	OnBUTTONCFUpdate();
	if(m_Channels=="CH0-CH15")	Cal_c[5]=Cal_c[5]-Phyout[5];
	if(m_Channels=="CH16-CH31")	Cal_c[21]=Cal_c[21]-Phyout[21];
	CF_Load();
}

void CCalibrationFactor::OnBUTTONZero06() 
{
	// TODO: この位置にコントロール通知ハンドラ用のコードを追加してください
	OnBUTTONCFUpdate();
	if(m_Channels=="CH0-CH15")	Cal_c[6]=Cal_c[6]-Phyout[6];
	if(m_Channels=="CH16-CH31")	Cal_c[22]=Cal_c[22]-Phyout[22];
	CF_Load();
}

void CCalibrationFactor::OnBUTTONZero07() 
{
	// TODO: この位置にコントロール通知ハンドラ用のコードを追加してください
	OnBUTTONCFUpdate();
	if(m_Channels=="CH0-CH15")	Cal_c[7]=Cal_c[7]-Phyout[7];
	if(m_Channels=="CH16-CH31")	Cal_c[23]=Cal_c[23]-Phyout[23];
	CF_Load();
}

void CCalibrationFactor::OnBUTTONZero08() 
{
	// TODO: この位置にコントロール通知ハンドラ用のコードを追加してください
	OnBUTTONCFUpdate();
	if(m_Channels=="CH0-CH15")	Cal_c[8]=Cal_c[8]-Phyout[8];
	if(m_Channels=="CH16-CH31")	Cal_c[24]=Cal_c[24]-Phyout[24];
	CF_Load();
}

void CCalibrationFactor::OnBUTTONZero09() 
{
	// TODO: この位置にコントロール通知ハンドラ用のコードを追加してください
	OnBUTTONCFUpdate();
	if(m_Channels=="CH0-CH15")	Cal_c[9]=Cal_c[9]-Phyout[9];
	if(m_Channels=="CH16-CH31")	Cal_c[25]=Cal_c[25]-Phyout[25];
	CF_Load();
}

void CCalibrationFactor::OnBUTTONZero10() 
{
	// TODO: この位置にコントロール通知ハンドラ用のコードを追加してください
	OnBUTTONCFUpdate();
	if(m_Channels=="CH0-CH15")	Cal_c[10]=Cal_c[10]-Phyout[10];
	if(m_Channels=="CH16-CH31")	Cal_c[26]=Cal_c[26]-Phyout[26];
	CF_Load();
}

void CCalibrationFactor::OnBUTTONZero11() 
{
	// TODO: この位置にコントロール通知ハンドラ用のコードを追加してください
	OnBUTTONCFUpdate();
	if(m_Channels=="CH0-CH15")	Cal_c[11]=Cal_c[11]-Phyout[11];
	if(m_Channels=="CH16-CH31")	Cal_c[27]=Cal_c[27]-Phyout[27];
	CF_Load();	
}

void CCalibrationFactor::OnBUTTONZero12() 
{
	// TODO: この位置にコントロール通知ハンドラ用のコードを追加してください
	OnBUTTONCFUpdate();
	if(m_Channels=="CH0-CH15")	Cal_c[12]=Cal_c[12]-Phyout[12];
	if(m_Channels=="CH16-CH31")	Cal_c[28]=Cal_c[28]-Phyout[28];
	CF_Load();	
}

void CCalibrationFactor::OnBUTTONZero13() 
{
	// TODO: この位置にコントロール通知ハンドラ用のコードを追加してください
	OnBUTTONCFUpdate();
	if(m_Channels=="CH0-CH15")	Cal_c[13]=Cal_c[13]-Phyout[13];
	if(m_Channels=="CH16-CH31")	Cal_c[29]=Cal_c[29]-Phyout[29];
	CF_Load();	
}

void CCalibrationFactor::OnBUTTONZero14() 
{
	// TODO: この位置にコントロール通知ハンドラ用のコードを追加してください
	OnBUTTONCFUpdate();
	if(m_Channels=="CH0-CH15")	Cal_c[14]=Cal_c[14]-Phyout[14];
	if(m_Channels=="CH16-CH31")	Cal_c[30]=Cal_c[30]-Phyout[30];
	CF_Load();	
}

void CCalibrationFactor::OnBUTTONZero15() 
{
	// TODO: この位置にコントロール通知ハンドラ用のコードを追加してください
	OnBUTTONCFUpdate();
	if(m_Channels=="CH0-CH15")	Cal_c[15]=Cal_c[15]-Phyout[15];
	if(m_Channels=="CH16-CH31")	Cal_c[31]=Cal_c[31]-Phyout[31];
	CF_Load();
}



void CCalibrationFactor::OnBUTTONAmp00() 
{
	// TODO: Add your control notification handler code here
	int	nResult;
	if(m_Channels=="CH0-CH15")	AmpID=0;
	if(m_Channels=="CH16-CH31")	AmpID=16;
	if( AmpID<=AdMaxCH ){
		CCalibrationAmp CalibrationAmp;
		nResult = CalibrationAmp.DoModal();
		if(nResult==IDOK){
			CF_Load();
		}
	}	
}

void CCalibrationFactor::OnBUTTONAmp01() 
{
	// TODO: この位置にコントロール通知ハンドラ用のコードを追加してください
	int	nResult;
	if(m_Channels=="CH0-CH15")	AmpID=1;
	if(m_Channels=="CH16-CH31")	AmpID=17;
	if( AmpID<=AdMaxCH ){
		CCalibrationAmp CalibrationAmp;
		nResult = CalibrationAmp.DoModal();
		if(nResult==IDOK){
			CF_Load();
		}
	}
}

void CCalibrationFactor::OnBUTTONAmp02() 
{
	// TODO: この位置にコントロール通知ハンドラ用のコードを追加してください
	int	nResult;
	if(m_Channels=="CH0-CH15")	AmpID=2;
	if(m_Channels=="CH16-CH31")	AmpID=18;
	if( AmpID<=AdMaxCH ){
		CCalibrationAmp CalibrationAmp;
		nResult = CalibrationAmp.DoModal();
		if(nResult==IDOK){
			CF_Load();
		}
	}
}

void CCalibrationFactor::OnBUTTONAmp03() 
{
	// TODO: この位置にコントロール通知ハンドラ用のコードを追加してください
	int	nResult;
	if(m_Channels=="CH0-CH15")	AmpID=3;
	if(m_Channels=="CH16-CH31")	AmpID=19;
	if( AmpID<=AdMaxCH ){
		CCalibrationAmp CalibrationAmp;
		nResult = CalibrationAmp.DoModal();
		if(nResult==IDOK){
			CF_Load();
		}
	}	
}

void CCalibrationFactor::OnBUTTONAmp04() 
{
	// TODO: この位置にコントロール通知ハンドラ用のコードを追加してください
	int	nResult;
	if(m_Channels=="CH0-CH15")	AmpID=4;
	if(m_Channels=="CH16-CH31")	AmpID=20;
	if( AmpID<=AdMaxCH ){
		CCalibrationAmp CalibrationAmp;
		nResult = CalibrationAmp.DoModal();
		if(nResult==IDOK){
			CF_Load();
		}
	}	
}

void CCalibrationFactor::OnBUTTONAmp05() 
{
	// TODO: この位置にコントロール通知ハンドラ用のコードを追加してください
	int	nResult;
	if(m_Channels=="CH0-CH15")	AmpID=5;
	if(m_Channels=="CH16-CH31")	AmpID=21;
	if( AmpID<=AdMaxCH ){
		CCalibrationAmp CalibrationAmp;
		nResult = CalibrationAmp.DoModal();
		if(nResult==IDOK){
			CF_Load();
		}
	}	
}

void CCalibrationFactor::OnBUTTONAmp06() 
{
	// TODO: この位置にコントロール通知ハンドラ用のコードを追加してください
	int	nResult;
	if(m_Channels=="CH0-CH15")	AmpID=6;
	if(m_Channels=="CH16-CH31")	AmpID=22;
	if( AmpID<=AdMaxCH ){
		CCalibrationAmp CalibrationAmp;
		nResult = CalibrationAmp.DoModal();
		if(nResult==IDOK){
			CF_Load();
		}
	}	
}

void CCalibrationFactor::OnBUTTONAmp07() 
{
	// TODO: この位置にコントロール通知ハンドラ用のコードを追加してください
	int	nResult;
	if(m_Channels=="CH0-CH15")	AmpID=7;
	if(m_Channels=="CH16-CH31")	AmpID=23;
	if( AmpID<=AdMaxCH ){
		CCalibrationAmp CalibrationAmp;
		nResult = CalibrationAmp.DoModal();
		if(nResult==IDOK){
			CF_Load();
		}
	}
}

void CCalibrationFactor::OnBUTTONAmp08() 
{
	// TODO: この位置にコントロール通知ハンドラ用のコードを追加してください
	int	nResult;
	if(m_Channels=="CH0-CH15")	AmpID=8;
	if(m_Channels=="CH16-CH31")	AmpID=24;
	if( AmpID<=AdMaxCH ){
		CCalibrationAmp CalibrationAmp;
		nResult = CalibrationAmp.DoModal();
		if(nResult==IDOK){
			CF_Load();
		}
	}	
}

void CCalibrationFactor::OnBUTTONAmp09() 
{
	// TODO: この位置にコントロール通知ハンドラ用のコードを追加してください
	int	nResult;
	if(m_Channels=="CH0-CH15")	AmpID=9;
	if(m_Channels=="CH16-CH31")	AmpID=25;
	if( AmpID<=AdMaxCH ){
		CCalibrationAmp CalibrationAmp;
		nResult = CalibrationAmp.DoModal();
		if(nResult==IDOK){
			CF_Load();
		}
	}	
}

void CCalibrationFactor::OnBUTTONAmp10() 
{
	// TODO: この位置にコントロール通知ハンドラ用のコードを追加してください
	int	nResult;
	if(m_Channels=="CH0-CH15")	AmpID=10;
	if(m_Channels=="CH16-CH31")	AmpID=26;
	if( AmpID<=AdMaxCH ){
		CCalibrationAmp CalibrationAmp;
		nResult = CalibrationAmp.DoModal();
		if(nResult==IDOK){
			CF_Load();
		}
	}	
}

void CCalibrationFactor::OnBUTTONAmp11() 
{
	// TODO: この位置にコントロール通知ハンドラ用のコードを追加してください
	int	nResult;
	if(m_Channels=="CH0-CH15")	AmpID=11;
	if(m_Channels=="CH16-CH31")	AmpID=27;
	if( AmpID<=AdMaxCH ){
		CCalibrationAmp CalibrationAmp;
		nResult = CalibrationAmp.DoModal();
		if(nResult==IDOK){
			CF_Load();
		}
	}	
}

void CCalibrationFactor::OnBUTTONAmp12() 
{
	// TODO: この位置にコントロール通知ハンドラ用のコードを追加してください
	int	nResult;
	if(m_Channels=="CH0-CH15")	AmpID=12;
	if(m_Channels=="CH16-CH31")	AmpID=28;
	if( AmpID<=AdMaxCH ){
		CCalibrationAmp CalibrationAmp;
		nResult = CalibrationAmp.DoModal();
		if(nResult==IDOK){
			CF_Load();
		}
	}	
}

void CCalibrationFactor::OnBUTTONAmp13() 
{
	// TODO: この位置にコントロール通知ハンドラ用のコードを追加してください
	int	nResult;
	if(m_Channels=="CH0-CH15")	AmpID=13;
	if(m_Channels=="CH16-CH31")	AmpID=29;
	if( AmpID<=AdMaxCH ){
		CCalibrationAmp CalibrationAmp;
		nResult = CalibrationAmp.DoModal();
		if(nResult==IDOK){
			CF_Load();
		}
	}	
}

void CCalibrationFactor::OnBUTTONAmp14() 
{
	// TODO: この位置にコントロール通知ハンドラ用のコードを追加してください
	int	nResult;
	if(m_Channels=="CH0-CH15")	AmpID=14;
	if(m_Channels=="CH16-CH31")	AmpID=30;
	if( AmpID<=AdMaxCH ){
		CCalibrationAmp CalibrationAmp;
		nResult = CalibrationAmp.DoModal();
		if(nResult==IDOK){
			CF_Load();
		}
	}	
}

void CCalibrationFactor::OnBUTTONAmp15() 
{
	// TODO: この位置にコントロール通知ハンドラ用のコードを追加してください
	int	nResult;
	if(m_Channels=="CH0-CH15")	AmpID=15;
	if(m_Channels=="CH16-CH31")	AmpID=31;
	if( AmpID<=AdMaxCH ){
		CCalibrationAmp CalibrationAmp;
		nResult = CalibrationAmp.DoModal();
		if(nResult==IDOK){
			CF_Load();
		}
	}	
}

void CCalibrationFactor::OnBUTTONCFSave() 
{
	// TODO: この位置にコントロール通知ハンドラ用のコードを追加してください
	int		i;
	CString	pFileName;
	FILE	*FileCalData;
	errno_t err; 

	CFileDialog CalSaveFile_dlg( FALSE, NULL, "*.cal",  OFN_CREATEPROMPT | OFN_OVERWRITEPROMPT,
		"Calibration Files(*.cal)|*.cal| All Files(*.*)|*.*| |",NULL);

	if (CalSaveFile_dlg.DoModal()==IDOK)
	{
	    pFileName = CalSaveFile_dlg.GetPathName();	
		//FileCalData = fopen((LPCSTR)pFileName , "w" );
		if((err = fopen_s(&FileCalData,(LPCSTR)pFileName , _T("w"))) == 0)
		{
			fprintf(FileCalData,"64 \n");
			for(i=0;i<64;i++){
				fprintf(FileCalData,"%d	%lf	%lf	%lf\n",i,Cal_a[i],Cal_b[i],Cal_c[i]);
			}
			fclose(FileCalData);
		}
	}	
}

void CCalibrationFactor::OnBUTTONCFLoadFile() 
{
	// TODO: この位置にコントロール通知ハンドラ用のコードを追加してください
	int		i,j;
	CString	pFileName;
	FILE	*FileCalData;
	errno_t err; 

	CFileDialog CalLoadFile_dlg( TRUE, NULL, "*.cal",  OFN_FILEMUSTEXIST | OFN_HIDEREADONLY,
		"Calibration Files(*.cal)|*.cal| All Files(*.*)|*.*| |",NULL);

	if (CalLoadFile_dlg.DoModal()==IDOK)
	{
	    pFileName = CalLoadFile_dlg.GetPathName();	
	//	FileCalData = fopen((LPCSTR)pFileName , "r" );
		if((err = fopen_s(&FileCalData,(LPCSTR)pFileName , _T("r"))) == 0)
		{
			//fscanf(FileCalData,"%d",&j);
			fscanf_s(FileCalData,_T("%d"),&j);
			for(i=0;i<64;i++){
				//fscanf(FileCalData,"%d%lf%lf%lf",&j,&Cal_a[i],&Cal_b[i],&Cal_c[i]);
				fscanf_s(FileCalData,_T("%d%lf%lf%lf"),&j,&Cal_a[i],&Cal_b[i],&Cal_c[i]);
			}
			fclose(FileCalData);
			CF_Load();
		}
	}	
}

void CCalibrationFactor::OnBUTTONChannelChange() 
{
	// TODO: この位置にコントロール通知ハンドラ用のコードを追加してください
	if(m_Channels=="CH0-CH15"){
		m_Channels="CH16-CH31";
	}
	else if(m_Channels=="CH16-CH31"){
		m_Channels="CH0-CH15";
	}
	CF_Load();
}

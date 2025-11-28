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

// DA_Pout.cpp : インプリメンテーション ファイル
//

#include "stdafx.h"
#include "DigitShowBasic.h"
#include "DA_Pout.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDA_Pout ダイアログ
extern	float		DAVout[8];					// Output Voltage to D/A board
extern	double		DA_Cal_a[8],DA_Cal_b[8];	// D/A Calibration Factor

CDA_Pout::CDA_Pout(CWnd* pParent /*=NULL*/)
	: CDialog(CDA_Pout::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDA_Pout)
	m_DACala00 = DA_Cal_a[0];
	m_DACala01 = DA_Cal_a[1];
	m_DACala02 = DA_Cal_a[2];
	m_DACala03 = DA_Cal_a[3];
	m_DACala04 = DA_Cal_a[4];
	m_DACala05 = DA_Cal_a[5];
	m_DACala06 = DA_Cal_a[6];
	m_DACala07 = DA_Cal_a[7];
	m_DACalb00 = DA_Cal_b[0];
	m_DACalb01 = DA_Cal_b[1];
	m_DACalb02 = DA_Cal_b[2];
	m_DACalb03 = DA_Cal_b[3];
	m_DACalb04 = DA_Cal_b[4];
	m_DACalb05 = DA_Cal_b[5];
	m_DACalb06 = DA_Cal_b[6];
	m_DACalb07 = DA_Cal_b[7];
	m_DAPvalue00 = 0.0;
	m_DAPvalue01 = 0.0;
	m_DAPvalue02 = 0.0;
	m_DAPvalue03 = 0.0;
	m_DAPvalue04 = 0.0;
	m_DAPvalue05 = 0.0;
	m_DAPvalue06 = 0.0;
	m_DAPvalue07 = 0.0;
	m_DAVout00 = DAVout[0];
	m_DAVout01 = DAVout[1];
	m_DAVout02 = DAVout[2];
	m_DAVout03 = DAVout[3];
	m_DAVout04 = DAVout[4];
	m_DAVout05 = DAVout[5];
	m_DAVout06 = DAVout[6];
	m_DAVout07 = DAVout[7];
	//}}AFX_DATA_INIT
}


void CDA_Pout::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDA_Pout)
	DDX_Text(pDX, IDC_EDIT_DACala00, m_DACala00);
	DDX_Text(pDX, IDC_EDIT_DACala01, m_DACala01);
	DDX_Text(pDX, IDC_EDIT_DACala02, m_DACala02);
	DDX_Text(pDX, IDC_EDIT_DACala03, m_DACala03);
	DDX_Text(pDX, IDC_EDIT_DACala04, m_DACala04);
	DDX_Text(pDX, IDC_EDIT_DACala05, m_DACala05);
	DDX_Text(pDX, IDC_EDIT_DACala06, m_DACala06);
	DDX_Text(pDX, IDC_EDIT_DACala07, m_DACala07);
	DDX_Text(pDX, IDC_EDIT_DACalb00, m_DACalb00);
	DDX_Text(pDX, IDC_EDIT_DACalb01, m_DACalb01);
	DDX_Text(pDX, IDC_EDIT_DACalb02, m_DACalb02);
	DDX_Text(pDX, IDC_EDIT_DACalb03, m_DACalb03);
	DDX_Text(pDX, IDC_EDIT_DACalb04, m_DACalb04);
	DDX_Text(pDX, IDC_EDIT_DACalb05, m_DACalb05);
	DDX_Text(pDX, IDC_EDIT_DACalb06, m_DACalb06);
	DDX_Text(pDX, IDC_EDIT_DACalb07, m_DACalb07);
	DDX_Text(pDX, IDC_EDIT_DAPvalue00, m_DAPvalue00);
	DDX_Text(pDX, IDC_EDIT_DAPvalue01, m_DAPvalue01);
	DDX_Text(pDX, IDC_EDIT_DAPvalue02, m_DAPvalue02);
	DDX_Text(pDX, IDC_EDIT_DAPvalue03, m_DAPvalue03);
	DDX_Text(pDX, IDC_EDIT_DAPvalue04, m_DAPvalue04);
	DDX_Text(pDX, IDC_EDIT_DAPvalue05, m_DAPvalue05);
	DDX_Text(pDX, IDC_EDIT_DAPvalue06, m_DAPvalue06);
	DDX_Text(pDX, IDC_EDIT_DAPvalue07, m_DAPvalue07);
	DDX_Text(pDX, IDC_EDIT_DAVout00, m_DAVout00);
	DDX_Text(pDX, IDC_EDIT_DAVout01, m_DAVout01);
	DDX_Text(pDX, IDC_EDIT_DAVout02, m_DAVout02);
	DDX_Text(pDX, IDC_EDIT_DAVout03, m_DAVout03);
	DDX_Text(pDX, IDC_EDIT_DAVout04, m_DAVout04);
	DDX_Text(pDX, IDC_EDIT_DAVout05, m_DAVout05);
	DDX_Text(pDX, IDC_EDIT_DAVout06, m_DAVout06);
	DDX_Text(pDX, IDC_EDIT_DAVout07, m_DAVout07);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDA_Pout, CDialog)
	//{{AFX_MSG_MAP(CDA_Pout)
	ON_BN_CLICKED(IDC_BUTTON_DAOutput, OnBUTTONDAOutput)
	ON_BN_CLICKED(IDC_BUTTON_Calculation00, OnBUTTONCalculation00)
	ON_BN_CLICKED(IDC_BUTTON_Calculation01, OnBUTTONCalculation01)
	ON_BN_CLICKED(IDC_BUTTON_Calculation02, OnBUTTONCalculation02)
	ON_BN_CLICKED(IDC_BUTTON_Calculation03, OnBUTTONCalculation03)
	ON_BN_CLICKED(IDC_BUTTON_Calculation04, OnBUTTONCalculation04)
	ON_BN_CLICKED(IDC_BUTTON_Calculation05, OnBUTTONCalculation05)
	ON_BN_CLICKED(IDC_BUTTON_Calculation06, OnBUTTONCalculation06)
	ON_BN_CLICKED(IDC_BUTTON_Calculation07, OnBUTTONCalculation07)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDA_Pout メッセージ ハンドラ



void CDA_Pout::OnBUTTONDAOutput() 
{
	// TODO: この位置にコントロール通知ハンドラ用のコードを追加してください
	UpdateData(TRUE);
	DAVout[0] = m_DAVout00;
	DAVout[1] = m_DAVout01;
	DAVout[2] = m_DAVout02;
	DAVout[3] = m_DAVout03;
	DAVout[4] = m_DAVout04;
	DAVout[5] = m_DAVout05;
	DAVout[6] = m_DAVout06;
	DAVout[7] = m_DAVout07;
	pDoc2->DA_OUTPUT();
}

void CDA_Pout::OnBUTTONCalculation00() 
{
	// TODO: この位置にコントロール通知ハンドラ用のコードを追加してください
	UpdateData(TRUE);
	m_DAVout00=float(DA_Cal_a[0]*m_DAPvalue00+DA_Cal_b[0]);
	UpdateData(FALSE);
}

void CDA_Pout::OnBUTTONCalculation01() 
{
	// TODO: この位置にコントロール通知ハンドラ用のコードを追加してください
	UpdateData(TRUE);
	m_DAVout01=float(DA_Cal_a[1]*m_DAPvalue01+DA_Cal_b[1]);
	UpdateData(FALSE);	
}

void CDA_Pout::OnBUTTONCalculation02() 
{
	// TODO: この位置にコントロール通知ハンドラ用のコードを追加してください
	UpdateData(TRUE);
	m_DAVout02=float(DA_Cal_a[2]*m_DAPvalue02+DA_Cal_b[2]);
	UpdateData(FALSE);	
}

void CDA_Pout::OnBUTTONCalculation03() 
{
	// TODO: この位置にコントロール通知ハンドラ用のコードを追加してください
	UpdateData(TRUE);
	m_DAVout03=float(DA_Cal_a[3]*m_DAPvalue03+DA_Cal_b[3]);
	UpdateData(FALSE);	
}

void CDA_Pout::OnBUTTONCalculation04() 
{
	// TODO: この位置にコントロール通知ハンドラ用のコードを追加してください
	UpdateData(TRUE);
	m_DAVout04=float(DA_Cal_a[4]*m_DAPvalue04+DA_Cal_b[4]);
	UpdateData(FALSE);	
}

void CDA_Pout::OnBUTTONCalculation05() 
{
	// TODO: この位置にコントロール通知ハンドラ用のコードを追加してください
	UpdateData(TRUE);
	m_DAVout05=float(DA_Cal_a[5]*m_DAPvalue05+DA_Cal_b[5]);
	UpdateData(FALSE);	
}

void CDA_Pout::OnBUTTONCalculation06() 
{
	// TODO: この位置にコントロール通知ハンドラ用のコードを追加してください
	UpdateData(TRUE);
	m_DAVout06=float(DA_Cal_a[6]*m_DAPvalue06+DA_Cal_b[6]);
	UpdateData(FALSE);	
}

void CDA_Pout::OnBUTTONCalculation07() 
{
	// TODO: この位置にコントロール通知ハンドラ用のコードを追加してください
	UpdateData(TRUE);
	m_DAVout07=float(DA_Cal_a[7]*m_DAPvalue07+DA_Cal_b[7]);
	UpdateData(FALSE);	
}

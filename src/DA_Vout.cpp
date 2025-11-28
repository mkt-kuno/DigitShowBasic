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

// DA_Vout.cpp : インプリメンテーション ファイル
//

#include "stdafx.h"
#include "DigitShowBasic.h"
#include "DA_Vout.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDA_Vout ダイアログ
extern	float		DAVout[8];	

CDA_Vout::CDA_Vout(CWnd* pParent /*=NULL*/)
	: CDialog(CDA_Vout::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDA_Vout)
	m_DAVout01 = DAVout[0];
	m_DAVout02 = DAVout[1];
	m_DAVout03 = DAVout[2];
	m_DAVout04 = DAVout[3];
	m_DAVout05 = DAVout[4];
	m_DAVout06 = DAVout[5];
	m_DAVout07 = DAVout[6];
	m_DAVout08 = DAVout[7];
	//}}AFX_DATA_INIT
}


void CDA_Vout::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDA_Vout)
	DDX_Text(pDX, IDC_EDIT_DAVout01, m_DAVout01);
	DDX_Text(pDX, IDC_EDIT_DAVout02, m_DAVout02);
	DDX_Text(pDX, IDC_EDIT_DAVout03, m_DAVout03);
	DDX_Text(pDX, IDC_EDIT_DAVout04, m_DAVout04);
	DDX_Text(pDX, IDC_EDIT_DAVout05, m_DAVout05);
	DDX_Text(pDX, IDC_EDIT_DAVout06, m_DAVout06);
	DDX_Text(pDX, IDC_EDIT_DAVout07, m_DAVout07);
	DDX_Text(pDX, IDC_EDIT_DAVout08, m_DAVout08);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDA_Vout, CDialog)
	//{{AFX_MSG_MAP(CDA_Vout)
	ON_BN_CLICKED(IDC_BUTTON_DA_Vout, OnBUTTONDAVout)
	ON_BN_CLICKED(IDC_BUTTON_Reflesh, OnBUTTONReflesh)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDA_Vout メッセージ ハンドラ

void CDA_Vout::OnBUTTONDAVout() 
{
	// TODO: この位置にコントロール通知ハンドラ用のコードを追加してください
	UpdateData(TRUE);
	DAVout[0]=m_DAVout01;
	DAVout[1]=m_DAVout02;
	DAVout[2]=m_DAVout03;
	DAVout[3]=m_DAVout04;
	DAVout[4]=m_DAVout05;
	DAVout[5]=m_DAVout06;
	DAVout[6]=m_DAVout07;
	DAVout[7]=m_DAVout08;
	pDoc->DA_OUTPUT();
}

void CDA_Vout::OnBUTTONReflesh() 
{
	// TODO: この位置にコントロール通知ハンドラ用のコードを追加してください
	m_DAVout01 = DAVout[0];
	m_DAVout02 = DAVout[1];
	m_DAVout03 = DAVout[2];
	m_DAVout04 = DAVout[3];
	m_DAVout05 = DAVout[4];
	m_DAVout06 = DAVout[5];
	m_DAVout07 = DAVout[6];
	m_DAVout08 = DAVout[7];
	UpdateData(FALSE);
}

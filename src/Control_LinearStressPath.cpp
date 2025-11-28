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

// Control_LinearStressPath.cpp : インプリメンテーション ファイル
//

#include "stdafx.h"
#include "DigitShowBasic.h"
#include "Control_LinearStressPath.h"

#include "DigitShowBasicDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CControl_LinearStressPath ダイアログ
extern	Control	ControlData[16];	

CControl_LinearStressPath::CControl_LinearStressPath(CWnd* pParent /*=NULL*/)
	: CDialog(CControl_LinearStressPath::IDD, pParent)
{
	//{{AFX_DATA_INIT(CControl_LinearStressPath)
	m_e_sigma1 = ControlData[7].e_sigma[0];
	m_e_sigma2 = ControlData[7].e_sigma[1];
	m_MotorSpeed = ControlData[7].MotorSpeed;
	m_sigma_rate = ControlData[7].sigmaRate[0];
	m_sigma1 = ControlData[7].sigma[0];
	m_sigma2 = ControlData[7].sigma[1];
	//}}AFX_DATA_INIT
}


void CControl_LinearStressPath::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CControl_LinearStressPath)
	DDX_Text(pDX, IDC_EDIT_e_sigma1, m_e_sigma1);
	DDX_Text(pDX, IDC_EDIT_e_sigma2, m_e_sigma2);
	DDX_Text(pDX, IDC_EDIT_MotorSpeed, m_MotorSpeed);
	DDX_Text(pDX, IDC_EDIT_sigma_rate, m_sigma_rate);
	DDX_Text(pDX, IDC_EDIT_sigma1, m_sigma1);
	DDX_Text(pDX, IDC_EDIT_sigma2, m_sigma2);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CControl_LinearStressPath, CDialog)
	//{{AFX_MSG_MAP(CControl_LinearStressPath)
	ON_BN_CLICKED(IDC_BUTTON_Update, OnBUTTONUpdate)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CControl_LinearStressPath メッセージ ハンドラ

void CControl_LinearStressPath::OnBUTTONUpdate() 
{
	// TODO: この位置にコントロール通知ハンドラ用のコードを追加してください
	UpdateData(TRUE);
	ControlData[7].e_sigma[0] = m_e_sigma1;
	ControlData[7].e_sigma[1] = m_e_sigma2;
	ControlData[7].MotorSpeed = m_MotorSpeed;
	ControlData[7].sigmaRate[0] = m_sigma_rate;
	ControlData[7].sigma[0] = m_sigma1;
	ControlData[7].sigma[1] = m_sigma2;
}

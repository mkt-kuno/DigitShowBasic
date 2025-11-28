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

// Control_PreConsolidation.cpp : インプリメンテーション ファイル
//

#include "stdafx.h"
#include "DigitShowBasic.h"
#include "Control_PreConsolidation.h"
#include "DigitShowBasicDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CControl_PreConsolidation ダイアログ
extern	Control	ControlData[16];


CControl_PreConsolidation::CControl_PreConsolidation(CWnd* pParent /*=NULL*/)
	: CDialog(CControl_PreConsolidation::IDD, pParent)
{
	//{{AFX_DATA_INIT(CControl_PreConsolidation)
	m_q = ControlData[1].q;
	m_MotorSpeed = ControlData[1].MotorSpeed;
	//}}AFX_DATA_INIT
}


void CControl_PreConsolidation::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CControl_PreConsolidation)
	DDX_Text(pDX, IDC_EDIT_q, m_q);
	DDX_Text(pDX, IDC_EDIT_MotorSpeed, m_MotorSpeed);
	DDV_MinMaxDouble(pDX, m_MotorSpeed, 0., 3000.);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CControl_PreConsolidation, CDialog)
	//{{AFX_MSG_MAP(CControl_PreConsolidation)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CControl_PreConsolidation メッセージ ハンドラ

void CControl_PreConsolidation::OnOK() 
{
	// TODO: この位置にその他の検証用のコードを追加してください
	UpdateData(TRUE);
	ControlData[1].q = m_q;
	ControlData[1].MotorSpeed = m_MotorSpeed;
	CDialog::OnOK();
}

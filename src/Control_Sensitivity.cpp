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

// Control_Sensitivity.cpp : インプリメンテーション ファイル
//

#include "stdafx.h"
#include "DigitShowBasic.h"
#include "Control_Sensitivity.h"

#include "DigitShowBasicDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CControl_Sensitivity ダイアログ
extern	double		ERR_StressCom, ERR_StressExt;
extern	double		ERR_StressA;

CControl_Sensitivity::CControl_Sensitivity(CWnd* pParent /*=NULL*/)
	: CDialog(CControl_Sensitivity::IDD, pParent)
{
	//{{AFX_DATA_INIT(CControl_Sensitivity)
	m_ERR_StressA = ERR_StressA;
	m_ERR_StressCom = ERR_StressCom;
	m_ERR_StressExt = ERR_StressExt;
	//}}AFX_DATA_INIT
}


void CControl_Sensitivity::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CControl_Sensitivity)
	DDX_Text(pDX, IDC_EDIT_ERR_StressA, m_ERR_StressA);
	DDX_Text(pDX, IDC_EDIT_ERR_StressCom, m_ERR_StressCom);
	DDV_MinMaxDouble(pDX, m_ERR_StressCom, 0., 50.);
	DDX_Text(pDX, IDC_EDIT_ERR_StressExt, m_ERR_StressExt);
	DDV_MinMaxDouble(pDX, m_ERR_StressExt, -50., 0.);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CControl_Sensitivity, CDialog)
	//{{AFX_MSG_MAP(CControl_Sensitivity)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CControl_Sensitivity メッセージ ハンドラ

void CControl_Sensitivity::OnOK() 
{
	// TODO: この位置にその他の検証用のコードを追加してください
	UpdateData(TRUE);
	ERR_StressA = m_ERR_StressA;
	ERR_StressCom = m_ERR_StressCom;
	ERR_StressExt = m_ERR_StressExt;
	CDialog::OnOK();
}

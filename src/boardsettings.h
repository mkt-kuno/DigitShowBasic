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

#if !defined(AFX_BoardSettings_H__7C23E5A9_7E1B_4821_857B_F8600C2F6EA6__INCLUDED_)
#define AFX_BoardSettings_H__7C23E5A9_7E1B_4821_857B_F8600C2F6EA6__INCLUDED_

#include "DigitShowBasicDoc.h"

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// BoardSettings.h : ヘッダー ファイル
//

/////////////////////////////////////////////////////////////////////////////
// CBoardSettings ダイアログ

class CBoardSettings : public CDialog
{
// コンストラクション
public:
	CBoardSettings(CWnd* pParent = NULL);   // 標準のコンストラクタ
	CDigitShowBasicDoc* pDoc;
// ダイアログ データ
	//{{AFX_DATA(CBoardSettings)
	enum { IDD = IDD_BoardSettings };
	CString	m_ADMethod0;
	CString	m_ADMethod1;
	CString	m_ADResolution0;
	CString	m_ADResolution1;
	CString	m_ADRange0;
	CString	m_ADRange1;
	CString	m_ADMaxChannel0;
	CString	m_ADMaxChannel1;
	CString	m_DAMaxChannel0;
	CString	m_DARange0;
	CString	m_DAResolution0;
	//}}AFX_DATA


// オーバーライド
	// ClassWizard は仮想関数のオーバーライドを生成します。
	//{{AFX_VIRTUAL(CBoardSettings)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV サポート
	//}}AFX_VIRTUAL

// インプリメンテーション
protected:

	// 生成されたメッセージ マップ関数
	//{{AFX_MSG(CBoardSettings)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ は前行の直前に追加の宣言を挿入します。

#endif // !defined(AFX_BoardSettings_H__7C23E5A9_7E1B_4821_857B_F8600C2F6EA6__INCLUDED_)

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

#if !defined(AFX_DA_VOUT_H__ECF95710_176B_4995_B453_27585B2276F0__INCLUDED_)
#define AFX_DA_VOUT_H__ECF95710_176B_4995_B453_27585B2276F0__INCLUDED_

#include "DigitShowBasicDoc.h"

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DA_Vout.h : ヘッダー ファイル
//

/////////////////////////////////////////////////////////////////////////////
// CDA_Vout ダイアログ

class CDA_Vout : public CDialog
{
// コンストラクション
public:
	CDA_Vout(CWnd* pParent = NULL);   // 標準のコンストラクタ
	CDigitShowBasicDoc* pDoc;
// ダイアログ データ
	//{{AFX_DATA(CDA_Vout)
	enum { IDD = IDD_DA_Vout };
	float	m_DAVout01;
	float	m_DAVout02;
	float	m_DAVout03;
	float	m_DAVout04;
	float	m_DAVout05;
	float	m_DAVout06;
	float	m_DAVout07;
	float	m_DAVout08;
	float	m_DAVout09;
	float	m_DAVout10;
	float	m_DAVout11;
	float	m_DAVout12;
	float	m_DAVout13;
	float	m_DAVout14;
	float	m_DAVout15;
	float	m_DAVout16;
	//}}AFX_DATA


// オーバーライド
	// ClassWizard は仮想関数のオーバーライドを生成します。
	//{{AFX_VIRTUAL(CDA_Vout)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV サポート
	//}}AFX_VIRTUAL

// インプリメンテーション
protected:

	// 生成されたメッセージ マップ関数
	//{{AFX_MSG(CDA_Vout)
	afx_msg void OnBUTTONDAVout();
	afx_msg void OnBUTTONReflesh();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ は前行の直前に追加の宣言を挿入します。

#endif // !defined(AFX_DA_VOUT_H__ECF95710_176B_4995_B453_27585B2276F0__INCLUDED_)

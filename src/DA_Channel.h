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

#if !defined(AFX_DA_CHANNEL_H__DE7B2C20_A5DC_11D5_B813_0020E0636BD5__INCLUDED_)
#define AFX_DA_CHANNEL_H__DE7B2C20_A5DC_11D5_B813_0020E0636BD5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DA_Channel.h : ヘッダー ファイル
//

/////////////////////////////////////////////////////////////////////////////
// CDA_Channel ダイアログ

class CDA_Channel : public CDialog
{
// コンストラクション
public:
	CDA_Channel(CWnd* pParent = NULL);   // 標準のコンストラクタ

// ダイアログ データ
	//{{AFX_DATA(CDA_Channel)
	enum { IDD = IDD_DA_Channel };
	double	m_DA_Cala00;
	double	m_DA_Cala01;
	double	m_DA_Cala02;
	double	m_DA_Cala03;
	double	m_DA_Cala04;
	double	m_DA_Cala05;
	double	m_DA_Cala06;
	double	m_DA_Cala07;
	double	m_DA_Calb00;
	double	m_DA_Calb01;
	double	m_DA_Calb02;
	double	m_DA_Calb03;
	double	m_DA_Calb04;
	double	m_DA_Calb05;
	double	m_DA_Calb06;
	double	m_DA_Calb07;
	//}}AFX_DATA


// オーバーライド
	// ClassWizard は仮想関数のオーバーライドを生成します。
	//{{AFX_VIRTUAL(CDA_Channel)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV サポート
	//}}AFX_VIRTUAL

// インプリメンテーション
protected:

	// 生成されたメッセージ マップ関数
	//{{AFX_MSG(CDA_Channel)
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ は前行の直前に追加の宣言を挿入します。

#endif // !defined(AFX_DA_CHANNEL_H__DE7B2C20_A5DC_11D5_B813_0020E0636BD5__INCLUDED_)

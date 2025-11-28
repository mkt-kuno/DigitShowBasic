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

#if !defined(AFX_CONTROL_MLOADING_H__DE7B2C22_A5DC_11D5_B813_0020E0636BD5__INCLUDED_)
#define AFX_CONTROL_MLOADING_H__DE7B2C22_A5DC_11D5_B813_0020E0636BD5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// Control_MLoading.h : ヘッダー ファイル
//

/////////////////////////////////////////////////////////////////////////////
// CControl_MLoading ダイアログ

class CControl_MLoading : public CDialog
{
// コンストラクション
public:
	CControl_MLoading(CWnd* pParent = NULL);   // 標準のコンストラクタ

// ダイアログ データ
	//{{AFX_DATA(CControl_MLoading)
	enum { IDD = IDD_Control_MLoading };
	int		m_MotorCruch;
	double	m_MotorSpeed;
	int		m_flag0;
	double	m_q;
	//}}AFX_DATA


// オーバーライド
	// ClassWizard は仮想関数のオーバーライドを生成します。
	//{{AFX_VIRTUAL(CControl_MLoading)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV サポート
	//}}AFX_VIRTUAL

// インプリメンテーション
protected:

	// 生成されたメッセージ マップ関数
	//{{AFX_MSG(CControl_MLoading)
	afx_msg void OnBUTTONUpdate();
	afx_msg void OnBUTTONReflesh();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ は前行の直前に追加の宣言を挿入します。

#endif // !defined(AFX_CONTROL_MLOADING_H__DE7B2C22_A5DC_11D5_B813_0020E0636BD5__INCLUDED_)

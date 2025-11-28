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

#if !defined(AFX_CONTROL_CLOADING_H__70A0B936_F88A_4CAE_ADF5_9173A66733BE__INCLUDED_)
#define AFX_CONTROL_CLOADING_H__70A0B936_F88A_4CAE_ADF5_9173A66733BE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// Control_CLoading.h : ヘッダー ファイル
//

/////////////////////////////////////////////////////////////////////////////
// CControl_CLoading ダイアログ

class CControl_CLoading : public CDialog
{
// コンストラクション
public:
	CControl_CLoading(CWnd* pParent = NULL);   // 標準のコンストラクタ

// ダイアログ データ
	//{{AFX_DATA(CControl_CLoading)
	enum { IDD = IDD_Control_Cloading };
	int		m_flag0;
	double	m_MotorSpeed;
	double	m_q_lower;
	double	m_q_upper;
	int		m_time0;
	int		m_time1;
	int		m_time2;
	//}}AFX_DATA


// オーバーライド
	// ClassWizard は仮想関数のオーバーライドを生成します。
	//{{AFX_VIRTUAL(CControl_CLoading)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV サポート
	//}}AFX_VIRTUAL

// インプリメンテーション
protected:

	// 生成されたメッセージ マップ関数
	//{{AFX_MSG(CControl_CLoading)
	afx_msg void OnBUTTONUpdate();
	afx_msg void OnBUTTONReflesh();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ は前行の直前に追加の宣言を挿入します。

#endif // !defined(AFX_CONTROL_CLOADING_H__70A0B936_F88A_4CAE_ADF5_9173A66733BE__INCLUDED_)

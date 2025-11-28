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

#if !defined(AFX_TRANSADJUSTMENT_H__9C426B86_E15E_42FA_9A0E_A35AF4BFAD55__INCLUDED_)
#define AFX_TRANSAJUSTMENT_H__9C426B86_E15E_42FA_9A0E_A35AF4BFAD55__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// TransAdjustment.h : ヘッダー ファイル
//

/////////////////////////////////////////////////////////////////////////////
// CTransAdjustment ダイアログ

class CTransAdjustment : public CDialog
{
// コンストラクション
public:
	CTransAdjustment(CWnd* pParent = NULL);   // 標準のコンストラクタ

// ダイアログ データ
	//{{AFX_DATA(CTransAdjustment)
	enum { IDD = IDD_TransAdjustment };
	double	m_FinalDisp;
	double	m_InitialDisp;
	double	m_FinalBullet;
	double	m_InitialBullet;
	//}}AFX_DATA


// オーバーライド
	// ClassWizard は仮想関数のオーバーライドを生成します。
	//{{AFX_VIRTUAL(CTransAdjustment)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV サポート
	//}}AFX_VIRTUAL

// インプリメンテーション
protected:

	// 生成されたメッセージ マップ関数
	//{{AFX_MSG(CTransAdjustment)
	afx_msg void OnBUTTONInitialDisp();
	afx_msg void OnBUTTONEndDisp();
	afx_msg void OnBUTTONInitialBullet();
	afx_msg void OnBUTTONEndBullet();
	afx_msg void OnBUTTONUpdateDisp();
	afx_msg void OnBUTTONUpdateBullet();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ は前行の直前に追加の宣言を挿入します。

#endif // !defined(AFX_TRANSADJUSTMENT_H__9C426B86_E15E_42FA_9A0E_A35AF4BFAD55__INCLUDED_)

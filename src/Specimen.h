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

#if !defined(AFX_SPECIMEN_H__417DC7FD_CE21_4B6B_988B_2D3050CBC61D__INCLUDED_)
#define AFX_SPECIMEN_H__417DC7FD_CE21_4B6B_988B_2D3050CBC61D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// Specimen.h : ヘッダー ファイル
//

/////////////////////////////////////////////////////////////////////////////
// CSpecimen ダイアログ

class CSpecimen : public CDialog
{
// コンストラクション
public:
	CSpecimen(CWnd* pParent = NULL);   // 標準のコンストラクタ

// ダイアログ データ
	//{{AFX_DATA(CSpecimen)
	enum { IDD = IDD_SpecimenData };
	double	m_Area0;
	double	m_Area1;
	double	m_Area2;
	double	m_Area3;
	double	m_Depth0;
	double	m_Depth1;
	double	m_Depth2;
	double	m_Gs;
	double	m_Height0;
	double	m_Height1;
	double	m_Height2;
	double	m_Height3;
	double	m_MembraneE;
	double	m_MembraneT;
	double	m_RodArea;
	double	m_RodWeight;
	double	m_Volume0;
	double	m_Volume1;
	double	m_Volume2;
	double	m_Volume3;
	double	m_Weight0;
	double	m_Weight1;
	double	m_Weight2;
	double	m_Weight3;
	double	m_Width0;
	double	m_Width1;
	double	m_Width2;
	double	m_Width3;
	double	m_Depth3;
	double	m_Diameter0;
	double	m_Diameter1;
	double	m_Diameter2;
	double	m_Diameter3;
	double	m_VLDT1_0;
	double	m_VLDT1_1;
	double	m_VLDT1_2;
	double	m_VLDT1_3;
	double	m_VLDT2_0;
	double	m_VLDT2_1;
	double	m_VLDT2_2;
	double	m_VLDT2_3;
	//}}AFX_DATA


// オーバーライド
	// ClassWizard は仮想関数のオーバーライドを生成します。
	//{{AFX_VIRTUAL(CSpecimen)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV サポート
	//}}AFX_VIRTUAL

// インプリメンテーション
protected:
	void Reflesh();

	// 生成されたメッセージ マップ関数
	//{{AFX_MSG(CSpecimen)
	afx_msg void OnBUTTONSave();
	afx_msg void OnBUTTONUpdate();
	afx_msg void OnBUTTONBeConsol();
	afx_msg void OnBUTTONAfConsolidation();
	afx_msg void OnBUTTONToPresent1();
	afx_msg void OnBUTTONToPresent2();
	afx_msg void OnBUTTONToPresent3();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ は前行の直前に追加の宣言を挿入します。

#endif // !defined(AFX_SPECIMEN_H__417DC7FD_CE21_4B6B_988B_2D3050CBC61D__INCLUDED_)

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

// DigitShowBasicView.h : CDigitShowBasicView クラスの宣言およびインターフェイスの定義をします。
//
/////////////////////////////////////////////////////////////////////////////


#include "DigitShowBasic.h"
#include "sys/timeb.h"

#if !defined(AFX_DIGITSHOWBASICVIEW_H__44354AEA_B72E_464C_AAD2_A696AA19EF89__INCLUDED_)
#define AFX_DIGITSHOWBASICVIEW_H__44354AEA_B72E_464C_AAD2_A696AA19EF89__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


class CDigitShowBasicView : public CFormView
{
protected: // シリアライズ機能のみから作成します。
	CDigitShowBasicView();
	DECLARE_DYNCREATE(CDigitShowBasicView)

public:
	//{{AFX_DATA(CDigitShowBasicView)
	enum { IDD = IDD_DIGITSHOWBASIC_FORM };
	CString	m_Vout00;
	CString	m_Vout01;
	CString	m_Vout02;
	CString	m_Vout03;
	CString	m_Vout04;
	CString	m_Vout05;
	CString	m_Vout06;
	CString	m_Vout07;
	CString	m_Vout08;
	CString	m_Vout09;
	CString	m_Vout10;
	CString	m_Vout11;
	CString	m_Vout12;
	CString	m_Vout13;
	CString	m_Vout14;
	CString	m_Vout15;
	CString	m_Vout16;
	CString	m_Vout17;
	CString	m_Vout18;
	CString	m_Vout19;
	CString	m_Vout20;
	CString	m_Vout21;
	CString	m_Vout22;
	CString	m_Vout23;
	CString	m_Vout24;
	CString	m_Vout25;
	CString	m_Vout26;
	CString	m_Vout27;
	CString	m_Vout28;
	CString	m_Vout29;
	CString	m_Vout30;
	CString	m_Vout31;
	CString	m_Phyout00;
	CString	m_Phyout01;
	CString	m_Phyout02;
	CString	m_Phyout03;
	CString	m_Phyout04;
	CString	m_Phyout05;
	CString	m_Phyout06;
	CString	m_Phyout07;
	CString	m_Phyout08;
	CString	m_Phyout09;
	CString	m_Phyout10;
	CString	m_Phyout11;
	CString	m_Phyout12;
	CString	m_Phyout13;
	CString	m_Phyout14;
	CString	m_Phyout15;
	CString	m_Phyout16;
	CString	m_Phyout17;
	CString	m_Phyout18;
	CString	m_Phyout19;
	CString	m_Phyout20;
	CString	m_Phyout21;
	CString	m_Phyout22;
	CString	m_Phyout23;
	CString	m_Phyout24;
	CString	m_Phyout25;
	CString	m_Phyout26;
	CString	m_Phyout27;
	CString	m_Phyout28;
	CString	m_Phyout29;
	CString	m_Phyout30;
	CString	m_Phyout31;
	CString	m_Para00;
	CString	m_Para01;
	CString	m_Para02;
	CString	m_Para03;
	CString	m_Para04;
	CString	m_Para05;
	CString	m_Para06;
	CString	m_Para07;
	CString	m_Para08;
	CString	m_Para09;
	CString	m_Para10;
	CString	m_Para11;
	CString	m_Para12;
	CString	m_Para13;
	CString	m_Para14;
	CString	m_Para15;
	int		m_Ctrl_ID;
	CString	m_NowTime;
	long	m_SeqTime;
	long	m_SamplingTime;
	CString	m_FileName;


	//}}AFX_DATA

// アトリビュート
public:
	CDigitShowBasicDoc* GetDocument();
	struct _timeb StartTime2,NowTime2;
	struct _timeb StepTime0, StepTime1;

// オペレーション
public:
CBrush	*m_pEditBrush,*m_pStaticBrush,*m_pDlgBrush;
// オーバーライド
	// ClassWizard は仮想関数のオーバーライドを生成します。
	//{{AFX_VIRTUAL(CDigitShowBasicView)
	public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV のサポート
	virtual void OnInitialUpdate(); // 構築後の最初の１度だけ呼び出されます。
	virtual LRESULT DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// インプリメンテーション
public:
	void ShowData();
	virtual ~CDigitShowBasicView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// 生成されたメッセージ マップ関数
protected:
	//{{AFX_MSG(CDigitShowBasicView)
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnBUTTONCtrlOff();
	afx_msg void OnBUTTONCtrlOn();
	afx_msg void OnBUTTONStartSave();
	afx_msg void OnBUTTONStopSave();
	afx_msg void OnDestroy();
	afx_msg void OnBUTTONInterceptSave();
	afx_msg void OnBUTTONFIFOStart();
	afx_msg void OnBUTTONFIFOStop();
	afx_msg void OnBUTTONWriteData();
	afx_msg void OnBUTTONSetCtrlID();
	afx_msg void OnBUTTONSetTimeInterval();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // DigitShowBasicView.cpp ファイルがデバッグ環境の時使用されます。
inline CDigitShowBasicDoc* CDigitShowBasicView::GetDocument()
   { return (CDigitShowBasicDoc*)m_pDocument; }
#endif

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ は前行の直前に追加の宣言を挿入します。

#endif // !defined(AFX_DIGITSHOWBASICVIEW_H__44354AEA_B72E_464C_AAD2_A696AA19EF89__INCLUDED_)

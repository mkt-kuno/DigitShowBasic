#if !defined(AFX_DA_POUT_H__0001B6E0_A540_11D5_B813_0020E0636BD5__INCLUDED_)
#define AFX_DA_POUT_H__0001B6E0_A540_11D5_B813_0020E0636BD5__INCLUDED_

#include "DigitShowBasicDoc.H"

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DA_Pout.h : ヘッダー ファイル
//

/////////////////////////////////////////////////////////////////////////////
// CDA_Pout ダイアログ

class CDA_Pout : public CDialog
{
// コンストラクション
public:
	CDA_Pout(CWnd* pParent = NULL);   // 標準のコンストラクタ
	CDigitShowBasicDoc* pDoc2;
// ダイアログ データ
	//{{AFX_DATA(CDA_Pout)
	enum { IDD = IDD_DA_Pout };
	double	m_DACala00;
	double	m_DACala01;
	double	m_DACala02;
	double	m_DACala03;
	double	m_DACala04;
	double	m_DACala05;
	double	m_DACala06;
	double	m_DACala07;
	double	m_DACalb00;
	double	m_DACalb01;
	double	m_DACalb02;
	double	m_DACalb03;
	double	m_DACalb04;
	double	m_DACalb05;
	double	m_DACalb06;
	double	m_DACalb07;
	double	m_DAPvalue00;
	double	m_DAPvalue01;
	double	m_DAPvalue02;
	double	m_DAPvalue03;
	double	m_DAPvalue04;
	double	m_DAPvalue05;
	double	m_DAPvalue06;
	double	m_DAPvalue07;
	float	m_DAVout00;
	float	m_DAVout01;
	float	m_DAVout02;
	float	m_DAVout03;
	float	m_DAVout04;
	float	m_DAVout05;
	float	m_DAVout06;
	float	m_DAVout07;
	//}}AFX_DATA


// オーバーライド
	// ClassWizard は仮想関数のオーバーライドを生成します。
	//{{AFX_VIRTUAL(CDA_Pout)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV サポート
	//}}AFX_VIRTUAL

// インプリメンテーション
protected:

	// 生成されたメッセージ マップ関数
	//{{AFX_MSG(CDA_Pout)
	afx_msg void OnBUTTONDAOutput();
	afx_msg void OnBUTTONCalculation00();
	afx_msg void OnBUTTONCalculation01();
	afx_msg void OnBUTTONCalculation02();
	afx_msg void OnBUTTONCalculation03();
	afx_msg void OnBUTTONCalculation04();
	afx_msg void OnBUTTONCalculation05();
	afx_msg void OnBUTTONCalculation06();
	afx_msg void OnBUTTONCalculation07();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ は前行の直前に追加の宣言を挿入します。

#endif // !defined(AFX_DA_POUT_H__0001B6E0_A540_11D5_B813_0020E0636BD5__INCLUDED_)

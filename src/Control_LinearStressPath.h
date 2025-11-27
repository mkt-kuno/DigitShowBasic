#if !defined(AFX_CONTROL_LINEARSTRESSPATH_H__F7C348F0_6750_4E58_8225_E84353B5B8FC__INCLUDED_)
#define AFX_CONTROL_LINEARSTRESSPATH_H__F7C348F0_6750_4E58_8225_E84353B5B8FC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// Control_LinearStressPath.h : ヘッダー ファイル
//

/////////////////////////////////////////////////////////////////////////////
// CControl_LinearStressPath ダイアログ

class CControl_LinearStressPath : public CDialog
{
// コンストラクション
public:
	CControl_LinearStressPath(CWnd* pParent = NULL);   // 標準のコンストラクタ

// ダイアログ データ
	//{{AFX_DATA(CControl_LinearStressPath)
	enum { IDD = IDD_Control_LinearStressPathLoading };
	double	m_e_sigma1;
	double	m_e_sigma2;
	double	m_MotorSpeed;
	double	m_sigma_rate;
	double	m_sigma1;
	double	m_sigma2;
	//}}AFX_DATA


// オーバーライド
	// ClassWizard は仮想関数のオーバーライドを生成します。
	//{{AFX_VIRTUAL(CControl_LinearStressPath)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV サポート
	//}}AFX_VIRTUAL

// インプリメンテーション
protected:

	// 生成されたメッセージ マップ関数
	//{{AFX_MSG(CControl_LinearStressPath)
	afx_msg void OnBUTTONUpdate();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ は前行の直前に追加の宣言を挿入します。

#endif // !defined(AFX_CONTROL_LINEARSTRESSPATH_H__F7C348F0_6750_4E58_8225_E84353B5B8FC__INCLUDED_)

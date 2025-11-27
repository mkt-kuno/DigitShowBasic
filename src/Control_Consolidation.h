#if !defined(AFX_CONTROL_CONSOLIDATION_H__DE7B2C21_A5DC_11D5_B813_0020E0636BD5__INCLUDED_)
#define AFX_CONTROL_CONSOLIDATION_H__DE7B2C21_A5DC_11D5_B813_0020E0636BD5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// Control_Consolidation.h : ヘッダー ファイル
//

/////////////////////////////////////////////////////////////////////////////
// CControl_Consolidation ダイアログ

class CControl_Consolidation : public CDialog
{
// コンストラクション
public:
	CControl_Consolidation(CWnd* pParent = NULL);   // 標準のコンストラクタ

// ダイアログ データ
	//{{AFX_DATA(CControl_Consolidation)
	enum { IDD = IDD_Control_Consolidation };
	double	m_MotorK0;
	double	m_MotorSpeed;
	double	m_MotorSrRate;
	double	m_MotorESa;
	//}}AFX_DATA


// オーバーライド
	// ClassWizard は仮想関数のオーバーライドを生成します。
	//{{AFX_VIRTUAL(CControl_Consolidation)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV サポート
	//}}AFX_VIRTUAL

// インプリメンテーション
protected:

	// 生成されたメッセージ マップ関数
	//{{AFX_MSG(CControl_Consolidation)
	afx_msg void OnBUTTONUpdate();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ は前行の直前に追加の宣言を挿入します。

#endif // !defined(AFX_CONTROL_CONSOLIDATION_H__DE7B2C21_A5DC_11D5_B813_0020E0636BD5__INCLUDED_)

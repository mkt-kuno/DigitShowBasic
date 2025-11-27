#if !defined(AFX_CONTROL_ID_H__61FF15F5_71CB_46D3_BE0E_6D3B18E13BDC__INCLUDED_)
#define AFX_CONTROL_ID_H__61FF15F5_71CB_46D3_BE0E_6D3B18E13BDC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// Control_ID.h : ヘッダー ファイル
//

/////////////////////////////////////////////////////////////////////////////
// CControl_ID ダイアログ

class CControl_ID : public CDialog
{
// コンストラクション
public:
	CControl_ID(CWnd* pParent = NULL);   // 標準のコンストラクタ

// ダイアログ データ
	//{{AFX_DATA(CControl_ID)
	enum { IDD = IDD_Control_ID };
	int		m_Control_ID;
	double	m_esigma0;
	double	m_esigma1;
	double	m_esigma2;
	double	m_esigmaAmp0;
	double	m_esigmaAmp1;
	double	m_esigmaAmp2;
	double	m_esigmaRate0;
	double	m_esigmaRate1;
	double	m_esigmaRate2;
	int		m_flag0;
	int		m_flag1;
	int		m_flag2;
	double	m_K0;
	int		m_Motor;
	int		m_MotorCruch;
	double	m_MotorSpeed;
	double	m_p;
	double	m_sigma0;
	double	m_sigma1;
	double	m_sigma2;
	double	m_sigmaAmp0;
	double	m_sigmaAmp1;
	double	m_sigmaAmp2;
	double	m_sigmaRate0;
	double	m_sigmaRate1;
	double	m_sigmaRate2;
	double	m_strain0;
	double	m_strain1;
	double	m_strain2;
	double	m_strainAmp0;
	double	m_strainAmp1;
	double	m_strainAmp2;
	double	m_strainRate0;
	double	m_strainRate1;
	double	m_strainRate2;
	int		m_time0;
	int		m_time1;
	int		m_time2;
	double	m_u;
	double	m_q;
	//}}AFX_DATA


// オーバーライド
	// ClassWizard は仮想関数のオーバーライドを生成します。
	//{{AFX_VIRTUAL(CControl_ID)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV サポート
	//}}AFX_VIRTUAL

// インプリメンテーション
private:
	int			i;
protected:

	// 生成されたメッセージ マップ関数
	//{{AFX_MSG(CControl_ID)
	afx_msg void OnBUTTONLoad();
	afx_msg void OnBUTTONUpdate();
	afx_msg void OnBUTTONLoadfromfile();
	afx_msg void OnBUTTONSaveFile();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ は前行の直前に追加の宣言を挿入します。

#endif // !defined(AFX_CONTROL_ID_H__61FF15F5_71CB_46D3_BE0E_6D3B18E13BDC__INCLUDED_)

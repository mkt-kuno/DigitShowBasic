#if !defined(AFX_SAMPLINGSETTINGS_H__A100D0D8_4CDC_4617_978A_A28957A25A20__INCLUDED_)
#define AFX_SAMPLINGSETTINGS_H__A100D0D8_4CDC_4617_978A_A28957A25A20__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SamplingSettings.h : ヘッダー ファイル
//

/////////////////////////////////////////////////////////////////////////////
// CSamplingSettings ダイアログ

class CSamplingSettings : public CDialog
{
// コンストラクション
public:
	CSamplingSettings(CWnd* pParent = NULL);   // 標準のコンストラクタ

// ダイアログ データ
	//{{AFX_DATA(CSamplingSettings)
	enum { IDD = IDD_SamplingSettings };
	long	m_TimeInterval1;
	long	m_TimeInterval2;
	long	m_TimeInterval3;
	CString	m_AllocatedMemory;
	int		m_AvSmplNum;
	int		m_Channels;
	int		m_EventSamplingTimes;
	CString	m_MemoryType;
	float	m_SamplingClock;
	int		m_SavingTime;
	long	m_TotalSamplingTimes;
	//}}AFX_DATA


// オーバーライド
	// ClassWizard は仮想関数のオーバーライドを生成します。
	//{{AFX_VIRTUAL(CSamplingSettings)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV サポート
	//}}AFX_VIRTUAL

// インプリメンテーション
protected:

	// 生成されたメッセージ マップ関数
	//{{AFX_MSG(CSamplingSettings)
	afx_msg void OnBUTTONCheck();
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ は前行の直前に追加の宣言を挿入します。

#endif // !defined(AFX_SAMPLINGSETTINGS_H__A100D0D8_4CDC_4617_978A_A28957A25A20__INCLUDED_)

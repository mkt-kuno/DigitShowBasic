// MainFrm.h : CMainFrame クラスの宣言およびインターフェイスの定義をします。
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_MAINFRM_H__AC57187F_9544_4DFA_BC03_5FC6D5E29FB4__INCLUDED_)
#define AFX_MAINFRM_H__AC57187F_9544_4DFA_BC03_5FC6D5E29FB4__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CMainFrame : public CFrameWnd
{
	
protected: // シリアライズ機能のみから作成します。
	CMainFrame();
	DECLARE_DYNCREATE(CMainFrame)

// アトリビュート
public:

// オペレーション
public:

// オーバーライド
	// ClassWizard は仮想関数のオーバーライドを生成します。
	//{{AFX_VIRTUAL(CMainFrame)
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	//}}AFX_VIRTUAL

// インプリメンテーション
public:
	virtual ~CMainFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

// 生成されたメッセージ マップ関数
private:
	int			nResult;
protected:
	//{{AFX_MSG(CMainFrame)
	afx_msg void OnBoardSettings();
	afx_msg void OnCalibrationFactor();
	afx_msg void OnSpecimenData();
	afx_msg void OnDAVout();
	afx_msg void OnControlID();
	afx_msg void OnDAPout();
	afx_msg void OnDAChannel();
	afx_msg void OnControlConsolidation();
	afx_msg void OnControlMLoading();
	afx_msg void OnControlSensitivity();
	afx_msg void OnControlCLoading();
	afx_msg void OnControlFile();
	afx_msg void OnSamplingSettings();
	afx_msg void OnControlPreConsolidation();
	afx_msg void OnTransAdjustment();
	afx_msg void OnControlLinearStressPath();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ は前行の直前に追加の宣言を挿入します。

#endif // !defined(AFX_MAINFRM_H__AC57187F_9544_4DFA_BC03_5FC6D5E29FB4__INCLUDED_)

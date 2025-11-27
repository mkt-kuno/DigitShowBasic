#if !defined(AFX_CALIBRATIONAMP_H__C9D79E38_8396_494A_8202_AC899D8267E6__INCLUDED_)
#define AFX_CALIBRATIONAMP_H__C9D79E38_8396_494A_8202_AC899D8267E6__INCLUDED_

#include "DigitShowBasicDoc.h"

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// CalibrationAmp.h : ヘッダー ファイル
//

/////////////////////////////////////////////////////////////////////////////
// CCalibrationAmp ダイアログ

class CCalibrationAmp : public CDialog
{
// コンストラクション
public:
	CCalibrationAmp(CWnd* pParent = NULL);   // 標準のコンストラクタ
// ダイアログ データ
	//{{AFX_DATA(CCalibrationAmp)
	enum { IDD = IDD_CalibrationAmp };
	int		m_AmpNo;
	float	m_AmpPB;
	float	m_AmpVB;
	float	m_AmpVO;
	float	m_AmpPO;
	//}}AFX_DATA


// オーバーライド
	// ClassWizard は仮想関数のオーバーライドを生成します。
	//{{AFX_VIRTUAL(CCalibrationAmp)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV サポート
	//}}AFX_VIRTUAL

// インプリメンテーション
protected:

	// 生成されたメッセージ マップ関数
	//{{AFX_MSG(CCalibrationAmp)
	afx_msg void OnBUTTONAmpBase();
	afx_msg void OnBUTTONAmpOffset();
	afx_msg void OnBUTTONAmpUpdate();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ は前行の直前に追加の宣言を挿入します。

#endif // !defined(AFX_CALIBRATIONAMP_H__C9D79E38_8396_494A_8202_AC899D8267E6__INCLUDED_)

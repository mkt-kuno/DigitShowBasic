// Control_Sensitivity.cpp : インプリメンテーション ファイル
//

#include "stdafx.h"
#include "DigitShowBasic.h"
#include "Control_Sensitivity.h"

#include "DigitShowBasicDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CControl_Sensitivity ダイアログ
extern	double		ERR_StressCom, ERR_StressExt;
extern	double		ERR_StressA;

CControl_Sensitivity::CControl_Sensitivity(CWnd* pParent /*=NULL*/)
	: CDialog(CControl_Sensitivity::IDD, pParent)
{
	//{{AFX_DATA_INIT(CControl_Sensitivity)
	m_ERR_StressA = ERR_StressA;
	m_ERR_StressCom = ERR_StressCom;
	m_ERR_StressExt = ERR_StressExt;
	//}}AFX_DATA_INIT
}


void CControl_Sensitivity::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CControl_Sensitivity)
	DDX_Text(pDX, IDC_EDIT_ERR_StressA, m_ERR_StressA);
	DDX_Text(pDX, IDC_EDIT_ERR_StressCom, m_ERR_StressCom);
	DDV_MinMaxDouble(pDX, m_ERR_StressCom, 0., 50.);
	DDX_Text(pDX, IDC_EDIT_ERR_StressExt, m_ERR_StressExt);
	DDV_MinMaxDouble(pDX, m_ERR_StressExt, -50., 0.);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CControl_Sensitivity, CDialog)
	//{{AFX_MSG_MAP(CControl_Sensitivity)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CControl_Sensitivity メッセージ ハンドラ

void CControl_Sensitivity::OnOK() 
{
	// TODO: この位置にその他の検証用のコードを追加してください
	UpdateData(TRUE);
	ERR_StressA = m_ERR_StressA;
	ERR_StressCom = m_ERR_StressCom;
	ERR_StressExt = m_ERR_StressExt;
	CDialog::OnOK();
}

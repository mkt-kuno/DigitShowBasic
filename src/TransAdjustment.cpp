// TransAdjustment.cpp : インプリメンテーション ファイル
//

#include "stdafx.h"
#include "DigitShowBasic.h"
#include "TransAdjustment.h"
#include "DigitShowBasicDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTransAdjustment ダイアログ
extern	double		Phyout[64],Cal_c[64];

CTransAdjustment::CTransAdjustment(CWnd* pParent /*=NULL*/)
	: CDialog(CTransAdjustment::IDD, pParent)
{
	//{{AFX_DATA_INIT(CTransAdjustment)
	m_FinalDisp = 0.0;
	m_InitialDisp = 0.0;
	m_FinalBullet = 0.0;
	m_InitialBullet = 0.0;
	//}}AFX_DATA_INIT
}


void CTransAdjustment::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTransAdjustment)
	DDX_Text(pDX, IDC_EDIT_FinalDisp, m_FinalDisp);
	DDX_Text(pDX, IDC_EDIT_InitialDisp, m_InitialDisp);
	DDX_Text(pDX, IDC_EDIT_FinalBullet, m_FinalBullet);
	DDX_Text(pDX, IDC_EDIT_InitialBullet, m_InitialBullet);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CTransAdjustment, CDialog)
	//{{AFX_MSG_MAP(CTransAdjustment)
	ON_BN_CLICKED(IDC_BUTTON_InitialDisp, OnBUTTONInitialDisp)
	ON_BN_CLICKED(IDC_BUTTON_EndDisp, OnBUTTONEndDisp)
	ON_BN_CLICKED(IDC_BUTTON_InitialBullet, OnBUTTONInitialBullet)
	ON_BN_CLICKED(IDC_BUTTON_EndBullet, OnBUTTONEndBullet)
	ON_BN_CLICKED(IDC_BUTTON_UpdateDisp, OnBUTTONUpdateDisp)
	ON_BN_CLICKED(IDC_BUTTON_UpdateBullet, OnBUTTONUpdateBullet)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTransAdjustment メッセージ ハンドラ

void CTransAdjustment::OnBUTTONInitialDisp() 
{
	// TODO: この位置にコントロール通知ハンドラ用のコードを追加してください
	m_InitialDisp = Phyout[1];
	UpdateData(FALSE);
	CButton* myBTN1=(CButton*)GetDlgItem(IDC_BUTTON_UpdateDisp);
	myBTN1->EnableWindow(TRUE);
}

void CTransAdjustment::OnBUTTONEndDisp() 
{
	// TODO: この位置にコントロール通知ハンドラ用のコードを追加してください
	m_FinalDisp = Phyout[1];
	UpdateData(FALSE);	
	CButton* myBTN1=(CButton*)GetDlgItem(IDC_BUTTON_UpdateDisp);
	myBTN1->EnableWindow(TRUE);
}

void CTransAdjustment::OnBUTTONUpdateDisp() 
{
	// TODO: この位置にコントロール通知ハンドラ用のコードを追加してください
	UpdateData(TRUE);
	Cal_c[1]=Cal_c[1]+(m_InitialDisp-m_FinalDisp);
	CButton* myBTN1=(CButton*)GetDlgItem(IDC_BUTTON_UpdateDisp);
	myBTN1->EnableWindow(FALSE);
}

void CTransAdjustment::OnBUTTONInitialBullet() 
{
	// TODO: この位置にコントロール通知ハンドラ用のコードを追加してください
	m_InitialBullet = Phyout[4];
	UpdateData(FALSE);	
	CButton* myBTN1=(CButton*)GetDlgItem(IDC_BUTTON_UpdateBullet);
	myBTN1->EnableWindow(TRUE);
}

void CTransAdjustment::OnBUTTONEndBullet() 
{
	// TODO: この位置にコントロール通知ハンドラ用のコードを追加してください
	m_FinalBullet = Phyout[4];
	UpdateData(FALSE);	
	CButton* myBTN1=(CButton*)GetDlgItem(IDC_BUTTON_UpdateBullet);
	myBTN1->EnableWindow(TRUE);
}

void CTransAdjustment::OnBUTTONUpdateBullet() 
{
	// TODO: この位置にコントロール通知ハンドラ用のコードを追加してください
	UpdateData(TRUE);
	Cal_c[4]=Cal_c[4]+(m_InitialBullet-m_FinalBullet);
	CButton* myBTN1=(CButton*)GetDlgItem(IDC_BUTTON_UpdateBullet);
	myBTN1->EnableWindow(FALSE);
}

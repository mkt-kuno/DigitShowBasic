// Control_MLoading.cpp : インプリメンテーション ファイル
//

#include "stdafx.h"
#include "DigitShowBasic.h"
#include "Control_MLoading.h"
#include "DigitShowBasicDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CControl_MLoading ダイアログ
extern	int		Control_ID;
extern	Control	ControlData[16];

CControl_MLoading::CControl_MLoading(CWnd* pParent /*=NULL*/)
	: CDialog(CControl_MLoading::IDD, pParent)
{
	//{{AFX_DATA_INIT(CControl_MLoading)
	m_MotorCruch = ControlData[3].MotorCruch;
	m_MotorSpeed = ControlData[3].MotorSpeed;
	m_flag0 = ControlData[3].flag[0];
	m_q = ControlData[3].q;
	//}}AFX_DATA_INIT
}


void CControl_MLoading::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CControl_MLoading)
	DDX_Text(pDX, IDC_EDIT_Motor_Cruch, m_MotorCruch);
	DDV_MinMaxInt(pDX, m_MotorCruch, 0, 1);
	DDX_Text(pDX, IDC_EDIT_MotorSpeed, m_MotorSpeed);
	DDX_Text(pDX, IDC_EDIT_flag0, m_flag0);
	DDX_Text(pDX, IDC_EDIT_q, m_q);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CControl_MLoading, CDialog)
	//{{AFX_MSG_MAP(CControl_MLoading)
	ON_BN_CLICKED(IDC_BUTTON_Update, OnBUTTONUpdate)
	ON_BN_CLICKED(IDC_BUTTON_Reflesh, OnBUTTONReflesh)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CControl_MLoading メッセージ ハンドラ

void CControl_MLoading::OnBUTTONUpdate() 
{
	// TODO: この位置にコントロール通知ハンドラ用のコードを追加してください
	UpdateData(TRUE);	
	ControlData[3].MotorCruch = m_MotorCruch;
	ControlData[3].MotorSpeed = m_MotorSpeed;
	if(m_flag0==0) ControlData[3].flag[0]=FALSE;
	if(m_flag0==1) ControlData[3].flag[0]=TRUE;
	ControlData[3].q = m_q;
	ControlData[4]=ControlData[3];
}

void CControl_MLoading::OnBUTTONReflesh() 
{
	// TODO: この位置にコントロール通知ハンドラ用のコードを追加してください
	if(Control_ID==4) ControlData[3]=ControlData[4];
	m_MotorCruch = ControlData[3].MotorCruch;
	m_MotorSpeed = ControlData[3].MotorSpeed;
	m_flag0 = ControlData[3].flag[0];
	m_q = ControlData[3].q;	
	UpdateData(FALSE);	
}

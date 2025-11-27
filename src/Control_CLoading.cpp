// Control_CLoading.cpp : インプリメンテーション ファイル
//

#include "stdafx.h"
#include "DigitShowBasic.h"
#include "Control_CLoading.h"

#include "DigitShowBasicDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CControl_CLoading ダイアログ
extern	int		Control_ID;
extern	Control	ControlData[16];

CControl_CLoading::CControl_CLoading(CWnd* pParent /*=NULL*/)
	: CDialog(CControl_CLoading::IDD, pParent)
{
	//{{AFX_DATA_INIT(CControl_CLoading)
	m_flag0 = ControlData[5].flag[0];
	m_MotorSpeed = ControlData[5].MotorSpeed;
	m_q_lower = ControlData[5].sigma[0];
	m_q_upper = ControlData[5].sigma[1];
	m_time0 = ControlData[5].time[0];
	m_time1 = ControlData[5].time[1];
	m_time2 = ControlData[5].time[2];
	//}}AFX_DATA_INIT
}


void CControl_CLoading::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CControl_CLoading)
	DDX_Text(pDX, IDC_EDIT_flag0, m_flag0);
	DDX_Text(pDX, IDC_EDIT_MotorSpeed, m_MotorSpeed);
	DDX_Text(pDX, IDC_EDIT_q_lower, m_q_lower);
	DDX_Text(pDX, IDC_EDIT_q_upper, m_q_upper);
	DDX_Text(pDX, IDC_EDIT_time0, m_time0);
	DDX_Text(pDX, IDC_EDIT_time1, m_time1);
	DDX_Text(pDX, IDC_EDIT_time2, m_time2);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CControl_CLoading, CDialog)
	//{{AFX_MSG_MAP(CControl_CLoading)
	ON_BN_CLICKED(IDC_BUTTON_Update, OnBUTTONUpdate)
	ON_BN_CLICKED(IDC_BUTTON_Reflesh, OnBUTTONReflesh)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CControl_CLoading メッセージ ハンドラ

void CControl_CLoading::OnBUTTONUpdate() 
{
	// TODO: この位置にコントロール通知ハンドラ用のコードを追加してください
	UpdateData(TRUE);
	if(m_flag0==0)	ControlData[5].flag[0] = FALSE;
	if(m_flag0==1)	ControlData[5].flag[0] = TRUE;
	ControlData[5].MotorSpeed = m_MotorSpeed;
	ControlData[5].sigma[0] = m_q_lower;
	ControlData[5].sigma[1] = m_q_upper;
	ControlData[5].time[0] = m_time0;
	ControlData[5].time[1] = m_time1;
	ControlData[5].time[2] = m_time2;
	ControlData[6]=ControlData[5];
	
}

void CControl_CLoading::OnBUTTONReflesh() 
{
	// TODO: この位置にコントロール通知ハンドラ用のコードを追加してください
	if(Control_ID==6) ControlData[5]=ControlData[6];
	m_flag0 = ControlData[5].flag[0];
	m_MotorSpeed = ControlData[5].MotorSpeed;
	m_q_lower = ControlData[5].sigma[0];
	m_q_upper = ControlData[5].sigma[1];
	m_time0 = ControlData[5].time[0];
	m_time1 = ControlData[5].time[1];
	m_time2 = ControlData[5].time[2];
	UpdateData(FALSE);	
}

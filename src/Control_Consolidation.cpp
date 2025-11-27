// Control_Consolidation.cpp : インプリメンテーション ファイル
//

#include "stdafx.h"
#include "DigitShowBasic.h"
#include "Control_Consolidation.h"
#include "DigitShowBasicDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CControl_Consolidation ダイアログ
extern	Control		ControlData[16];

CControl_Consolidation::CControl_Consolidation(CWnd* pParent /*=NULL*/)
	: CDialog(CControl_Consolidation::IDD, pParent)
{
	//{{AFX_DATA_INIT(CControl_Consolidation)
	m_MotorK0 = ControlData[2].K0;
	m_MotorSpeed = ControlData[2].MotorSpeed;
	m_MotorSrRate = ControlData[2].sigmaRate[2];
	m_MotorESa = ControlData[2].e_sigma[0];	
	//}}AFX_DATA_INIT
}

void CControl_Consolidation::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CControl_Consolidation)
	DDX_Text(pDX, IDC_EDIT_Motor_K0, m_MotorK0);
	DDX_Text(pDX, IDC_EDIT_Motor_speed, m_MotorSpeed);
	DDX_Text(pDX, IDC_EDIT_Motor_sr_rate, m_MotorSrRate);
	DDX_Text(pDX, IDC_EDIT_Motor_e_sa, m_MotorESa);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CControl_Consolidation, CDialog)
	//{{AFX_MSG_MAP(CControl_Consolidation)
	ON_BN_CLICKED(IDC_BUTTON_Update, OnBUTTONUpdate)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CControl_Consolidation メッセージ ハンドラ

void CControl_Consolidation::OnBUTTONUpdate() 
{
	// TODO: この位置にコントロール通知ハンドラ用のコードを追加してください
	UpdateData(TRUE);
	ControlData[2].e_sigma[0] = m_MotorESa;
	ControlData[2].K0 = m_MotorK0;
	ControlData[2].sigmaRate[2] = m_MotorSrRate;
	ControlData[2].MotorSpeed = m_MotorSpeed;
}

// SamplingSettings.cpp : インプリメンテーション ファイル
//

#include "stdafx.h"
#include "DigitShowBasic.h"
#include "SamplingSettings.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSamplingSettings ダイアログ
extern	int			NUMAD;
extern	int			AdMaxCH;
extern	short		AdMemoryType[2];
extern	float		AdSamplingClock[2];
extern	long		AdSamplingTimes[2];
extern	int			SavingTime;
extern	long		TotalSamplingTimes;
extern	float		AllocatedMemory;
extern	int			AvSmplNum;

extern	bool		Flag_FIFO;

extern	unsigned int	TimeInterval_1;	// Time interval (ms) to display output data.		
extern	unsigned int	TimeInterval_2;	// Time interval (ms) to feed back.		
extern	unsigned int	TimeInterval_3;	// Time interval (ms) to save the data.


CSamplingSettings::CSamplingSettings(CWnd* pParent /*=NULL*/)
	: CDialog(CSamplingSettings::IDD, pParent)
{
	//{{AFX_DATA_INIT(CSamplingSettings)
	m_TimeInterval1 = 0;
	m_TimeInterval2 = 0;
	m_TimeInterval3 = 0;
	m_AllocatedMemory = _T("");
	m_AvSmplNum = 0;
	m_Channels = 0;
	m_EventSamplingTimes = 0;
	m_MemoryType = _T("");
	m_SamplingClock = 0.0f;
	m_SavingTime = 0;
	m_TotalSamplingTimes = 0;
	//}}AFX_DATA_INIT
}


void CSamplingSettings::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSamplingSettings)
	DDX_Text(pDX, IDC_EDIT_TimeInterval1, m_TimeInterval1);
	DDX_Text(pDX, IDC_EDIT_TimeInterval2, m_TimeInterval2);
	DDX_Text(pDX, IDC_EDIT_TimeInterval3, m_TimeInterval3);
	DDX_Text(pDX, IDC_EDIT_AllocatedMemory, m_AllocatedMemory);
	DDX_Text(pDX, IDC_EDIT_AvSmplNum, m_AvSmplNum);
	DDX_Text(pDX, IDC_EDIT_Channels, m_Channels);
	DDX_Text(pDX, IDC_EDIT_EventSamplingTimes, m_EventSamplingTimes);
	DDX_Text(pDX, IDC_EDIT_MemoryType, m_MemoryType);
	DDX_Text(pDX, IDC_EDIT_SamplingClock, m_SamplingClock);
	DDX_Text(pDX, IDC_EDIT_SavingTime, m_SavingTime);
	DDX_Text(pDX, IDC_EDIT_TotalSamplingTimes, m_TotalSamplingTimes);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CSamplingSettings, CDialog)
	//{{AFX_MSG_MAP(CSamplingSettings)
	ON_BN_CLICKED(IDC_BUTTON_Check, OnBUTTONCheck)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSamplingSettings メッセージ ハンドラ

BOOL CSamplingSettings::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: この位置に初期化の補足処理を追加してください
	m_TimeInterval1 = TimeInterval_1;
	m_TimeInterval2 = TimeInterval_2;
	m_TimeInterval3 = TimeInterval_3;
//
	m_AllocatedMemory.Format("%.1f",AllocatedMemory);
	m_Channels = AdMaxCH;
	m_EventSamplingTimes = AdSamplingTimes[0];
	m_AvSmplNum = AvSmplNum;
	if(AdMemoryType[0]==0) m_MemoryType = _T("FIFO");
	if(AdMemoryType[0]==1) m_MemoryType = _T("RING");
	m_SamplingClock = AdSamplingClock[0]/1000.0f;
	m_SavingTime = SavingTime;
	m_TotalSamplingTimes = TotalSamplingTimes;
	UpdateData(FALSE);
//
	CButton* myBTN1=(CButton*)GetDlgItem(IDC_BUTTON_Check);
	CButton* myBTN2=(CButton*)GetDlgItem(IDOK);
	if(Flag_FIFO==TRUE)	myBTN1->EnableWindow(FALSE);
	myBTN2->EnableWindow(FALSE);
	
	return TRUE;  // コントロールにフォーカスを設定しないとき、戻り値は TRUE となります
	              // 例外: OCX プロパティ ページの戻り値は FALSE となります
}


void CSamplingSettings::OnBUTTONCheck() 
{
	// TODO: この位置にコントロール通知ハンドラ用のコードを追加してください
	UpdateData(TRUE);
	m_TotalSamplingTimes=long(m_SavingTime*1000/m_SamplingClock);
	m_AllocatedMemory.Format("%.1f",4*AdMaxCH*m_TotalSamplingTimes/1024.0f/1024.0f);
	m_EventSamplingTimes=long(TimeInterval_1/m_SamplingClock);
	UpdateData(FALSE);

	CButton* myBTN1=(CButton*)GetDlgItem(IDOK);
	myBTN1->EnableWindow(TRUE);
	
}

void CSamplingSettings::OnOK() 
{
	// TODO: この位置にその他の検証用のコードを追加してください
	UpdateData(TRUE);
	AdSamplingClock[0] = m_SamplingClock*1000.0f;
	SavingTime = m_SavingTime;
	AdSamplingTimes[0] = m_EventSamplingTimes;
	TotalSamplingTimes=long(SavingTime*1000000/AdSamplingClock[0]);
	AllocatedMemory=4*AdMaxCH*m_TotalSamplingTimes/1024.0f/1024.0f;
	m_AllocatedMemory.Format("%.1f",AllocatedMemory);
	m_TotalSamplingTimes=TotalSamplingTimes;
	UpdateData(FALSE);

	if(NUMAD>1){
		AdSamplingClock[1]=AdSamplingClock[0];
		AdSamplingTimes[1]=AdSamplingTimes[0];
	}
	
	CDialog::OnOK();
}


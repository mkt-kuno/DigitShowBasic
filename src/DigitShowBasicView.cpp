/*
 * DigitShowBasic - Triaxial Test Machine Control Software
 * Copyright (C) 2025 Makoto KUNO
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */


#include "time.h"
#include "stdafx.h"
#include "DigitShowBasic.h"

#include "DigitShowBasicDoc.h"
#include "DigitShowBasicView.h"

#include "caio.h"
#include "SamplingSettings.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////
// CDigitShowBasicView
// --- A/D ---
extern	int			NUMAD,NUMDA;		// The Number of A/D and D/A boards
extern	short		AdId[2];			// ID
extern	short		AdChannels[2];		// The Number of Channel
extern	long		DaData[8];			// Array of data for D/A
extern	long		AdData0[262144],AdData1[262144];	// Array of data for D/A 
extern	int			AdMaxCH;			// Maximum Channel Number
extern	short		AdMemoryType[2];	// Variant showes the Memory Type
extern	float		AdSamplingClock[2]; // Time interval of a A/D conversion
extern	long		AdSamplingTimes[2]; // The Number of Sampling 
extern	long		AdEvent;
extern	float		SavingClock;
extern	int			SavingTime;
extern	long		TotalSamplingTimes;
extern	long		CurrentSamplingTimes;
extern	float		AllocatedMemory;
extern	char		ErrorString[256];
extern	CString		TextString;

extern	PVOID		pSmplData0,pSmplData1,pSmplData2;	// Source of binary data

//------
extern	float		Vout[64];	
extern	double		Phyout[64];				
extern	double		CalParam[64];
				
//---Flag---
extern	bool		Flag_SetBoard;
extern	bool		Flag_FIFO;
extern	bool		Flag_SaveData;
		bool		Flag_Ctrl;
		//---Control---
extern	int	Control_ID;

//---Time---
extern	CTime		StartTime, NowTime;
extern	CTimeSpan	SpanTime;
extern	CString		SNowTime;
extern	long		SequentTime1;
extern	double		SequentTime2;
extern	double		CtrlStepTime;
extern	unsigned int	TimeInterval_1;
extern	unsigned int	TimeInterval_2;
extern	unsigned int	TimeInterval_3;

//---File---
extern	FILE        *FileSaveData0;
extern	FILE		*FileSaveData1;
extern	FILE		*FileSaveData2;

IMPLEMENT_DYNCREATE(CDigitShowBasicView, CFormView)

BEGIN_MESSAGE_MAP(CDigitShowBasicView, CFormView)
	//{{AFX_MSG_MAP(CDigitShowBasicView)
	ON_WM_CTLCOLOR()
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_BUTTON_CtrlOff, OnBUTTONCtrlOff)
	ON_BN_CLICKED(IDC_BUTTON_CtrlOn, OnBUTTONCtrlOn)
	ON_BN_CLICKED(IDC_BUTTON_StartSave, OnBUTTONStartSave)
	ON_BN_CLICKED(IDC_BUTTON_StopSave, OnBUTTONStopSave)
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_BUTTON_InterceptSave, OnBUTTONInterceptSave)
	ON_BN_CLICKED(IDC_BUTTON_FIFOStart, OnBUTTONFIFOStart)
	ON_BN_CLICKED(IDC_BUTTON_FIFOStop, OnBUTTONFIFOStop)
	ON_BN_CLICKED(IDC_BUTTON_WriteData, OnBUTTONWriteData)
	ON_BN_CLICKED(IDC_BUTTON_SetCtrlID, OnBUTTONSetCtrlID)
	ON_BN_CLICKED(IDC_BUTTON_SetTimeInterval, OnBUTTONSetTimeInterval)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDigitShowBasicView クラスの構築/消滅

CDigitShowBasicView::CDigitShowBasicView()
	: CFormView(CDigitShowBasicView::IDD)
{
	//{{AFX_DATA_INIT(CDigitShowBasicView)
	m_Vout00 = _T("");
	m_Vout01 = _T("");
	m_Vout02 = _T("");
	m_Vout03 = _T("");
	m_Vout04 = _T("");
	m_Vout05 = _T("");
	m_Vout06 = _T("");
	m_Vout07 = _T("");
	m_Vout08 = _T("");
	m_Vout09 = _T("");
	m_Vout10 = _T("");
	m_Vout11 = _T("");
	m_Vout12 = _T("");
	m_Vout13 = _T("");
	m_Vout14 = _T("");
	m_Vout15 = _T("");
	m_Vout16 = _T("");
	m_Vout17 = _T("");
	m_Vout18 = _T("");
	m_Vout19 = _T("");
	m_Vout20 = _T("");
	m_Vout21 = _T("");
	m_Vout22 = _T("");
	m_Vout23 = _T("");
	m_Vout24 = _T("");
	m_Vout25 = _T("");
	m_Vout26 = _T("");
	m_Vout27 = _T("");
	m_Vout28 = _T("");
	m_Vout29 = _T("");
	m_Vout30 = _T("");
	m_Vout31 = _T("");
	m_Phyout00 = _T("");
	m_Phyout01 = _T("");
	m_Phyout02 = _T("");
	m_Phyout03 = _T("");
	m_Phyout04 = _T("");
	m_Phyout05 = _T("");
	m_Phyout06 = _T("");
	m_Phyout07 = _T("");
	m_Phyout08 = _T("");
	m_Phyout09 = _T("");
	m_Phyout10 = _T("");
	m_Phyout11 = _T("");
	m_Phyout12 = _T("");
	m_Phyout13 = _T("");
	m_Phyout14 = _T("");
	m_Phyout15 = _T("");
	m_Phyout16 = _T("");
	m_Phyout17 = _T("");
	m_Phyout18 = _T("");
	m_Phyout19 = _T("");
	m_Phyout20 = _T("");
	m_Phyout21 = _T("");
	m_Phyout22 = _T("");
	m_Phyout23 = _T("");
	m_Phyout24 = _T("");
	m_Phyout25 = _T("");
	m_Phyout26 = _T("");
	m_Phyout27 = _T("");
	m_Phyout28 = _T("");
	m_Phyout29 = _T("");
	m_Phyout30 = _T("");
	m_Phyout31 = _T("");
	m_Para00 = _T("");
	m_Para01 = _T("");
	m_Para02 = _T("");
	m_Para03 = _T("");
	m_Para04 = _T("");
	m_Para05 = _T("");
	m_Para06 = _T("");
	m_Para07 = _T("");
	m_Para08 = _T("");
	m_Para09 = _T("");
	m_Para10 = _T("");
	m_Para11 = _T("");
	m_Para12 = _T("");
	m_Para13 = _T("");
	m_Para14 = _T("");
	m_Para15 = _T("");
	m_Ctrl_ID = 0;
	m_NowTime = _T("");
	m_SeqTime = 0;
	m_SamplingTime = TimeInterval_3;
	m_FileName = _T("");
	//}}AFX_DATA_INIT

	Flag_Ctrl=FALSE;
	m_pEditBrush= new CBrush(RGB(255,255,255));
	m_pStaticBrush= new CBrush(RGB(0,128,128));	
	m_pDlgBrush= new CBrush(RGB(0,128,128));
}

CDigitShowBasicView::~CDigitShowBasicView()
{
	CDigitShowBasicDoc* pDoc=(CDigitShowBasicDoc *)GetDocument();
	pDoc->CloseBoard();
}

void CDigitShowBasicView::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDigitShowBasicView)
	DDX_Text(pDX, IDC_EDIT_Vout00, m_Vout00);
	DDX_Text(pDX, IDC_EDIT_Vout01, m_Vout01);
	DDX_Text(pDX, IDC_EDIT_Vout02, m_Vout02);
	DDX_Text(pDX, IDC_EDIT_Vout03, m_Vout03);
	DDX_Text(pDX, IDC_EDIT_Vout04, m_Vout04);
	DDX_Text(pDX, IDC_EDIT_Vout05, m_Vout05);
	DDX_Text(pDX, IDC_EDIT_Vout06, m_Vout06);
	DDX_Text(pDX, IDC_EDIT_Vout07, m_Vout07);
	DDX_Text(pDX, IDC_EDIT_Vout08, m_Vout08);
	DDX_Text(pDX, IDC_EDIT_Vout09, m_Vout09);
	DDX_Text(pDX, IDC_EDIT_Vout10, m_Vout10);
	DDX_Text(pDX, IDC_EDIT_Vout11, m_Vout11);
	DDX_Text(pDX, IDC_EDIT_Vout12, m_Vout12);
	DDX_Text(pDX, IDC_EDIT_Vout13, m_Vout13);
	DDX_Text(pDX, IDC_EDIT_Vout14, m_Vout14);
	DDX_Text(pDX, IDC_EDIT_Vout15, m_Vout15);
	DDX_Text(pDX, IDC_EDIT_Vout16, m_Vout16);
	DDX_Text(pDX, IDC_EDIT_Vout17, m_Vout17);
	DDX_Text(pDX, IDC_EDIT_Vout18, m_Vout18);
	DDX_Text(pDX, IDC_EDIT_Vout19, m_Vout19);
	DDX_Text(pDX, IDC_EDIT_Vout20, m_Vout20);
	DDX_Text(pDX, IDC_EDIT_Vout21, m_Vout21);
	DDX_Text(pDX, IDC_EDIT_Vout22, m_Vout22);
	DDX_Text(pDX, IDC_EDIT_Vout23, m_Vout23);
	DDX_Text(pDX, IDC_EDIT_Vout24, m_Vout24);
	DDX_Text(pDX, IDC_EDIT_Vout25, m_Vout25);
	DDX_Text(pDX, IDC_EDIT_Vout26, m_Vout26);
	DDX_Text(pDX, IDC_EDIT_Vout27, m_Vout27);
	DDX_Text(pDX, IDC_EDIT_Vout28, m_Vout28);
	DDX_Text(pDX, IDC_EDIT_Vout29, m_Vout29);
	DDX_Text(pDX, IDC_EDIT_Vout30, m_Vout30);
	DDX_Text(pDX, IDC_EDIT_Vout31, m_Vout31);
	DDX_Text(pDX, IDC_EDIT_Phyout00, m_Phyout00);
	DDX_Text(pDX, IDC_EDIT_Phyout01, m_Phyout01);
	DDX_Text(pDX, IDC_EDIT_Phyout02, m_Phyout02);
	DDX_Text(pDX, IDC_EDIT_Phyout03, m_Phyout03);
	DDX_Text(pDX, IDC_EDIT_Phyout04, m_Phyout04);
	DDX_Text(pDX, IDC_EDIT_Phyout05, m_Phyout05);
	DDX_Text(pDX, IDC_EDIT_Phyout06, m_Phyout06);
	DDX_Text(pDX, IDC_EDIT_Phyout07, m_Phyout07);
	DDX_Text(pDX, IDC_EDIT_Phyout08, m_Phyout08);
	DDX_Text(pDX, IDC_EDIT_Phyout09, m_Phyout09);
	DDX_Text(pDX, IDC_EDIT_Phyout10, m_Phyout10);
	DDX_Text(pDX, IDC_EDIT_Phyout11, m_Phyout11);
	DDX_Text(pDX, IDC_EDIT_Phyout12, m_Phyout12);
	DDX_Text(pDX, IDC_EDIT_Phyout13, m_Phyout13);
	DDX_Text(pDX, IDC_EDIT_Phyout14, m_Phyout14);
	DDX_Text(pDX, IDC_EDIT_Phyout15, m_Phyout15);
	DDX_Text(pDX, IDC_EDIT_Phyout16, m_Phyout16);
	DDX_Text(pDX, IDC_EDIT_Phyout17, m_Phyout17);
	DDX_Text(pDX, IDC_EDIT_Phyout18, m_Phyout18);
	DDX_Text(pDX, IDC_EDIT_Phyout19, m_Phyout19);
	DDX_Text(pDX, IDC_EDIT_Phyout20, m_Phyout20);
	DDX_Text(pDX, IDC_EDIT_Phyout21, m_Phyout21);
	DDX_Text(pDX, IDC_EDIT_Phyout22, m_Phyout22);
	DDX_Text(pDX, IDC_EDIT_Phyout23, m_Phyout23);
	DDX_Text(pDX, IDC_EDIT_Phyout24, m_Phyout24);
	DDX_Text(pDX, IDC_EDIT_Phyout25, m_Phyout25);
	DDX_Text(pDX, IDC_EDIT_Phyout26, m_Phyout26);
	DDX_Text(pDX, IDC_EDIT_Phyout27, m_Phyout27);
	DDX_Text(pDX, IDC_EDIT_Phyout28, m_Phyout28);
	DDX_Text(pDX, IDC_EDIT_Phyout29, m_Phyout29);
	DDX_Text(pDX, IDC_EDIT_Phyout30, m_Phyout30);
	DDX_Text(pDX, IDC_EDIT_Phyout31, m_Phyout31);
	DDX_Text(pDX, IDC_EDIT_Para00, m_Para00);
	DDX_Text(pDX, IDC_EDIT_Para01, m_Para01);
	DDX_Text(pDX, IDC_EDIT_Para02, m_Para02);
	DDX_Text(pDX, IDC_EDIT_Para03, m_Para03);
	DDX_Text(pDX, IDC_EDIT_Para04, m_Para04);
	DDX_Text(pDX, IDC_EDIT_Para05, m_Para05);
	DDX_Text(pDX, IDC_EDIT_Para06, m_Para06);
	DDX_Text(pDX, IDC_EDIT_Para07, m_Para07);
	DDX_Text(pDX, IDC_EDIT_Para08, m_Para08);
	DDX_Text(pDX, IDC_EDIT_Para09, m_Para09);
	DDX_Text(pDX, IDC_EDIT_Para10, m_Para10);
	DDX_Text(pDX, IDC_EDIT_Para11, m_Para11);
	DDX_Text(pDX, IDC_EDIT_Para12, m_Para12);
	DDX_Text(pDX, IDC_EDIT_Para13, m_Para13);
	DDX_Text(pDX, IDC_EDIT_Para14, m_Para14);
	DDX_Text(pDX, IDC_EDIT_Para15, m_Para15);
	DDX_Text(pDX, IDC_EDIT_Ctrl_ID, m_Ctrl_ID);
	DDX_Text(pDX, IDC_EDIT_NowTime, m_NowTime);
	DDX_Text(pDX, IDC_EDIT_SeqTime, m_SeqTime);
	DDX_Text(pDX, IDC_EDIT_SamplingTime, m_SamplingTime);
	DDV_MinMaxLong(pDX, m_SamplingTime, 100, 86400000);
	DDX_Text(pDX, IDC_EDIT_FileName, m_FileName);
	//}}AFX_DATA_MAP
}

BOOL CDigitShowBasicView::PreCreateWindow(CREATESTRUCT& cs)
{

	//  修正してください。
	return CFormView::PreCreateWindow(cs);
}


/////////////////////////////////////////////////////////////////////////////
// CDigitShowBasicView クラスの診断

#ifdef _DEBUG
void CDigitShowBasicView::AssertValid() const
{
	CFormView::AssertValid();
}

void CDigitShowBasicView::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}

CDigitShowBasicDoc* CDigitShowBasicView::GetDocument() // 非デバッグ バージョンはインラインです。
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CDigitShowBasicDoc)));
	return (CDigitShowBasicDoc*)m_pDocument;
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CDigitShowBasicView クラスのメッセージ ハンドラ
void CDigitShowBasicView::OnInitialUpdate()
{
	float AiScanClocka;
	long		Ret;
	CFormView::OnInitialUpdate();
	GetParentFrame()->RecalcLayout();
	ResizeParentToFit();
	CButton* myBTN1=(CButton*)GetDlgItem(IDC_BUTTON_CtrlOff);
	CButton* myBTN2=(CButton*)GetDlgItem(IDC_BUTTON_StopSave);
	CButton* myBTN3=(CButton*)GetDlgItem(IDC_BUTTON_InterceptSave);
	CButton* myBTN4=(CButton*)GetDlgItem(IDC_BUTTON_FIFOStop);
	CButton* myBTN5=(CButton*)GetDlgItem(IDC_BUTTON_WriteData);
	myBTN1->EnableWindow(FALSE);
	myBTN2->EnableWindow(FALSE);
	myBTN3->EnableWindow(FALSE);
	myBTN4->EnableWindow(FALSE);
	myBTN5->EnableWindow(FALSE);
	CString tmp;
	CComboBox* m_Combo1 = (CComboBox*)GetDlgItem(IDC_COMBO_Control_ID);
	m_Combo1->InsertString(-1,"0");	m_Combo1->InsertString(-1,"1");
	m_Combo1->InsertString(-1,"2");	m_Combo1->InsertString(-1,"3");
	m_Combo1->InsertString(-1,"4");	m_Combo1->InsertString(-1,"5");
	m_Combo1->InsertString(-1,"6");	m_Combo1->InsertString(-1,"7");
	m_Combo1->InsertString(-1,"8");	m_Combo1->InsertString(-1,"9");
	m_Combo1->InsertString(-1,"10");	m_Combo1->InsertString(-1,"11");
	m_Combo1->InsertString(-1,"12");	m_Combo1->InsertString(-1,"13");
	m_Combo1->InsertString(-1,"14");	m_Combo1->InsertString(-1,"15");
	m_Combo1->SetWindowText("0");
	CComboBox* m_Combo2 = (CComboBox*)GetDlgItem(IDC_COMBO_SamplingTime);
	m_Combo2->InsertString(-1,"0.2 s");		m_Combo2->InsertString(-1,"0.5 s");
	m_Combo2->InsertString(-1,"1.0 s");		m_Combo2->InsertString(-1,"2.0 s");
	m_Combo2->InsertString(-1,"3.0 s");		m_Combo2->InsertString(-1,"5.0 s");
	m_Combo2->InsertString(-1,"10.0 s");	m_Combo2->InsertString(-1,"20.0 s");
	m_Combo2->InsertString(-1,"30.0 s");	m_Combo2->InsertString(-1,"1.0 min");
	m_Combo2->InsertString(-1,"2.0 min");	m_Combo2->InsertString(-1,"3.0 min");
	m_Combo2->InsertString(-1,"5.0 min");	m_Combo2->InsertString(-1,"10.0 min");	
	m_Combo2->SetWindowText("1.0 s");
	CDigitShowBasicDoc* pDoc=(CDigitShowBasicDoc *)GetDocument();
	pDoc->OpenBoard();
	if(Flag_SetBoard){
		if(NUMAD>0)	{
			Ret = AioSetAiSamplingClock ( AdId[0] , 1000 );
			Ret = AioGetAiSamplingClock ( AdId[0] , &AdSamplingClock[0] );

//			Ret = AioGetAiScanClock(AdId[0], &AiScanClocka);
			AiScanClocka = 60.0;
			Ret = AioSetAiScanClock(AdId[0], AiScanClocka);

			AdSamplingTimes[0]=long(TimeInterval_1*1000/AdSamplingClock[0]);
			Ret = AioSetAiEventSamplingTimes ( AdId[0] , AdSamplingTimes[0] );
			Ret = AioGetAiEventSamplingTimes ( AdId[0] , &AdSamplingTimes[0] );
			Ret = AioSetAiStopTrigger(AdId[0], 4);
			Ret = AioResetAiMemory(AdId[0]);
		}
		if(NUMAD>1)	{
			Ret = AioSetAiSamplingClock ( AdId[1] , 1000 );
			Ret = AioGetAiSamplingClock ( AdId[1] , &AdSamplingClock[1] );

			//	Ret = AioGetAiScanClock(AdId[1], &AiScanClocka);
			AiScanClocka = 60.0;
			Ret = AioSetAiScanClock(AdId[1], AiScanClocka);

			AdSamplingTimes[1]=long(TimeInterval_1*1000/AdSamplingClock[1]);
			Ret = AioSetAiEventSamplingTimes ( AdId[1] , AdSamplingTimes[1] );
			Ret = AioGetAiEventSamplingTimes ( AdId[1] , &AdSamplingTimes[1] );
			Ret = AioSetAiStopTrigger(AdId[1], 4);
			Ret = AioResetAiMemory(AdId[1]);
		}
		AdEvent = AIE_DATA_NUM | AIE_OFERR | AIE_SCERR | AIE_ADERR;
		Ret = AioSetAiEvent(AdId[NUMAD-1], m_hWnd, AdEvent);
		Ret = AioSetAiEventSamplingTimes(AdId[NUMAD-1], AdSamplingTimes[NUMAD-1]);
		if(NUMAD>0) Ret = AioStartAi(AdId[0]);
		if(NUMAD>1) Ret = AioStartAi(AdId[1]);
	}
	SetTimer(1,TimeInterval_1,NULL);	
}

HBRUSH CDigitShowBasicView::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) 
{
	switch(nCtlColor)
	{
		case CTLCOLOR_EDIT:
			pDC->SetBkColor(RGB(255,255,255));				// white background
			pDC->SetTextColor(RGB(0,0,0));					// black for text color in EDIT
			return (HBRUSH)(m_pEditBrush->GetSafeHandle());	// EDITBOX color
		case CTLCOLOR_STATIC:								// Static label properties  
			pDC->SetBkMode(TRANSPARENT);	
			pDC->SetTextColor(RGB(255,255,255)); 
			return (HBRUSH)(m_pStaticBrush->GetSafeHandle());  
		case CTLCOLOR_DLG:					 				// Setting Dialog Box Color
			pDC->SetTextColor(RGB(0,128,128)); 
			return (HBRUSH)(m_pDlgBrush->GetSafeHandle());
		default:
			return CFormView::OnCtlColor(pDC, pWnd, nCtlColor);
	}	
}
void CDigitShowBasicView::OnDestroy() 
{
	CFormView::OnDestroy();

	delete	m_pEditBrush;
	delete	m_pStaticBrush;	
	delete	m_pDlgBrush;
}
void CDigitShowBasicView::OnTimer(UINT_PTR nIDEvent) 
{


	CDigitShowBasicDoc* pDoc=(CDigitShowBasicDoc *)GetDocument();

//	errno_t err;

	switch (nIDEvent)
	{
	case 1:
		{ 
			NowTime=NowTime.GetCurrentTime();
			SNowTime=NowTime.Format("%m/%d  %H:%M:%S");
			if(Flag_SaveData){
				SpanTime=NowTime-StartTime;
				SequentTime1=(long)SpanTime.GetTotalSeconds();
			}	
			if(Flag_SetBoard)	pDoc -> AD_INPUT();
			pDoc -> Cal_Physical();
			pDoc -> Cal_Param();
			ShowData();
		}
		break;
	case 2:
		{ 
			/*_ftime(&StepTime1);*/
			_ftime_s(&StepTime1);
			if(Flag_Ctrl==FALSE){
				StepTime0=StepTime1;
				Flag_Ctrl=TRUE;
			}
			CtrlStepTime=double(StepTime1.time-StepTime0.time)+double( (StepTime1.millitm-StepTime0.millitm)/1000.0 );
			StepTime0=StepTime1;
			if(Flag_SetBoard)	pDoc -> Control_DA(); 
		}
		break;
	case 3:
		{ 
			/*_ftime(&NowTime2);*/
			_ftime_s(&NowTime2);
			SequentTime2=double(NowTime2.time-StartTime2.time)+double( (NowTime2.millitm-StartTime2.millitm)/1000.0 );
			if(Flag_SetBoard)	pDoc -> AD_INPUT();
			pDoc -> Cal_Physical();
			pDoc -> Cal_Param();
			pDoc -> SaveToFile();
		}
		break;
	}	
	CFormView::OnTimer(nIDEvent);
}

void CDigitShowBasicView::ShowData()
{
	m_Vout00.Format("%11.4f",Vout[0]);	m_Vout01.Format("%11.4f",Vout[1]);
	m_Vout02.Format("%11.4f",Vout[2]);	m_Vout03.Format("%11.4f",Vout[3]);
	m_Vout04.Format("%11.4f",Vout[4]); 	m_Vout05.Format("%11.4f",Vout[5]);
	m_Vout06.Format("%11.4f",Vout[6]); 	m_Vout07.Format("%11.4f",Vout[7]);
	m_Vout08.Format("%11.4f",Vout[8]);	m_Vout09.Format("%11.4f",Vout[9]);
	m_Vout10.Format("%11.4f",Vout[10]);	m_Vout11.Format("%11.4f",Vout[11]);
	m_Vout12.Format("%11.4f",Vout[12]);	m_Vout13.Format("%11.4f",Vout[13]);
	m_Vout14.Format("%11.4f",Vout[14]);	m_Vout15.Format("%11.4f",Vout[15]);
	m_Vout16.Format("%11.4f",Vout[16]);	m_Vout17.Format("%11.4f",Vout[17]);
	m_Vout18.Format("%11.4f",Vout[18]);	m_Vout19.Format("%11.4f",Vout[19]);
	m_Vout20.Format("%11.4f",Vout[20]);	m_Vout21.Format("%11.4f",Vout[21]);
	m_Vout22.Format("%11.4f",Vout[22]);	m_Vout23.Format("%11.4f",Vout[23]);
	m_Vout24.Format("%11.4f",Vout[24]);	m_Vout25.Format("%11.4f",Vout[25]);
	m_Vout26.Format("%11.4f",Vout[26]);	m_Vout27.Format("%11.4f",Vout[27]);
	m_Vout28.Format("%11.4f",Vout[28]);	m_Vout29.Format("%11.4f",Vout[29]);
	m_Vout30.Format("%11.4f",Vout[30]);	m_Vout31.Format("%11.4f",Vout[31]);

	m_Phyout00.Format("%11.4f",Phyout[0]);	m_Phyout01.Format("%11.4f",Phyout[1]);
	m_Phyout02.Format("%11.4f",Phyout[2]);	m_Phyout03.Format("%11.4f",Phyout[3]);
	m_Phyout04.Format("%11.4f",Phyout[4]);	m_Phyout05.Format("%11.4f",Phyout[5]);
	m_Phyout06.Format("%11.4f",Phyout[6]);	m_Phyout07.Format("%11.4f",Phyout[7]);
	m_Phyout08.Format("%11.4f",Phyout[8]);	m_Phyout09.Format("%11.4f",Phyout[9]);
	m_Phyout10.Format("%11.4f",Phyout[10]);	m_Phyout11.Format("%11.4f",Phyout[11]);
	m_Phyout12.Format("%11.4f",Phyout[12]);	m_Phyout13.Format("%11.4f",Phyout[13]);
	m_Phyout14.Format("%11.4f",Phyout[14]);	m_Phyout15.Format("%11.4f",Phyout[15]);
	m_Phyout16.Format("%11.4f",Phyout[16]);	m_Phyout17.Format("%11.4f",Phyout[17]);
	m_Phyout18.Format("%11.4f",Phyout[18]);	m_Phyout19.Format("%11.4f",Phyout[19]);
	m_Phyout20.Format("%11.4f",Phyout[20]);	m_Phyout21.Format("%11.4f",Phyout[21]);
	m_Phyout22.Format("%11.4f",Phyout[22]);	m_Phyout23.Format("%11.4f",Phyout[23]);
	m_Phyout24.Format("%11.4f",Phyout[24]);	m_Phyout25.Format("%11.4f",Phyout[25]);
	m_Phyout26.Format("%11.4f",Phyout[26]);	m_Phyout27.Format("%11.4f",Phyout[27]);
	m_Phyout28.Format("%11.4f",Phyout[28]);	m_Phyout29.Format("%11.4f",Phyout[29]);
	m_Phyout30.Format("%11.4f",Phyout[30]);	m_Phyout31.Format("%11.4f",Phyout[31]);

	m_Para00.Format("%11.4f",CalParam[0]);	m_Para01.Format("%11.4f",CalParam[1]);
	m_Para02.Format("%11.4f",CalParam[2]);	m_Para03.Format("%11.4f",CalParam[3]);
	m_Para04.Format("%11.4f",CalParam[4]);	m_Para05.Format("%11.4f",CalParam[5]);
	m_Para06.Format("%11.4f",CalParam[6]);	m_Para07.Format("%11.4f",CalParam[7]);
	m_Para08.Format("%11.4f",CalParam[8]);	m_Para09.Format("%11.4f",CalParam[9]);
	m_Para10.Format("%11.4f",CalParam[10]);	m_Para11.Format("%11.4f",CalParam[11]);
	m_Para12.Format("%11.4f",CalParam[12]);	m_Para13.Format("%11.4f",CalParam[13]);
	m_Para14.Format("%11.4f",CalParam[14]);	m_Para15.Format("%11.4f",CalParam[15]);
	
	m_Ctrl_ID=Control_ID;
	m_NowTime=SNowTime;
	m_SeqTime=SequentTime1;
	m_SamplingTime=TimeInterval_3;
	if(Flag_FIFO) m_SamplingTime=long(AdSamplingClock[0]/1.0E+03);
	UpdateData(FALSE);
}

void CDigitShowBasicView::OnBUTTONCtrlOn() 
{

	CDigitShowBasicDoc* pDoc=(CDigitShowBasicDoc *)GetDocument();
	if(Flag_SetBoard){
		SetTimer(2,TimeInterval_2,NULL);
		CButton* myBTN1=(CButton*)GetDlgItem(IDC_BUTTON_CtrlOn);
		CButton* myBTN2=(CButton*)GetDlgItem(IDC_BUTTON_CtrlOff);
		myBTN1->EnableWindow(FALSE);	
		myBTN2->EnableWindow(TRUE);
		pDoc->Start_Control();
	}
}

void CDigitShowBasicView::OnBUTTONCtrlOff() 
{

	CDigitShowBasicDoc* pDoc=(CDigitShowBasicDoc *)GetDocument();
	KillTimer(2);
	Flag_Ctrl=FALSE;
	CButton* myBTN1=(CButton*)GetDlgItem(IDC_BUTTON_CtrlOn);
	CButton* myBTN2=(CButton*)GetDlgItem(IDC_BUTTON_CtrlOff);
	myBTN1->EnableWindow(TRUE);
	myBTN2->EnableWindow(FALSE);
	pDoc->Stop_Control();
}

void CDigitShowBasicView::OnBUTTONStartSave() 
{

	long		Ret;
	CString	TmpString;
	errno_t err;
	CDigitShowBasicDoc* pDoc=(CDigitShowBasicDoc *)GetDocument();

	if(Flag_FIFO==FALSE){
		CString	pFileName0, pFileName1, pFileName2;
		CFileDialog SaveFile_dlg( FALSE, NULL, "*.dat",  OFN_CREATEPROMPT | OFN_OVERWRITEPROMPT,
				"Data Files(*.dat)|*.dat| All Files(*.*)|*.*| |",NULL);
		if (SaveFile_dlg.DoModal()==IDOK){
			// File for saving the physical data 
			pFileName1= SaveFile_dlg.GetPathName();	
			m_FileName=	SaveFile_dlg.GetFileTitle();
			TmpString = SaveFile_dlg.GetFileExt();	
			if(TmpString == "" ){
				TmpString=".dat";
				pFileName1=pFileName1+TmpString;
				m_FileName=m_FileName+TmpString;
			}
			else if(TmpString != "dat"){
				TmpString=_T(".")+TmpString;
				pFileName1.Replace(TmpString,".dat");
				m_FileName=m_FileName+_T(".dat");
			}
			if((err = fopen_s(&FileSaveData1,(LPCSTR)pFileName1, _T("w"))) == 0)
			{
				fprintf(FileSaveData1,"%s	","Time(s)");
				fprintf(FileSaveData1,"%s	","Load_(N)");
				fprintf(FileSaveData1,"%s	","Disp.(mm)");
				fprintf(FileSaveData1,"%s	","Cell_P.(kPa)");
				fprintf(FileSaveData1,"%s	","ECellP.(kPa)");
				fprintf(FileSaveData1,"%s	","SP.Vol.(mm3)");
				fprintf(FileSaveData1,"%s	","V-LDT1_(mm)");
				fprintf(FileSaveData1,"%s	","V-LDT2_(mm)");
				fprintf(FileSaveData1,"%s	","CH07_(V)");
				fprintf(FileSaveData1,"%s	","CH08_(V)");
				fprintf(FileSaveData1,"%s	","CH09_(V)");
				fprintf(FileSaveData1,"%s	","CH10_(V)");
				fprintf(FileSaveData1,"%s	","CH11_(V)");
				fprintf(FileSaveData1,"%s	","CH12_(V)");
				fprintf(FileSaveData1,"%s	","CH13_(V)");
				fprintf(FileSaveData1,"%s	","CH14_(V)");
				fprintf(FileSaveData1,"%s	","CH15_(V)");
				fprintf(FileSaveData1,"\n");
			}


			// File for saving the voltage data
			pFileName0=pFileName1;
			pFileName0.Replace(".dat",".vlt");
			if((err = fopen_s(&FileSaveData0,(LPCSTR)pFileName0, _T("w"))) == 0)
			{
				fprintf(FileSaveData0,"%s	","Time(s)");
				fprintf(FileSaveData0,"%s	","CH00_(V)");
				fprintf(FileSaveData0,"%s	","CH01_(V)");
				fprintf(FileSaveData0,"%s	","CH02_(V)");
				fprintf(FileSaveData0,"%s	","CH03_(V)");
				fprintf(FileSaveData0,"%s	","CH04_(V)");
				fprintf(FileSaveData0,"%s	","CH05_(V)");
				fprintf(FileSaveData0,"%s	","CH06_(V)");
				fprintf(FileSaveData0,"%s	","CH07_(V)");
				fprintf(FileSaveData0,"%s	","CH08_(V)");
				fprintf(FileSaveData0,"%s	","CH09_(V)");
				fprintf(FileSaveData0,"%s	","CH10_(V)");
				fprintf(FileSaveData0,"%s	","CH11_(V)");
				fprintf(FileSaveData0,"%s	","CH12_(V)");
				fprintf(FileSaveData0,"%s	","CH13_(V)");
				fprintf(FileSaveData0,"%s	","CH14_(V)");
				fprintf(FileSaveData0,"%s	","CH15_(V)");
				fprintf(FileSaveData0,"\n");
			}


			// File for saving the parameter data
			pFileName2=pFileName1;
			pFileName2.Replace(".dat",".out");
			if((err = fopen_s(&FileSaveData2,(LPCSTR)pFileName2, _T("w"))) == 0)
			{
				fprintf(FileSaveData2,"%s	","Time(s)");
				fprintf(FileSaveData2,"%s	","s(a)_(kPa)");
				fprintf(FileSaveData2,"%s	","s(r)_(kPa)");
				fprintf(FileSaveData2,"%s	","s'(a)(kPa)");
				fprintf(FileSaveData2,"%s	","s'(r)(kPa)");
				fprintf(FileSaveData2,"%s	","Pore_(kPa)");
				fprintf(FileSaveData2,"%s	","p____(kPa)");
				fprintf(FileSaveData2,"%s	","q____(kPa)");
				fprintf(FileSaveData2,"%s	","p'___(kPa)");
				fprintf(FileSaveData2,"%s	","e(a)_(%)_");
				fprintf(FileSaveData2,"%s	","e(r)_(%)_");
				fprintf(FileSaveData2,"%s	","e(v)_(%)_");
				fprintf(FileSaveData2,"%s	","eLDT1(%)_");
				fprintf(FileSaveData2,"%s	","eLDT2(%)_");
				fprintf(FileSaveData2,"%s	","AvLDT(%)_");
				fprintf(FileSaveData2,"%s	","(s'a+s'r)/2");
				fprintf(FileSaveData2,"%s	","(s'a-s'r)/2");
				fprintf(FileSaveData2,"\n");
			}
// Timer starts
			SetTimer(3,TimeInterval_3,NULL);
			NowTime=NowTime.GetCurrentTime();
			StartTime=NowTime;
			SpanTime=NowTime-StartTime;
			SequentTime1=(long)SpanTime.GetTotalSeconds();
			/*_ftime(&NowTime2);*/
			_ftime_s(&NowTime2);
			StartTime2=NowTime2;
			SequentTime2=double(NowTime2.time-StartTime2.time)+double( (NowTime2.millitm-StartTime2.millitm)/1000.0 );
			Flag_SaveData=TRUE;
			CButton* myBTN1=(CButton*)GetDlgItem(IDC_BUTTON_StartSave);
			CButton* myBTN2=(CButton*)GetDlgItem(IDC_BUTTON_StopSave);
			CButton* myBTN3=(CButton*)GetDlgItem(IDC_BUTTON_InterceptSave);
			CButton* myBTN4=(CButton*)GetDlgItem(IDC_BUTTON_FIFOStart);
			CButton* myBTN5=(CButton*)GetDlgItem(IDC_BUTTON_FIFOStop);
			myBTN1->EnableWindow(FALSE);	
			myBTN2->EnableWindow(TRUE);
			myBTN3->EnableWindow(TRUE);
			myBTN4->EnableWindow(FALSE);
			myBTN5->EnableWindow(FALSE);
			if(Flag_SetBoard)	pDoc -> AD_INPUT();
			pDoc -> Cal_Physical();
			pDoc -> Cal_Param();
			pDoc -> SaveToFile();
		}
	}
	if(Flag_SetBoard==TRUE && Flag_FIFO==TRUE){
		NowTime=NowTime.GetCurrentTime();
		StartTime=NowTime;
		SpanTime=NowTime-StartTime;
		SequentTime1= (long)SpanTime.GetTotalSeconds();
	    if(NUMAD>0) Ret = AioStopAi(AdId[0]);
		if(NUMAD>1) Ret = AioStopAi(AdId[1]);
		if(NUMAD>2) Ret = AioStopAi(AdId[2]);
		Flag_SaveData=TRUE;
		CurrentSamplingTimes=0;
		pDoc->Allocate_Memory();
		CButton* myBTN1=(CButton*)GetDlgItem(IDC_BUTTON_StartSave);
		CButton* myBTN2=(CButton*)GetDlgItem(IDC_BUTTON_StopSave);
		CButton* myBTN3=(CButton*)GetDlgItem(IDC_BUTTON_InterceptSave);
		CButton* myBTN4=(CButton*)GetDlgItem(IDC_BUTTON_FIFOStart);
		CButton* myBTN5=(CButton*)GetDlgItem(IDC_BUTTON_FIFOStop);
		CButton* myBTN6=(CButton*)GetDlgItem(IDC_BUTTON_WriteData);
		myBTN1->EnableWindow(FALSE);	
		myBTN2->EnableWindow(TRUE);
		myBTN3->EnableWindow(FALSE);
		myBTN4->EnableWindow(FALSE);
		myBTN5->EnableWindow(FALSE);
		myBTN6->EnableWindow(FALSE);
		if(NUMAD>0) Ret = AioResetAiMemory(AdId[0]);
		if(NUMAD>1) Ret = AioResetAiMemory(AdId[1]);
		if(NUMAD>0) Ret = AioStartAi(AdId[0]);
		if(NUMAD>1) Ret = AioStartAi(AdId[1]);
	}
}

void CDigitShowBasicView::OnBUTTONStopSave() 
{

	long	Ret;
	CDigitShowBasicDoc* pDoc=(CDigitShowBasicDoc *)GetDocument();

	if(Flag_SaveData==TRUE && Flag_FIFO==FALSE){
		KillTimer(3);
		//_ftime(&NowTime2);
		_ftime_s(&NowTime2);
		SequentTime2=double(NowTime2.time-StartTime2.time)+double( (NowTime2.millitm-StartTime2.millitm)/1000.0 );
		if(Flag_SetBoard)	pDoc -> AD_INPUT();
		pDoc -> Cal_Physical();
		pDoc -> Cal_Param();
		pDoc -> SaveToFile();
		fclose(FileSaveData0);
		fclose(FileSaveData1);
		fclose(FileSaveData2);
		CButton* myBTN1=(CButton*)GetDlgItem(IDC_BUTTON_StartSave);
		CButton* myBTN2=(CButton*)GetDlgItem(IDC_BUTTON_StopSave);	
		CButton* myBTN3=(CButton*)GetDlgItem(IDC_BUTTON_InterceptSave);
		CButton* myBTN4=(CButton*)GetDlgItem(IDC_BUTTON_FIFOStart);
		CButton* myBTN5=(CButton*)GetDlgItem(IDC_BUTTON_FIFOStop);
		myBTN1->EnableWindow(TRUE);	
		myBTN2->EnableWindow(FALSE);
		myBTN3->EnableWindow(FALSE);	
		myBTN4->EnableWindow(TRUE);
		myBTN5->EnableWindow(FALSE);
		Flag_SaveData=FALSE;
	}
	if(Flag_SaveData==TRUE && Flag_FIFO==TRUE){
		Flag_SaveData=FALSE;
	    if(NUMAD>0) Ret = AioStopAi(AdId[0]);
		if(NUMAD>1) Ret = AioStopAi(AdId[1]);
		if(NUMAD>0) Ret = AioResetAiMemory(AdId[0]);
		if(NUMAD>1) Ret = AioResetAiMemory(AdId[1]);
		if(NUMAD>0) Ret = AioStartAi(AdId[0]);
		if(NUMAD>1) Ret = AioStartAi(AdId[1]);
		CButton* myBTN1=(CButton*)GetDlgItem(IDC_BUTTON_StartSave);
		CButton* myBTN2=(CButton*)GetDlgItem(IDC_BUTTON_StopSave);	
		CButton* myBTN3=(CButton*)GetDlgItem(IDC_BUTTON_InterceptSave);
		CButton* myBTN4=(CButton*)GetDlgItem(IDC_BUTTON_FIFOStart);
		CButton* myBTN5=(CButton*)GetDlgItem(IDC_BUTTON_FIFOStop);
		CButton* myBTN6=(CButton*)GetDlgItem(IDC_BUTTON_WriteData);
		myBTN1->EnableWindow(TRUE);	
		myBTN2->EnableWindow(FALSE);
		myBTN3->EnableWindow(FALSE);	
		myBTN4->EnableWindow(FALSE);
		myBTN5->EnableWindow(TRUE);
		myBTN6->EnableWindow(TRUE);
	}
}

void CDigitShowBasicView::OnBUTTONInterceptSave() 
{

	CDigitShowBasicDoc* pDoc=(CDigitShowBasicDoc *)GetDocument();
	
	//_ftime(&NowTime2);
	_ftime_s(&NowTime2);
	SequentTime2=double(NowTime2.time-StartTime2.time)+double( (NowTime2.millitm-StartTime2.millitm)/1000.0 );	
	if(Flag_SetBoard)	pDoc -> AD_INPUT();
	pDoc -> Cal_Physical();
	pDoc -> Cal_Param();
	pDoc -> SaveToFile();	
}

void CDigitShowBasicView::OnBUTTONFIFOStart() 
{

	long	Ret;
	int		nResult;
	CDigitShowBasicDoc* pDoc=(CDigitShowBasicDoc *)GetDocument();
	CButton* myBTN1=(CButton*)GetDlgItem(IDC_BUTTON_FIFOStart);
	CButton* myBTN2=(CButton*)GetDlgItem(IDC_BUTTON_FIFOStop);	

	if(Flag_SetBoard==TRUE){
		if(NUMAD>0) Ret = AioStopAi(AdId[0]);
	    if(NUMAD>1) Ret = AioStopAi(AdId[1]);
		CSamplingSettings SamplingSettings;
		nResult = SamplingSettings.DoModal();
		if(nResult==IDOK){
			if(NUMAD>0)	{
				Ret = AioSetAiSamplingClock ( AdId[0] , AdSamplingClock[0] );
				Ret = AioGetAiSamplingClock ( AdId[0] , &AdSamplingClock[0] );
				Ret = AioSetAiStopTrigger(AdId[0], 4);
				Ret = AioSetAiEventSamplingTimes ( AdId[0] , AdSamplingTimes[0] );
				Ret = AioGetAiEventSamplingTimes ( AdId[0] , &AdSamplingTimes[0] );
				Ret = AioResetAiMemory(AdId[0]);
			}
			if(NUMAD>1)	{
				Ret = AioSetAiSamplingClock ( AdId[1] , AdSamplingClock[1] );
				Ret = AioGetAiSamplingClock ( AdId[1] , &AdSamplingClock[1] );
				Ret = AioSetAiStopTrigger(AdId[1], 4);
				Ret = AioSetAiEventSamplingTimes ( AdId[1] , AdSamplingTimes[1] );
				Ret = AioGetAiEventSamplingTimes ( AdId[1] , &AdSamplingTimes[1] );
				Ret = AioResetAiMemory(AdId[1]);
			}
			Ret = AioSetAiEventSamplingTimes(AdId[NUMAD-1], AdSamplingTimes[NUMAD-1]);
			SavingClock=AdSamplingClock[0];
			Flag_FIFO=TRUE;
			myBTN1->EnableWindow(FALSE);
			myBTN2->EnableWindow(TRUE);
		}
		if(NUMAD>0) Ret = AioStartAi(AdId[0]);
		if(NUMAD>1) Ret = AioStartAi(AdId[1]);
	}
	else{
		AfxMessageBox("Board Setting has not been accomplished yet.", MB_OK | MB_ICONSTOP, 0);	
	}
}

void CDigitShowBasicView::OnBUTTONFIFOStop() 
{

	long	Ret;
	CDigitShowBasicDoc* pDoc=(CDigitShowBasicDoc *)GetDocument();
	CButton* myBTN1=(CButton*)GetDlgItem(IDC_BUTTON_FIFOStart);
	CButton* myBTN2=(CButton*)GetDlgItem(IDC_BUTTON_FIFOStop);	
    if(NUMAD>0) Ret = AioStopAi(AdId[0]);
    if(NUMAD>1) Ret = AioStopAi(AdId[1]);
	Flag_FIFO=FALSE;
	myBTN1->EnableWindow(TRUE);
	myBTN2->EnableWindow(FALSE);
	if(NUMAD>0)	{
		Ret = AioSetAiSamplingClock ( AdId[0] , 1000 );
		Ret = AioGetAiSamplingClock ( AdId[0] , &AdSamplingClock[0] );
		AdSamplingTimes[0]=long(TimeInterval_1*1000/AdSamplingClock[0]);
		Ret = AioSetAiEventSamplingTimes ( AdId[0] , AdSamplingTimes[0] );
		Ret = AioGetAiEventSamplingTimes ( AdId[0] , &AdSamplingTimes[0] );
		Ret = AioSetAiStopTrigger(AdId[0], 4);
		Ret = AioResetAiMemory(AdId[0]);
	}
	if(NUMAD>1)	{
		Ret = AioSetAiSamplingClock ( AdId[1] , 1000 );
		Ret = AioGetAiSamplingClock ( AdId[1] , &AdSamplingClock[1] );
		AdSamplingTimes[1]=long(TimeInterval_1*1000/AdSamplingClock[1]);
		Ret = AioSetAiEventSamplingTimes ( AdId[1] , AdSamplingTimes[1] );
		Ret = AioGetAiEventSamplingTimes ( AdId[1] , &AdSamplingTimes[1] );
		Ret = AioSetAiStopTrigger(AdId[1], 4);
		Ret = AioResetAiMemory(AdId[1]);
	}
	Ret = AioSetAiEventSamplingTimes(AdId[NUMAD-1], AdSamplingTimes[NUMAD-1]);
	if(NUMAD>0) Ret = AioStartAi(AdId[0]);
	if(NUMAD>1) Ret = AioStartAi(AdId[1]);
}

void CDigitShowBasicView::OnBUTTONWriteData() 
{

	long	Ret;
    if(NUMAD>0) Ret = AioStopAi(AdId[0]);
    if(NUMAD>1) Ret = AioStopAi(AdId[1]);

	CString	pFileName0, pFileName1,TmpString;
	CButton* myBTN1=(CButton*)GetDlgItem(IDC_BUTTON_WriteData);
	CDigitShowBasicDoc* pDoc=(CDigitShowBasicDoc *)GetDocument();
	errno_t err; 

	CFileDialog SaveFile_dlg( FALSE, NULL, "*.dat",  OFN_CREATEPROMPT | OFN_OVERWRITEPROMPT,
	"Data Files(*.dat)|*.dat| All Files(*.*)|*.*| |",NULL);
	if (SaveFile_dlg.DoModal()==IDOK){
		// File for saving the physical data 
		pFileName1= SaveFile_dlg.GetPathName();	
		m_FileName=	SaveFile_dlg.GetFileTitle();
		TmpString = SaveFile_dlg.GetFileExt();	
		if(TmpString == "" ){
			TmpString=".dat";
			pFileName1=pFileName1+TmpString;
			m_FileName=m_FileName+TmpString;
		}
		else if(TmpString != "dat"){
			TmpString=_T(".")+TmpString;
			pFileName1.Replace(TmpString,".dat");
			m_FileName=m_FileName+_T(".dat");
		}
		if((err = fopen_s(&FileSaveData1,(LPCSTR)pFileName1 , _T("w"))) == 0)
		{
			fprintf(FileSaveData1,"%s	","Time(s)");
			fprintf(FileSaveData1,"%s	","Load_(N)");
			fprintf(FileSaveData1,"%s	","Disp.(mm)");
			fprintf(FileSaveData1,"%s	","Cell_P.(kPa)");
			fprintf(FileSaveData1,"%s	","E_Cell_P.(kPa)");
			fprintf(FileSaveData1,"%s	","SP.Vol.(cm3)");
			fprintf(FileSaveData1,"%s	","LDT-V1(mm)");
			fprintf(FileSaveData1,"%s	","LDT-V2(mm)");
			fprintf(FileSaveData1,"%s	","CH07_(V)");
			fprintf(FileSaveData1,"%s	","CH08_(V)");
			fprintf(FileSaveData1,"%s	","CH09_(V)");
			fprintf(FileSaveData1,"%s	","CH10_(V)");
			fprintf(FileSaveData1,"%s	","CH11_(V)");
			fprintf(FileSaveData1,"%s	","CH12_(V)");
			fprintf(FileSaveData1,"%s	","CH13_(V)");
			fprintf(FileSaveData1,"%s	","CH14_(V)");
			fprintf(FileSaveData1,"%s	","CH15_(V)");
			fprintf(FileSaveData1,"\n");
		}


		// File for saving the voltage data
		pFileName0=pFileName1;
		pFileName0.Replace(".dat",".vlt");
		if((err = fopen_s(&FileSaveData0,(LPCSTR)pFileName0 , _T("w"))) == 0)
		{
			fprintf(FileSaveData0,"%s	","Time(s)");
			fprintf(FileSaveData0,"%s	","CH00_(V)");
			fprintf(FileSaveData0,"%s	","CH01_(V)");
			fprintf(FileSaveData0,"%s	","CH02_(V)");
			fprintf(FileSaveData0,"%s	","CH03_(V)");
			fprintf(FileSaveData0,"%s	","CH04_(V)");
			fprintf(FileSaveData0,"%s	","CH05_(V)");
			fprintf(FileSaveData0,"%s	","CH06_(V)");
			fprintf(FileSaveData0,"%s	","CH07_(V)");
			fprintf(FileSaveData0,"%s	","CH08_(V)");
			fprintf(FileSaveData0,"%s	","CH09_(V)");
			fprintf(FileSaveData0,"%s	","CH10_(V)");
			fprintf(FileSaveData0,"%s	","CH11_(V)");
			fprintf(FileSaveData0,"%s	","CH12_(V)");
			fprintf(FileSaveData0,"%s	","CH13_(V)");
			fprintf(FileSaveData0,"%s	","CH14_(V)");
			fprintf(FileSaveData0,"%s	","CH15_(V)");
			fprintf(FileSaveData0,"\n");
		}
		pDoc -> SaveToFile2();
		fclose(FileSaveData0);
		fclose(FileSaveData1);
		pDoc -> Allocate_Memory();
		myBTN1->EnableWindow(FALSE);
	}
	if(NUMAD>0)	{
		Ret = AioSetAiSamplingClock ( AdId[0] , 1000 );
		Ret = AioGetAiSamplingClock ( AdId[0] , &AdSamplingClock[0] );
		AdSamplingTimes[0]=long(TimeInterval_1*1000/AdSamplingClock[0]);
		Ret = AioSetAiEventSamplingTimes ( AdId[0] , AdSamplingTimes[0] );
		Ret = AioGetAiEventSamplingTimes ( AdId[0] , &AdSamplingTimes[0] );
		Ret = AioSetAiStopTrigger(AdId[0], 4);
		Ret = AioResetAiMemory(AdId[0]);
	}
	if(NUMAD>1)	{
		Ret = AioSetAiSamplingClock ( AdId[1] , 1000 );
		Ret = AioGetAiSamplingClock ( AdId[1] , &AdSamplingClock[1] );
		AdSamplingTimes[1]=long(TimeInterval_1*1000/AdSamplingClock[1]);
		Ret = AioSetAiEventSamplingTimes ( AdId[1] , AdSamplingTimes[1] );
		Ret = AioGetAiEventSamplingTimes ( AdId[1] , &AdSamplingTimes[1] );
		Ret = AioSetAiStopTrigger(AdId[1], 4);
		Ret = AioResetAiMemory(AdId[1]);
	}
	Ret = AioSetAiEventSamplingTimes(AdId[NUMAD-1], AdSamplingTimes[NUMAD-1]);
	if(NUMAD>0) Ret = AioStartAi(AdId[0]);
	if(NUMAD>1) Ret = AioStartAi(AdId[1]);
}

LRESULT CDigitShowBasicView::DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam) 
{

	int		i,j;
	long	tmp,tmp0,tmp1;
	long	Ret,Ret2;

	switch(message){
	case AIOM_AIE_DATA_NUM:
		if(NUMAD>0)	{
			Ret = AioGetAiSamplingCount ( AdId[0] , &tmp0 );
			tmp=tmp0;
		}
		if(NUMAD>1)	{
			Ret = AioGetAiSamplingCount ( AdId[1] , &tmp1 );
			if(tmp>tmp1) tmp=tmp1;
		}
		if(NUMAD>0){
			Ret = AioGetAiSamplingData(AdId[0], &tmp, &AdData0[0]);
		    if(Ret != 0){
			    Ret2 = AioGetErrorString(Ret, ErrorString);
				TextString.Format("AioGetAiSamplingData = %d : %s", Ret, ErrorString);
				AfxMessageBox(TextString, MB_ICONSTOP | MB_OK );
			}
		}
		if(NUMAD>1){
			Ret = AioGetAiSamplingData(AdId[1], &tmp, &AdData1[0]);
		    if(Ret != 0){
			    Ret2 = AioGetErrorString(Ret, ErrorString);
				TextString.Format("AioGetAiSamplingData = %d : %s", Ret, ErrorString);
				AfxMessageBox(TextString, MB_ICONSTOP | MB_OK );
			}
		}
		if(Flag_SaveData==TRUE && Flag_FIFO==TRUE){
			for(i=0;i<tmp;i++){
				if(CurrentSamplingTimes>=TotalSamplingTimes) {
					OnBUTTONStopSave();
				}
				else{
					if(NUMAD > 0){
						for(j=0;j<AdChannels[0]/2;j++){
							*((PLONG)pSmplData0+CurrentSamplingTimes*AdChannels[0]/2+j)=AdData0[i*AdChannels[0]+2*j];
						}
					}
					if(NUMAD > 1){
						for(j=0;j<AdChannels[1]/2;j++){
							*((PLONG)pSmplData1+CurrentSamplingTimes*AdChannels[1]/2+j)=AdData1[i*AdChannels[1]+2*j];
						}
					}
					CurrentSamplingTimes=CurrentSamplingTimes+1;
				}
			}
		}
		return TRUE;
	case AIOM_AIE_OFERR:
		if(Flag_FIFO){
			AfxMessageBox("FIFO sttoped by the over flow int the memory of A/D board.", MB_OK | MB_ICONSTOP, 0);	
		}
		else{
			if(NUMAD>0){
				Ret = AioResetAiMemory(AdId[0]);
				Ret = AioStartAi(AdId[0]);
			}
			if(NUMAD>1){
				Ret = AioResetAiMemory(AdId[1]);
				Ret = AioStartAi(AdId[1]);
			}
			AfxMessageBox("FIFO sttoped by the over flow, but restarted automatically.", MB_OK | MB_ICONSTOP, 0);	
		}
		return TRUE;
	case AIOM_AIE_SCERR:
		AfxMessageBox("FIFO sttoped by sampling error.", MB_OK | MB_ICONSTOP, 0);	
		return TRUE;
	case AIOM_AIE_ADERR:
		AfxMessageBox("FIFO sttoped by the error in A/D convert.", MB_OK | MB_ICONSTOP, 0);	
		return TRUE;
	case AIOM_AIE_END:
		AfxMessageBox("FIFO sttoped to reach the end.", MB_OK | MB_ICONSTOP, 0);	
		return TRUE;
	}	
	return CFormView::DefWindowProc(message, wParam, lParam);
}


void CDigitShowBasicView::OnBUTTONSetCtrlID() 
{

	CString		tmp;
	CComboBox* m_Combo1 = (CComboBox*)GetDlgItem(IDC_COMBO_Control_ID);
	m_Combo1->GetWindowText(tmp);
	Control_ID=atoi(tmp);
}

void CDigitShowBasicView::OnBUTTONSetTimeInterval() 
{

	CString		tmp;
	CComboBox* m_Combo1 = (CComboBox*)GetDlgItem(IDC_COMBO_SamplingTime);
	m_Combo1->GetWindowText(tmp);
	if(tmp=="0.2 s")	TimeInterval_3=200;
	if(tmp=="0.5 s")	TimeInterval_3=500;
	if(tmp=="1.0 s")	TimeInterval_3=1000;
	if(tmp=="2.0 s")	TimeInterval_3=2000;
	if(tmp=="3.0 s")	TimeInterval_3=3000;
	if(tmp=="5.0 s")	TimeInterval_3=5000;
	if(tmp=="10.0 s")	TimeInterval_3=10000;
	if(tmp=="20.0 s")	TimeInterval_3=20000;
	if(tmp=="30.0 s")	TimeInterval_3=30000;
	if(tmp=="1.0 min")	TimeInterval_3=60000;
	if(tmp=="2.0 min")	TimeInterval_3=120000;
	if(tmp=="3.0 min")	TimeInterval_3=180000;
	if(tmp=="5.0 min")	TimeInterval_3=300000;
	if(tmp=="10.0 min")	TimeInterval_3=600000;
	if(Flag_SaveData){
		KillTimer(3);
		SetTimer(3,TimeInterval_3,NULL);
	}	
}



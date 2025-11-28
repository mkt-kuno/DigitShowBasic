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

// Control_File.cpp : インプリメンテーション ファイル
//

#include "stdafx.h"
#include "DigitShowBasic.h"
#include "Control_File.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CControl_File ダイアログ
extern	int			CURNUM;
extern	int			CFNUM[128];
extern	double		CFPARA[128][10];
extern	int			NUM_Cyclic;
extern	double		TotalStepTime;

CControl_File::CControl_File(CWnd* pParent /*=NULL*/)
	: CDialog(CControl_File::IDD, pParent)
{
	//{{AFX_DATA_INIT(CControl_File)
	m_CurNum = CURNUM;
	m_CFNum = CFNUM[CURNUM];
	m_StepNum = CURNUM;
	m_SCFNum = CFNUM[CURNUM];
	m_CFPARA0 = CFPARA[CURNUM][0];
	m_CFPARA1 = CFPARA[CURNUM][1];
	m_CFPARA2 = CFPARA[CURNUM][2];
	m_CFPARA3 = CFPARA[CURNUM][3];
	m_CFPARA4 = CFPARA[CURNUM][4];
	m_CFPARA5 = CFPARA[CURNUM][5];
	m_CFPARA6 = CFPARA[CURNUM][6];
	m_CFPARA7 = CFPARA[CURNUM][7];
	m_CFPARA8 = CFPARA[CURNUM][8];
	m_CFPARA9 = CFPARA[CURNUM][9];
	//}}AFX_DATA_INIT
}


void CControl_File::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CControl_File)
	DDX_Text(pDX, IDC_EDIT_CFPARA0, m_CFPARA0);
	DDX_Text(pDX, IDC_EDIT_CFPARA1, m_CFPARA1);
	DDX_Text(pDX, IDC_EDIT_CFPARA2, m_CFPARA2);
	DDX_Text(pDX, IDC_EDIT_CFPARA3, m_CFPARA3);
	DDX_Text(pDX, IDC_EDIT_CFPARA4, m_CFPARA4);
	DDX_Text(pDX, IDC_EDIT_CFPARA5, m_CFPARA5);
	DDX_Text(pDX, IDC_EDIT_CFPARA6, m_CFPARA6);
	DDX_Text(pDX, IDC_EDIT_CFPARA7, m_CFPARA7);
	DDX_Text(pDX, IDC_EDIT_CFPARA8, m_CFPARA8);
	DDX_Text(pDX, IDC_EDIT_CFPARA9, m_CFPARA9);
	DDX_Text(pDX, IDC_EDIT_StepNum, m_StepNum);
	DDV_MinMaxInt(pDX, m_StepNum, 0, 127);
	DDX_Text(pDX, IDC_EDIT_SCFNum, m_SCFNum);
	DDX_Text(pDX, IDC_EDIT_CurNum, m_CurNum);
	DDX_Text(pDX, IDC_EDIT_CFNum, m_CFNum);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CControl_File, CDialog)
	//{{AFX_MSG_MAP(CControl_File)
	ON_BN_CLICKED(IDC_BUTTON_Update, OnBUTTONUpdate)
	ON_BN_CLICKED(IDC_BUTTON_ReadFile, OnBUTTONReadFile)
	ON_BN_CLICKED(IDC_BUTTON_SaveFile, OnBUTTONSaveFile)
	ON_BN_CLICKED(IDC_BUTTON_Load, OnBUTTONLoad)
	ON_BN_CLICKED(IDC_CHECK_ChangeNo, OnCHECKChangeNo)
	ON_BN_CLICKED(IDC_BUTTON_StepDec, OnBUTTONStepDec)
	ON_BN_CLICKED(IDC_BUTTON_StepInc, OnBUTTONStepInc)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CControl_File メッセージ ハンドラ
BOOL CControl_File::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: この位置に初期化の補足処理を追加してください
	CButton* myBTN1=(CButton*)GetDlgItem(IDC_BUTTON_StepDec);
	CButton* myBTN2=(CButton*)GetDlgItem(IDC_BUTTON_StepInc);
	myBTN1->EnableWindow(FALSE);
	myBTN2->EnableWindow(FALSE);
	CButton* chkbox1=(CButton*)GetDlgItem(IDC_CHECK_ChangeNo);
	chkbox1->SetCheck(0);
	return TRUE;  // コントロールにフォーカスを設定しないとき、戻り値は TRUE となります
	              // 例外: OCX プロパティ ページの戻り値は FALSE となります
}

void CControl_File::OnBUTTONLoad() 
{
	// TODO: この位置にコントロール通知ハンドラ用のコードを追加してください
	UpdateData(TRUE);
	m_SCFNum = CFNUM[m_StepNum];
	m_CFPARA0 = CFPARA[m_StepNum][0];
	m_CFPARA1 = CFPARA[m_StepNum][1];
	m_CFPARA2 = CFPARA[m_StepNum][2];
	m_CFPARA3 = CFPARA[m_StepNum][3];
	m_CFPARA4 = CFPARA[m_StepNum][4];
	m_CFPARA5 = CFPARA[m_StepNum][5];
	m_CFPARA6 = CFPARA[m_StepNum][6];
	m_CFPARA7 = CFPARA[m_StepNum][7];
	m_CFPARA8 = CFPARA[m_StepNum][8];
	m_CFPARA9 = CFPARA[m_StepNum][9];
	UpdateData(FALSE);
}

void CControl_File::OnBUTTONUpdate() 
{
	// TODO: この位置にコントロール通知ハンドラ用のコードを追加してください
	UpdateData(TRUE);
	CFNUM[m_StepNum] = m_SCFNum;
	CFPARA[m_StepNum][0] = m_CFPARA0;
	CFPARA[m_StepNum][1] = m_CFPARA1;
	CFPARA[m_StepNum][2] = m_CFPARA2;
	CFPARA[m_StepNum][3] = m_CFPARA3;
	CFPARA[m_StepNum][4] = m_CFPARA4;
	CFPARA[m_StepNum][5] = m_CFPARA5;
	CFPARA[m_StepNum][6] = m_CFPARA6;
	CFPARA[m_StepNum][7] = m_CFPARA7;
	CFPARA[m_StepNum][8] = m_CFPARA8;
	CFPARA[m_StepNum][9] = m_CFPARA9;
	UpdateData(FALSE);	
}

void CControl_File::OnBUTTONReadFile() 
{
	// TODO: この位置にコントロール通知ハンドラ用のコードを追加してください
	CURNUM=0;
	CString	pFileName;
	FILE	*FileCtlData;
	errno_t err; 
	int		i,j;

	CFileDialog CtlLoadFile_dlg( TRUE, NULL, "*.ctl",  OFN_FILEMUSTEXIST | OFN_HIDEREADONLY,
		"Control Files(*.ctl)|*.ctl| All Files(*.*)|*.*| |",NULL);

	if (CtlLoadFile_dlg.DoModal()==IDOK)
	{
	    pFileName = CtlLoadFile_dlg.GetPathName();	
//		FileCtlData = fopen((LPCSTR)pFileName , "r" );
		if((err = fopen_s(&FileCtlData,(LPCSTR)pFileName , _T("r"))) == 0)
		{
			for(i=0;i<128;i++){
				//fscanf(FileCtlData,"%d",&CFNUM[i]);
				fscanf_s(FileCtlData,_T("%d"),&CFNUM[i]);
				for(j=0;j<10;j++){
					//fscanf(FileCtlData,"%lf",&CFPARA[i][j]);
					fscanf_s(FileCtlData,_T("%lf"),&CFPARA[i][j]);
				}
			}
			fclose(FileCtlData);
		}
	}		
	m_CFNum=CFNUM[CURNUM];
	UpdateData(FALSE);	
}

void CControl_File::OnBUTTONSaveFile() 
{
	// TODO: この位置にコントロール通知ハンドラ用のコードを追加してください
	CString	pFileName;
	FILE	*FileCtlData;
	errno_t err; 
	int		i,j;

	CFileDialog CtlSaveFile_dlg( FALSE, NULL, "*.ctl",  OFN_OVERWRITEPROMPT,
		"Control Files(*.ctl)|*.ctl| All Files(*.*)|*.*| |",NULL);

	if (CtlSaveFile_dlg.DoModal()==IDOK)
	{
	    pFileName = CtlSaveFile_dlg.GetPathName();	
//		FileCtlData = fopen((LPCSTR)pFileName , "w" );
		if((err = fopen_s(&FileCtlData,(LPCSTR)pFileName , _T("w"))) == 0)
		{
			for(i=0;i<128;i++){
				fprintf(FileCtlData,"%d	",CFNUM[i]);
				for(j=0;j<10;j++){
					fprintf(FileCtlData,"%lf	",CFPARA[i][j]);
				}
				fprintf(FileCtlData,"\n");
			}
			fclose(FileCtlData);
		}
	}		
}


void CControl_File::OnCHECKChangeNo() 
{
	// TODO: この位置にコントロール通知ハンドラ用のコードを追加してください
	CButton* myBTN1=(CButton*)GetDlgItem(IDC_BUTTON_StepDec);
	CButton* myBTN2=(CButton*)GetDlgItem(IDC_BUTTON_StepInc);
	CButton* chkbox1=(CButton*)GetDlgItem(IDC_CHECK_ChangeNo);
	if(chkbox1->GetCheck()){
		if(CURNUM > 0)		myBTN1->EnableWindow(TRUE);
		if(CURNUM < 127)	myBTN2->EnableWindow(TRUE);
	}
	else{
		myBTN1->EnableWindow(FALSE);
		myBTN2->EnableWindow(FALSE);
	}
}

void CControl_File::OnBUTTONStepDec() 
{
	// TODO: この位置にコントロール通知ハンドラ用のコードを追加してください
	CURNUM=CURNUM-1;
	NUM_Cyclic=0;
	TotalStepTime=0.0;
	m_CurNum = CURNUM;
	m_CFNum = CFNUM[CURNUM];
	CButton* myBTN1=(CButton*)GetDlgItem(IDC_BUTTON_StepDec);
	CButton* myBTN2=(CButton*)GetDlgItem(IDC_BUTTON_StepInc);
	if(CURNUM > 0)		myBTN1->EnableWindow(TRUE);
	else				myBTN1->EnableWindow(FALSE);
	if(CURNUM < 127)	myBTN2->EnableWindow(TRUE);
	else				myBTN2->EnableWindow(FALSE);
	UpdateData(FALSE);
}

void CControl_File::OnBUTTONStepInc() 
{
	// TODO: この位置にコントロール通知ハンドラ用のコードを追加してください
	CURNUM=CURNUM+1;
	NUM_Cyclic=0;
	TotalStepTime=0.0;
	m_CurNum = CURNUM;
	m_CFNum = CFNUM[CURNUM];
	CButton* myBTN1=(CButton*)GetDlgItem(IDC_BUTTON_StepDec);
	CButton* myBTN2=(CButton*)GetDlgItem(IDC_BUTTON_StepInc);
	if(CURNUM > 0)		myBTN1->EnableWindow(TRUE);
	else				myBTN1->EnableWindow(FALSE);
	if(CURNUM < 127)	myBTN2->EnableWindow(TRUE);
	else				myBTN2->EnableWindow(FALSE);
	UpdateData(FALSE);
}


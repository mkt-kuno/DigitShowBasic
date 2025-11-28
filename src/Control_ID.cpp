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

#include "stdafx.h"
#include "DigitShowBasic.h"
#include "Control_ID.h"
#include "DigitShowBasicDoc.h"
#include "DigitShowContext.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

static int tmp;

CControl_ID::CControl_ID(CWnd* pParent)
	: CDialog(CControl_ID::IDD, pParent)
{
	DigitShowContext* ctx = GetContext();
	ControlData* ControlData = ctx->control;
	m_Control_ID = tmp;
	m_esigma0 = ControlData[tmp].e_sigma[0];
	m_esigma1 = ControlData[tmp].e_sigma[1];
	m_esigma2 = ControlData[tmp].e_sigma[2];
	m_esigmaAmp0 = ControlData[tmp].e_sigmaAmp[0];
	m_esigmaAmp1 = ControlData[tmp].e_sigmaAmp[1];
	m_esigmaAmp2 = ControlData[tmp].e_sigmaAmp[2];
	m_esigmaRate0 = ControlData[tmp].e_sigmaRate[0];
	m_esigmaRate1 = ControlData[tmp].e_sigmaRate[1];
	m_esigmaRate2 = ControlData[tmp].e_sigmaRate[2];
	m_flag0 = ControlData[tmp].flag[0];
	m_flag1 = ControlData[tmp].flag[1];
	m_flag2 = ControlData[tmp].flag[2];
	m_K0 = ControlData[tmp].K0;
	m_Motor = ControlData[tmp].Motor;
	m_MotorCruch = ControlData[tmp].MotorCruch;
	m_MotorSpeed = ControlData[tmp].MotorSpeed;
	m_p = ControlData[tmp].p;
	m_sigma0 = ControlData[tmp].sigma[0];
	m_sigma1 = ControlData[tmp].sigma[1];
	m_sigma2 = ControlData[tmp].sigma[2];
	m_sigmaAmp0 = ControlData[tmp].sigmaAmp[0];
	m_sigmaAmp1 = ControlData[tmp].sigmaAmp[1];
	m_sigmaAmp2 = ControlData[tmp].sigmaAmp[2];
	m_sigmaRate0 = ControlData[tmp].sigmaRate[0];
	m_sigmaRate1 = ControlData[tmp].sigmaRate[1];
	m_sigmaRate2 = ControlData[tmp].sigmaRate[2];
	m_strain0 = ControlData[tmp].strain[0];
	m_strain1 = ControlData[tmp].strain[1];
	m_strain2 = ControlData[tmp].strain[2];
	m_strainAmp0 = ControlData[tmp].strainAmp[0];
	m_strainAmp1 = ControlData[tmp].strainAmp[1];
	m_strainAmp2 = ControlData[tmp].strainAmp[2];
	m_strainRate0 = ControlData[tmp].strainRate[0];
	m_strainRate1 = ControlData[tmp].strainRate[1];
	m_strainRate2 = ControlData[tmp].strainRate[2];
	m_time0 = ControlData[tmp].time[0];
	m_time1 = ControlData[tmp].time[1];
	m_time2 = ControlData[tmp].time[2];
	m_u = ControlData[tmp].u;
	m_q = ControlData[tmp].q;
	//}}AFX_DATA_INIT
}


void CControl_ID::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CControl_ID)
	DDX_Text(pDX, IDC_EDIT_Control_ID, m_Control_ID);
	DDX_Text(pDX, IDC_EDIT_esigma0, m_esigma0);
	DDX_Text(pDX, IDC_EDIT_esigma1, m_esigma1);
	DDX_Text(pDX, IDC_EDIT_esigma2, m_esigma2);
	DDX_Text(pDX, IDC_EDIT_esigmaAmp0, m_esigmaAmp0);
	DDX_Text(pDX, IDC_EDIT_esigmaAmp1, m_esigmaAmp1);
	DDX_Text(pDX, IDC_EDIT_esigmaAmp2, m_esigmaAmp2);
	DDX_Text(pDX, IDC_EDIT_esigmaRate0, m_esigmaRate0);
	DDX_Text(pDX, IDC_EDIT_esigmaRate1, m_esigmaRate1);
	DDX_Text(pDX, IDC_EDIT_esigmaRate2, m_esigmaRate2);
	DDX_Text(pDX, IDC_EDIT_flag0, m_flag0);
	DDX_Text(pDX, IDC_EDIT_flag1, m_flag1);
	DDX_Text(pDX, IDC_EDIT_flag2, m_flag2);
	DDX_Text(pDX, IDC_EDIT_K0, m_K0);
	DDX_Text(pDX, IDC_EDIT_Motor, m_Motor);
	DDX_Text(pDX, IDC_EDIT_MotorCruch, m_MotorCruch);
	DDX_Text(pDX, IDC_EDIT_MotorSpeed, m_MotorSpeed);
	DDX_Text(pDX, IDC_EDIT_p, m_p);
	DDX_Text(pDX, IDC_EDIT_sigma0, m_sigma0);
	DDX_Text(pDX, IDC_EDIT_sigma1, m_sigma1);
	DDX_Text(pDX, IDC_EDIT_sigma2, m_sigma2);
	DDX_Text(pDX, IDC_EDIT_sigmaAmp0, m_sigmaAmp0);
	DDX_Text(pDX, IDC_EDIT_sigmaAmp1, m_sigmaAmp1);
	DDX_Text(pDX, IDC_EDIT_sigmaAmp2, m_sigmaAmp2);
	DDX_Text(pDX, IDC_EDIT_sigmaRate0, m_sigmaRate0);
	DDX_Text(pDX, IDC_EDIT_sigmaRate1, m_sigmaRate1);
	DDX_Text(pDX, IDC_EDIT_sigmaRate2, m_sigmaRate2);
	DDX_Text(pDX, IDC_EDIT_strain0, m_strain0);
	DDX_Text(pDX, IDC_EDIT_strain1, m_strain1);
	DDX_Text(pDX, IDC_EDIT_strain2, m_strain2);
	DDX_Text(pDX, IDC_EDIT_strainAmp0, m_strainAmp0);
	DDX_Text(pDX, IDC_EDIT_strainAmp1, m_strainAmp1);
	DDX_Text(pDX, IDC_EDIT_strainAmp2, m_strainAmp2);
	DDX_Text(pDX, IDC_EDIT_strainRate0, m_strainRate0);
	DDX_Text(pDX, IDC_EDIT_strainRate1, m_strainRate1);
	DDX_Text(pDX, IDC_EDIT_strainRate2, m_strainRate2);
	DDX_Text(pDX, IDC_EDIT_time0, m_time0);
	DDX_Text(pDX, IDC_EDIT_time1, m_time1);
	DDX_Text(pDX, IDC_EDIT_time2, m_time2);
	DDX_Text(pDX, IDC_EDIT_u, m_u);
	DDX_Text(pDX, IDC_EDIT_q, m_q);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CControl_ID, CDialog)
	//{{AFX_MSG_MAP(CControl_ID)
	ON_BN_CLICKED(IDC_BUTTON_Load, OnBUTTONLoad)
	ON_BN_CLICKED(IDC_BUTTON_Update, OnBUTTONUpdate)
	ON_BN_CLICKED(IDC_BUTTON_Loadfromfile, OnBUTTONLoadfromfile)
	ON_BN_CLICKED(IDC_BUTTON_SaveFile, OnBUTTONSaveFile)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CControl_ID メッセージ ハンドラ

void CControl_ID::OnBUTTONLoad() 
{
	DigitShowContext* ctx = GetContext();
	ControlData* ControlData = ctx->control;

	UpdateData(TRUE);
	tmp=m_Control_ID;
	m_flag0 = ControlData[tmp].flag[0];
	m_flag1 = ControlData[tmp].flag[1];
	m_flag2 = ControlData[tmp].flag[2];
	m_esigma0 = ControlData[tmp].e_sigma[0];
	m_esigma1 = ControlData[tmp].e_sigma[1];
	m_esigma2 = ControlData[tmp].e_sigma[2];
	m_esigmaAmp1 = ControlData[tmp].e_sigmaAmp[1];
	m_esigmaAmp2 = ControlData[tmp].e_sigmaAmp[2];
	m_esigmaAmp0 = ControlData[tmp].e_sigmaAmp[0];	
	m_esigmaRate0 = ControlData[tmp].e_sigmaRate[0];
	m_esigmaRate1 = ControlData[tmp].e_sigmaRate[1];
	m_esigmaRate2 = ControlData[tmp].e_sigmaRate[2];
	m_K0 = ControlData[tmp].K0;
	m_MotorSpeed = ControlData[tmp].MotorSpeed;
	m_Motor = ControlData[tmp].Motor;
	m_MotorCruch = ControlData[tmp].MotorCruch;
	m_p = ControlData[tmp].p;
	m_q = ControlData[tmp].q;
	m_u = ControlData[tmp].u;
	m_sigma0 = ControlData[tmp].sigma[0];
	m_sigma1 = ControlData[tmp].sigma[1];
	m_sigma2 = ControlData[tmp].sigma[2];
	m_sigmaAmp0 = ControlData[tmp].sigmaAmp[0];
	m_sigmaAmp1 = ControlData[tmp].sigmaAmp[1];
	m_sigmaAmp2 = ControlData[tmp].sigmaAmp[2];
	m_sigmaRate0 = ControlData[tmp].sigmaRate[0];
	m_sigmaRate1 = ControlData[tmp].sigmaRate[1];
	m_sigmaRate2 = ControlData[tmp].sigmaRate[2];
	m_strain0 = ControlData[tmp].strain[0];
	m_strain1 = ControlData[tmp].strain[1];
	m_strain2 = ControlData[tmp].strain[2];
	m_strainAmp0 = ControlData[tmp].strainAmp[0];
	m_strainAmp1 = ControlData[tmp].strainAmp[1];
	m_strainAmp2 = ControlData[tmp].strainAmp[2];
	m_strainRate0 = ControlData[tmp].strainRate[0];
	m_strainRate1 = ControlData[tmp].strainRate[1];
	m_strainRate2 = ControlData[tmp].strainRate[2];
	m_time0 = ControlData[tmp].time[0];
	m_time1 = ControlData[tmp].time[1];
	m_time2 = ControlData[tmp].time[2];
	

	UpdateData(FALSE);
}

void CControl_ID::OnBUTTONUpdate() 
{
	DigitShowContext* ctx = GetContext();
	ControlData* ControlData = ctx->control;

	UpdateData(TRUE);
	tmp=m_Control_ID;
	ControlData[tmp].e_sigma[0] = m_esigma0; 
	ControlData[tmp].e_sigma[1] = m_esigma1;
	ControlData[tmp].e_sigma[2] = m_esigma2;
	ControlData[tmp].e_sigmaAmp[0] = m_esigmaAmp0;
	ControlData[tmp].e_sigmaAmp[1] = m_esigmaAmp1;
	ControlData[tmp].e_sigmaAmp[2] = m_esigmaAmp2;
	ControlData[tmp].e_sigmaRate[0] = m_esigmaRate0;
	ControlData[tmp].e_sigmaRate[1] = m_esigmaRate1;
	ControlData[tmp].e_sigmaRate[2] = m_esigmaRate2;
	if(m_flag0==0)	ControlData[tmp].flag[0] = FALSE;
	if(m_flag0==1)	ControlData[tmp].flag[0] = TRUE;
	if(m_flag1==0)	ControlData[tmp].flag[1] = FALSE;
	if(m_flag1==1)	ControlData[tmp].flag[1] = TRUE;
	if(m_flag2==0)	ControlData[tmp].flag[2] = FALSE;
	if(m_flag2==1)	ControlData[tmp].flag[2] = TRUE;	
	ControlData[tmp].K0 = m_K0;
	ControlData[tmp].MotorSpeed = m_MotorSpeed;
	ControlData[tmp].Motor = m_Motor;
	ControlData[tmp].MotorCruch = m_MotorCruch;
	ControlData[tmp].p = m_p;
	ControlData[tmp].q = m_q;
	ControlData[tmp].u = m_u;
	ControlData[tmp].sigma[0] = m_sigma0;
	ControlData[tmp].sigma[1] = m_sigma1;
	ControlData[tmp].sigma[2] = m_sigma2;
	ControlData[tmp].sigmaAmp[0] = m_sigmaAmp0;
	ControlData[tmp].sigmaAmp[1] = m_sigmaAmp1;
	ControlData[tmp].sigmaAmp[2] = m_sigmaAmp2;
	ControlData[tmp].sigmaRate[0] = m_sigmaRate0;
	ControlData[tmp].sigmaRate[1] = m_sigmaRate1;
	ControlData[tmp].sigmaRate[2] = m_sigmaRate2;
	ControlData[tmp].strain[0] = m_strain0;
	ControlData[tmp].strain[1] = m_strain1;
	ControlData[tmp].strain[2] = m_strain2;
	ControlData[tmp].strainAmp[0] = m_strainAmp0;
	ControlData[tmp].strainAmp[1] = m_strainAmp1;
	ControlData[tmp].strainAmp[2] = m_strainAmp2;
	ControlData[tmp].strainRate[0] = m_strainRate0;
	ControlData[tmp].strainRate[1] = m_strainRate1;
	ControlData[tmp].strainRate[2] = m_strainRate2;
	ControlData[tmp].time[0] = m_time0;
	ControlData[tmp].time[1] = m_time1;
	ControlData[tmp].time[2] = m_time2;
}

void CControl_ID::OnBUTTONLoadfromfile() 
{
	DigitShowContext* ctx = GetContext();
	ControlData* ControlData = ctx->control;

	CString	pFileName;
	FILE	*FileCtlData;
	errno_t err; 

	CFileDialog CtlReadFile_dlg( TRUE, NULL, "*.ctl",  OFN_FILEMUSTEXIST | OFN_HIDEREADONLY,
		"Specimen Files(*.ctl)|*.ctl| All Files(*.*)|*.*| |",NULL);

	if (CtlReadFile_dlg.DoModal()==IDOK)
	{
	    pFileName = CtlReadFile_dlg.GetPathName();	
//		FileCtlData = fopen((LPCSTR)pFileName , "r" );
		if((err = fopen_s(&FileCtlData,(LPCSTR)pFileName , _T("r"))) == 0)
		{
			for(i=0;i<16;i++){
				int tmpFlag[3];
				fscanf_s(FileCtlData,_T("%d"),&tmp);
				fscanf_s(FileCtlData,_T("%d%d%d"),
					&tmpFlag[0],&tmpFlag[1],&tmpFlag[2]);
				ControlData[i].flag[0] = (tmpFlag[0] != 0);
				ControlData[i].flag[1] = (tmpFlag[1] != 0);
				ControlData[i].flag[2] = (tmpFlag[2] != 0);
				fscanf_s(FileCtlData,_T("%d%d%d"),
					&ControlData[i].time[0],&ControlData[i].time[1],&ControlData[i].time[2]);
				fscanf_s(FileCtlData,_T("%lf"),&ControlData[i].p);
				fscanf_s(FileCtlData,_T("%lf"),&ControlData[i].q);
				fscanf_s(FileCtlData,_T("%lf"),&ControlData[i].u);
				fscanf_s(FileCtlData,_T("%lf%lf%lf"),
					&ControlData[i].sigma[0],&ControlData[i].sigma[1],&ControlData[i].sigma[2]);
				fscanf_s(FileCtlData,_T("%lf%lf%lf"),
					&ControlData[i].sigmaAmp[0],&ControlData[i].sigmaAmp[1],&ControlData[i].sigmaAmp[2]);
				fscanf_s(FileCtlData,_T("%lf%lf%lf"),
					&ControlData[i].sigmaRate[0],&ControlData[i].sigmaRate[1],&ControlData[i].sigmaRate[2]);
				fscanf_s(FileCtlData,_T("%lf%lf%lf"),
					&ControlData[i].e_sigma[0],&ControlData[i].e_sigma[1],&ControlData[i].e_sigma[2]);
				fscanf_s(FileCtlData,_T("%lf%lf%lf"),
					&ControlData[i].e_sigmaAmp[0],&ControlData[i].e_sigmaAmp[1],&ControlData[i].e_sigmaAmp[2]);
				fscanf_s(FileCtlData,_T("%lf%lf%lf"),
					&ControlData[i].e_sigmaRate[0],&ControlData[i].e_sigmaRate[1],&ControlData[i].e_sigmaRate[2]);
				fscanf_s(FileCtlData,_T("%lf%lf%lf"),
					&ControlData[i].strain[0],&ControlData[i].strain[1],&ControlData[i].strain[2]);
				fscanf_s(FileCtlData,_T("%lf%lf%lf"),
					&ControlData[i].strainAmp[0],&ControlData[i].strainAmp[1],&ControlData[i].strainAmp[2]);
				fscanf_s(FileCtlData,_T("%lf%lf%lf"),
					&ControlData[i].strainRate[0],&ControlData[i].strainRate[1],&ControlData[i].strainRate[2]);
				fscanf_s(FileCtlData,_T("%lf"),&ControlData[i].K0);
				fscanf_s(FileCtlData,_T("%lf"),&ControlData[i].MotorSpeed);
				fscanf_s(FileCtlData,_T("%d"),&ControlData[i].Motor);
				fscanf_s(FileCtlData,_T("%d"),&ControlData[i].MotorCruch);
			}
			fclose(FileCtlData);
		}

		OnBUTTONLoad();
	}	
	else{
				AfxMessageBox("Opening file is failed.", MB_ICONEXCLAMATION | MB_OK );
	}
}

void CControl_ID::OnBUTTONSaveFile() 
{
	DigitShowContext* ctx = GetContext();
	ControlData* ControlData = ctx->control;

	CString	pFileName;
	FILE	*FileCtlData;
	errno_t err;

	CFileDialog CtlSaveFile_dlg( FALSE, NULL, "*.ctl",  OFN_CREATEPROMPT | OFN_OVERWRITEPROMPT,
		"Specimen Files(*.ctl)|*.ctl| All Files(*.*)|*.*| |",NULL);

	if (CtlSaveFile_dlg.DoModal()==IDOK)
	{
	    pFileName = CtlSaveFile_dlg.GetPathName();	
//		FileCtlData = fopen((LPCSTR)pFileName , "w" );
		if((err = fopen_s(&FileCtlData,(LPCSTR)pFileName , _T("w"))) == 0)
		{
			for(i=0;i<16;i++){
				fprintf(FileCtlData,"%d	\n",i);
				fprintf(FileCtlData,"%d	%d	%d	\n",
					ControlData[i].flag[0],ControlData[i].flag[1],ControlData[i].flag[2]);
				fprintf(FileCtlData,"%d	%d	%d	\n",
					ControlData[i].time[0],ControlData[i].time[1],ControlData[i].time[2]);
				fprintf(FileCtlData,"%lf	\n",ControlData[i].p);
				fprintf(FileCtlData,"%lf	\n",ControlData[i].q);
				fprintf(FileCtlData,"%lf	\n",ControlData[i].u);
				fprintf(FileCtlData,"%lf	%lf	%lf	\n",
					ControlData[i].sigma[0],ControlData[i].sigma[1],ControlData[i].sigma[2]);
				fprintf(FileCtlData,"%lf	%lf	%lf	\n",
					ControlData[i].sigmaAmp[0],ControlData[i].sigmaAmp[1],ControlData[i].sigmaAmp[2]);
				fprintf(FileCtlData,"%lf	%lf	%lf	\n",
					ControlData[i].sigmaRate[0],ControlData[i].sigmaRate[1],ControlData[i].sigmaRate[2]);
				fprintf(FileCtlData,"%lf	%lf	%lf	\n",
					ControlData[i].e_sigma[0],ControlData[i].e_sigma[1],ControlData[i].e_sigma[2]);
				fprintf(FileCtlData,"%lf	%lf	%lf	\n",
					ControlData[i].e_sigmaAmp[0],ControlData[i].e_sigmaAmp[1],ControlData[i].e_sigmaAmp[2]);
				fprintf(FileCtlData,"%lf	%lf	%lf	\n",
					ControlData[i].e_sigmaRate[0],ControlData[i].e_sigmaRate[1],ControlData[i].e_sigmaRate[2]);
				fprintf(FileCtlData,"%lf	%lf	%lf	\n",
					ControlData[i].strain[0],ControlData[i].strain[1],ControlData[i].strain[2]);
				fprintf(FileCtlData,"%lf	%lf	%lf	\n",
					ControlData[i].strainAmp[0],ControlData[i].strainAmp[1],ControlData[i].strainAmp[2]);
				fprintf(FileCtlData,"%lf	%lf	%lf	\n",
					ControlData[i].strainRate[0],ControlData[i].strainRate[1],ControlData[i].strainRate[2]);
				fprintf(FileCtlData,"%lf	\n",ControlData[i].K0);
				fprintf(FileCtlData,"%lf	\n",ControlData[i].MotorSpeed);
				fprintf(FileCtlData,"%d	\n",ControlData[i].Motor);
				fprintf(FileCtlData,"%d	\n",ControlData[i].MotorCruch);	
			}
			fclose(FileCtlData);
		}
	}
	else{
				AfxMessageBox("Opening file is failed.", MB_ICONEXCLAMATION | MB_OK );
	}		
}

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


#include	"stdafx.h"
#include	"DigitShowBasic.h"
#include	"DigitShowBasicDoc.h"
#include	"caio.h"
#include	"dataconvert.h"

#include	"time.h"
#include	"math.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDigitShowBasicDoc

//---A/D---
	long		Ret,Ret2;
	int			NUMAD,NUMDA;						// The Number of A/D and D/A boards
	short		AdId[2],DaId[1];					// ID
	short		AdChannels[2],DaChannels[1];		// The Number of Channel
	short		AdRange[2],DaRange[1];				// Range of A/D and D/A boards
	float		AdRangeMax[2], AdRangeMin[2];		// Maximum and Minimum in the range of A/D
	float		DaRangeMax[1], DaRangeMin[1];		// Maximum and Minimum in the range of D/A
	short		AdResolution[2],DaResolution[1];	// Resolution of A/D and D/A Boards
	short		AdInputMethod[2];					// Input Method of A/D
	long		DaData[8];
	long		AdData0[262144],AdData1[262144];
	int			AdMaxCH;
	short		AdMemoryType[2];
	float		AdSamplingClock[2];
	long		AdSamplingTimes[2];
	float       AdScanClock[2];
	long		AdEvent;
	float		SavingClock;
	int			SavingTime;
	long		TotalSamplingTimes;
	long		CurrentSamplingTimes;
	float		AllocatedMemory;
	int			AvSmplNum;

	char		ErrorString[256];
	CString		TextString;

	PVOID		pSmplData0,pSmplData1;	// Source of binary data
	HANDLE		hHeap0,hHeap1;			// Handle to make the source area

	int			CH_Motor;				// D/A Channel of Motor On / Off
	int			CH_MotorCruch;			// D/A Channel of Motor Cruch (Loading/Unloading)
	int			CH_MotorSpeed;			// D/A Channel of Motor Speed
	int			CH_EP_Cell;				// D/A Channel of EP (Cell Pressure)

//---Array---
	float		Vout[64],Vtmp;					// Output Voltage from A/D board
	double		Phyout[64],Ptmp;				// Physical Value Calcurated from Vout
	double		CalParam[64];					// The others of the above to display
	double		Cal_a[64],Cal_b[64],Cal_c[64];	// Calibration Factor
	float		DAVout[8];						// Output Voltage to D/A board
	double		DA_Cal_a[8],DA_Cal_b[8];		// D/A Calibration Factor

//---Physical Value---
	double		sa,e_sa,sr,e_sr,p,e_p,q,u;		// stress and pressure
	double		ea,er,ev;						// strain
	double		eLDT,eLDT1,eLDT2;				// strain from LDTs

//---Flag---
	bool		Flag_SetBoard;
	bool		Flag_SaveData;
	bool		Flag_FIFO;

//---SpecimenData---
	Specimen	SpecimenData;						// Structure of Specimen Data
	double		height,volume,area;					// Current Specimen Size		

//---Control---	
	int			Control_ID;
	Control		ControlData[16];					// Structure of Control Data
	double		ERR_StressCom,ERR_StressExt;		// Sencitivity of Stress Control by Motor
	double		ERR_StressA;						// Sencitivity of Stress Control by Motor
	bool		Flag_Cyclic;
	int			NUM_Cyclic;
	double		TotalStepTime;

//---Control File---
	int			CURNUM;
	int			CFNUM[128];
	double		CFPARA[128][10];

//---Time---
	CTime		StartTime, NowTime;
	CTimeSpan	SpanTime;
	CString		SNowTime;
	long		SequentTime1;
	double		SequentTime2;
	double		CtrlStepTime;

	unsigned int	TimeInterval_1;	// Time interval (ms) to display output data.		
	unsigned int	TimeInterval_2;	// Time interval (ms) to feed back.		
	unsigned int	TimeInterval_3;	// Time interval (ms) to save the data.

//---File---
	FILE        *FileSaveData0;						// File to save the voltage value
	FILE		*FileSaveData1;						// File to save the physical value
	FILE		*FileSaveData2;						// File to save the parameter value


IMPLEMENT_DYNCREATE(CDigitShowBasicDoc, CDocument)

BEGIN_MESSAGE_MAP(CDigitShowBasicDoc, CDocument)
	//{{AFX_MSG_MAP(CDigitShowBasicDoc)
		// メモ - ClassWizard はこの位置にマッピング用のマクロを追加または削除します。
		//        この位置に生成されるコードを編集しないでください。
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDigitShowBasicDoc クラスの構築/消滅

CDigitShowBasicDoc::CDigitShowBasicDoc()
{

	// Initialize of data
	int	i,j;
	Flag_SetBoard=FALSE;
	Flag_SaveData=FALSE;
	Flag_FIFO=FALSE;
	NUMAD=1;		// The Number of A/D Board ( NUMAD=1-2 )
	NUMDA=0;		// The Number of D/A Board ( NUMDA=1 )
	for(i=0;i<64;i++){
		Vout[i]=0.0;	
		Phyout[i]=0.0;		CalParam[i]=0.0;
		Cal_a[i]=0.0;		Cal_b[i]=1.0;			Cal_c[i]=0.0;
	}
	for(i=0;i<8;i++){
		DAVout[i]=0.0;
		DA_Cal_a[i]=0.0;	DA_Cal_b[i]=0.0;
	}
	Control_ID=0;
	SequentTime1=0;
	SequentTime2=0.0;
	CtrlStepTime=0.0;
	TimeInterval_1=50;
	TimeInterval_2=500;
	TimeInterval_3=1000;
	for(j=0;j<4;j++){
		SpecimenData.Diameter[j]=50.0;	
		SpecimenData.Width[j]=0.0;
		SpecimenData.Depth[j]=0.0;
		SpecimenData.Height[j]=100.0;
		SpecimenData.Area[j]=1963.495408;		
		SpecimenData.Volume[j]=196349.5408;	
		SpecimenData.Weight[j]=0.0;
		SpecimenData.VLDT1[j]=70.0;
		SpecimenData.VLDT2[j]=70.0;
	}
	SpecimenData.Gs=0.0;		
	SpecimenData.MembraneModulus=0.0;
	SpecimenData.MembraneThickness=0.0;
	SpecimenData.RodArea=0.0;
	SpecimenData.RodWeight=0.0;
	for(i=0;i<16;i++){
		ControlData[i].p=0.0;
		ControlData[i].q=0.0;
		ControlData[i].u=0.0;
		for(j=0;j<3;j++){
			ControlData[i].flag[j]=FALSE;
			ControlData[i].time[j]=0;
			ControlData[i].sigma[j]=0.0;
			ControlData[i].sigmaRate[j]=0.0;
			ControlData[i].sigmaAmp[j]=0.0;	
			ControlData[i].e_sigma[j]=0.0;
			ControlData[i].e_sigmaAmp[j]=0.0;
			ControlData[i].strain[j]=0.0;
			ControlData[i].strainAmp[j]=0.0;
		}
		ControlData[i].K0=1.0;			ControlData[i].MotorSpeed=0.0;
		ControlData[i].Motor=0;			ControlData[i].MotorCruch=0;
	}
	CURNUM=0;
	for(i=0;i<128;i++){
		CFNUM[i]=0;
		for(j=0;j<10;j++){
			CFPARA[i][j]=0.0;
		}
	}
//	
	ControlData[1].MotorSpeed=1000.0;	// Motor Speed in the control of "Pre-Consolidation"
	ControlData[1].q=10.0;				// Referential Stress in the control of "Pre-Consolidation"
	ERR_StressCom=0.5;					// (kPa)
	ERR_StressExt=-0.5;					// (kPa)
	ERR_StressA=0.1;					// (kPa)
	CH_Motor		=0;					// Motor D/A Channel
	CH_MotorCruch	=1;					// Motor Cruch D/A Channel
	CH_MotorSpeed	=2;					// Motor Speed D/A Channel 
	CH_EP_Cell		=3;					// Cell Pressure D/A Channel
	DA_Cal_a[CH_MotorSpeed]=0.003378059;	// D/A Calibration Factor, MotoSpeed (V/rpm)
	DA_Cal_b[CH_MotorSpeed]=0.0;		// D/A Calibration Factor, MotoSpeed (V) 
	DA_Cal_a[CH_EP_Cell]=0.003401361;			// D/A Calibration Factor, EP for Cell Pressure (V/kPa)
	DA_Cal_b[CH_EP_Cell]=0.0;			// D/A Calibration Factor, EP for Cell Pressure (V/kPa)
	Flag_Cyclic=FALSE;
	NUM_Cyclic=0;
	TotalStepTime=0.0;
}

CDigitShowBasicDoc::~CDigitShowBasicDoc()
{
}

BOOL CDigitShowBasicDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;


	// (SDI ドキュメントはこのドキュメントを再利用します。)

	return TRUE;
}



/////////////////////////////////////////////////////////////////////////////
// CDigitShowBasicDoc シリアライゼーション

void CDigitShowBasicDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
	
	}
	else
	{
	
	}
}

/////////////////////////////////////////////////////////////////////////////
// CDigitShowBasicDoc クラスの診断

#ifdef _DEBUG
void CDigitShowBasicDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CDigitShowBasicDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CDigitShowBasicDoc コマンド
void CDigitShowBasicDoc::OpenBoard()
{
	int	i;
	
	if( Flag_SetBoard){
		AfxMessageBox("Initialization has been already accomplished", MB_ICONSTOP | MB_OK );
		return;
	}
	else{
		// OPEN A/D BOARDS.
		if(NUMAD > 0 ){
			Ret = AioInit ( "AIO000" , &AdId[0] );
		    if(Ret != 0){
			    Ret2 = AioGetErrorString(Ret, ErrorString);
				TextString.Format("AioInit = %d : %s", Ret, ErrorString);
				AfxMessageBox(TextString, MB_ICONSTOP | MB_OK );
				return;
			}
			else{
				Ret = AioResetDevice(AdId[0]);
			    if(Ret != 0){
				    Ret2 = AioGetErrorString(Ret, ErrorString);
					TextString.Format("AioResetDevice = %d : %s", Ret, ErrorString);
					AfxMessageBox(TextString, MB_ICONSTOP | MB_OK );
					return;
				}
			}
		}
		if(NUMAD > 1){
			Ret = AioInit ( "AIO001" , &AdId[1] );
		    if(Ret != 0){
			    Ret2 = AioGetErrorString(Ret, ErrorString);
				TextString.Format("AioInit = %d : %s", Ret, ErrorString);
				AfxMessageBox(TextString, MB_ICONSTOP | MB_OK );
				return;
			}
			else{
				Ret = AioResetDevice(AdId[1]);
			    if(Ret != 0){
				    Ret2 = AioGetErrorString(Ret, ErrorString);
					TextString.Format("AioResetDevice = %d : %s", Ret, ErrorString);
					AfxMessageBox(TextString, MB_ICONSTOP | MB_OK );
					return;
				}
			}
		}
		// OPEN D/A BOARDS.
		if(NUMDA > 0){
			Ret = AioInit("AIO003" , &DaId[0] );
		    if(Ret != 0){
			    Ret2 = AioGetErrorString(Ret, ErrorString);
				TextString.Format("AioInit = %d : %s", Ret, ErrorString);
				AfxMessageBox(TextString, MB_ICONSTOP | MB_OK );
				return;
			}
			else{
				Ret = AioResetDevice(DaId[0]);
			    if(Ret != 0){
				    Ret2 = AioGetErrorString(Ret, ErrorString);
					TextString.Format("AioResetDevice = %d : %s", Ret, ErrorString);
					AfxMessageBox(TextString, MB_ICONSTOP | MB_OK );
					return;
				}
			}
		}
		// Set Sampling Condition
		AdMaxCH=0;
		for(i=0;i<NUMAD;i++){
			Ret = AioGetAiInputMethod ( AdId[i] , &AdInputMethod[i] );
			Ret = AioGetAiResolution ( AdId[i] , &AdResolution[i] );
			Ret = AioGetAiMaxChannels ( AdId[i] , &AdChannels[i] );
			Ret = AioSetAiChannels ( AdId[i] , AdChannels[i] );
			AdMaxCH=AdMaxCH+AdChannels[i]/2;
			Ret = AioSetAiRangeAll ( AdId[i], 1 );	// (-5V, 5V)
			Ret = AioGetAiRange ( AdId[i] , 0 , &AdRange[i] );
			Ret = GetRangeValue(AdRange[i], &AdRangeMax[i], &AdRangeMin[i]);
			Ret = AioGetAiMemoryType ( AdId[i] , &AdMemoryType[i] );
			Ret = AioGetAiSamplingClock ( AdId[i] , &AdSamplingClock[i] );
			//2020.02.13 M.Kuno ScanClock制御を追加
			{
				short maxChannels = 64;
				Ret = AioGetAiMaxChannels(AdId[i], &maxChannels);
				float scanClock = 1000.f / maxChannels;
				Ret = AioSetAiScanClock(AdId[i], scanClock);
				Ret = AioGetAiScanClock(AdId[i], &AdScanClock[i]);
			}
			Ret = AioGetAiEventSamplingTimes ( AdId[i] , &AdSamplingTimes[i] );
		}
		SavingTime=300;
		TotalSamplingTimes=long(SavingTime*1000000/AdSamplingClock[0]);
		AllocatedMemory=4*AdMaxCH*TotalSamplingTimes/1024.0f/1024.0f;
		AvSmplNum=20;
		for(i=0;i<NUMDA;i++){
			Ret = AioGetAoResolution ( DaId[i] , &DaResolution[i] );
			Ret = AioGetAoMaxChannels ( DaId[i] , &DaChannels[i] );
			Ret = AioSetAoRangeAll ( DaId[i] , 50 );	// 0 - 10V
			Ret = AioGetAoRange ( DaId[i] , 0 , &DaRange[i] );
			Ret = GetRangeValue(DaRange[i], &DaRangeMax[i], &DaRangeMin[i]);
		}
		Flag_SetBoard=TRUE;
	}
	return;
}

void CDigitShowBasicDoc::CloseBoard()
{
	// Close A/D and D/A board to end the application 
	if( Flag_SetBoard==TRUE ){
		if(NUMAD > 0)	Ret = AioExit(AdId[0]);
		if(NUMAD > 1)	Ret = AioExit(AdId[1]);
		if(NUMDA > 0)	Ret = AioExit(DaId[0]);
	}
}

//--- Input from A/D Board ---
void CDigitShowBasicDoc::AD_INPUT()
{
	int	i,j,k;
	k=0;
	if(NUMAD>0){
		for(i=0;i<AdChannels[0]/2;i++){
			Vout[k]=0.0f;
			for(j=0;j<AvSmplNum;j++){
				Vout[k] = Vout[k]+BinaryToVolt(AdRangeMax[0], AdRangeMin[0], AdResolution[0], AdData0[AdChannels[0]*j+2*i])/float(AvSmplNum);
			}
			k=k+1;
		}
	}
	if(NUMAD>1){
		for(i=0;i<AdChannels[1]/2;i++){
			Vout[k]=0.0f;
			for(j=0;j<AvSmplNum;j++){
				Vout[k] = Vout[k]+BinaryToVolt(AdRangeMax[1], AdRangeMin[1], AdResolution[1], AdData1[AdChannels[1]*j+2*i])/float(AvSmplNum);
			}
			k=k+1;
		}
	}
}

//--- Output to D/A Board ---
void CDigitShowBasicDoc::DA_OUTPUT()
{
	int	i,j,k;
	k=0;
	for(i=0;i<NUMDA;i++){
		for(j=0;j<DaChannels[i];j++){
			if(DAVout[k]<0.0f)	DAVout[k]=0.0f;
			if(DAVout[k]>9.9999f) DAVout[k]=9.9999f;
			DaData[j] = VoltToBinary(DaRangeMax[i], DaRangeMin[i], DaResolution[i], DAVout[k]);
			k=k+1;
		}
		Ret = AioMultiAo(DaId[i], DaChannels[i], &DaData[0]);
	}
}

//--- Calcuration of Physical Value ---
void CDigitShowBasicDoc::Cal_Physical()
{
	int	i;
	for(i=0;i<64;i++){
		Phyout[i]=	Cal_a[i]*Vout[i]*Vout[i] + Cal_b[i]*Vout[i] + Cal_c[i];
	}
}

//--- Calcuration of the Other Parameters ---
void CDigitShowBasicDoc::Cal_Param()
{
//	Specimen Data in drain and undrain condition
	height=SpecimenData.Height[0]-Phyout[1];				// Current height
	volume=SpecimenData.Volume[0]-Phyout[4];				// Current volume in drain condition
	area=volume/height;										// Current area
	ea=-log(height/SpecimenData.Height[0])*100.0;			// True Axial Strain (%)
	ev=-log(volume/SpecimenData.Volume[0])*100.0;			// True Volumetric Strain in drain condition (%)
	er=(ev-ea)/2.0;											// True Radial strain (%)
	if(SpecimenData.VLDT1[0]>0.0 && Phyout[5]>0.0) {
		eLDT1=-log(Phyout[5]/SpecimenData.VLDT1[0])*100.0;	// True LDT Strain (%)
	}
	else{
		eLDT1=0.0;
	}
	if(SpecimenData.VLDT2[0]>0.0 && Phyout[6]>0.0) {
		eLDT2=-log(Phyout[6]/SpecimenData.VLDT2[0])*100.0;	// True LDT Strain (%)
	}
	else{
		eLDT2=0.0;
	}
	eLDT=(eLDT1+eLDT2)/2.0;
	q=Phyout[0]/area*1000.0;		// Deviator Stress (kPa)
	sr=Phyout[2];					// Cell(Radial) Stress (kPa)
	sa=q+sr;						// Axial Stress (kPa)
	p=(sa+2.0*sr)/3.0;				// Mean Principal Stress (kPa)
	e_sr=Phyout[3];					// Cell Effective Stress (kPa)
	e_sa=q+e_sr;					// Axial Effective Stress (kPa)
	u=sr-e_sr;						// Pore Pressure
	e_p=(e_sa+2.0*e_sr)/3.0;		// Mean Effective Stress (kPa)
// 
	//---The Value to display---
	CalParam[0]=sa;
	CalParam[1]=sr;
	CalParam[2]=e_sa;
	CalParam[3]=e_sr;
	CalParam[4]=u;
	CalParam[5]=p;
	CalParam[6]=q;
	CalParam[7]=e_p;
	CalParam[8]=ea;
	CalParam[9]=er;
	CalParam[10]=ev;
	CalParam[11]=eLDT1;
	CalParam[12]=eLDT2;
	CalParam[13]=eLDT;
	CalParam[14]=(e_sa+e_sr)/2.0;
	CalParam[15]=(e_sa-e_sr)/2.0;
}

void CDigitShowBasicDoc::SaveToFile()
{
	// Save Voltage and Physical Data
	int	i,j,k;

	k=0;
	fprintf(FileSaveData0,"%.3lf	",SequentTime2);
	fprintf(FileSaveData1,"%.3lf	",SequentTime2);
	for(i=0;i<NUMAD;i++){
		for(j=0;j<AdChannels[i]/2;j++){
			fprintf(FileSaveData0,"%lf	",Vout[k]);
			fprintf(FileSaveData1,"%lf	",Phyout[k]);
			k=k+1;
		}
	}
	fprintf(FileSaveData0,"\n");
	fprintf(FileSaveData1,"\n");
	// Save Parameter Data
	fprintf(FileSaveData2,"%.3lf	",SequentTime2);	
	for(i=0;i<16;i++){
		fprintf(FileSaveData2,"%lf	",CalParam[i]);
	}
	fprintf(FileSaveData2,"\n");
}

void CDigitShowBasicDoc::SaveToFile2()
{
	int	i,j,k;
	for(i=0;i<CurrentSamplingTimes;i++){
		k=0;
		fprintf(FileSaveData0,"%.3lf	",SavingClock/1000000.0*i);
		fprintf(FileSaveData1,"%.3lf	",SavingClock/1000000.0*i);
		if(NUMAD>0){
			for(j=0;j<AdChannels[0]/2;j++){
				Vtmp = BinaryToVolt(AdRangeMax[0], AdRangeMin[0], AdResolution[0], *((PLONG)pSmplData0+i*AdChannels[0]/2+j));
				Ptmp = Cal_a[k]*Vtmp*Vtmp+Cal_b[k]*Vtmp+Cal_c[k];
				k=k+1;
				fprintf(FileSaveData0,"%lf	",Vtmp);
				fprintf(FileSaveData1,"%lf	",Ptmp);
			}
		}
		if(NUMAD>1){
			for(j=0;j<AdChannels[1]/2;j++){
				Vtmp = BinaryToVolt(AdRangeMax[1], AdRangeMin[1], AdResolution[1], *((PLONG)pSmplData1+i*AdChannels[1]/2+j));
				Ptmp = Cal_a[k]*Vtmp*Vtmp+Cal_b[k]*Vtmp+Cal_c[k];
				k=k+1;
				fprintf(FileSaveData0,"%lf	",Vtmp);
				fprintf(FileSaveData1,"%lf	",Ptmp);
			}
		}
		fprintf(FileSaveData0,"\n");
		fprintf(FileSaveData1,"\n");
	}
}

void CDigitShowBasicDoc::Allocate_Memory()
{
	if(Flag_SaveData==TRUE){
		if(NUMAD>0){
			hHeap0 = GetProcessHeap();
			pSmplData0 = HeapAlloc(hHeap0,HEAP_ZERO_MEMORY,unsigned long(TotalSamplingTimes*AdChannels[0]/2*sizeof(LONG)));
		}
		if(NUMAD>1){
			hHeap1 = GetProcessHeap();
			pSmplData1 = HeapAlloc(hHeap1,HEAP_ZERO_MEMORY,unsigned long(TotalSamplingTimes*AdChannels[1]/2*sizeof(LONG)));
		}
	}
	if(Flag_SaveData==FALSE){
		if(NUMAD>0)	HeapFree(hHeap0,0,pSmplData0);
		if(NUMAD>1)	HeapFree(hHeap1,0,pSmplData1);
	}
}

//--- Control Statements ---
void CDigitShowBasicDoc::Control_DA()
{
	switch (Control_ID)
	{
	case 0:
		{ 
		}
		break;
	case 1:
		{ 
		//---Before Consolidation: Keep the specimen isotropic condition by Motor Control.--- 
		// ControlData[1].q: Reference Error Stress (kPa).
		// ControlData[1].MotorSpeed: The Maximum Motor Speed (rpm).
			DAVout[CH_Motor]=5.0f;			// Motor: On
			if( q > ERR_StressCom ){
				DAVout[CH_MotorCruch]=5.0f;		// Cruch: Up
				if( q > ControlData[1].q ){
					DAVout[CH_MotorSpeed]=float(DA_Cal_a[CH_MotorSpeed]*ControlData[1].MotorSpeed+DA_Cal_b[CH_MotorSpeed]);
				}
				if( q <= ControlData[1].q ){
					DAVout[CH_MotorSpeed]=float(DA_Cal_a[CH_MotorSpeed]*(q/ControlData[1].q)*ControlData[1].MotorSpeed+DA_Cal_b[CH_MotorSpeed]);
				}
			}
			else if( q < ERR_StressExt ){
				DAVout[CH_MotorCruch]=0.0f;		// Cruch: Down
				if( q < -ControlData[1].q ){
					DAVout[CH_MotorSpeed]=float(DA_Cal_a[CH_MotorSpeed]*ControlData[1].MotorSpeed+DA_Cal_b[CH_MotorSpeed]);
				}
				if( q >= -ControlData[1].q ){
					DAVout[CH_MotorSpeed]=float(DA_Cal_a[CH_MotorSpeed]*(-q/ControlData[1].q)*ControlData[1].MotorSpeed+DA_Cal_b[CH_MotorSpeed]);
				}
			}
			else {
					DAVout[CH_MotorSpeed]=0.0f;	// RPM->0
			}
			DA_OUTPUT();
		}
		break;
	case 2:
		{ 
		// Consolidation (Motor Control):
		// ControlData[2].e_sigma[0]:	Target Axial Effectve Stress,
		// ControlData[2].K0:			K0 value,
		// ControlData[2].sigmaRate[2]:	Increase Rate of Cell Pressure 
		// ControlData[2].MotorSpeed:	Motor Speed
			DAVout[CH_Motor]=5.0f;			// Motor: On
			DAVout[CH_MotorSpeed]=float(DA_Cal_a[CH_MotorSpeed]*ControlData[2].MotorSpeed+DA_Cal_b[CH_MotorSpeed]);
			if( e_sr < ControlData[2].e_sigma[0]*ControlData[2].K0-ERR_StressA){
				DAVout[CH_EP_Cell]=DAVout[CH_EP_Cell]+float(DA_Cal_a[CH_EP_Cell]*ControlData[2].sigmaRate[2]/60.0*TimeInterval_2/1000.0);
			}	
			if( e_sr > ControlData[2].e_sigma[0]*ControlData[2].K0+ERR_StressA){
				DAVout[CH_EP_Cell]=DAVout[CH_EP_Cell]-float(DA_Cal_a[CH_EP_Cell]*ControlData[2].sigmaRate[2]/60.0*TimeInterval_2/1000.0);
			}	
			if( e_sa < e_sr/ControlData[2].K0+ERR_StressExt ){
				DAVout[CH_MotorCruch]=0.0f;		// Cruch: Down
			}			
			else if( e_sa > e_sr/ControlData[2].K0+ERR_StressCom ){
				DAVout[CH_MotorCruch]=5.0f;		// Cruch: Up
			}
			else {
				DAVout[CH_MotorSpeed]=0.0f;		// RPM->0
			}
			DA_OUTPUT();
		}
		break;
	case 3:
		{ 
		// Monotonic Loading (Motor Control)
		// ControlData[3].MotorSpeed:	Motor Speed
		// ControlData[3].MotorCruch:	Compression:1 /Extension:0						
		// ControlData[3].flag[0]:		Monotonic_Loading:0 /Creep:1
		// ControlData[3].sigma[0];		Limiter
			DAVout[CH_Motor]=5.0f;					// Motor: On
			DAVout[CH_MotorSpeed]=float(DA_Cal_a[CH_MotorSpeed]*ControlData[3].MotorSpeed+DA_Cal_b[CH_MotorSpeed]);
			if(ControlData[3].flag[0]==FALSE){		// Monotonic Loading
				if(ControlData[3].MotorCruch == 0 ){
					DAVout[CH_MotorCruch]=0.0f;		// Cruch: Down
					if(q >= ControlData[3].q) ControlData[3].flag[0]=TRUE;
				}
				if(ControlData[3].MotorCruch == 1 )	{
					DAVout[CH_MotorCruch]=5.0f;		// Cruch: Up
					if(q <= ControlData[3].q) ControlData[3].flag[0]=TRUE;
				}
			}
			if(ControlData[3].flag[0]==TRUE){		// Creep
				if(ControlData[3].MotorCruch == 0 ){
					DAVout[CH_MotorCruch]=0.0f;		// Cruch: Down
					if(q>=ControlData[3].q+ERR_StressExt)	DAVout[CH_MotorSpeed]=0.0f;	// RPM->0 
				}
				if(ControlData[3].MotorCruch == 1 ){
					DAVout[CH_MotorCruch]=5.0f;		// Cruch: Up
					if(q<=ControlData[3].q+ERR_StressCom)	DAVout[CH_MotorSpeed]=0.0f;	// RPM->0
				}
			}
			DA_OUTPUT();
		}
		break;
	case 4:
		{ 
		// Monotonic Loading (Motor Control)
		// ControlData[4].MotorSpeed:	Motor Speed
		// ControlData[4].MotorCruch:	Cruch Loading:1 /Unloading:0						
		// ControlData[4].flag:			Loading:0 /Creep:1
		// ControlData[4].sigma[0];		Limiter
			DAVout[CH_MotorSpeed]=float(DA_Cal_a[CH_MotorSpeed]*ControlData[4].MotorSpeed+DA_Cal_b[CH_MotorSpeed]);
			DAVout[CH_Motor]=5.0f;					// Motor:On
			if(ControlData[4].flag[0]==FALSE){		// Monotonic Loading
				if(ControlData[4].MotorCruch == 0 ){
					DAVout[CH_MotorCruch]=0.0f;		// Cruch:Down
					if(q >= ControlData[4].q) ControlData[4].flag[0]=TRUE;
				}
				if(ControlData[4].MotorCruch == 1 )	{
					DAVout[CH_MotorCruch]=5.0f;		// Cruch:Up
					if(q <= ControlData[4].q) ControlData[4].flag[0]=TRUE;
				}
			}
			if(ControlData[4].flag[0]==TRUE){		// Creep
				if(ControlData[4].MotorCruch == 0 ){
					DAVout[CH_MotorCruch]=0.0f;		// Cruch:Down
					if(q>=ControlData[4].q+ERR_StressExt)	DAVout[CH_MotorSpeed]=0.0f;	// RPM->0 
				}
				if(ControlData[4].MotorCruch == 1 ){
					DAVout[CH_MotorCruch]=5.0f;		// Cruch:Up
					if(q<=ControlData[4].q+ERR_StressCom)	DAVout[CH_MotorSpeed]=0.0f;	// RPM->0
				}
			}
			DA_OUTPUT();
		}
		break;
	case 5:
		{ 
			// Cyclic Loading
			DAVout[CH_MotorSpeed]=float(DA_Cal_a[CH_MotorSpeed]*ControlData[5].MotorSpeed+DA_Cal_b[CH_MotorSpeed]);
			DAVout[CH_Motor]=5.0f;						// Motor:On
			if(ControlData[5].flag[0]==FALSE){			// Cyclic in compression test
				if(ControlData[5].time[0]<ControlData[5].time[1]){ 
					DAVout[CH_MotorCruch]=0.0f;			// Cruch:Down
					if(q>=ControlData[5].sigma[1]) {
						ControlData[5].time[0]=ControlData[5].time[1];
						Flag_Cyclic=FALSE;
					}
				}
				if(ControlData[5].time[1]<=ControlData[5].time[0] || ControlData[5].time[0]<=ControlData[5].time[2]){
					if(Flag_Cyclic==FALSE){
						DAVout[CH_MotorCruch]=5.0f;		// Cruch:Up
						if(q<=ControlData[5].sigma[0]) Flag_Cyclic=TRUE;
					}
					if(Flag_Cyclic==TRUE){
						DAVout[CH_MotorCruch]=0.0f;		// Cruch:Down
						if(q>=ControlData[5].sigma[1]) {
							Flag_Cyclic=FALSE;
							ControlData[5].time[0]=ControlData[5].time[0]+1;
						}
					}
				}
				if(ControlData[5].time[0]>ControlData[5].time[2]){ 
					DAVout[CH_MotorCruch]=0.0f;			// Cruch:Down
				}
			}
			if(ControlData[5].flag[0]==TRUE){
				if(ControlData[5].time[0]<ControlData[5].time[1]){ 
					DAVout[CH_MotorCruch]=5.0f;			// Cruch:Up
					if(q<=ControlData[5].sigma[0]) {
						ControlData[5].time[0]=ControlData[5].time[1];
						Flag_Cyclic=TRUE;
					}
				}
				if(ControlData[5].time[1]<=ControlData[5].time[0] || ControlData[5].time[0]<=ControlData[5].time[2]){
					if(Flag_Cyclic==TRUE){
						DAVout[CH_MotorCruch]=0.0f;		// Cruch:Down
						if(q>=ControlData[5].sigma[1]) Flag_Cyclic=FALSE;
					}
					if(Flag_Cyclic==FALSE){
						DAVout[CH_MotorCruch]=5.0f;		// Cruch:Up
						if(q<=ControlData[5].sigma[0]) {
							Flag_Cyclic=TRUE;
							ControlData[5].time[0]=ControlData[5].time[0]+1;
						}
					}
				}
				if(ControlData[5].time[0]>ControlData[5].time[2]){ 
					DAVout[CH_MotorCruch]=5.0f;			// Cruch:Up
				}
			}
			DA_OUTPUT();
		}
		break;
	case 6:
		{ 
			// Drain Cyclic Loading
			DAVout[CH_MotorSpeed]=float(DA_Cal_a[CH_MotorSpeed]*ControlData[6].MotorSpeed+DA_Cal_b[CH_MotorSpeed]);
			DAVout[CH_Motor]=5.0f;
			if(ControlData[6].flag[0]==FALSE){
				if(ControlData[6].time[0]<ControlData[6].time[1]){ 
					DAVout[CH_MotorCruch]=0.0f;			// Cruch:Down
					if(q>=ControlData[6].sigma[1]) {
						ControlData[6].time[0]=ControlData[6].time[1];
						Flag_Cyclic=FALSE;
					}
				}
				if(ControlData[6].time[1]<=ControlData[6].time[0] || ControlData[6].time[0]<=ControlData[6].time[2]){
					if(Flag_Cyclic==FALSE){
						DAVout[CH_MotorCruch]=5.0f;		// Cruch:Up
						if(q<=ControlData[6].sigma[0]) Flag_Cyclic=TRUE;
					}
					if(Flag_Cyclic==TRUE){
						DAVout[CH_MotorCruch]=0.0f;		// Cruch:Down
						if(q>=ControlData[6].sigma[1]) {
							Flag_Cyclic=FALSE;
							ControlData[6].time[0]=ControlData[6].time[0]+1;
						}
					}
				}
				if(ControlData[6].time[0]>ControlData[6].time[2]){ 
					DAVout[CH_MotorCruch]=0.0f;			// Cruch:Down
				}
			}
			if(ControlData[6].flag[0]==TRUE){
				if(ControlData[6].time[0]<ControlData[6].time[1]){ 
					DAVout[CH_MotorCruch]=5.0f;			// Cruch:Up
					if(q<=ControlData[6].sigma[0]) {
						ControlData[6].time[0]=ControlData[6].time[1];
						Flag_Cyclic=TRUE;
					}
				}
				if(ControlData[6].time[1]<=ControlData[6].time[0] || ControlData[6].time[0]<=ControlData[6].time[2]){
					if(Flag_Cyclic==TRUE){
						DAVout[CH_MotorCruch]=0.0f;		// Cruch:Down
						if(q>=ControlData[6].sigma[1]) Flag_Cyclic=FALSE;
					}
					if(Flag_Cyclic==FALSE){
						DAVout[CH_MotorCruch]=5.0f;		// Cruch:Up
						if(q<=ControlData[5].sigma[0]) {
							Flag_Cyclic=TRUE;
							ControlData[6].time[0]=ControlData[6].time[0]+1;
						}
					}
				}
				if(ControlData[6].time[0]>ControlData[6].time[2]){ 
					DAVout[CH_MotorCruch]=5.0f;			// Cruch:Up
				}
			}
			DA_OUTPUT();
		}
		break;
	case 7:
		{ 
			DAVout[CH_Motor]=5.0f;
			DAVout[CH_MotorSpeed]=float(DA_Cal_a[CH_MotorSpeed]*ControlData[7].MotorSpeed+DA_Cal_b[CH_MotorSpeed]);
			if(ControlData[7].sigma[1] == ControlData[7].e_sigma[1]){
				DAVout[CH_EP_Cell]=DAVout[CH_EP_Cell]+float(0.2*DA_Cal_a[CH_EP_Cell]*(ControlData[7].e_sigma[1]-e_sr));
				if(e_sa > ControlData[7].e_sigma[0]+ERR_StressCom)		DAVout[CH_MotorCruch]=5.0f;	// Cruch:Up
				else if(e_sa < ControlData[7].e_sigma[0]+ERR_StressExt)	DAVout[CH_MotorCruch]=0.0f;	// Cruch:Down
				else DAVout[CH_MotorSpeed]=0.0f;
			}
			if(ControlData[7].sigma[1] < ControlData[7].e_sigma[1]){
				if(e_sr >= ControlData[7].e_sigma[1]) {
					DAVout[CH_EP_Cell]=DAVout[CH_EP_Cell]-float(0.2*DA_Cal_a[CH_EP_Cell]*(e_sr-ControlData[7].e_sigma[1]));
				}
				if(e_sr < ControlData[7].e_sigma[1]) {
					DAVout[CH_EP_Cell]=DAVout[CH_EP_Cell]+float(DA_Cal_a[CH_EP_Cell]*fabs(ControlData[7].sigmaRate[0])/60.0*TimeInterval_2/1000.0);
				}
				if(e_sa > (ControlData[7].e_sigma[0]-ControlData[7].sigma[0])/(ControlData[7].e_sigma[1]-ControlData[7].sigma[1])*(e_sr-ControlData[7].sigma[1])+ControlData[7].sigma[0]+ERR_StressCom){
					DAVout[CH_MotorCruch]=5.0f;	// Cruch:Up
				}
				else if(e_sa < (ControlData[7].e_sigma[0]-ControlData[7].sigma[0])/(ControlData[7].e_sigma[1]-ControlData[7].sigma[1])*(e_sr-ControlData[7].sigma[1])+ControlData[7].sigma[0]+ERR_StressExt){
					DAVout[CH_MotorCruch]=0.0f;	// Cruch:Down
				}
				else {
					DAVout[CH_MotorSpeed]=0.0f;	// RPM -> 0
				}
			}
			if(ControlData[7].sigma[1] > ControlData[7].e_sigma[1]){
				if(e_sr > ControlData[7].e_sigma[1]) {
					DAVout[CH_EP_Cell]=DAVout[CH_EP_Cell]-float(DA_Cal_a[CH_EP_Cell]*fabs(ControlData[7].sigmaRate[0])/60.0*TimeInterval_2/1000.0);
				}
				if(e_sr <= ControlData[7].e_sigma[1]) {
					DAVout[CH_EP_Cell]=DAVout[CH_EP_Cell]+float(0.2*DA_Cal_a[CH_EP_Cell]*(ControlData[7].e_sigma[1]-e_sr));
				}
				if(e_sa > (ControlData[7].e_sigma[0]-ControlData[7].sigma[0])/(ControlData[7].e_sigma[1]-ControlData[7].sigma[1])*(e_sr-ControlData[7].sigma[1])+ControlData[7].sigma[0]+ERR_StressCom){
					DAVout[CH_MotorCruch]=5.0f;	// Cruch:Up
				}
				else if(e_sa < (ControlData[7].e_sigma[0]-ControlData[7].sigma[0])/(ControlData[7].e_sigma[1]-ControlData[7].sigma[1])*(e_sr-ControlData[7].sigma[1])+ControlData[7].sigma[0]+ERR_StressExt){
					DAVout[CH_MotorCruch]=0.0f;	// Cruch:Down
				}
				else {
					DAVout[CH_MotorSpeed]=0.0f;	// RPM -> 0
				}
			}
			DA_OUTPUT();
		}
		break;
	case 8:
		{ 
			DA_OUTPUT();
		}
		break;
	case 9:
		{ 
			DA_OUTPUT();
		}
		break;
	case 10:
		{ 
			DA_OUTPUT();
		}
		break;
	case 11:
		{ 
			DA_OUTPUT();
		}
		break;
	case 12:
		{ 
			DA_OUTPUT();
		}
		break;
	case 13:
		{ 
			DA_OUTPUT();
		}
		break;
	case 14:
		{ 
			DA_OUTPUT();
		}
		break;
	case 15:
		{ 
			if( CURNUM >=0 && CURNUM < 128 ){
				if( CFNUM[CURNUM]==0 ){
					DAVout[CH_Motor]=0.0f;
				}
				if( CFNUM[CURNUM]==1 )	MLoading_Stress();
				if( CFNUM[CURNUM]==2 )	MLoading_Strain();
				if( CFNUM[CURNUM]==3 )	CLoading_Stress();
				if( CFNUM[CURNUM]==4 )	CLoading_Strain();
				if( CFNUM[CURNUM]==5 )	Creep();
				if( CFNUM[CURNUM]==6 )	LinearEffectiveStressPath();
				if( CFNUM[CURNUM]==7 )	Creep2();
				DA_OUTPUT();
			}
		}
		break;
	}
}
void CDigitShowBasicDoc::Start_Control()
{

}

void CDigitShowBasicDoc::Stop_Control()
{
	DAVout[CH_MotorSpeed]=0.0f;	//Motor Speed->0
	DA_OUTPUT();
}

void CDigitShowBasicDoc::MLoading_Stress()
{
	TotalStepTime=TotalStepTime+CtrlStepTime/60.0;
	DAVout[CH_Motor]=5.0f;			// Motor: On
	DAVout[CH_MotorSpeed]=float(DA_Cal_a[CH_MotorSpeed]*CFPARA[CURNUM][1]+DA_Cal_b[CH_MotorSpeed]);	// Motor_Speed
	if(CFPARA[CURNUM][0]==0.0){
		if(q <= CFPARA[CURNUM][2]) {
			DAVout[CH_MotorCruch]=0.0f;		// Cruch:Down
		}
		else {
			CURNUM=CURNUM+1;
			TotalStepTime=0.0;
		}
	}
	else if(CFPARA[CURNUM][0]==1.0){
		if(q >= CFPARA[CURNUM][2]) {
			DAVout[CH_MotorCruch]=5.0f;		// Cruch:Up
		}
		else {
			CURNUM=CURNUM+1;
			TotalStepTime=0.0;
		}
	}
}

void CDigitShowBasicDoc::MLoading_Strain()
{
	TotalStepTime=TotalStepTime+CtrlStepTime/60.0;
	DAVout[CH_Motor]=5.0f;					// Motor: On
	DAVout[CH_MotorSpeed]=float(DA_Cal_a[CH_MotorSpeed]*CFPARA[CURNUM][1]+DA_Cal_b[CH_MotorSpeed]);	// Motor_Speed
	if(CFPARA[CURNUM][0]==0.0){
		if(ea <= CFPARA[CURNUM][2]) {
			DAVout[CH_MotorCruch]=0.0f;		// Cruch:Down
		}
		else {
			CURNUM=CURNUM+1;
			TotalStepTime=0.0;
		}
	}
	else if(CFPARA[CURNUM][0]==1.0){
		if(ea >= CFPARA[CURNUM][2]) {
			DAVout[CH_MotorCruch]=5.0f;		// Cruch:Up
		}
		else {
			CURNUM=CURNUM+1;
			TotalStepTime=0.0;
		}
	}
}

void CDigitShowBasicDoc::CLoading_Stress()
{
	TotalStepTime=TotalStepTime+CtrlStepTime/60.0;
	DAVout[CH_Motor]=5.0f;					// Motor: On
	DAVout[CH_MotorSpeed]=float(DA_Cal_a[CH_MotorSpeed]*CFPARA[CURNUM][1]+DA_Cal_b[CH_MotorSpeed]);	// Motor_Speed
	if(CFPARA[CURNUM][0]==0.0){
		if(NUM_Cyclic==0){
			Flag_Cyclic=FALSE;
			NUM_Cyclic=1;
		}
		if(NUM_Cyclic!=0 && NUM_Cyclic <= CFPARA[CURNUM][4]){
			if(Flag_Cyclic==FALSE){
				DAVout[CH_MotorCruch]=5.0f;		// Cruch:Up
				if(q<=CFPARA[CURNUM][2]) Flag_Cyclic=TRUE;
			}
			if(Flag_Cyclic==TRUE){
				DAVout[CH_MotorCruch]=0.0f;		// Cruch:Down
				if(q>=CFPARA[CURNUM][3]) {
					Flag_Cyclic=FALSE;
					NUM_Cyclic=NUM_Cyclic+1;
				}
			}
		}
		if(NUM_Cyclic>CFPARA[CURNUM][4]){ 
			CURNUM=CURNUM+1;
			TotalStepTime=0.0;
			NUM_Cyclic=0;
		}
	}
	else if(CFPARA[CURNUM][0]==1.0){
		if(NUM_Cyclic==0){
			Flag_Cyclic=TRUE;
			NUM_Cyclic=1;
		}
		if(NUM_Cyclic!=0 && NUM_Cyclic <= CFPARA[CURNUM][4]){
			if(Flag_Cyclic==FALSE){
				DAVout[CH_MotorCruch]=5.0f;		// Cruch:Up
				if(q<=CFPARA[CURNUM][2]) {
					Flag_Cyclic=TRUE;
					NUM_Cyclic=NUM_Cyclic+1;
				}
			}
			if(Flag_Cyclic==TRUE){
				DAVout[CH_MotorCruch]=0.0f;		// Cruch:Down
				if(q>=CFPARA[CURNUM][3]) Flag_Cyclic=FALSE;
			}
		}
		if(NUM_Cyclic>CFPARA[CURNUM][4]){ 
			CURNUM=CURNUM+1;
			TotalStepTime=0.0;
			NUM_Cyclic=0;
		}
	}
}

void CDigitShowBasicDoc::CLoading_Strain()
{
	TotalStepTime=TotalStepTime+CtrlStepTime/60.0;
	DAVout[CH_Motor]=5.0f;
	DAVout[CH_MotorSpeed]=float(DA_Cal_a[CH_MotorSpeed]*CFPARA[CURNUM][1]+DA_Cal_b[CH_MotorSpeed]);	// Motor_Speed
	if(CFPARA[CURNUM][0]==0.0){
		if(NUM_Cyclic==0){
			Flag_Cyclic=FALSE;
			NUM_Cyclic=1;
		}
		if(NUM_Cyclic!=0 && NUM_Cyclic <= CFPARA[CURNUM][4]){
			if(Flag_Cyclic==FALSE){
				DAVout[CH_MotorCruch]=5.0f;		// Cruch:Up
				if(ea<=CFPARA[CURNUM][2]) Flag_Cyclic=TRUE;
			}
			if(Flag_Cyclic==TRUE){
				DAVout[CH_MotorCruch]=0.0f;		// Cruch:Down
				if(ea>=CFPARA[CURNUM][3]) {
					Flag_Cyclic=FALSE;
					NUM_Cyclic=NUM_Cyclic+1;
				}
			}
		}
		if(NUM_Cyclic>CFPARA[CURNUM][4]){ 
			CURNUM=CURNUM+1;
			TotalStepTime=0.0;
			NUM_Cyclic=0;
		}
	}
	else if(CFPARA[CURNUM][0]==1.0){
		if(NUM_Cyclic==0){
			Flag_Cyclic=TRUE;
			NUM_Cyclic=1;
		}
		if(NUM_Cyclic!=0 && NUM_Cyclic <= CFPARA[CURNUM][4]){
			if(Flag_Cyclic==FALSE){
				DAVout[CH_MotorCruch]=5.0f;		// Cruch:Up
				if(ea<=CFPARA[CURNUM][2]) {
					Flag_Cyclic=TRUE;
					NUM_Cyclic=NUM_Cyclic+1;
				}
			}
			if(Flag_Cyclic==TRUE){
				DAVout[CH_MotorCruch]=0.0f;		// Cruch:Down
				if(ea>=CFPARA[CURNUM][3]) Flag_Cyclic=FALSE;
			}
		}
		if(NUM_Cyclic>CFPARA[CURNUM][4]){ 
			CURNUM=CURNUM+1;
			TotalStepTime=0.0;
			NUM_Cyclic=0;
		}
	}
}

void CDigitShowBasicDoc::Creep()
{
	TotalStepTime=TotalStepTime+CtrlStepTime/60.0;
	DAVout[CH_Motor]=5.0f;		// Motor:On
	DAVout[CH_MotorSpeed]=float(DA_Cal_a[CH_MotorSpeed]*CFPARA[CURNUM][0]+DA_Cal_b[CH_MotorSpeed]);
	if(q>=CFPARA[CURNUM][1]+ERR_StressCom)	{
		DAVout[CH_MotorCruch]=5.0f;	// Cruch:Up
	}
	else if(q<=CFPARA[CURNUM][1]+ERR_StressExt)	{
		DAVout[CH_MotorCruch]=0.0f;	// Cruch:Down
	}		
	else {
		DAVout[CH_MotorSpeed]=0.0f;	// RPM->0
	}
	if(TotalStepTime>= CFPARA[CURNUM][2]) {
		CURNUM=CURNUM+1;
		TotalStepTime=0.0;
	}
}

void CDigitShowBasicDoc::LinearEffectiveStressPath()
{
	TotalStepTime=TotalStepTime+CtrlStepTime/60.0;
	DAVout[CH_Motor]=5.0f;
	DAVout[CH_MotorSpeed]=float(DA_Cal_a[CH_MotorSpeed]*CFPARA[CURNUM][4]+DA_Cal_b[CH_MotorSpeed]);
	if(CFPARA[CURNUM][1]==CFPARA[CURNUM][3]){
		DAVout[CH_EP_Cell]=DAVout[CH_EP_Cell]+float(0.2*DA_Cal_a[CH_EP_Cell]*(CFPARA[CURNUM][3]-e_sr));
		if(e_sa > CFPARA[CURNUM][2]+ERR_StressCom){
			DAVout[CH_MotorCruch]=5.0f;	// Cruch:Up
		}
		else if(e_sa < CFPARA[CURNUM][2]+ERR_StressExt){
			DAVout[CH_MotorCruch]=0.0f;	// Cruch:Down
		}
		else {
			CURNUM=CURNUM+1;
			TotalStepTime=0.0;
		}
	}
	else if(CFPARA[CURNUM][1] < CFPARA[CURNUM][3]){
		if(e_sr >= CFPARA[CURNUM][3]-ERR_StressA) {
			DAVout[CH_EP_Cell]=DAVout[CH_EP_Cell]-float(0.2*DA_Cal_a[CH_EP_Cell]*(e_sr-CFPARA[CURNUM][3]));
		}
		if(e_sr < CFPARA[CURNUM][3]-ERR_StressA) {
			DAVout[CH_EP_Cell]=DAVout[CH_EP_Cell]+float(DA_Cal_a[CH_EP_Cell]*fabs(CFPARA[CURNUM][5])/60.0*TimeInterval_2/1000.0);
		}
		if(e_sa > (CFPARA[CURNUM][2]-CFPARA[CURNUM][0])/(CFPARA[CURNUM][3]-CFPARA[CURNUM][1])*(e_sr-CFPARA[CURNUM][1])+CFPARA[CURNUM][0]+ERR_StressCom){
			DAVout[CH_MotorCruch]=5.0f;	// Cruch:Up
		}
		else if(e_sa < (CFPARA[CURNUM][2]-CFPARA[CURNUM][0])/(CFPARA[CURNUM][3]-CFPARA[CURNUM][1])*(e_sr-CFPARA[CURNUM][1])+CFPARA[CURNUM][0]+ERR_StressExt){
			DAVout[CH_MotorCruch]=0.0f;	// Cruch:Down
		}
		else {
			DAVout[CH_MotorSpeed]=0.0f;	// RPM -> 0
			if(fabs(e_sr-CFPARA[CURNUM][3]) <= ERR_StressA) {
				CURNUM=CURNUM+1;
				TotalStepTime=0.0;
			}
		}
	}
	else if(CFPARA[CURNUM][1] > CFPARA[CURNUM][3]){
		if(e_sr > CFPARA[CURNUM][3]+ERR_StressA) {
			DAVout[CH_EP_Cell]=DAVout[CH_EP_Cell]-float(DA_Cal_a[CH_EP_Cell]*fabs(CFPARA[CURNUM][5])/60.0*TimeInterval_2/1000.0);
		}
		if(e_sr <= CFPARA[CURNUM][3]+ERR_StressA) {
			DAVout[CH_EP_Cell]=DAVout[CH_EP_Cell]+float(0.2*DA_Cal_a[CH_EP_Cell]*(CFPARA[CURNUM][3]-e_sr));
		}
		if(e_sa > (CFPARA[CURNUM][2]-CFPARA[CURNUM][0])/(CFPARA[CURNUM][3]-CFPARA[CURNUM][1])*(e_sr-CFPARA[CURNUM][1])+CFPARA[CURNUM][0]+ERR_StressCom){
			DAVout[CH_MotorCruch]=5.0f;	// Cruch:Up
		}
		else if(e_sa < (CFPARA[CURNUM][2]-CFPARA[CURNUM][0])/(CFPARA[CURNUM][3]-CFPARA[CURNUM][1])*(e_sr-CFPARA[CURNUM][1])+CFPARA[CURNUM][0]+ERR_StressExt){
			DAVout[CH_MotorCruch]=0.0f;	// Cruch:Down
		}
		else {
			DAVout[CH_MotorSpeed]=0.0f;	// RPM -> 0
			if(fabs(e_sr-CFPARA[CURNUM][3]) <= ERR_StressA){
				CURNUM=CURNUM+1;
				TotalStepTime=0.0;
			}
		}
	}
}

void CDigitShowBasicDoc::Creep2()
{
	TotalStepTime=TotalStepTime+CtrlStepTime/60.0;
	DAVout[CH_Motor]=5.0f;		// Motor:On
	DAVout[CH_MotorSpeed]=float(DA_Cal_a[CH_MotorSpeed]*CFPARA[CURNUM][0]+DA_Cal_b[CH_MotorSpeed]);
	if(q <= CFPARA[CURNUM][1]+ERR_StressExt)	{
		DAVout[CH_MotorCruch]=0.0f;	// Cruch:Down
	}		
	else {
		DAVout[CH_MotorSpeed]=0.0f;	// RPM->0
	}
	if(TotalStepTime >= CFPARA[CURNUM][2]) {
		CURNUM=CURNUM+1;
		TotalStepTime=0.0;
	}
}

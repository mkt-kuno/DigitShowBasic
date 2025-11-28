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
 * but WITHOUT ANY WARRANTY;
 without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */


#include    "stdafx.h"
#include    "DigitShowBasic.h"
#include    "DigitShowBasicDoc.h"
#include    "caio.h"
#include    "dataconvert.h"

#include    "time.h"
#include    "math.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


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
    DigitShowContext* ctx = GetContext();
    int    i;
    
    if(ctx->FlagSetBoard){
        AfxMessageBox("Initialization has been already accomplished", MB_ICONSTOP | MB_OK );
        return;
    }
    else{
        // OPEN A/D BOARDS.
        if(ctx->NumAD > 0 ){
            ctx->Ret = AioInit ( "AIO000" , &ctx->ad.Id[0] );
            if(ctx->Ret != 0){
                ctx->Ret2 = AioGetErrorString(ctx->Ret, ctx->ErrorString);
                 ctx->TextString.Format("AioInit = %d : %s", ctx->Ret, ctx->ErrorString);
                AfxMessageBox( ctx->TextString, MB_ICONSTOP | MB_OK );
                return;
            }
            else{
                ctx->Ret = AioResetDevice(ctx->ad.Id[0]);
                if(ctx->Ret != 0){
                    ctx->Ret2 = AioGetErrorString(ctx->Ret, ctx->ErrorString);
                     ctx->TextString.Format("AioResetDevice = %d : %s", ctx->Ret, ctx->ErrorString);
                    AfxMessageBox( ctx->TextString, MB_ICONSTOP | MB_OK );
                    return;
                }
            }
        }
        if(ctx->NumAD > 1){
            ctx->Ret = AioInit ( "AIO001" , &ctx->ad.Id[1] );
            if(ctx->Ret != 0){
                ctx->Ret2 = AioGetErrorString(ctx->Ret, ctx->ErrorString);
                 ctx->TextString.Format("AioInit = %d : %s", ctx->Ret, ctx->ErrorString);
                AfxMessageBox( ctx->TextString, MB_ICONSTOP | MB_OK );
                return;
            }
            else{
                ctx->Ret = AioResetDevice(ctx->ad.Id[1]);
                if(ctx->Ret != 0){
                    ctx->Ret2 = AioGetErrorString(ctx->Ret, ctx->ErrorString);
                     ctx->TextString.Format("AioResetDevice = %d : %s", ctx->Ret, ctx->ErrorString);
                    AfxMessageBox( ctx->TextString, MB_ICONSTOP | MB_OK );
                    return;
                }
            }
        }
        // OPEN D/A BOARDS.
        if(ctx->NumDA > 0){
            ctx->Ret = AioInit("AIO003" , &ctx->da.Id[0] );
            if(ctx->Ret != 0){
                ctx->Ret2 = AioGetErrorString(ctx->Ret, ctx->ErrorString);
                 ctx->TextString.Format("AioInit = %d : %s", ctx->Ret, ctx->ErrorString);
                AfxMessageBox( ctx->TextString, MB_ICONSTOP | MB_OK );
                return;
            }
            else{
                ctx->Ret = AioResetDevice(ctx->da.Id[0]);
                if(ctx->Ret != 0){
                    ctx->Ret2 = AioGetErrorString(ctx->Ret, ctx->ErrorString);
                     ctx->TextString.Format("AioResetDevice = %d : %s", ctx->Ret, ctx->ErrorString);
                    AfxMessageBox( ctx->TextString, MB_ICONSTOP | MB_OK );
                    return;
                }
            }
        }
        // Set Sampling Condition
        for(i = 0;i<ctx->NumAD;i++){
            ctx->Ret = AioGetAiInputMethod ( ctx->ad.Id[i] , &ctx->ad.InputMethod[i] );
            ctx->Ret = AioGetAiResolution ( ctx->ad.Id[i] , &ctx->ad.Resolution[i] );
            ctx->Ret = AioGetAiMaxChannels ( ctx->ad.Id[i] , &ctx->ad.Channels[i] );
            ctx->Ret = AioSetAiChannels ( ctx->ad.Id[i] , ctx->ad.Channels[i] );
            ctx->AdMaxChannels = ctx->AdMaxChannels+ctx->ad.Channels[i]/2;
            ctx->Ret = AioSetAiRangeAll ( ctx->ad.Id[i], 1 );
            // (-5V, 5V)
            ctx->Ret = AioGetAiRange ( ctx->ad.Id[i] , 0 , &ctx->ad.Range[i] );
            ctx->Ret = GetRangeValue(ctx->ad.Range[i], &ctx->ad.RangeMax[i], &ctx->ad.RangeMin[i]);
            ctx->Ret = AioGetAiMemoryType ( ctx->ad.Id[i] , &ctx->ad.MemoryType[i] );
            ctx->Ret = AioGetAiSamplingClock ( ctx->ad.Id[i] , &ctx->ad.SamplingClock[i] );
            //2020.02.13 M.Kuno ScanClock制御を追加
            {
                short maxChannels = 64;
                ctx->Ret = AioGetAiMaxChannels(ctx->ad.Id[i], &maxChannels);
                float scanClock = 1000.f / maxChannels;
                ctx->Ret = AioSetAiScanClock(ctx->ad.Id[i], scanClock);
                ctx->Ret = AioGetAiScanClock(ctx->ad.Id[i], &ctx->ad.ScanClock[i]);
            }
            ctx->Ret = AioGetAiEventSamplingTimes ( ctx->ad.Id[i] , &ctx->ad.SamplingTimes[i] );
        }
        ctx->sampling.SavingTime = 300;
        ctx->sampling.TotalSamplingTimes = long(ctx->sampling.SavingTime*1000000/ctx->ad.SamplingClock[0]);
        ctx->sampling.AllocatedMemory = 4*ctx->AdMaxChannels* ctx->sampling.TotalSamplingTimes/1024.0f/1024.0f;
        ctx->sampling.AvSmplNum = 20;
        for(i = 0;i<ctx->NumDA;i++){
            ctx->Ret = AioGetAoResolution ( ctx->da.Id[i] , &ctx->da.Resolution[i] );
            ctx->Ret = AioGetAoMaxChannels ( ctx->da.Id[i] , &ctx->da.Channels[i] );
            ctx->Ret = AioSetAoRangeAll ( ctx->da.Id[i] , 50 );
            // 0 - 10V
            ctx->Ret = AioGetAoRange ( ctx->da.Id[i] , 0 , &ctx->da.Range[i] );
            ctx->Ret = GetRangeValue(ctx->da.Range[i], &ctx->da.RangeMax[i], &ctx->da.RangeMin[i]);
        }
        ctx->FlagSetBoard = TRUE;
    }
    return;
}

void CDigitShowBasicDoc::CloseBoard()
{
    DigitShowContext* ctx = GetContext();
    // Close A/D and D/A board to end the application 
    if( ctx->FlagSetBoard==TRUE ){
        if(ctx->NumAD > 0)    ctx->Ret = AioExit(ctx->ad.Id[0]);
        if(ctx->NumAD > 1)    ctx->Ret = AioExit(ctx->ad.Id[1]);
        if(ctx->NumDA > 0)    ctx->Ret = AioExit(ctx->da.Id[0]);
    }
}

//--- Input from A/D Board ---
void CDigitShowBasicDoc::AD_INPUT()
{
    DigitShowContext* ctx = GetContext();
    int    i,j,k;
    k = 0;
    if(ctx->NumAD>0){
        for(i = 0;i<ctx->ad.Channels[0]/2;i++){
            ctx->Vout[k] = 0.0f;
            for(j = 0;j<ctx->sampling.AvSmplNum;j++){
                ctx->Vout[k] = ctx->Vout[k]+BinaryToVolt(ctx->ad.RangeMax[0], ctx->ad.RangeMin[0], ctx->ad.Resolution[0], ctx->ad.Data0[ctx->ad.Channels[0]*j+2*i])/float(ctx->sampling.AvSmplNum);
            }
            k = k+1;
        }
    }
    if(ctx->NumAD>1){
        for(i = 0;i<ctx->ad.Channels[1]/2;i++){
            ctx->Vout[k] = 0.0f;
            for(j = 0;j<ctx->sampling.AvSmplNum;j++){
                ctx->Vout[k] = ctx->Vout[k]+BinaryToVolt(ctx->ad.RangeMax[1], ctx->ad.RangeMin[1], ctx->ad.Resolution[1], ctx->ad.Data1[ctx->ad.Channels[1]*j+2*i])/float(ctx->sampling.AvSmplNum);
            }
            k = k+1;
        }
    }
}

//--- Output to D/A Board ---
void CDigitShowBasicDoc::DA_OUTPUT()
{
    DigitShowContext* ctx = GetContext();
    int    i,j,k;
    k = 0;
    for(i = 0;i<ctx->NumDA;i++){
        for(j = 0;j<ctx->da.Channels[i];j++){
            if(ctx->DAVout[k]<0.0f)    ctx->DAVout[k] = 0.0f;
            if(ctx->DAVout[k]>9.9999f) ctx->DAVout[k] = 9.9999f;
            ctx->da.Data[j] = VoltToBinary(ctx->da.RangeMax[i], ctx->da.RangeMin[i], ctx->da.Resolution[i], ctx->DAVout[k]);
            k = k+1;
        }
        ctx->Ret = AioMultiAo(ctx->da.Id[i], ctx->da.Channels[i], &ctx->da.Data[0]);
    }
}

//--- Calcuration of Physical Value ---
void CDigitShowBasicDoc::Cal_Physical()
{
    DigitShowContext* ctx = GetContext();
    int    i;
    for(i = 0;i<64;i++){
        ctx->Phyout[i] =    ctx->cal.a[i]* ctx->Vout[i]* ctx->Vout[i] + ctx->cal.b[i]* ctx->Vout[i] + ctx->cal.c[i];
    }
}

//--- Calcuration of the Other Parameters ---
void CDigitShowBasicDoc::Cal_Param()
{
    DigitShowContext* ctx = GetContext();
    auto SpecimenData = &ctx->specimen;
    //    Specimen Data in drain and undrain condition
    ctx->height = SpecimenData->Height[0]-ctx->Phyout[1];
    // Current height
    ctx->volume = SpecimenData->Volume[0]- ctx->Phyout[4];
    // Current volume in drain condition
    ctx->area = ctx->volume/ ctx->height;
    // Current area
    ctx->phys.ea = -log(ctx->height/SpecimenData->Height[0])*100.0;
    // True Axial Strain (%)
    ctx->phys.ev = -log(ctx->volume/SpecimenData->Volume[0])*100.0;
    // True Volumetric Strain in drain condition (%)
    ctx->phys.er = (ctx->phys.ev- ctx->phys.ea)/2.0;
    // True Radial strain (%)
    if(SpecimenData->VLDT1[0]>0.0 && ctx->Phyout[5]>0.0) {
        ctx->phys.eLDT1 = -log(ctx->Phyout[5]/SpecimenData->VLDT1[0])*100.0;
        // True LDT Strain (%)
    }
    else{
        ctx->phys.eLDT1 = 0.0;
    }
    if(SpecimenData->VLDT2[0]>0.0 && ctx->Phyout[6]>0.0) {
        ctx->phys.eLDT2 = -log(ctx->Phyout[6]/SpecimenData->VLDT2[0])*100.0;
        // True LDT Strain (%)
    }
    else{
        ctx->phys.eLDT2 = 0.0;
    }
    ctx->phys.eLDT = (ctx->phys.eLDT1+ ctx->phys.eLDT2)/2.0;
    ctx->phys.q = ctx->Phyout[0]/ctx->area*1000.0;
    // Deviator Stress (kPa)
    ctx->phys.sr = ctx->Phyout[2];
    // Cell(Radial) Stress (kPa)
    ctx->phys.sa = ctx->phys.q+ ctx->phys.sr;
    // Axial Stress (kPa)
    ctx->phys.p = (ctx->phys.sa+2.0* ctx->phys.sr)/3.0;
    // Mean Principal Stress (kPa)
    ctx->phys.e_sr = ctx->Phyout[3];
    // Cell Effective Stress (kPa)
    ctx->phys.e_sa = ctx->phys.q+ ctx->phys.e_sr;
    // Axial Effective Stress (kPa)
    ctx->phys.u = ctx->phys.sr- ctx->phys.e_sr;
    // Pore Pressure
    ctx->phys.e_p = (ctx->phys.e_sa+2.0* ctx->phys.e_sr)/3.0;
    // Mean Effective Stress (kPa)
    //---The Value to display---
    ctx->CalParam[0] = ctx->phys.sa;
    ctx->CalParam[1] = ctx->phys.sr;
    ctx->CalParam[2] = ctx->phys.e_sa;
    ctx->CalParam[3] = ctx->phys.e_sr;
    ctx->CalParam[4] = ctx->phys.u;
    ctx->CalParam[5] = ctx->phys.p;
    ctx->CalParam[6] = ctx->phys.q;
    ctx->CalParam[7] = ctx->phys.e_p;
    ctx->CalParam[8] = ctx->phys.ea;
    ctx->CalParam[9] = ctx->phys.er;
    ctx->CalParam[10] = ctx->phys.ev;
    ctx->CalParam[11] = ctx->phys.eLDT1;
    ctx->CalParam[12] = ctx->phys.eLDT2;
    ctx->CalParam[13] = ctx->phys.eLDT;
    ctx->CalParam[14] = (ctx->phys.e_sa+ ctx->phys.e_sr)/2.0;
    ctx->CalParam[15] = (ctx->phys.e_sa- ctx->phys.e_sr)/2.0;
}

void CDigitShowBasicDoc::SaveToFile()
{
    DigitShowContext* ctx = GetContext();
    // Save Voltage and Physical Data
    int    i,j,k;

    k = 0;
    fprintf(ctx->FileSaveData0,"%.3lf    ",ctx->SequentTime2);
    fprintf(ctx->FileSaveData1,"%.3lf    ",ctx->SequentTime2);
    for(i = 0;i<ctx->NumAD;i++){
        for(j = 0;j<ctx->ad.Channels[i]/2;j++){
            fprintf(ctx->FileSaveData0,"%lf    ",ctx->Vout[k]);
            fprintf(ctx->FileSaveData1,"%lf    ", ctx->Phyout[k]);
            k = k+1;
        }
    }
    fprintf(ctx->FileSaveData0,"\n");
    fprintf(ctx->FileSaveData1,"\n");
    // Save Parameter Data
    fprintf(ctx->FileSaveData2,"%.3lf    ",ctx->SequentTime2);    
    for(i = 0;i<16;i++){
        fprintf(ctx->FileSaveData2,"%lf    ",ctx->CalParam[i]);
    }
    fprintf(ctx->FileSaveData2,"\n");
}

void CDigitShowBasicDoc::SaveToFile2()
{
    DigitShowContext* ctx = GetContext();
    int    i,j,k;
    for(i = 0;i<ctx->sampling.CurrentSamplingTimes;i++){
        k = 0;
        fprintf(ctx->FileSaveData0,"%.3lf    ",ctx->sampling.SavingClock/1000000.0*i);
        fprintf(ctx->FileSaveData1,"%.3lf    ",ctx->sampling.SavingClock/1000000.0*i);
        if(ctx->NumAD>0){
            for(j = 0;j<ctx->ad.Channels[0]/2;j++){
                ctx->Vtmp = BinaryToVolt(ctx->ad.RangeMax[0], ctx->ad.RangeMin[0], ctx->ad.Resolution[0], *((PLONG)ctx->pSmplData0+i*ctx->ad.Channels[0]/2+j));
                ctx->Ptmp = ctx->cal.a[k]*ctx->Vtmp*ctx->Vtmp+ctx->cal.b[k]*ctx->Vtmp+ctx->cal.c[k];
                k = k+1;
                fprintf(ctx->FileSaveData0,"%lf    ",ctx->Vtmp);
                fprintf(ctx->FileSaveData1,"%lf    ",ctx->Ptmp);
            }
        }
        if(ctx->NumAD>1){
            for(j = 0;j<ctx->ad.Channels[1]/2;j++){
                ctx->Vtmp = BinaryToVolt(ctx->ad.RangeMax[1], ctx->ad.RangeMin[1], ctx->ad.Resolution[1], *((PLONG)ctx->pSmplData1+i*ctx->ad.Channels[1]/2+j));
                ctx->Ptmp = ctx->cal.a[k]*ctx->Vtmp*ctx->Vtmp+ctx->cal.b[k]*ctx->Vtmp+ctx->cal.c[k];
                k = k+1;
                fprintf(ctx->FileSaveData0,"%lf    ",ctx->Vtmp);
                fprintf(ctx->FileSaveData1,"%lf    ",ctx->Ptmp);
            }
        }
        fprintf(ctx->FileSaveData0,"\n");
        fprintf(ctx->FileSaveData1,"\n");
    }
}

void CDigitShowBasicDoc::Allocate_Memory()
{
    DigitShowContext* ctx = GetContext();
    if(ctx->FlagSaveData==TRUE){
        if(ctx->NumAD>0){
            ctx->hHeap0 = GetProcessHeap();
            ctx->pSmplData0 = HeapAlloc(ctx->hHeap0,HEAP_ZERO_MEMORY,unsigned long(ctx->sampling.TotalSamplingTimes*ctx->ad.Channels[0]/2*sizeof(LONG)));
        }
        if(ctx->NumAD>1){
            ctx->hHeap1 = GetProcessHeap();
            ctx->pSmplData1 = HeapAlloc(ctx->hHeap1,HEAP_ZERO_MEMORY,unsigned long(ctx->sampling.TotalSamplingTimes*ctx->ad.Channels[1]/2*sizeof(LONG)));
        }
    }
    if(ctx->FlagSaveData==FALSE){
        if(ctx->NumAD>0)    HeapFree(ctx->hHeap0,0,ctx->pSmplData0);
        if(ctx->NumAD>1)    HeapFree(ctx->hHeap1,0,ctx->pSmplData1);
    }
}

//--- Control Statements ---
void CDigitShowBasicDoc::Control_DA()
{
    DigitShowContext* ctx = GetContext();
    auto ControlData = ctx->control;

    switch (ctx->ControlID)
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
            ctx->DAVout[ctx->daChannel.Motor] = 5.0f;
            // Motor: On
            if(ctx->phys.q > ctx->errTol.StressCom ){
                ctx->DAVout[ctx->daChannel.MotorCruch] = 5.0f;
                // Cruch: Up
                if( ctx->phys.q > ControlData[1].q ){
                    ctx->DAVout[ctx->daChannel.MotorSpeed] = float(ctx->cal.DA_a[ctx->daChannel.MotorSpeed]*ControlData[1].MotorSpeed+ctx->cal.DA_b[ctx->daChannel.MotorSpeed]);
                }
                if( ctx->phys.q <= ControlData[1].q ){
                    ctx->DAVout[ctx->daChannel.MotorSpeed] = float(ctx->cal.DA_a[ctx->daChannel.MotorSpeed]*(ctx->phys.q/ControlData[1].q)*ControlData[1].MotorSpeed+ctx->cal.DA_b[ctx->daChannel.MotorSpeed]);
                }
            }
            else if( ctx->phys.q < ctx->errTol.StressExt ){
                ctx->DAVout[ctx->daChannel.MotorCruch] = 0.0f;
                // Cruch: Down
                if( ctx->phys.q < -ControlData[1].q ){
                    ctx->DAVout[ctx->daChannel.MotorSpeed] = float(ctx->cal.DA_a[ctx->daChannel.MotorSpeed]*ControlData[1].MotorSpeed+ctx->cal.DA_b[ctx->daChannel.MotorSpeed]);
                }
                if( ctx->phys.q >= -ControlData[1].q ){
                    ctx->DAVout[ctx->daChannel.MotorSpeed] = float(ctx->cal.DA_a[ctx->daChannel.MotorSpeed]*(-ctx->phys.q/ControlData[1].q)*ControlData[1].MotorSpeed+ctx->cal.DA_b[ctx->daChannel.MotorSpeed]);
                }
            }
            else {
                    ctx->DAVout[ctx->daChannel.MotorSpeed] = 0.0f;
                    // RPM->0
            }
            DA_OUTPUT();
        }
        break;
    case 2:
        { 
        // Consolidation (Motor Control):
        // ControlData[2].e_sigma[0]:    Target Axial Effectve Stress,
        // ControlData[2].K0:            K0 value,
        // ControlData[2].sigmaRate[2]:    Increase Rate of Cell Pressure 
        // ControlData[2].MotorSpeed:    Motor Speed
            ctx->DAVout[ctx->daChannel.Motor] = 5.0f;
            // Motor: On
            ctx->DAVout[ctx->daChannel.MotorSpeed] = float(ctx->cal.DA_a[ctx->daChannel.MotorSpeed]*ControlData[2].MotorSpeed+ctx->cal.DA_b[ctx->daChannel.MotorSpeed]);
            if( ctx->phys.e_sr < ControlData[2].e_sigma[0]*ControlData[2].K0-ctx->errTol.StressA){
                ctx->DAVout[ctx->daChannel.EP_Cell] = ctx->DAVout[ctx->daChannel.EP_Cell]+float(ctx->cal.DA_a[ctx->daChannel.EP_Cell]*ControlData[2].sigmaRate[2]/60.0*ctx->timeSettings.Interval2/1000.0);
            }    
            if( ctx->phys.e_sr > ControlData[2].e_sigma[0]*ControlData[2].K0+ctx->errTol.StressA){
                ctx->DAVout[ctx->daChannel.EP_Cell] = ctx->DAVout[ctx->daChannel.EP_Cell]-float(ctx->cal.DA_a[ctx->daChannel.EP_Cell]*ControlData[2].sigmaRate[2]/60.0*ctx->timeSettings.Interval2/1000.0);
            }    
            if( ctx->phys.e_sa < ctx->phys.e_sr/ControlData[2].K0+ctx->errTol.StressExt ){
                ctx->DAVout[ctx->daChannel.MotorCruch] = 0.0f;
                // Cruch: Down
            }            
            else if( ctx->phys.e_sa > ctx->phys.e_sr/ControlData[2].K0+ctx->errTol.StressCom ){
                ctx->DAVout[ctx->daChannel.MotorCruch] = 5.0f;
                // Cruch: Up
            }
            else {
                ctx->DAVout[ctx->daChannel.MotorSpeed] = 0.0f;
                // RPM->0
            }
            DA_OUTPUT();
        }
        break;
    case 3:
        { 
        // Monotonic Loading (Motor Control)
        // ControlData[3].MotorSpeed:    Motor Speed
        // ControlData[3].MotorCruch:    Compression:1 /Extension:0                        
        // ControlData[3].flag[0]:        Monotonic_Loading:0 /Creep:1
        // ControlData[3].sigma[0];        Limiter
            ctx->DAVout[ctx->daChannel.Motor] = 5.0f;
            // Motor: On
            ctx->DAVout[ctx->daChannel.MotorSpeed] = float(ctx->cal.DA_a[ctx->daChannel.MotorSpeed]*ControlData[3].MotorSpeed+ctx->cal.DA_b[ctx->daChannel.MotorSpeed]);
            if(ControlData[3].flag[0]==FALSE){        // Monotonic Loading
                if(ControlData[3].MotorCruch == 0 ){
                    ctx->DAVout[ctx->daChannel.MotorCruch] = 0.0f;
                    // Cruch: Down
                    if( ctx->phys.q >= ControlData[3].q) ControlData[3].flag[0] = TRUE;
                }
                if(ControlData[3].MotorCruch == 1 )    {
                    ctx->DAVout[ctx->daChannel.MotorCruch] = 5.0f;
                    // Cruch: Up
                    if( ctx->phys.q <= ControlData[3].q) ControlData[3].flag[0] = TRUE;
                }
            }
            if(ControlData[3].flag[0]==TRUE){        // Creep
                if(ControlData[3].MotorCruch == 0 ){
                    ctx->DAVout[ctx->daChannel.MotorCruch] = 0.0f;
                    // Cruch: Down
                    if( ctx->phys.q>=ControlData[3].q+ctx->errTol.StressExt)    ctx->DAVout[ctx->daChannel.MotorSpeed] = 0.0f;
                    // RPM->0
                }
                if(ControlData[3].MotorCruch == 1 ){
                    ctx->DAVout[ctx->daChannel.MotorCruch] = 5.0f;
                    // Cruch: Up
                    if( ctx->phys.q<=ControlData[3].q+ctx->errTol.StressCom)    ctx->DAVout[ctx->daChannel.MotorSpeed] = 0.0f;
                    // RPM->0
                }
            }
            DA_OUTPUT();
        }
        break;
    case 4:
        { 
        // Monotonic Loading (Motor Control)
        // ControlData[4].MotorSpeed:    Motor Speed
        // ControlData[4].MotorCruch:    Cruch Loading:1 /Unloading:0                        
        // ControlData[4].flag:            Loading:0 /Creep:1
        // ControlData[4].sigma[0];        Limiter
            ctx->DAVout[ctx->daChannel.MotorSpeed] = float(ctx->cal.DA_a[ctx->daChannel.MotorSpeed]*ControlData[4].MotorSpeed+ctx->cal.DA_b[ctx->daChannel.MotorSpeed]);
            ctx->DAVout[ctx->daChannel.Motor] = 5.0f;
            // Motor:On
            if(ControlData[4].flag[0]==FALSE){        // Monotonic Loading
                if(ControlData[4].MotorCruch == 0 ){
                    ctx->DAVout[ctx->daChannel.MotorCruch] = 0.0f;
                    // Cruch:Down
                    if( ctx->phys.q >= ControlData[4].q) ControlData[4].flag[0] = TRUE;
                }
                if(ControlData[4].MotorCruch == 1 )    {
                    ctx->DAVout[ctx->daChannel.MotorCruch] = 5.0f;
                    // Cruch:Up
                    if( ctx->phys.q <= ControlData[4].q) ControlData[4].flag[0] = TRUE;
                }
            }
            if(ControlData[4].flag[0]==TRUE){        // Creep
                if(ControlData[4].MotorCruch == 0 ){
                    ctx->DAVout[ctx->daChannel.MotorCruch] = 0.0f;
                    // Cruch:Down
                    if( ctx->phys.q>=ControlData[4].q+ctx->errTol.StressExt)    ctx->DAVout[ctx->daChannel.MotorSpeed] = 0.0f;
                    // RPM->0
                }
                if(ControlData[4].MotorCruch == 1 ){
                    ctx->DAVout[ctx->daChannel.MotorCruch] = 5.0f;
                    // Cruch:Up
                    if( ctx->phys.q<=ControlData[4].q+ctx->errTol.StressCom)    ctx->DAVout[ctx->daChannel.MotorSpeed] = 0.0f;
                    // RPM->0
                }
            }
            DA_OUTPUT();
        }
        break;
    case 5:
        { 
            // Cyclic Loading
            ctx->DAVout[ctx->daChannel.MotorSpeed] = float(ctx->cal.DA_a[ctx->daChannel.MotorSpeed]*ControlData[5].MotorSpeed+ctx->cal.DA_b[ctx->daChannel.MotorSpeed]);
            ctx->DAVout[ctx->daChannel.Motor] = 5.0f;
            // Motor:On
            if(ControlData[5].flag[0]==FALSE){            // Cyclic in compression test
                if(ControlData[5].time[0]<ControlData[5].time[1]){ 
                    ctx->DAVout[ctx->daChannel.MotorCruch] = 0.0f;
                    // Cruch:Down
                    if( ctx->phys.q>=ControlData[5].sigma[1]) {
                        ControlData[5].time[0] = ControlData[5].time[1];
                        ctx->FlagCyclic = FALSE;
                    }
                }
                if(ControlData[5].time[1]<=ControlData[5].time[0] || ControlData[5].time[0]<=ControlData[5].time[2]){
                    if(ctx->FlagCyclic==FALSE){
                        ctx->DAVout[ctx->daChannel.MotorCruch] = 5.0f;
                        // Cruch:Up
                        if( ctx->phys.q<=ControlData[5].sigma[0]) ctx->FlagCyclic = TRUE;
                    }
                    if(ctx->FlagCyclic==TRUE){
                        ctx->DAVout[ctx->daChannel.MotorCruch] = 0.0f;
                        // Cruch:Down
                        if( ctx->phys.q>=ControlData[5].sigma[1]) {
                            ctx->FlagCyclic = FALSE;
                            ControlData[5].time[0] = ControlData[5].time[0]+1;
                        }
                    }
                }
                if(ControlData[5].time[0]>ControlData[5].time[2]){ 
                    ctx->DAVout[ctx->daChannel.MotorCruch] = 0.0f;
                    // Cruch:Down
                }
            }
            if(ControlData[5].flag[0]==TRUE){
                if(ControlData[5].time[0]<ControlData[5].time[1]){ 
                    ctx->DAVout[ctx->daChannel.MotorCruch] = 5.0f;
                    // Cruch:Up
                    if( ctx->phys.q<=ControlData[5].sigma[0]) {
                        ControlData[5].time[0] = ControlData[5].time[1];
                        ctx->FlagCyclic = TRUE;
                    }
                }
                if(ControlData[5].time[1]<=ControlData[5].time[0] || ControlData[5].time[0]<=ControlData[5].time[2]){
                    if(ctx->FlagCyclic==TRUE){
                        ctx->DAVout[ctx->daChannel.MotorCruch] = 0.0f;
                        // Cruch:Down
                        if( ctx->phys.q>=ControlData[5].sigma[1]) ctx->FlagCyclic = FALSE;
                    }
                    if(ctx->FlagCyclic==FALSE){
                        ctx->DAVout[ctx->daChannel.MotorCruch] = 5.0f;
                        // Cruch:Up
                        if( ctx->phys.q<=ControlData[5].sigma[0]) {
                            ctx->FlagCyclic = TRUE;
                            ControlData[5].time[0] = ControlData[5].time[0]+1;
                        }
                    }
                }
                if(ControlData[5].time[0]>ControlData[5].time[2]){ 
                    ctx->DAVout[ctx->daChannel.MotorCruch] = 5.0f;
                    // Cruch:Up
                }
            }
            DA_OUTPUT();
        }
        break;
    case 6:
        { 
            // Drain Cyclic Loading
            ctx->DAVout[ctx->daChannel.MotorSpeed] = float(ctx->cal.DA_a[ctx->daChannel.MotorSpeed]*ControlData[6].MotorSpeed+ctx->cal.DA_b[ctx->daChannel.MotorSpeed]);
            ctx->DAVout[ctx->daChannel.Motor] = 5.0f;
            if(ControlData[6].flag[0]==FALSE){
                if(ControlData[6].time[0]<ControlData[6].time[1]){ 
                    ctx->DAVout[ctx->daChannel.MotorCruch] = 0.0f;
                    // Cruch:Down
                    if( ctx->phys.q>=ControlData[6].sigma[1]) {
                        ControlData[6].time[0] = ControlData[6].time[1];
                        ctx->FlagCyclic = FALSE;
                    }
                }
                if(ControlData[6].time[1]<=ControlData[6].time[0] || ControlData[6].time[0]<=ControlData[6].time[2]){
                    if(ctx->FlagCyclic==FALSE){
                        ctx->DAVout[ctx->daChannel.MotorCruch] = 5.0f;
                        // Cruch:Up
                        if( ctx->phys.q<=ControlData[6].sigma[0]) ctx->FlagCyclic = TRUE;
                    }
                    if(ctx->FlagCyclic==TRUE){
                        ctx->DAVout[ctx->daChannel.MotorCruch] = 0.0f;
                        // Cruch:Down
                        if( ctx->phys.q>=ControlData[6].sigma[1]) {
                            ctx->FlagCyclic = FALSE;
                            ControlData[6].time[0] = ControlData[6].time[0]+1;
                        }
                    }
                }
                if(ControlData[6].time[0]>ControlData[6].time[2]){ 
                    ctx->DAVout[ctx->daChannel.MotorCruch] = 0.0f;
                    // Cruch:Down
                }
            }
            if(ControlData[6].flag[0]==TRUE){
                if(ControlData[6].time[0]<ControlData[6].time[1]){ 
                    ctx->DAVout[ctx->daChannel.MotorCruch] = 5.0f;
                    // Cruch:Up
                    if( ctx->phys.q<=ControlData[6].sigma[0]) {
                        ControlData[6].time[0] = ControlData[6].time[1];
                        ctx->FlagCyclic = TRUE;
                    }
                }
                if(ControlData[6].time[1]<=ControlData[6].time[0] || ControlData[6].time[0]<=ControlData[6].time[2]){
                    if(ctx->FlagCyclic==TRUE){
                        ctx->DAVout[ctx->daChannel.MotorCruch] = 0.0f;
                        // Cruch:Down
                        if( ctx->phys.q>=ControlData[6].sigma[1]) ctx->FlagCyclic = FALSE;
                    }
                    if(ctx->FlagCyclic==FALSE){
                        ctx->DAVout[ctx->daChannel.MotorCruch] = 5.0f;
                        // Cruch:Up
                        if( ctx->phys.q<=ControlData[5].sigma[0]) {
                            ctx->FlagCyclic = TRUE;
                            ControlData[6].time[0] = ControlData[6].time[0]+1;
                        }
                    }
                }
                if(ControlData[6].time[0]>ControlData[6].time[2]){ 
                    ctx->DAVout[ctx->daChannel.MotorCruch] = 5.0f;
                    // Cruch:Up
                }
            }
            DA_OUTPUT();
        }
        break;
    case 7:
        { 
            ctx->DAVout[ctx->daChannel.Motor] = 5.0f;
            ctx->DAVout[ctx->daChannel.MotorSpeed] = float(ctx->cal.DA_a[ctx->daChannel.MotorSpeed]*ControlData[7].MotorSpeed+ctx->cal.DA_b[ctx->daChannel.MotorSpeed]);
            if(ControlData[7].sigma[1] == ControlData[7].e_sigma[1]){
                ctx->DAVout[ctx->daChannel.EP_Cell] = ctx->DAVout[ctx->daChannel.EP_Cell]+float(0.2*ctx->cal.DA_a[ctx->daChannel.EP_Cell]*(ControlData[7].e_sigma[1]- ctx->phys.e_sr));
                if( ctx->phys.e_sa > ControlData[7].e_sigma[0]+ctx->errTol.StressCom)        ctx->DAVout[ctx->daChannel.MotorCruch] = 5.0f;
                // Cruch:Up
                else if( ctx->phys.e_sa < ControlData[7].e_sigma[0]+ctx->errTol.StressExt)    ctx->DAVout[ctx->daChannel.MotorCruch] = 0.0f;
                // Cruch:Down
                else ctx->DAVout[ctx->daChannel.MotorSpeed] = 0.0f;
            }
            if(ControlData[7].sigma[1] < ControlData[7].e_sigma[1]){
                if( ctx->phys.e_sr >= ControlData[7].e_sigma[1]) {
                    ctx->DAVout[ctx->daChannel.EP_Cell] = ctx->DAVout[ctx->daChannel.EP_Cell]-float(0.2*ctx->cal.DA_a[ctx->daChannel.EP_Cell]*(ctx->phys.e_sr-ControlData[7].e_sigma[1]));
                }
                if( ctx->phys.e_sr < ControlData[7].e_sigma[1]) {
                    ctx->DAVout[ctx->daChannel.EP_Cell] = ctx->DAVout[ctx->daChannel.EP_Cell]+float(ctx->cal.DA_a[ctx->daChannel.EP_Cell]*fabs(ControlData[7].sigmaRate[0])/60.0*ctx->timeSettings.Interval2/1000.0);
                }
                if( ctx->phys.e_sa > (ControlData[7].e_sigma[0]-ControlData[7].sigma[0])/(ControlData[7].e_sigma[1]-ControlData[7].sigma[1])*(ctx->phys.e_sr-ControlData[7].sigma[1])+ControlData[7].sigma[0]+ctx->errTol.StressCom){
                    ctx->DAVout[ctx->daChannel.MotorCruch] = 5.0f;
                    // Cruch:Up
                }
                else if( ctx->phys.e_sa < (ControlData[7].e_sigma[0]-ControlData[7].sigma[0])/(ControlData[7].e_sigma[1]-ControlData[7].sigma[1])*(ctx->phys.e_sr-ControlData[7].sigma[1])+ControlData[7].sigma[0]+ctx->errTol.StressExt){
                    ctx->DAVout[ctx->daChannel.MotorCruch] = 0.0f;
                    // Cruch:Down
                }
                else {
                    ctx->DAVout[ctx->daChannel.MotorSpeed] = 0.0f;
                    // RPM -> 0
                }
            }
            if(ControlData[7].sigma[1] > ControlData[7].e_sigma[1]){
                if( ctx->phys.e_sr > ControlData[7].e_sigma[1]) {
                    ctx->DAVout[ctx->daChannel.EP_Cell] = ctx->DAVout[ctx->daChannel.EP_Cell]-float(ctx->cal.DA_a[ctx->daChannel.EP_Cell]*fabs(ControlData[7].sigmaRate[0])/60.0*ctx->timeSettings.Interval2/1000.0);
                }
                if( ctx->phys.e_sr <= ControlData[7].e_sigma[1]) {
                    ctx->DAVout[ctx->daChannel.EP_Cell] = ctx->DAVout[ctx->daChannel.EP_Cell]+float(0.2*ctx->cal.DA_a[ctx->daChannel.EP_Cell]*(ControlData[7].e_sigma[1]- ctx->phys.e_sr));
                }
                if( ctx->phys.e_sa > (ControlData[7].e_sigma[0]-ControlData[7].sigma[0])/(ControlData[7].e_sigma[1]-ControlData[7].sigma[1])*(ctx->phys.e_sr-ControlData[7].sigma[1])+ControlData[7].sigma[0]+ctx->errTol.StressCom){
                    ctx->DAVout[ctx->daChannel.MotorCruch] = 5.0f;
                    // Cruch:Up
                }
                else if( ctx->phys.e_sa < (ControlData[7].e_sigma[0]-ControlData[7].sigma[0])/(ControlData[7].e_sigma[1]-ControlData[7].sigma[1])*(ctx->phys.e_sr-ControlData[7].sigma[1])+ControlData[7].sigma[0]+ctx->errTol.StressExt){
                    ctx->DAVout[ctx->daChannel.MotorCruch] = 0.0f;
                    // Cruch:Down
                }
                else {
                    ctx->DAVout[ctx->daChannel.MotorSpeed] = 0.0f;
                    // RPM -> 0
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
            if( ctx->controlFile.CurrentNum >=0 && ctx->controlFile.CurrentNum < 128 ){
                if( ctx->controlFile.Num[ctx->controlFile.CurrentNum]==0 ){
                    ctx->DAVout[ctx->daChannel.Motor] = 0.0f;
                }
                if( ctx->controlFile.Num[ctx->controlFile.CurrentNum]==1 )    MLoading_Stress();
                if( ctx->controlFile.Num[ctx->controlFile.CurrentNum]==2 )    MLoading_Strain();
                if( ctx->controlFile.Num[ctx->controlFile.CurrentNum]==3 )    CLoading_Stress();
                if( ctx->controlFile.Num[ctx->controlFile.CurrentNum]==4 )    CLoading_Strain();
                if( ctx->controlFile.Num[ctx->controlFile.CurrentNum]==5 )    Creep();
                if( ctx->controlFile.Num[ctx->controlFile.CurrentNum]==6 )    LinearEffectiveStressPath();
                if( ctx->controlFile.Num[ctx->controlFile.CurrentNum]==7 )    Creep2();
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
    DigitShowContext* ctx = GetContext();
    ctx->DAVout[ctx->daChannel.MotorSpeed] = 0.0f;
    //Motor Speed->0
    DA_OUTPUT();
}

void CDigitShowBasicDoc::MLoading_Stress()
{
    DigitShowContext* ctx = GetContext();
    ctx->TotalStepTime = ctx->TotalStepTime+ctx->CtrlStepTime/60.0;
    ctx->DAVout[ctx->daChannel.Motor] = 5.0f;
    // Motor: On
    ctx->DAVout[ctx->daChannel.MotorSpeed] = float(ctx->cal.DA_a[ctx->daChannel.MotorSpeed]*ctx->controlFile.Para[ctx->controlFile.CurrentNum][1]+ctx->cal.DA_b[ctx->daChannel.MotorSpeed]);
    // Motor_Speed
    if(ctx->controlFile.Para[ctx->controlFile.CurrentNum][0]==0.0){
        if( ctx->phys.q <= ctx->controlFile.Para[ctx->controlFile.CurrentNum][2]) {
            ctx->DAVout[ctx->daChannel.MotorCruch] = 0.0f;
            // Cruch:Down
        }
        else {
            ctx->controlFile.CurrentNum = ctx->controlFile.CurrentNum+1;
            ctx->TotalStepTime = 0.0;
        }
    }
    else if(ctx->controlFile.Para[ctx->controlFile.CurrentNum][0]==1.0){
        if( ctx->phys.q >= ctx->controlFile.Para[ctx->controlFile.CurrentNum][2]) {
            ctx->DAVout[ctx->daChannel.MotorCruch] = 5.0f;
            // Cruch:Up
        }
        else {
            ctx->controlFile.CurrentNum = ctx->controlFile.CurrentNum+1;
            ctx->TotalStepTime = 0.0;
        }
    }
}

void CDigitShowBasicDoc::MLoading_Strain()
{
    DigitShowContext* ctx = GetContext();
    ctx->TotalStepTime = ctx->TotalStepTime+ctx->CtrlStepTime/60.0;
    ctx->DAVout[ctx->daChannel.Motor] = 5.0f;
    // Motor: On
    ctx->DAVout[ctx->daChannel.MotorSpeed] = float(ctx->cal.DA_a[ctx->daChannel.MotorSpeed]*ctx->controlFile.Para[ctx->controlFile.CurrentNum][1]+ctx->cal.DA_b[ctx->daChannel.MotorSpeed]);
    // Motor_Speed
    if(ctx->controlFile.Para[ctx->controlFile.CurrentNum][0]==0.0){
        if(ctx->phys.ea <= ctx->controlFile.Para[ctx->controlFile.CurrentNum][2]) {
            ctx->DAVout[ctx->daChannel.MotorCruch] = 0.0f;
            // Cruch:Down
        }
        else {
            ctx->controlFile.CurrentNum = ctx->controlFile.CurrentNum+1;
            ctx->TotalStepTime = 0.0;
        }
    }
    else if(ctx->controlFile.Para[ctx->controlFile.CurrentNum][0]==1.0){
        if(ctx->phys.ea >= ctx->controlFile.Para[ctx->controlFile.CurrentNum][2]) {
            ctx->DAVout[ctx->daChannel.MotorCruch] = 5.0f;
            // Cruch:Up
        }
        else {
            ctx->controlFile.CurrentNum = ctx->controlFile.CurrentNum+1;
            ctx->TotalStepTime = 0.0;
        }
    }
}

void CDigitShowBasicDoc::CLoading_Stress()
{
    DigitShowContext* ctx = GetContext();
    ctx->TotalStepTime = ctx->TotalStepTime+ctx->CtrlStepTime/60.0;
    ctx->DAVout[ctx->daChannel.Motor] = 5.0f;
    // Motor: On
    ctx->DAVout[ctx->daChannel.MotorSpeed] = float(ctx->cal.DA_a[ctx->daChannel.MotorSpeed]*ctx->controlFile.Para[ctx->controlFile.CurrentNum][1]+ctx->cal.DA_b[ctx->daChannel.MotorSpeed]);
    // Motor_Speed
    if(ctx->controlFile.Para[ctx->controlFile.CurrentNum][0]==0.0){
        if(ctx->NumCyclic==0){
            ctx->FlagCyclic = FALSE;
            ctx->NumCyclic = 1;
        }
        if(ctx->NumCyclic!=0 && ctx->NumCyclic <= ctx->controlFile.Para[ctx->controlFile.CurrentNum][4]){
            if(ctx->FlagCyclic==FALSE){
                ctx->DAVout[ctx->daChannel.MotorCruch] = 5.0f;
                // Cruch:Up
                if( ctx->phys.q<=ctx->controlFile.Para[ctx->controlFile.CurrentNum][2]) ctx->FlagCyclic = TRUE;
            }
            if(ctx->FlagCyclic==TRUE){
                ctx->DAVout[ctx->daChannel.MotorCruch] = 0.0f;
                // Cruch:Down
                if( ctx->phys.q>=ctx->controlFile.Para[ctx->controlFile.CurrentNum][3]) {
                    ctx->FlagCyclic = FALSE;
                    ctx->NumCyclic = ctx->NumCyclic+1;
                }
            }
        }
        if(ctx->NumCyclic>ctx->controlFile.Para[ctx->controlFile.CurrentNum][4]){ 
            ctx->controlFile.CurrentNum = ctx->controlFile.CurrentNum+1;
            ctx->TotalStepTime = 0.0;
            ctx->NumCyclic = 0;
        }
    }
    else if(ctx->controlFile.Para[ctx->controlFile.CurrentNum][0]==1.0){
        if(ctx->NumCyclic==0){
            ctx->FlagCyclic = TRUE;
            ctx->NumCyclic = 1;
        }
        if(ctx->NumCyclic!=0 && ctx->NumCyclic <= ctx->controlFile.Para[ctx->controlFile.CurrentNum][4]){
            if(ctx->FlagCyclic==FALSE){
                ctx->DAVout[ctx->daChannel.MotorCruch] = 5.0f;
                // Cruch:Up
                if( ctx->phys.q<=ctx->controlFile.Para[ctx->controlFile.CurrentNum][2]) {
                    ctx->FlagCyclic = TRUE;
                    ctx->NumCyclic = ctx->NumCyclic+1;
                }
            }
            if(ctx->FlagCyclic==TRUE){
                ctx->DAVout[ctx->daChannel.MotorCruch] = 0.0f;
                // Cruch:Down
                if( ctx->phys.q>=ctx->controlFile.Para[ctx->controlFile.CurrentNum][3]) ctx->FlagCyclic = FALSE;
            }
        }
        if(ctx->NumCyclic>ctx->controlFile.Para[ctx->controlFile.CurrentNum][4]){ 
            ctx->controlFile.CurrentNum = ctx->controlFile.CurrentNum+1;
            ctx->TotalStepTime = 0.0;
            ctx->NumCyclic = 0;
        }
    }
}

void CDigitShowBasicDoc::CLoading_Strain()
{
    DigitShowContext* ctx = GetContext();
    ctx->TotalStepTime = ctx->TotalStepTime+ctx->CtrlStepTime/60.0;
    ctx->DAVout[ctx->daChannel.Motor] = 5.0f;
    ctx->DAVout[ctx->daChannel.MotorSpeed] = float(ctx->cal.DA_a[ctx->daChannel.MotorSpeed]*ctx->controlFile.Para[ctx->controlFile.CurrentNum][1]+ctx->cal.DA_b[ctx->daChannel.MotorSpeed]);
    // Motor_Speed
    if(ctx->controlFile.Para[ctx->controlFile.CurrentNum][0]==0.0){
        if(ctx->NumCyclic==0){
            ctx->FlagCyclic = FALSE;
            ctx->NumCyclic = 1;
        }
        if(ctx->NumCyclic!=0 && ctx->NumCyclic <= ctx->controlFile.Para[ctx->controlFile.CurrentNum][4]){
            if(ctx->FlagCyclic==FALSE){
                ctx->DAVout[ctx->daChannel.MotorCruch] = 5.0f;
                // Cruch:Up
                if(ctx->phys.ea<=ctx->controlFile.Para[ctx->controlFile.CurrentNum][2]) ctx->FlagCyclic = TRUE;
            }
            if(ctx->FlagCyclic==TRUE){
                ctx->DAVout[ctx->daChannel.MotorCruch] = 0.0f;
                // Cruch:Down
                if(ctx->phys.ea>=ctx->controlFile.Para[ctx->controlFile.CurrentNum][3]) {
                    ctx->FlagCyclic = FALSE;
                    ctx->NumCyclic = ctx->NumCyclic+1;
                }
            }
        }
        if(ctx->NumCyclic>ctx->controlFile.Para[ctx->controlFile.CurrentNum][4]){ 
            ctx->controlFile.CurrentNum = ctx->controlFile.CurrentNum+1;
            ctx->TotalStepTime = 0.0;
            ctx->NumCyclic = 0;
        }
    }
    else if(ctx->controlFile.Para[ctx->controlFile.CurrentNum][0]==1.0){
        if(ctx->NumCyclic==0){
            ctx->FlagCyclic = TRUE;
            ctx->NumCyclic = 1;
        }
        if(ctx->NumCyclic!=0 && ctx->NumCyclic <= ctx->controlFile.Para[ctx->controlFile.CurrentNum][4]){
            if(ctx->FlagCyclic==FALSE){
                ctx->DAVout[ctx->daChannel.MotorCruch] = 5.0f;
                // Cruch:Up
                if(ctx->phys.ea<=ctx->controlFile.Para[ctx->controlFile.CurrentNum][2]) {
                    ctx->FlagCyclic = TRUE;
                    ctx->NumCyclic = ctx->NumCyclic+1;
                }
            }
            if(ctx->FlagCyclic==TRUE){
                ctx->DAVout[ctx->daChannel.MotorCruch] = 0.0f;
                // Cruch:Down
                if(ctx->phys.ea>=ctx->controlFile.Para[ctx->controlFile.CurrentNum][3]) ctx->FlagCyclic = FALSE;
            }
        }
        if(ctx->NumCyclic>ctx->controlFile.Para[ctx->controlFile.CurrentNum][4]){ 
            ctx->controlFile.CurrentNum = ctx->controlFile.CurrentNum+1;
            ctx->TotalStepTime = 0.0;
            ctx->NumCyclic = 0;
        }
    }
}

void CDigitShowBasicDoc::Creep()
{
    DigitShowContext* ctx = GetContext();
    ctx->TotalStepTime = ctx->TotalStepTime+ctx->CtrlStepTime/60.0;
    ctx->DAVout[ctx->daChannel.Motor] = 5.0f;
    // Motor:On
    ctx->DAVout[ctx->daChannel.MotorSpeed] = float(ctx->cal.DA_a[ctx->daChannel.MotorSpeed]*ctx->controlFile.Para[ctx->controlFile.CurrentNum][0]+ctx->cal.DA_b[ctx->daChannel.MotorSpeed]);
    if( ctx->phys.q>=ctx->controlFile.Para[ctx->controlFile.CurrentNum][1]+ctx->errTol.StressCom)    {
        ctx->DAVout[ctx->daChannel.MotorCruch] = 5.0f;
        // Cruch:Up
    }
    else if( ctx->phys.q<=ctx->controlFile.Para[ctx->controlFile.CurrentNum][1]+ctx->errTol.StressExt)    {
        ctx->DAVout[ctx->daChannel.MotorCruch] = 0.0f;
        // Cruch:Down
    }        
    else {
        ctx->DAVout[ctx->daChannel.MotorSpeed] = 0.0f;
        // RPM->0
    }
    if(ctx->TotalStepTime>= ctx->controlFile.Para[ctx->controlFile.CurrentNum][2]) {
        ctx->controlFile.CurrentNum = ctx->controlFile.CurrentNum+1;
        ctx->TotalStepTime = 0.0;
    }
}

void CDigitShowBasicDoc::LinearEffectiveStressPath()
{
    DigitShowContext* ctx = GetContext();
    ctx->TotalStepTime = ctx->TotalStepTime+ctx->CtrlStepTime/60.0;
    ctx->DAVout[ctx->daChannel.Motor] = 5.0f;
    ctx->DAVout[ctx->daChannel.MotorSpeed] = float(ctx->cal.DA_a[ctx->daChannel.MotorSpeed]*ctx->controlFile.Para[ctx->controlFile.CurrentNum][4]+ctx->cal.DA_b[ctx->daChannel.MotorSpeed]);
    if(ctx->controlFile.Para[ctx->controlFile.CurrentNum][1]==ctx->controlFile.Para[ctx->controlFile.CurrentNum][3]){
        ctx->DAVout[ctx->daChannel.EP_Cell] = ctx->DAVout[ctx->daChannel.EP_Cell]+float(0.2*ctx->cal.DA_a[ctx->daChannel.EP_Cell]*(ctx->controlFile.Para[ctx->controlFile.CurrentNum][3]- ctx->phys.e_sr));
        if( ctx->phys.e_sa > ctx->controlFile.Para[ctx->controlFile.CurrentNum][2]+ctx->errTol.StressCom){
            ctx->DAVout[ctx->daChannel.MotorCruch] = 5.0f;
            // Cruch:Up
        }
        else if( ctx->phys.e_sa < ctx->controlFile.Para[ctx->controlFile.CurrentNum][2]+ctx->errTol.StressExt){
            ctx->DAVout[ctx->daChannel.MotorCruch] = 0.0f;
            // Cruch:Down
        }
        else {
            ctx->controlFile.CurrentNum = ctx->controlFile.CurrentNum+1;
            ctx->TotalStepTime = 0.0;
        }
    }
    else if(ctx->controlFile.Para[ctx->controlFile.CurrentNum][1] < ctx->controlFile.Para[ctx->controlFile.CurrentNum][3]){
        if( ctx->phys.e_sr >= ctx->controlFile.Para[ctx->controlFile.CurrentNum][3]-ctx->errTol.StressA) {
            ctx->DAVout[ctx->daChannel.EP_Cell] = ctx->DAVout[ctx->daChannel.EP_Cell]-float(0.2*ctx->cal.DA_a[ctx->daChannel.EP_Cell]*(ctx->phys.e_sr-ctx->controlFile.Para[ctx->controlFile.CurrentNum][3]));
        }
        if( ctx->phys.e_sr < ctx->controlFile.Para[ctx->controlFile.CurrentNum][3]-ctx->errTol.StressA) {
            ctx->DAVout[ctx->daChannel.EP_Cell] = ctx->DAVout[ctx->daChannel.EP_Cell]+float(ctx->cal.DA_a[ctx->daChannel.EP_Cell]*fabs(ctx->controlFile.Para[ctx->controlFile.CurrentNum][5])/60.0*ctx->timeSettings.Interval2/1000.0);
        }
        if( ctx->phys.e_sa > (ctx->controlFile.Para[ctx->controlFile.CurrentNum][2]-ctx->controlFile.Para[ctx->controlFile.CurrentNum][0])/(ctx->controlFile.Para[ctx->controlFile.CurrentNum][3]-ctx->controlFile.Para[ctx->controlFile.CurrentNum][1])*(ctx->phys.e_sr-ctx->controlFile.Para[ctx->controlFile.CurrentNum][1])+ctx->controlFile.Para[ctx->controlFile.CurrentNum][0]+ctx->errTol.StressCom){
            ctx->DAVout[ctx->daChannel.MotorCruch] = 5.0f;
            // Cruch:Up
        }
        else if( ctx->phys.e_sa < (ctx->controlFile.Para[ctx->controlFile.CurrentNum][2]-ctx->controlFile.Para[ctx->controlFile.CurrentNum][0])/(ctx->controlFile.Para[ctx->controlFile.CurrentNum][3]-ctx->controlFile.Para[ctx->controlFile.CurrentNum][1])*(ctx->phys.e_sr-ctx->controlFile.Para[ctx->controlFile.CurrentNum][1])+ctx->controlFile.Para[ctx->controlFile.CurrentNum][0]+ctx->errTol.StressExt){
            ctx->DAVout[ctx->daChannel.MotorCruch] = 0.0f;
            // Cruch:Down
        }
        else {
            ctx->DAVout[ctx->daChannel.MotorSpeed] = 0.0f;
            // RPM -> 0
            if(fabs(ctx->phys.e_sr-ctx->controlFile.Para[ctx->controlFile.CurrentNum][3]) <= ctx->errTol.StressA) {
                ctx->controlFile.CurrentNum = ctx->controlFile.CurrentNum+1;
                ctx->TotalStepTime = 0.0;
            }
        }
    }
    else if(ctx->controlFile.Para[ctx->controlFile.CurrentNum][1] > ctx->controlFile.Para[ctx->controlFile.CurrentNum][3]){
        if( ctx->phys.e_sr > ctx->controlFile.Para[ctx->controlFile.CurrentNum][3]+ctx->errTol.StressA) {
            ctx->DAVout[ctx->daChannel.EP_Cell] = ctx->DAVout[ctx->daChannel.EP_Cell]-float(ctx->cal.DA_a[ctx->daChannel.EP_Cell]*fabs(ctx->controlFile.Para[ctx->controlFile.CurrentNum][5])/60.0*ctx->timeSettings.Interval2/1000.0);
        }
        if( ctx->phys.e_sr <= ctx->controlFile.Para[ctx->controlFile.CurrentNum][3]+ctx->errTol.StressA) {
            ctx->DAVout[ctx->daChannel.EP_Cell] = ctx->DAVout[ctx->daChannel.EP_Cell]+float(0.2*ctx->cal.DA_a[ctx->daChannel.EP_Cell]*(ctx->controlFile.Para[ctx->controlFile.CurrentNum][3]- ctx->phys.e_sr));
        }
        if( ctx->phys.e_sa > (ctx->controlFile.Para[ctx->controlFile.CurrentNum][2]-ctx->controlFile.Para[ctx->controlFile.CurrentNum][0])/(ctx->controlFile.Para[ctx->controlFile.CurrentNum][3]-ctx->controlFile.Para[ctx->controlFile.CurrentNum][1])*(ctx->phys.e_sr-ctx->controlFile.Para[ctx->controlFile.CurrentNum][1])+ctx->controlFile.Para[ctx->controlFile.CurrentNum][0]+ctx->errTol.StressCom){
            ctx->DAVout[ctx->daChannel.MotorCruch] = 5.0f;
            // Cruch:Up
        }
        else if( ctx->phys.e_sa < (ctx->controlFile.Para[ctx->controlFile.CurrentNum][2]-ctx->controlFile.Para[ctx->controlFile.CurrentNum][0])/(ctx->controlFile.Para[ctx->controlFile.CurrentNum][3]-ctx->controlFile.Para[ctx->controlFile.CurrentNum][1])*(ctx->phys.e_sr-ctx->controlFile.Para[ctx->controlFile.CurrentNum][1])+ctx->controlFile.Para[ctx->controlFile.CurrentNum][0]+ctx->errTol.StressExt){
            ctx->DAVout[ctx->daChannel.MotorCruch] = 0.0f;
            // Cruch:Down
        }
        else {
            ctx->DAVout[ctx->daChannel.MotorSpeed] = 0.0f;
            // RPM -> 0
            if(fabs(ctx->phys.e_sr-ctx->controlFile.Para[ctx->controlFile.CurrentNum][3]) <= ctx->errTol.StressA){
                ctx->controlFile.CurrentNum = ctx->controlFile.CurrentNum+1;
                ctx->TotalStepTime = 0.0;
            }
        }
    }
}

void CDigitShowBasicDoc::Creep2()
{
    DigitShowContext* ctx = GetContext();
    ctx->TotalStepTime = ctx->TotalStepTime+ctx->CtrlStepTime/60.0;
    ctx->DAVout[ctx->daChannel.Motor] = 5.0f;
    // Motor:On
    ctx->DAVout[ctx->daChannel.MotorSpeed] = float(ctx->cal.DA_a[ctx->daChannel.MotorSpeed]*ctx->controlFile.Para[ctx->controlFile.CurrentNum][0]+ctx->cal.DA_b[ctx->daChannel.MotorSpeed]);
    if( ctx->phys.q <= ctx->controlFile.Para[ctx->controlFile.CurrentNum][1]+ctx->errTol.StressExt)    {
        ctx->DAVout[ctx->daChannel.MotorCruch] = 0.0f;
        // Cruch:Down
    }        
    else {
        ctx->DAVout[ctx->daChannel.MotorSpeed] = 0.0f;
        // RPM->0
    }
    if(ctx->TotalStepTime >= ctx->controlFile.Para[ctx->controlFile.CurrentNum][2]) {
        ctx->controlFile.CurrentNum = ctx->controlFile.CurrentNum+1;
        ctx->TotalStepTime = 0.0;
    }
}

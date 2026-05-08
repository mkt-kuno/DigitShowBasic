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
            ctx->Ret = AioInit ( "AIO000" , &ctx->ad.Id );
            if(ctx->Ret != 0){
                ctx->Ret2 = AioGetErrorString(ctx->Ret, ctx->ErrorString);
                 ctx->TextString.Format("AioInit = %d : %s", ctx->Ret, ctx->ErrorString);
                AfxMessageBox( ctx->TextString, MB_ICONSTOP | MB_OK );
                return;
            }
            else{
                ctx->Ret = AioResetDevice(ctx->ad.Id);
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
            ctx->Ret = AioInit("AIO003" , &ctx->da.Id );
            if(ctx->Ret != 0){
                ctx->Ret2 = AioGetErrorString(ctx->Ret, ctx->ErrorString);
                 ctx->TextString.Format("AioInit = %d : %s", ctx->Ret, ctx->ErrorString);
                AfxMessageBox( ctx->TextString, MB_ICONSTOP | MB_OK );
                return;
            }
            else{
                ctx->Ret = AioResetDevice(ctx->da.Id);
                if(ctx->Ret != 0){
                    ctx->Ret2 = AioGetErrorString(ctx->Ret, ctx->ErrorString);
                     ctx->TextString.Format("AioResetDevice = %d : %s", ctx->Ret, ctx->ErrorString);
                    AfxMessageBox( ctx->TextString, MB_ICONSTOP | MB_OK );
                    return;
                }
            }
        }
        // ── AD board (single, fixed) ──────────────────────────────
        ctx->Ret = AioGetAiInputMethod(ctx->ad.Id, &ctx->ad.InputMethod);
        ctx->Ret = AioGetAiResolution (ctx->ad.Id, &ctx->ad.Resolution);

        short physicalChannels = 0;
        ctx->Ret = AioGetAiMaxChannels(ctx->ad.Id, &physicalChannels);
        if (physicalChannels < DSP_AD_CHANNELS) {
            ctx->TextString.Format(
                "AD board has only %d channels; %d are required. Aborting.",
                (int)physicalChannels, DSP_AD_CHANNELS);
            AfxMessageBox(ctx->TextString, MB_ICONSTOP | MB_OK);
            AioExit(ctx->ad.Id);
            if (ctx->NumDA > 0) AioExit(ctx->da.Id);
            return;
        }
        ctx->ad.Channels = DSP_AD_CHANNELS;   // clamp to 16
        ctx->Ret = AioSetAiChannels(ctx->ad.Id, ctx->ad.Channels);

        ctx->Ret = AioSetAiRangeAll(ctx->ad.Id, 1);   // ±5 V
        ctx->Ret = AioGetAiRange   (ctx->ad.Id, 0, &ctx->ad.Range);
        ctx->Ret = GetRangeValue   (ctx->ad.Range, &ctx->ad.RangeMax, &ctx->ad.RangeMin);
        ctx->Ret = AioGetAiMemoryType(ctx->ad.Id, &ctx->ad.MemoryType);

        // ScanClock: floor(208.33) = 208 µs/ch → slightly above 300 sps/ch
        // Round DOWN (floor) so the board clock period is never too long.
        const float scanClock_us =
            floorf(1000000.0f / (float(DSP_FS_HZ) * float(DSP_AD_CHANNELS)));
        ctx->Ret = AioSetAiScanClock    (ctx->ad.Id, scanClock_us);
        ctx->Ret = AioGetAiScanClock    (ctx->ad.Id, &ctx->ad.ScanClock);
        ctx->Ret = AioGetAiSamplingClock(ctx->ad.Id, &ctx->ad.SamplingClock);
        ctx->Ret = AioGetAiEventSamplingTimes(ctx->ad.Id, &ctx->ad.SamplingTimes);

        ctx->sampling.SavingTime = 300;
        ctx->sampling.TotalSamplingTimes =
            long(ctx->sampling.SavingTime * 1000000L / ctx->ad.SamplingClock);
        // ── DA board (single, fixed) ──────────────────────────────
        if (ctx->NumDA > 0) {
            ctx->Ret = AioGetAoResolution (ctx->da.Id, &ctx->da.Resolution);
            short daPhys = 0;
            ctx->Ret = AioGetAoMaxChannels(ctx->da.Id, &daPhys);
            ctx->da.Channels = (daPhys > DSP_DA_CHANNELS) ? DSP_DA_CHANNELS : daPhys;
            ctx->Ret = AioSetAoRangeAll(ctx->da.Id, 50);   // 0–10 V
            ctx->Ret = AioGetAoRange   (ctx->da.Id, 0, &ctx->da.Range);
            ctx->Ret = GetRangeValue   (ctx->da.Range, &ctx->da.RangeMax, &ctx->da.RangeMin);
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
        if(ctx->NumAD > 0)    ctx->Ret = AioExit(ctx->ad.Id);
        if(ctx->NumDA > 0)    ctx->Ret = AioExit(ctx->da.Id);
    }
}

//--- Input from A/D Board (20Hz-B: cascaded MA5 × MA6 @ 300 sps) ---
void CDigitShowBasicDoc::AD_INPUT()
{
    DigitShowContext* ctx = GetContext();
    if (ctx->NumAD <= 0) return;

    const int   N1   = DSP_MA1_TAPS;          // 5
    const int   N2   = DSP_MA2_TAPS;          // 6
    const float inv1 = 1.0f / float(N1);
    const float inv2 = 1.0f / float(N2);
    const int   nCh  = ctx->ad.Channels;   // always DSP_AD_CHANNELS = 16

    const long nScans = ctx->ad.LastDataCount;
    if (nScans <= 0) return;

    DspFilter& d = ctx->ai.dsp;

    for (long scan = 0; scan < nScans; scan++) {
        for (int ch = 0; ch < nCh; ch++) {
            // Raw ADC value — 16-ch layout: Data0[scan * nCh + ch]
            float raw = BinaryToVolt(
                ctx->ad.RangeMax, ctx->ad.RangeMin,
                ctx->ad.Resolution,
                ctx->ad.Data0[nCh * scan + ch]);

            // Stage 1: MA(5) — 60 Hz notch
            int   i1   = d.ma1_idx[ch];
            float old1 = d.ma1_buf[ch][i1];
            d.ma1_sum[ch] += raw - old1;
            d.ma1_buf[ch][i1] = raw;
            d.ma1_idx[ch] = (i1 + 1 >= N1) ? 0 : i1 + 1;
            float out1 = float(d.ma1_sum[ch] * inv1);

            // Stage 2: MA(6) — 50 Hz notch
            int   i2   = d.ma2_idx[ch];
            float old2 = d.ma2_buf[ch][i2];
            d.ma2_sum[ch] += out1 - old2;
            d.ma2_buf[ch][i2] = out1;
            d.ma2_idx[ch] = (i2 + 1 >= N2) ? 0 : i2 + 1;

            // Output: latest filtered value for this channel
            ctx->ai.raw[ch] = float(d.ma2_sum[ch] * inv2);
        }
    }
}

//--- Output to D/A Board ---
void CDigitShowBasicDoc::DA_OUTPUT()
{
    DigitShowContext* ctx = GetContext();
    if (ctx->NumDA <= 0) return;

    const int nCh = ctx->da.Channels;   // clamped to DSP_DA_CHANNELS = 8
    for (int j = 0; j < nCh; j++) {
        if (ctx->ao.raw[j] < 0.0f)     ctx->ao.raw[j] = 0.0f;
        if (ctx->ao.raw[j] > 9.9999f)  ctx->ao.raw[j] = 9.9999f;
        ctx->da.Data[j] = VoltToBinary(
            ctx->da.RangeMax, ctx->da.RangeMin,
            ctx->da.Resolution, ctx->ao.raw[j]);
    }
    ctx->Ret = AioMultiAo(ctx->da.Id, nCh, &ctx->da.Data[0]);
}

//--- Calcuration of Physical Value ---
void CDigitShowBasicDoc::Cal_Physical()
{
    DigitShowContext* ctx = GetContext();
    for(int i = 0;i<AI_MAX_CHANNELS;i++){
        ctx->ai.phy[i] = ctx->ai.cal.a[i] * ctx->ai.raw[i] * ctx->ai.raw[i]
                       + ctx->ai.cal.b[i] * ctx->ai.raw[i]
                       + ctx->ai.cal.c[i];
    }
}

//--- Calcuration of the Other Parameters ---
void CDigitShowBasicDoc::Cal_Param()
{
    DigitShowContext* ctx = GetContext();
    auto SpecimenData = &ctx->specimen;
    //    Specimen Data in drain and undrain condition
    ctx->height = SpecimenData->Height[0]-ctx->ai.phy[1];
    // Current height
    ctx->volume = SpecimenData->Volume[0]- ctx->ai.phy[9];
    // Current volume in drain condition
    ctx->area = ctx->volume/ ctx->height;
    // Current area
    ctx->phys.ea = -log(ctx->height/SpecimenData->Height[0])*100.0;
    // True Axial Strain (%)
    ctx->phys.ev = -log(ctx->volume/SpecimenData->Volume[0])*100.0;
    // True Volumetric Strain in drain condition (%)
    ctx->phys.er = (ctx->phys.ev- ctx->phys.ea)/2.0;
    // True Radial strain (%)
    if(SpecimenData->VLDT1[0]>0.0 && ctx->ai.phy[2]>0.0) {
        ctx->phys.eLDT1 = -log(ctx->ai.phy[2]/SpecimenData->VLDT1[0])*100.0;
        // True LDT Strain (%)
    }
    else{
        ctx->phys.eLDT1 = 0.0;
    }
    if(SpecimenData->VLDT2[0]>0.0 && ctx->ai.phy[3]>0.0) {
        ctx->phys.eLDT2 = -log(ctx->ai.phy[3]/SpecimenData->VLDT2[0])*100.0;
        // True LDT Strain (%)
    }
    else{
        ctx->phys.eLDT2 = 0.0;
    }
    ctx->phys.eLDT = (ctx->phys.eLDT1+ ctx->phys.eLDT2)/2.0;
    ctx->phys.q = ctx->ai.phy[0]/ctx->area*1000.0;
    // Deviator Stress (kPa)
    ctx->phys.sr = ctx->ai.phy[4];
    // Cell(Radial) Stress (kPa)
    ctx->phys.sa = ctx->phys.q+ ctx->phys.sr;
    // Axial Stress (kPa)
    ctx->phys.p = (ctx->phys.sa+2.0* ctx->phys.sr)/3.0;
    // Mean Principal Stress (kPa)
    ctx->phys.e_sr = ctx->ai.phy[8];
    // Cell Effective Stress (kPa)
    ctx->phys.e_sa = ctx->phys.q+ ctx->phys.e_sr;
    // Axial Effective Stress (kPa)
    ctx->phys.u = ctx->phys.sr- ctx->phys.e_sr;
    // Pore Pressure
    ctx->phys.e_p = (ctx->phys.e_sa+2.0* ctx->phys.e_sr)/3.0;
    // Mean Effective Stress (kPa)
    //---The Value to display---
    ctx->ai.param[0] = ctx->phys.sa;
    ctx->ai.param[1] = ctx->phys.sr;
    ctx->ai.param[2] = ctx->phys.e_sa;
    ctx->ai.param[3] = ctx->phys.e_sr;
    ctx->ai.param[4] = ctx->phys.u;
    ctx->ai.param[5] = ctx->phys.p;
    ctx->ai.param[6] = ctx->phys.q;
    ctx->ai.param[7] = ctx->phys.e_p;
    ctx->ai.param[8] = ctx->phys.ea;
    ctx->ai.param[9] = ctx->phys.er;
    ctx->ai.param[10] = ctx->phys.ev;
    ctx->ai.param[11] = ctx->phys.eLDT1;
    ctx->ai.param[12] = ctx->phys.eLDT2;
    ctx->ai.param[13] = ctx->phys.eLDT;
    ctx->ai.param[14] = (ctx->phys.e_sa+ ctx->phys.e_sr)/2.0;
    ctx->ai.param[15] = (ctx->phys.e_sa- ctx->phys.e_sr)/2.0;
}

void CDigitShowBasicDoc::SaveToFile()
{
    DigitShowContext* ctx = GetContext();
    fprintf(ctx->FileSaveData0, "%.3lf\t", ctx->SequentTime2);
    fprintf(ctx->FileSaveData1, "%.3lf\t", ctx->SequentTime2);

    // Single AD board, all DSP_AD_CHANNELS channels
    for (int j = 0; j < ctx->ad.Channels; j++) {
        fprintf(ctx->FileSaveData0, "%lf\t", ctx->ai.raw[j]);
        fprintf(ctx->FileSaveData1, "%lf\t", ctx->ai.phy[j]);
    }
    fprintf(ctx->FileSaveData0, "\n");
    fprintf(ctx->FileSaveData1, "\n");

    fprintf(ctx->FileSaveData2, "%.3lf\t", ctx->SequentTime2);
    for (int i = 0; i < AI_MAX_CHANNELS; i++) {
        fprintf(ctx->FileSaveData2, "%lf\t", ctx->ai.param[i]);
    }
    fprintf(ctx->FileSaveData2, "\n");
}

void CDigitShowBasicDoc::SaveToFile2()
{
    DigitShowContext* ctx = GetContext();
    const int nCh = ctx->ad.Channels;   // 16

    for (long i = 0; i < ctx->sampling.CurrentSamplingTimes; i++) {
        fprintf(ctx->FileSaveData0, "%.3lf\t", ctx->sampling.SavingClock / 1000000.0 * i);
        fprintf(ctx->FileSaveData1, "%.3lf\t", ctx->sampling.SavingClock / 1000000.0 * i);
        int k = 0;
        for (int j = 0; j < nCh; j++) {
            ctx->ai.raw_temp = BinaryToVolt(
                ctx->ad.RangeMax, ctx->ad.RangeMin,
                ctx->ad.Resolution,
                ctx->ad.pData[i * nCh + j]);
            ctx->ai.phy_temp = ctx->ai.cal.a[k] * ctx->ai.raw_temp * ctx->ai.raw_temp
                             + ctx->ai.cal.b[k] * ctx->ai.raw_temp
                             + ctx->ai.cal.c[k];
            k++;
            fprintf(ctx->FileSaveData0, "%lf\t", ctx->ai.raw_temp);
            fprintf(ctx->FileSaveData1, "%lf\t", ctx->ai.phy_temp);
        }
        fprintf(ctx->FileSaveData0, "\n");
        fprintf(ctx->FileSaveData1, "\n");
    }
}

void CDigitShowBasicDoc::Allocate_Memory()
{
    DigitShowContext* ctx = GetContext();
    if (ctx->FlagSaveData == TRUE) {
        ctx->ad.pData.assign(
            (size_t)ctx->sampling.TotalSamplingTimes * ctx->ad.Channels,
            0L);
    }
    if (ctx->FlagSaveData == FALSE) {
        ctx->ad.pData = {};
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
            ctx->ao.raw[DA_CH_MOTOR] = 5.0f;
            // Motor: On
            if(ctx->phys.q > ctx->errTol.StressCom ){
                ctx->ao.raw[DA_CH_MOTOR_CLUTCH] = 5.0f;
                // Cruch: Up
                if( ctx->phys.q > ControlData[1].q ){
                    ctx->ao.raw[DA_CH_MOTOR_SPEED] = float(ctx->ao.cal.a[DA_CH_MOTOR_SPEED]*ControlData[1].MotorSpeed+ctx->ao.cal.b[DA_CH_MOTOR_SPEED]);
                }
                if( ctx->phys.q <= ControlData[1].q ){
                    ctx->ao.raw[DA_CH_MOTOR_SPEED] = float(ctx->ao.cal.a[DA_CH_MOTOR_SPEED]*(ctx->phys.q/ControlData[1].q)*ControlData[1].MotorSpeed+ctx->ao.cal.b[DA_CH_MOTOR_SPEED]);
                }
            }
            else if( ctx->phys.q < ctx->errTol.StressExt ){
                ctx->ao.raw[DA_CH_MOTOR_CLUTCH] = 0.0f;
                // Cruch: Down
                if( ctx->phys.q < -ControlData[1].q ){
                    ctx->ao.raw[DA_CH_MOTOR_SPEED] = float(ctx->ao.cal.a[DA_CH_MOTOR_SPEED]*ControlData[1].MotorSpeed+ctx->ao.cal.b[DA_CH_MOTOR_SPEED]);
                }
                if( ctx->phys.q >= -ControlData[1].q ){
                    ctx->ao.raw[DA_CH_MOTOR_SPEED] = float(ctx->ao.cal.a[DA_CH_MOTOR_SPEED]*(-ctx->phys.q/ControlData[1].q)*ControlData[1].MotorSpeed+ctx->ao.cal.b[DA_CH_MOTOR_SPEED]);
                }
            }
            else {
                    ctx->ao.raw[DA_CH_MOTOR_SPEED] = 0.0f;
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
            ctx->ao.raw[DA_CH_MOTOR] = 5.0f;
            // Motor: On
            ctx->ao.raw[DA_CH_MOTOR_SPEED] = float(ctx->ao.cal.a[DA_CH_MOTOR_SPEED]*ControlData[2].MotorSpeed+ctx->ao.cal.b[DA_CH_MOTOR_SPEED]);
            if( ctx->phys.e_sr < ControlData[2].e_sigma[0]*ControlData[2].K0-ctx->errTol.StressA){
                ctx->ao.raw[DA_CH_EP_CELL] = ctx->ao.raw[DA_CH_EP_CELL]+float(ctx->ao.cal.a[DA_CH_EP_CELL]*ControlData[2].sigmaRate[2]/60.0*ctx->timeSettings.ControlInterval/1000.0);
            }    
            if( ctx->phys.e_sr > ControlData[2].e_sigma[0]*ControlData[2].K0+ctx->errTol.StressA){
                ctx->ao.raw[DA_CH_EP_CELL] = ctx->ao.raw[DA_CH_EP_CELL]-float(ctx->ao.cal.a[DA_CH_EP_CELL]*ControlData[2].sigmaRate[2]/60.0*ctx->timeSettings.ControlInterval/1000.0);
            }    
            if( ctx->phys.e_sa < ctx->phys.e_sr/ControlData[2].K0+ctx->errTol.StressExt ){
                ctx->ao.raw[DA_CH_MOTOR_CLUTCH] = 0.0f;
                // Cruch: Down
            }            
            else if( ctx->phys.e_sa > ctx->phys.e_sr/ControlData[2].K0+ctx->errTol.StressCom ){
                ctx->ao.raw[DA_CH_MOTOR_CLUTCH] = 5.0f;
                // Cruch: Up
            }
            else {
                ctx->ao.raw[DA_CH_MOTOR_SPEED] = 0.0f;
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
            ctx->ao.raw[DA_CH_MOTOR] = 5.0f;
            // Motor: On
            ctx->ao.raw[DA_CH_MOTOR_SPEED] = float(ctx->ao.cal.a[DA_CH_MOTOR_SPEED]*ControlData[3].MotorSpeed+ctx->ao.cal.b[DA_CH_MOTOR_SPEED]);
            if(ControlData[3].flag[0]==FALSE){        // Monotonic Loading
                if(ControlData[3].MotorCruch == 0 ){
                    ctx->ao.raw[DA_CH_MOTOR_CLUTCH] = 0.0f;
                    // Cruch: Down
                    if( ctx->phys.q >= ControlData[3].q) ControlData[3].flag[0] = TRUE;
                }
                if(ControlData[3].MotorCruch == 1 )    {
                    ctx->ao.raw[DA_CH_MOTOR_CLUTCH] = 5.0f;
                    // Cruch: Up
                    if( ctx->phys.q <= ControlData[3].q) ControlData[3].flag[0] = TRUE;
                }
            }
            if(ControlData[3].flag[0]==TRUE){        // Creep
                if(ControlData[3].MotorCruch == 0 ){
                    ctx->ao.raw[DA_CH_MOTOR_CLUTCH] = 0.0f;
                    // Cruch: Down
                    if( ctx->phys.q>=ControlData[3].q+ctx->errTol.StressExt)    ctx->ao.raw[DA_CH_MOTOR_SPEED] = 0.0f;
                    // RPM->0
                }
                if(ControlData[3].MotorCruch == 1 ){
                    ctx->ao.raw[DA_CH_MOTOR_CLUTCH] = 5.0f;
                    // Cruch: Up
                    if( ctx->phys.q<=ControlData[3].q+ctx->errTol.StressCom)    ctx->ao.raw[DA_CH_MOTOR_SPEED] = 0.0f;
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
            ctx->ao.raw[DA_CH_MOTOR_SPEED] = float(ctx->ao.cal.a[DA_CH_MOTOR_SPEED]*ControlData[4].MotorSpeed+ctx->ao.cal.b[DA_CH_MOTOR_SPEED]);
            ctx->ao.raw[DA_CH_MOTOR] = 5.0f;
            // Motor:On
            if(ControlData[4].flag[0]==FALSE){        // Monotonic Loading
                if(ControlData[4].MotorCruch == 0 ){
                    ctx->ao.raw[DA_CH_MOTOR_CLUTCH] = 0.0f;
                    // Cruch:Down
                    if( ctx->phys.q >= ControlData[4].q) ControlData[4].flag[0] = TRUE;
                }
                if(ControlData[4].MotorCruch == 1 )    {
                    ctx->ao.raw[DA_CH_MOTOR_CLUTCH] = 5.0f;
                    // Cruch:Up
                    if( ctx->phys.q <= ControlData[4].q) ControlData[4].flag[0] = TRUE;
                }
            }
            if(ControlData[4].flag[0]==TRUE){        // Creep
                if(ControlData[4].MotorCruch == 0 ){
                    ctx->ao.raw[DA_CH_MOTOR_CLUTCH] = 0.0f;
                    // Cruch:Down
                    if( ctx->phys.q>=ControlData[4].q+ctx->errTol.StressExt)    ctx->ao.raw[DA_CH_MOTOR_SPEED] = 0.0f;
                    // RPM->0
                }
                if(ControlData[4].MotorCruch == 1 ){
                    ctx->ao.raw[DA_CH_MOTOR_CLUTCH] = 5.0f;
                    // Cruch:Up
                    if( ctx->phys.q<=ControlData[4].q+ctx->errTol.StressCom)    ctx->ao.raw[DA_CH_MOTOR_SPEED] = 0.0f;
                    // RPM->0
                }
            }
            DA_OUTPUT();
        }
        break;
    case 5:
        { 
            // Cyclic Loading
            ctx->ao.raw[DA_CH_MOTOR_SPEED] = float(ctx->ao.cal.a[DA_CH_MOTOR_SPEED]*ControlData[5].MotorSpeed+ctx->ao.cal.b[DA_CH_MOTOR_SPEED]);
            ctx->ao.raw[DA_CH_MOTOR] = 5.0f;
            // Motor:On
            if(ControlData[5].flag[0]==FALSE){            // Cyclic in compression test
                if(ControlData[5].time[0]<ControlData[5].time[1]){ 
                    ctx->ao.raw[DA_CH_MOTOR_CLUTCH] = 0.0f;
                    // Cruch:Down
                    if( ctx->phys.q>=ControlData[5].sigma[1]) {
                        ControlData[5].time[0] = ControlData[5].time[1];
                        ctx->FlagCyclic = FALSE;
                    }
                }
                if(ControlData[5].time[1]<=ControlData[5].time[0] || ControlData[5].time[0]<=ControlData[5].time[2]){
                    if(ctx->FlagCyclic==FALSE){
                        ctx->ao.raw[DA_CH_MOTOR_CLUTCH] = 5.0f;
                        // Cruch:Up
                        if( ctx->phys.q<=ControlData[5].sigma[0]) ctx->FlagCyclic = TRUE;
                    }
                    if(ctx->FlagCyclic==TRUE){
                        ctx->ao.raw[DA_CH_MOTOR_CLUTCH] = 0.0f;
                        // Cruch:Down
                        if( ctx->phys.q>=ControlData[5].sigma[1]) {
                            ctx->FlagCyclic = FALSE;
                            ControlData[5].time[0] = ControlData[5].time[0]+1;
                        }
                    }
                }
                if(ControlData[5].time[0]>ControlData[5].time[2]){ 
                    ctx->ao.raw[DA_CH_MOTOR_CLUTCH] = 0.0f;
                    // Cruch:Down
                }
            }
            if(ControlData[5].flag[0]==TRUE){
                if(ControlData[5].time[0]<ControlData[5].time[1]){ 
                    ctx->ao.raw[DA_CH_MOTOR_CLUTCH] = 5.0f;
                    // Cruch:Up
                    if( ctx->phys.q<=ControlData[5].sigma[0]) {
                        ControlData[5].time[0] = ControlData[5].time[1];
                        ctx->FlagCyclic = TRUE;
                    }
                }
                if(ControlData[5].time[1]<=ControlData[5].time[0] || ControlData[5].time[0]<=ControlData[5].time[2]){
                    if(ctx->FlagCyclic==TRUE){
                        ctx->ao.raw[DA_CH_MOTOR_CLUTCH] = 0.0f;
                        // Cruch:Down
                        if( ctx->phys.q>=ControlData[5].sigma[1]) ctx->FlagCyclic = FALSE;
                    }
                    if(ctx->FlagCyclic==FALSE){
                        ctx->ao.raw[DA_CH_MOTOR_CLUTCH] = 5.0f;
                        // Cruch:Up
                        if( ctx->phys.q<=ControlData[5].sigma[0]) {
                            ctx->FlagCyclic = TRUE;
                            ControlData[5].time[0] = ControlData[5].time[0]+1;
                        }
                    }
                }
                if(ControlData[5].time[0]>ControlData[5].time[2]){ 
                    ctx->ao.raw[DA_CH_MOTOR_CLUTCH] = 5.0f;
                    // Cruch:Up
                }
            }
            DA_OUTPUT();
        }
        break;
    case 6:
        { 
            // Drain Cyclic Loading
            ctx->ao.raw[DA_CH_MOTOR_SPEED] = float(ctx->ao.cal.a[DA_CH_MOTOR_SPEED]*ControlData[6].MotorSpeed+ctx->ao.cal.b[DA_CH_MOTOR_SPEED]);
            ctx->ao.raw[DA_CH_MOTOR] = 5.0f;
            if(ControlData[6].flag[0]==FALSE){
                if(ControlData[6].time[0]<ControlData[6].time[1]){ 
                    ctx->ao.raw[DA_CH_MOTOR_CLUTCH] = 0.0f;
                    // Cruch:Down
                    if( ctx->phys.q>=ControlData[6].sigma[1]) {
                        ControlData[6].time[0] = ControlData[6].time[1];
                        ctx->FlagCyclic = FALSE;
                    }
                }
                if(ControlData[6].time[1]<=ControlData[6].time[0] || ControlData[6].time[0]<=ControlData[6].time[2]){
                    if(ctx->FlagCyclic==FALSE){
                        ctx->ao.raw[DA_CH_MOTOR_CLUTCH] = 5.0f;
                        // Cruch:Up
                        if( ctx->phys.q<=ControlData[6].sigma[0]) ctx->FlagCyclic = TRUE;
                    }
                    if(ctx->FlagCyclic==TRUE){
                        ctx->ao.raw[DA_CH_MOTOR_CLUTCH] = 0.0f;
                        // Cruch:Down
                        if( ctx->phys.q>=ControlData[6].sigma[1]) {
                            ctx->FlagCyclic = FALSE;
                            ControlData[6].time[0] = ControlData[6].time[0]+1;
                        }
                    }
                }
                if(ControlData[6].time[0]>ControlData[6].time[2]){ 
                    ctx->ao.raw[DA_CH_MOTOR_CLUTCH] = 0.0f;
                    // Cruch:Down
                }
            }
            if(ControlData[6].flag[0]==TRUE){
                if(ControlData[6].time[0]<ControlData[6].time[1]){ 
                    ctx->ao.raw[DA_CH_MOTOR_CLUTCH] = 5.0f;
                    // Cruch:Up
                    if( ctx->phys.q<=ControlData[6].sigma[0]) {
                        ControlData[6].time[0] = ControlData[6].time[1];
                        ctx->FlagCyclic = TRUE;
                    }
                }
                if(ControlData[6].time[1]<=ControlData[6].time[0] || ControlData[6].time[0]<=ControlData[6].time[2]){
                    if(ctx->FlagCyclic==TRUE){
                        ctx->ao.raw[DA_CH_MOTOR_CLUTCH] = 0.0f;
                        // Cruch:Down
                        if( ctx->phys.q>=ControlData[6].sigma[1]) ctx->FlagCyclic = FALSE;
                    }
                    if(ctx->FlagCyclic==FALSE){
                        ctx->ao.raw[DA_CH_MOTOR_CLUTCH] = 5.0f;
                        // Cruch:Up
                        if( ctx->phys.q<=ControlData[6].sigma[0]) {
                            ctx->FlagCyclic = TRUE;
                            ControlData[6].time[0] = ControlData[6].time[0]+1;
                        }
                    }
                }
                if(ControlData[6].time[0]>ControlData[6].time[2]){ 
                    ctx->ao.raw[DA_CH_MOTOR_CLUTCH] = 5.0f;
                    // Cruch:Up
                }
            }
            DA_OUTPUT();
        }
        break;
    case 7:
        { 
            ctx->ao.raw[DA_CH_MOTOR] = 5.0f;
            ctx->ao.raw[DA_CH_MOTOR_SPEED] = float(ctx->ao.cal.a[DA_CH_MOTOR_SPEED]*ControlData[7].MotorSpeed+ctx->ao.cal.b[DA_CH_MOTOR_SPEED]);
            if(ControlData[7].sigma[1] == ControlData[7].e_sigma[1]){
                ctx->ao.raw[DA_CH_EP_CELL] = ctx->ao.raw[DA_CH_EP_CELL]+float(0.2*ctx->ao.cal.a[DA_CH_EP_CELL]*(ControlData[7].e_sigma[1]- ctx->phys.e_sr));
                if( ctx->phys.e_sa > ControlData[7].e_sigma[0]+ctx->errTol.StressCom)        ctx->ao.raw[DA_CH_MOTOR_CLUTCH] = 5.0f;
                // Cruch:Up
                else if( ctx->phys.e_sa < ControlData[7].e_sigma[0]+ctx->errTol.StressExt)    ctx->ao.raw[DA_CH_MOTOR_CLUTCH] = 0.0f;
                // Cruch:Down
                else ctx->ao.raw[DA_CH_MOTOR_SPEED] = 0.0f;
            }
            if(ControlData[7].sigma[1] < ControlData[7].e_sigma[1]){
                if( ctx->phys.e_sr >= ControlData[7].e_sigma[1]) {
                    ctx->ao.raw[DA_CH_EP_CELL] = ctx->ao.raw[DA_CH_EP_CELL]-float(0.2*ctx->ao.cal.a[DA_CH_EP_CELL]*(ctx->phys.e_sr-ControlData[7].e_sigma[1]));
                }
                if( ctx->phys.e_sr < ControlData[7].e_sigma[1]) {
                    ctx->ao.raw[DA_CH_EP_CELL] = ctx->ao.raw[DA_CH_EP_CELL]+float(ctx->ao.cal.a[DA_CH_EP_CELL]*fabs(ControlData[7].sigmaRate[0])/60.0*ctx->timeSettings.ControlInterval/1000.0);
                }
                if( ctx->phys.e_sa > (ControlData[7].e_sigma[0]-ControlData[7].sigma[0])/(ControlData[7].e_sigma[1]-ControlData[7].sigma[1])*(ctx->phys.e_sr-ControlData[7].sigma[1])+ControlData[7].sigma[0]+ctx->errTol.StressCom){
                    ctx->ao.raw[DA_CH_MOTOR_CLUTCH] = 5.0f;
                    // Cruch:Up
                }
                else if( ctx->phys.e_sa < (ControlData[7].e_sigma[0]-ControlData[7].sigma[0])/(ControlData[7].e_sigma[1]-ControlData[7].sigma[1])*(ctx->phys.e_sr-ControlData[7].sigma[1])+ControlData[7].sigma[0]+ctx->errTol.StressExt){
                    ctx->ao.raw[DA_CH_MOTOR_CLUTCH] = 0.0f;
                    // Cruch:Down
                }
                else {
                    ctx->ao.raw[DA_CH_MOTOR_SPEED] = 0.0f;
                    // RPM -> 0
                }
            }
            if(ControlData[7].sigma[1] > ControlData[7].e_sigma[1]){
                if( ctx->phys.e_sr > ControlData[7].e_sigma[1]) {
                    ctx->ao.raw[DA_CH_EP_CELL] = ctx->ao.raw[DA_CH_EP_CELL]-float(ctx->ao.cal.a[DA_CH_EP_CELL]*fabs(ControlData[7].sigmaRate[0])/60.0*ctx->timeSettings.ControlInterval/1000.0);
                }
                if( ctx->phys.e_sr <= ControlData[7].e_sigma[1]) {
                    ctx->ao.raw[DA_CH_EP_CELL] = ctx->ao.raw[DA_CH_EP_CELL]+float(0.2*ctx->ao.cal.a[DA_CH_EP_CELL]*(ControlData[7].e_sigma[1]- ctx->phys.e_sr));
                }
                if( ctx->phys.e_sa > (ControlData[7].e_sigma[0]-ControlData[7].sigma[0])/(ControlData[7].e_sigma[1]-ControlData[7].sigma[1])*(ctx->phys.e_sr-ControlData[7].sigma[1])+ControlData[7].sigma[0]+ctx->errTol.StressCom){
                    ctx->ao.raw[DA_CH_MOTOR_CLUTCH] = 5.0f;
                    // Cruch:Up
                }
                else if( ctx->phys.e_sa < (ControlData[7].e_sigma[0]-ControlData[7].sigma[0])/(ControlData[7].e_sigma[1]-ControlData[7].sigma[1])*(ctx->phys.e_sr-ControlData[7].sigma[1])+ControlData[7].sigma[0]+ctx->errTol.StressExt){
                    ctx->ao.raw[DA_CH_MOTOR_CLUTCH] = 0.0f;
                    // Cruch:Down
                }
                else {
                    ctx->ao.raw[DA_CH_MOTOR_SPEED] = 0.0f;
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
                    ctx->ao.raw[DA_CH_MOTOR] = 0.0f;
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
    ctx->ao.raw[DA_CH_MOTOR_SPEED] = 0.0f;
    //Motor Speed->0
    DA_OUTPUT();
}

void CDigitShowBasicDoc::MLoading_Stress()
{
    DigitShowContext* ctx = GetContext();
    ctx->TotalStepTime = ctx->TotalStepTime+ctx->CtrlStepTime/60.0;
    ctx->ao.raw[DA_CH_MOTOR] = 5.0f;
    // Motor: On
    ctx->ao.raw[DA_CH_MOTOR_SPEED] = float(ctx->ao.cal.a[DA_CH_MOTOR_SPEED]*ctx->controlFile.Para[ctx->controlFile.CurrentNum][1]+ctx->ao.cal.b[DA_CH_MOTOR_SPEED]);
    // Motor_Speed
    if(ctx->controlFile.Para[ctx->controlFile.CurrentNum][0]==0.0){
        if( ctx->phys.q <= ctx->controlFile.Para[ctx->controlFile.CurrentNum][2]) {
            ctx->ao.raw[DA_CH_MOTOR_CLUTCH] = 0.0f;
            // Cruch:Down
        }
        else {
            ctx->controlFile.CurrentNum = ctx->controlFile.CurrentNum+1;
            ctx->TotalStepTime = 0.0;
        }
    }
    else if(ctx->controlFile.Para[ctx->controlFile.CurrentNum][0]==1.0){
        if( ctx->phys.q >= ctx->controlFile.Para[ctx->controlFile.CurrentNum][2]) {
            ctx->ao.raw[DA_CH_MOTOR_CLUTCH] = 5.0f;
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
    ctx->ao.raw[DA_CH_MOTOR] = 5.0f;
    // Motor: On
    ctx->ao.raw[DA_CH_MOTOR_SPEED] = float(ctx->ao.cal.a[DA_CH_MOTOR_SPEED]*ctx->controlFile.Para[ctx->controlFile.CurrentNum][1]+ctx->ao.cal.b[DA_CH_MOTOR_SPEED]);
    // Motor_Speed
    if(ctx->controlFile.Para[ctx->controlFile.CurrentNum][0]==0.0){
        if(ctx->phys.ea <= ctx->controlFile.Para[ctx->controlFile.CurrentNum][2]) {
            ctx->ao.raw[DA_CH_MOTOR_CLUTCH] = 0.0f;
            // Cruch:Down
        }
        else {
            ctx->controlFile.CurrentNum = ctx->controlFile.CurrentNum+1;
            ctx->TotalStepTime = 0.0;
        }
    }
    else if(ctx->controlFile.Para[ctx->controlFile.CurrentNum][0]==1.0){
        if(ctx->phys.ea >= ctx->controlFile.Para[ctx->controlFile.CurrentNum][2]) {
            ctx->ao.raw[DA_CH_MOTOR_CLUTCH] = 5.0f;
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
    ctx->ao.raw[DA_CH_MOTOR] = 5.0f;
    // Motor: On
    ctx->ao.raw[DA_CH_MOTOR_SPEED] = float(ctx->ao.cal.a[DA_CH_MOTOR_SPEED]*ctx->controlFile.Para[ctx->controlFile.CurrentNum][1]+ctx->ao.cal.b[DA_CH_MOTOR_SPEED]);
    // Motor_Speed
    if(ctx->controlFile.Para[ctx->controlFile.CurrentNum][0]==0.0){
        if(ctx->NumCyclic==0){
            ctx->FlagCyclic = FALSE;
            ctx->NumCyclic = 1;
        }
        if(ctx->NumCyclic!=0 && ctx->NumCyclic <= ctx->controlFile.Para[ctx->controlFile.CurrentNum][4]){
            if(ctx->FlagCyclic==FALSE){
                ctx->ao.raw[DA_CH_MOTOR_CLUTCH] = 5.0f;
                // Cruch:Up
                if( ctx->phys.q<=ctx->controlFile.Para[ctx->controlFile.CurrentNum][2]) ctx->FlagCyclic = TRUE;
            }
            if(ctx->FlagCyclic==TRUE){
                ctx->ao.raw[DA_CH_MOTOR_CLUTCH] = 0.0f;
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
                ctx->ao.raw[DA_CH_MOTOR_CLUTCH] = 5.0f;
                // Cruch:Up
                if( ctx->phys.q<=ctx->controlFile.Para[ctx->controlFile.CurrentNum][2]) {
                    ctx->FlagCyclic = TRUE;
                    ctx->NumCyclic = ctx->NumCyclic+1;
                }
            }
            if(ctx->FlagCyclic==TRUE){
                ctx->ao.raw[DA_CH_MOTOR_CLUTCH] = 0.0f;
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
    ctx->ao.raw[DA_CH_MOTOR] = 5.0f;
    ctx->ao.raw[DA_CH_MOTOR_SPEED] = float(ctx->ao.cal.a[DA_CH_MOTOR_SPEED]*ctx->controlFile.Para[ctx->controlFile.CurrentNum][1]+ctx->ao.cal.b[DA_CH_MOTOR_SPEED]);
    // Motor_Speed
    if(ctx->controlFile.Para[ctx->controlFile.CurrentNum][0]==0.0){
        if(ctx->NumCyclic==0){
            ctx->FlagCyclic = FALSE;
            ctx->NumCyclic = 1;
        }
        if(ctx->NumCyclic!=0 && ctx->NumCyclic <= ctx->controlFile.Para[ctx->controlFile.CurrentNum][4]){
            if(ctx->FlagCyclic==FALSE){
                ctx->ao.raw[DA_CH_MOTOR_CLUTCH] = 5.0f;
                // Cruch:Up
                if(ctx->phys.ea<=ctx->controlFile.Para[ctx->controlFile.CurrentNum][2]) ctx->FlagCyclic = TRUE;
            }
            if(ctx->FlagCyclic==TRUE){
                ctx->ao.raw[DA_CH_MOTOR_CLUTCH] = 0.0f;
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
                ctx->ao.raw[DA_CH_MOTOR_CLUTCH] = 5.0f;
                // Cruch:Up
                if(ctx->phys.ea<=ctx->controlFile.Para[ctx->controlFile.CurrentNum][2]) {
                    ctx->FlagCyclic = TRUE;
                    ctx->NumCyclic = ctx->NumCyclic+1;
                }
            }
            if(ctx->FlagCyclic==TRUE){
                ctx->ao.raw[DA_CH_MOTOR_CLUTCH] = 0.0f;
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
    ctx->ao.raw[DA_CH_MOTOR] = 5.0f;
    // Motor:On
    ctx->ao.raw[DA_CH_MOTOR_SPEED] = float(ctx->ao.cal.a[DA_CH_MOTOR_SPEED]*ctx->controlFile.Para[ctx->controlFile.CurrentNum][0]+ctx->ao.cal.b[DA_CH_MOTOR_SPEED]);
    if( ctx->phys.q>=ctx->controlFile.Para[ctx->controlFile.CurrentNum][1]+ctx->errTol.StressCom)    {
        ctx->ao.raw[DA_CH_MOTOR_CLUTCH] = 5.0f;
        // Cruch:Up
    }
    else if( ctx->phys.q<=ctx->controlFile.Para[ctx->controlFile.CurrentNum][1]+ctx->errTol.StressExt)    {
        ctx->ao.raw[DA_CH_MOTOR_CLUTCH] = 0.0f;
        // Cruch:Down
    }        
    else {
        ctx->ao.raw[DA_CH_MOTOR_SPEED] = 0.0f;
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
    ctx->ao.raw[DA_CH_MOTOR] = 5.0f;
    ctx->ao.raw[DA_CH_MOTOR_SPEED] = float(ctx->ao.cal.a[DA_CH_MOTOR_SPEED]*ctx->controlFile.Para[ctx->controlFile.CurrentNum][4]+ctx->ao.cal.b[DA_CH_MOTOR_SPEED]);
    if(ctx->controlFile.Para[ctx->controlFile.CurrentNum][1]==ctx->controlFile.Para[ctx->controlFile.CurrentNum][3]){
        ctx->ao.raw[DA_CH_EP_CELL] = ctx->ao.raw[DA_CH_EP_CELL]+float(0.2*ctx->ao.cal.a[DA_CH_EP_CELL]*(ctx->controlFile.Para[ctx->controlFile.CurrentNum][3]- ctx->phys.e_sr));
        if( ctx->phys.e_sa > ctx->controlFile.Para[ctx->controlFile.CurrentNum][2]+ctx->errTol.StressCom){
            ctx->ao.raw[DA_CH_MOTOR_CLUTCH] = 5.0f;
            // Cruch:Up
        }
        else if( ctx->phys.e_sa < ctx->controlFile.Para[ctx->controlFile.CurrentNum][2]+ctx->errTol.StressExt){
            ctx->ao.raw[DA_CH_MOTOR_CLUTCH] = 0.0f;
            // Cruch:Down
        }
        else {
            ctx->controlFile.CurrentNum = ctx->controlFile.CurrentNum+1;
            ctx->TotalStepTime = 0.0;
        }
    }
    else if(ctx->controlFile.Para[ctx->controlFile.CurrentNum][1] < ctx->controlFile.Para[ctx->controlFile.CurrentNum][3]){
        if( ctx->phys.e_sr >= ctx->controlFile.Para[ctx->controlFile.CurrentNum][3]-ctx->errTol.StressA) {
            ctx->ao.raw[DA_CH_EP_CELL] = ctx->ao.raw[DA_CH_EP_CELL]-float(0.2*ctx->ao.cal.a[DA_CH_EP_CELL]*(ctx->phys.e_sr-ctx->controlFile.Para[ctx->controlFile.CurrentNum][3]));
        }
        if( ctx->phys.e_sr < ctx->controlFile.Para[ctx->controlFile.CurrentNum][3]-ctx->errTol.StressA) {
            ctx->ao.raw[DA_CH_EP_CELL] = ctx->ao.raw[DA_CH_EP_CELL]+float(ctx->ao.cal.a[DA_CH_EP_CELL]*fabs(ctx->controlFile.Para[ctx->controlFile.CurrentNum][5])/60.0*ctx->timeSettings.ControlInterval/1000.0);
        }
        if( ctx->phys.e_sa > (ctx->controlFile.Para[ctx->controlFile.CurrentNum][2]-ctx->controlFile.Para[ctx->controlFile.CurrentNum][0])/(ctx->controlFile.Para[ctx->controlFile.CurrentNum][3]-ctx->controlFile.Para[ctx->controlFile.CurrentNum][1])*(ctx->phys.e_sr-ctx->controlFile.Para[ctx->controlFile.CurrentNum][1])+ctx->controlFile.Para[ctx->controlFile.CurrentNum][0]+ctx->errTol.StressCom){
            ctx->ao.raw[DA_CH_MOTOR_CLUTCH] = 5.0f;
            // Cruch:Up
        }
        else if( ctx->phys.e_sa < (ctx->controlFile.Para[ctx->controlFile.CurrentNum][2]-ctx->controlFile.Para[ctx->controlFile.CurrentNum][0])/(ctx->controlFile.Para[ctx->controlFile.CurrentNum][3]-ctx->controlFile.Para[ctx->controlFile.CurrentNum][1])*(ctx->phys.e_sr-ctx->controlFile.Para[ctx->controlFile.CurrentNum][1])+ctx->controlFile.Para[ctx->controlFile.CurrentNum][0]+ctx->errTol.StressExt){
            ctx->ao.raw[DA_CH_MOTOR_CLUTCH] = 0.0f;
            // Cruch:Down
        }
        else {
            ctx->ao.raw[DA_CH_MOTOR_SPEED] = 0.0f;
            // RPM -> 0
            if(fabs(ctx->phys.e_sr-ctx->controlFile.Para[ctx->controlFile.CurrentNum][3]) <= ctx->errTol.StressA) {
                ctx->controlFile.CurrentNum = ctx->controlFile.CurrentNum+1;
                ctx->TotalStepTime = 0.0;
            }
        }
    }
    else if(ctx->controlFile.Para[ctx->controlFile.CurrentNum][1] > ctx->controlFile.Para[ctx->controlFile.CurrentNum][3]){
        if( ctx->phys.e_sr > ctx->controlFile.Para[ctx->controlFile.CurrentNum][3]+ctx->errTol.StressA) {
            ctx->ao.raw[DA_CH_EP_CELL] = ctx->ao.raw[DA_CH_EP_CELL]-float(ctx->ao.cal.a[DA_CH_EP_CELL]*fabs(ctx->controlFile.Para[ctx->controlFile.CurrentNum][5])/60.0*ctx->timeSettings.ControlInterval/1000.0);
        }
        if( ctx->phys.e_sr <= ctx->controlFile.Para[ctx->controlFile.CurrentNum][3]+ctx->errTol.StressA) {
            ctx->ao.raw[DA_CH_EP_CELL] = ctx->ao.raw[DA_CH_EP_CELL]+float(0.2*ctx->ao.cal.a[DA_CH_EP_CELL]*(ctx->controlFile.Para[ctx->controlFile.CurrentNum][3]- ctx->phys.e_sr));
        }
        if( ctx->phys.e_sa > (ctx->controlFile.Para[ctx->controlFile.CurrentNum][2]-ctx->controlFile.Para[ctx->controlFile.CurrentNum][0])/(ctx->controlFile.Para[ctx->controlFile.CurrentNum][3]-ctx->controlFile.Para[ctx->controlFile.CurrentNum][1])*(ctx->phys.e_sr-ctx->controlFile.Para[ctx->controlFile.CurrentNum][1])+ctx->controlFile.Para[ctx->controlFile.CurrentNum][0]+ctx->errTol.StressCom){
            ctx->ao.raw[DA_CH_MOTOR_CLUTCH] = 5.0f;
            // Cruch:Up
        }
        else if( ctx->phys.e_sa < (ctx->controlFile.Para[ctx->controlFile.CurrentNum][2]-ctx->controlFile.Para[ctx->controlFile.CurrentNum][0])/(ctx->controlFile.Para[ctx->controlFile.CurrentNum][3]-ctx->controlFile.Para[ctx->controlFile.CurrentNum][1])*(ctx->phys.e_sr-ctx->controlFile.Para[ctx->controlFile.CurrentNum][1])+ctx->controlFile.Para[ctx->controlFile.CurrentNum][0]+ctx->errTol.StressExt){
            ctx->ao.raw[DA_CH_MOTOR_CLUTCH] = 0.0f;
            // Cruch:Down
        }
        else {
            ctx->ao.raw[DA_CH_MOTOR_SPEED] = 0.0f;
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
    ctx->ao.raw[DA_CH_MOTOR] = 5.0f;
    // Motor:On
    ctx->ao.raw[DA_CH_MOTOR_SPEED] = float(ctx->ao.cal.a[DA_CH_MOTOR_SPEED]*ctx->controlFile.Para[ctx->controlFile.CurrentNum][0]+ctx->ao.cal.b[DA_CH_MOTOR_SPEED]);
    if( ctx->phys.q <= ctx->controlFile.Para[ctx->controlFile.CurrentNum][1]+ctx->errTol.StressExt)    {
        ctx->ao.raw[DA_CH_MOTOR_CLUTCH] = 0.0f;
        // Cruch:Down
    }        
    else {
        ctx->ao.raw[DA_CH_MOTOR_SPEED] = 0.0f;
        // RPM->0
    }
    if(ctx->TotalStepTime >= ctx->controlFile.Para[ctx->controlFile.CurrentNum][2]) {
        ctx->controlFile.CurrentNum = ctx->controlFile.CurrentNum+1;
        ctx->TotalStepTime = 0.0;
    }
}

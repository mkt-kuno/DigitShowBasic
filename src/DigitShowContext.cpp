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
#include "DigitShowContext.h"

// Singleton instance
static DigitShowContext g_Context;
static bool g_ContextInitialized = false;

DigitShowContext* GetContext()
{
    if (!g_ContextInitialized) {
        InitContext(&g_Context);
        g_ContextInitialized = true;
    }
    return &g_Context;
}

void InitContext(DigitShowContext* ctx)
{
    if (ctx == nullptr) return;

    // Initialize board counts (CAIO)
    ctx->NumAD = 1;
    ctx->NumDA = 0;

    // Initialize A/D board config (zero all POD fields)
    ctx->ad = {};
    
    // Initialize D/A board config
    memset(&ctx->da, 0, sizeof(ctx->da));

    ctx->ad.LastDataCount = 0;

    // Initialize digital filter state (20Hz-B: MA5 × MA6 @ 300 sps)
    memset(&ctx->ai.dsp, 0, sizeof(ctx->ai.dsp));

    // Initialize flags
    ctx->flags.SetBoard  = false;
    ctx->flags.SaveData  = false;
    ctx->flags.Cyclic    = false;
    ctx->flags.Ctrl      = false;

    // Initialize control state
    ctx->ControlID = 0;
    ctx->NumCyclic = 0;
    ctx->TotalStepTime = 0.0;
    ctx->AmpID = 0;

    // Initialize time values
    ctx->SequentTime1 = 0;
    ctx->SequentTime2 = 0.0;
    ctx->CtrlStepTime = 0.0;

    // Initialize time intervals (ms)
    ctx->timeSettings.DisplayInterval = 50;
    ctx->timeSettings.ControlInterval = 500;
    ctx->timeSettings.SaveInterval = 1000;

    // Initialize physical values
    ctx->phys.sa = 0.0;
    ctx->phys.e_sa = 0.0;
    ctx->phys.sr = 0.0;
    ctx->phys.e_sr = 0.0;
    ctx->phys.p = 0.0;
    ctx->phys.e_p = 0.0;
    ctx->phys.q = 0.0;
    ctx->phys.u = 0.0;
    ctx->phys.ea = 0.0;
    ctx->phys.er = 0.0;
    ctx->phys.ev = 0.0;
    ctx->phys.eLDT = 0.0;
    ctx->phys.eLDT1 = 0.0;
    ctx->phys.eLDT2 = 0.0;
    ctx->height = 0.0;
    ctx->volume = 0.0;
    ctx->area = 0.0;

    // Initialize file handles
    ctx->fpVoltage  = nullptr;
    ctx->fpPhysical = nullptr;
    ctx->fpParam    = nullptr;

    // Initialize error handling
    ctx->Ret = 0;
    ctx->Ret2 = 0;
    memset(ctx->ErrorString, 0, sizeof(ctx->ErrorString));
    // Note: CString TextString, CTime, CTimeSpan are default-constructed by C++ runtime

    // Initialize calibration factors (default: linear y = x)
    for (int i = 0; i < AI_MAX_CHANNELS; i++) {
        ctx->ai.raw[i] = 0.0f;
        ctx->ai.phy[i] = 0.0;
        ctx->ai.param[i] = 0.0;
    }
    for (int i = 0; i < NUM_PARAM_MAX; i++) {
        ctx->ai.cal.a[i] = 0.0;
        ctx->ai.cal.b[i] = 1.0;
        ctx->ai.cal.c[i] = 0.0;
    }

    // Initialize D/A output
    for (int i = 0; i < AO_MAX_CHANNELS; i++) {
        ctx->ao.raw[i] = 0.0f;
    }
    for (int i = 0; i < AO_MAX_CHANNELS; i++) {
        ctx->ao.cal.a[i] = 0.0;
        ctx->ao.cal.b[i] = 0.0;
    }

    // Initialize specimen data
    for (int j = 0; j < 4; j++) {
        ctx->specimen.Diameter[j] = 50.0;
        ctx->specimen.Width[j] = 0.0;
        ctx->specimen.Depth[j] = 0.0;
        ctx->specimen.Height[j] = 100.0;
        ctx->specimen.Area[j] = 1963.495408;
        ctx->specimen.Volume[j] = 196349.5408;
        ctx->specimen.Weight[j] = 0.0;
        ctx->specimen.VLDT1[j] = 70.0;
        ctx->specimen.VLDT2[j] = 70.0;
    }
    ctx->specimen.Gs = 0.0;
    ctx->specimen.MembraneModulus = 0.0;
    ctx->specimen.MembraneThickness = 0.0;
    ctx->specimen.RodArea = 0.0;
    ctx->specimen.RodWeight = 0.0;

    // Initialize control data
    for (int i = 0; i < 16; i++) {
        ctx->control[i].p = 0.0;
        ctx->control[i].q = 0.0;
        ctx->control[i].u = 0.0;
        for (int j = 0; j < 3; j++) {
            ctx->control[i].flag[j] = false;
            ctx->control[i].time[j] = 0;
            ctx->control[i].sigma[j] = 0.0;
            ctx->control[i].sigmaRate[j] = 0.0;
            ctx->control[i].sigmaAmp[j] = 0.0;
            ctx->control[i].e_sigma[j] = 0.0;
            ctx->control[i].e_sigmaRate[j] = 0.0;
            ctx->control[i].e_sigmaAmp[j] = 0.0;
            ctx->control[i].strain[j] = 0.0;
            ctx->control[i].strainRate[j] = 0.0;
            ctx->control[i].strainAmp[j] = 0.0;
        }
        ctx->control[i].K0 = 1.0;
        ctx->control[i].MotorSpeed = 0.0;
        ctx->control[i].Motor = 0;
        ctx->control[i].MotorCruch = 0;
    }

    // Initialize control file data
    ctx->controlFile.CurrentNum = 0;
    for (int i = 0; i < 128; i++) {
        ctx->controlFile.Num[i] = 0;
        for (int j = 0; j < 10; j++) {
            ctx->controlFile.Para[i][j] = 0.0;
        }
    }

    // Pre-consolidation control defaults
    ctx->control[1].MotorSpeed = 1000.0;
    ctx->control[1].q = 10.0;

    // Error tolerance defaults
    ctx->errTol.StressCom = 0.5;
    ctx->errTol.StressExt = -0.5;
    ctx->errTol.StressA = 0.1;

    // D/A calibration for motor speed (V/rpm)
    ctx->ao.cal.a[DA_CH_MOTOR_SPEED] = 0.003378059;
    ctx->ao.cal.b[DA_CH_MOTOR_SPEED] = 0.0;

    // D/A calibration for cell pressure (V/kPa)
    ctx->ao.cal.a[DA_CH_EP_CELL] = 0.003401361;
    ctx->ao.cal.b[DA_CH_EP_CELL] = 0.0;
}

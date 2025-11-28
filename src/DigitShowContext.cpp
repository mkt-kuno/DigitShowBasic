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

    // Clear all memory
    memset(ctx, 0, sizeof(DigitShowContext));

    // Initialize board counts
    ctx->NumAD = 1;
    ctx->NumDA = 0;

    // Initialize flags
    ctx->FlagSetBoard = false;
    ctx->FlagSaveData = false;
    ctx->FlagFIFO = false;
    ctx->FlagCyclic = false;

    // Initialize control state
    ctx->ControlID = 0;
    ctx->NumCyclic = 0;
    ctx->TotalStepTime = 0.0;

    // Initialize time values
    ctx->SequentTime1 = 0;
    ctx->SequentTime2 = 0.0;
    ctx->CtrlStepTime = 0.0;

    // Initialize time intervals (ms)
    ctx->timeSettings.Interval1 = 50;
    ctx->timeSettings.Interval2 = 500;
    ctx->timeSettings.Interval3 = 1000;

    // Initialize sampling settings
    ctx->sampling.SavingTime = 300;
    ctx->sampling.AvSmplNum = 20;

    // Initialize calibration factors (default: linear y = x)
    for (int i = 0; i < 64; i++) {
        ctx->Vout[i] = 0.0f;
        ctx->Phyout[i] = 0.0;
        ctx->CalParam[i] = 0.0;
        ctx->cal.a[i] = 0.0;
        ctx->cal.b[i] = 1.0;
        ctx->cal.c[i] = 0.0;
    }

    // Initialize D/A output
    for (int i = 0; i < 8; i++) {
        ctx->DAVout[i] = 0.0f;
        ctx->cal.DA_a[i] = 0.0;
        ctx->cal.DA_b[i] = 0.0;
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

    // D/A channel assignments
    ctx->daChannel.Motor = 0;
    ctx->daChannel.MotorCruch = 1;
    ctx->daChannel.MotorSpeed = 2;
    ctx->daChannel.EP_Cell = 3;

    // D/A calibration for motor speed (V/rpm)
    ctx->cal.DA_a[ctx->daChannel.MotorSpeed] = 0.003378059;
    ctx->cal.DA_b[ctx->daChannel.MotorSpeed] = 0.0;

    // D/A calibration for cell pressure (V/kPa)
    ctx->cal.DA_a[ctx->daChannel.EP_Cell] = 0.003401361;
    ctx->cal.DA_b[ctx->daChannel.EP_Cell] = 0.0;
}

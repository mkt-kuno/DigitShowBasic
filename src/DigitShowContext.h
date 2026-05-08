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

#ifndef __DIGITSHOWCONTEXT_H_INCLUDE__
#define __DIGITSHOWCONTEXT_H_INCLUDE__

#pragma once

#include <afxwin.h>

#define NUM_PARAM_MAX    16  // Number of calibration parameter sets (cal.a/b/c array size)
#define AI_MAX_CHANNELS  16  // Maximum number of analog input channels (ai_raw / ai_phy array size)
#define AO_MAX_CHANNELS   8  // Maximum number of analog output channels (ao_raw array size)

// ── Digital Filter / Board Constants ──────────────────────
#define DSP_AD_CHANNELS   16     // Number of AD channels used (hard limit)
#define DSP_DA_CHANNELS    8     // Number of DA channels used (= AO_MAX_CHANNELS)
#define DSP_FS_HZ        300     // AD sampling rate [sps/ch]
#define DSP_MA1_TAPS       5     // Stage-1 MA taps  → notch at Fs/5 = 60 Hz
#define DSP_MA2_TAPS       6     // Stage-2 MA taps  → notch at Fs/6 = 50 Hz
// Group delay = (MA1_TAPS-1 + MA2_TAPS-1) / (2*Fs) = 15 ms
// -3dB ~ 18 Hz
// ScanClock = 1e6 / (DSP_FS_HZ * DSP_AD_CHANNELS) = 208.33 µs/ch

/**
 * Specimen data structure
 */
struct SpecimenData {
    double Diameter[4];
    double Width[4];
    double Depth[4];
    double Height[4];
    double Area[4];
    double Volume[4];
    double Weight[4];
    double VLDT1[4];
    double VLDT2[4];
    double Gs;
    double MembraneModulus;
    double MembraneThickness;
    double RodArea;
    double RodWeight;
};

/**
 * Control data structure
 */
struct ControlData {
    bool   flag[3];
    int    time[3];
    double p;
    double q;
    double u;
    double sigma[3];
    double sigmaRate[3];
    double sigmaAmp[3];
    double e_sigma[3];
    double e_sigmaRate[3];
    double e_sigmaAmp[3];
    double strain[3];
    double strainRate[3];
    double strainAmp[3];
    double K0;
    double MotorSpeed;
    int    Motor;
    int    MotorCruch;
};

/**
 * Calibration data
 */
struct CalibrationData {
    double a[NUM_PARAM_MAX];
    double b[NUM_PARAM_MAX];
    double c[NUM_PARAM_MAX];
    double DA_a[8];
    double DA_b[8];
};

/**
 * Physical values
 */
struct PhysicalValues {
    double sa;
    // axial stress
    double e_sa;
    // effective axial stress
    double sr;
    // radial stress
    double e_sr;
    // effective radial stress
    double p;
    // mean stress
    double e_p;
    // effective mean stress
    double q;
    // deviator stress
    double u;
    // pore pressure
    double ea;
    // axial strain
    double er;
    // radial strain
    double ev;
    // volumetric strain
    double eLDT;
    // LDT average strain
    double eLDT1;
    // LDT1 strain
    double eLDT2;
    // LDT2 strain
};

/**
 * Control file data
 */
struct ControlFileData {
    int    CurrentNum;
    int    Num[128];
    double Para[128][10];
};

/**
 * Time settings
 */
struct TimeSettings {
    unsigned int Interval1;
    // Time interval (ms) to display output data
    unsigned int Interval2;
    // Time interval (ms) to feed back
    unsigned int Interval3;
    // Time interval (ms) to save the data
};

/**
 * D/A Channel assignments
 */
struct DaChannelAssign {
    int Motor;
    int MotorCruch;
    int MotorSpeed;
    int EP_Cell;
};

/**
 * Sampling settings
 */
struct SamplingSettings {
    float SavingClock;
    int   SavingTime;
    long  TotalSamplingTimes;
    long  CurrentSamplingTimes;
    float AllocatedMemory;   // Estimated memory usage (MB)
    // AvSmplNum removed — replaced by DSP_MA1_TAPS × DSP_MA2_TAPS cascade filter
};

/**
 * Error tolerance settings
 */
struct ErrorTolerance {
    double StressCom;
    // Compression stress tolerance (kPa)
    double StressExt;
    // Extension stress tolerance (kPa)
    double StressA;
    // General stress tolerance (kPa)
};

/**
 * Cascaded sliding-window MA filter state (20Hz-B design)
 * Stage 1: MA(DSP_MA1_TAPS) — 60 Hz notch
 * Stage 2: MA(DSP_MA2_TAPS) — 50 Hz notch
 */
struct DspFilter {
    float  ma1_buf[AI_MAX_CHANNELS][DSP_MA1_TAPS];
    double ma1_sum[AI_MAX_CHANNELS];
    int    ma1_idx[AI_MAX_CHANNELS];

    float  ma2_buf[AI_MAX_CHANNELS][DSP_MA2_TAPS];
    double ma2_sum[AI_MAX_CHANNELS];
    int    ma2_idx[AI_MAX_CHANNELS];
};

/**
 * Main application context structure
 * Singleton pattern for global state management
 */
struct DigitShowContext {
    // Board configuration
    DaChannelAssign daChannel;

    // Sampling and calibration
    SamplingSettings sampling;

    // Analog input measurement data (post-filter)
    struct {
        float  raw[AI_MAX_CHANNELS];   // filtered ADC voltages [V]
        float  raw_temp;               // scratch for FIFO save conversion
        double phy[AI_MAX_CHANNELS];   // calibrated physical values
        double phy_temp;               // scratch for FIFO save conversion
        double param[AI_MAX_CHANNELS]; // derived stress/strain params
        struct {
            double a[NUM_PARAM_MAX];   // quadratic coefficient
            double b[NUM_PARAM_MAX];   // linear coefficient
            double c[NUM_PARAM_MAX];   // offset
        } cal;
    } ai;

    // Analog output setpoints [V]
    struct {
        float  raw[AO_MAX_CHANNELS];
        struct {
            double a[AO_MAX_CHANNELS]; // DA gain
            double b[AO_MAX_CHANNELS]; // DA offset
        } cal;
    } ao;

    // Physical values
    PhysicalValues phys;
    double height;
    double volume;
    double area;

    // Specimen and control
    SpecimenData specimen;
    ControlData control[16];
    ControlFileData controlFile;
    ErrorTolerance errTol;

    // Digital filter state (20Hz-B: MA5 × MA6 @ 300 sps)
    DspFilter dsp;

    // Control state
    int  ControlID;
    int  NumCyclic;
    double TotalStepTime;

    // Amplifier calibration
    int  AmpID;

    // System flags
    bool FlagSetBoard;
    bool FlagSaveData;
    bool FlagCtrl;
    bool FlagCyclic;

    // Time management
    TimeSettings timeSettings;
    CTime StartTime;
    CTime NowTime;
    CTimeSpan SpanTime;
    CString SNowTime;
    long   SequentTime1;
    double SequentTime2;
    double CtrlStepTime;

    // File handles
    FILE* FileSaveData0;
    FILE* FileSaveData1;
    FILE* FileSaveData2;

    // Error handling
    long    Ret;
    long    Ret2;
    char    ErrorString[256];
    CString TextString;

    // CAIO board configuration (CONTEC AIO)
    int NumAD;
    int NumDA;
    struct AdBoardConfig {
        short  Id[1];
        short  Channels[1];
        short  Range[1];
        float  RangeMax[1];
        float  RangeMin[1];
        short  Resolution[1];
        short  InputMethod[1];
        short  MemoryType[1];
        float  SamplingClock[1];
        long   SamplingTimes[1];
        float  ScanClock[1];
        long   LastDataCount;        // actual scan count from last AioGetAiSamplingData
        long   Data0[262144];
        PVOID  pData;                    // heap-allocated FIFO recording buffer
    } ad;
    struct DaBoardConfig {
        short  Id[1];
        short  Channels[1];
        short  Range[1];
        float  RangeMax[1];
        float  RangeMin[1];
        short  Resolution[1];
        long   Data[8];
    } da;
    bool   FlagFIFO;
};

/**
 * Get the global context instance (singleton)
 */
DigitShowContext* GetContext();

/**
 * Initialize the context with default values
 */
void InitContext(DigitShowContext* ctx);

// Legacy type aliases for backward compatibility
typedef SpecimenData Specimen;
typedef ControlData Control;

#endif // __DIGITSHOWCONTEXT_H_INCLUDE__

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
#include <vector>

#define NUM_PARAM_MAX    16  // Number of calibration parameter sets (cal.a/b/c array size)
#define AI_MAX_CHANNELS  16  // Maximum number of analog input channels (ai_raw / ai_phy array size)
#define AO_MAX_CHANNELS   8  // Maximum number of analog output channels (ao_raw array size)

// D/A channel index assignments (fixed hardware wiring)
#define DA_CH_MOTOR         0    // Motor on/off (5V = on)
#define DA_CH_MOTOR_CLUTCH  1    // Motor clutch (5V = engaged)
#define DA_CH_MOTOR_SPEED   2    // Motor speed setpoint [V]
#define DA_CH_EP_CELL       3    // Cell pressure [V]

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
 * Physical values
 */
struct PhysicalValues {
    double sa;      // axial stress
    double e_sa;    // effective axial stress
    double sr;      // radial stress
    double e_sr;    // effective radial stress
    double p;       // mean stress
    double e_p;     // effective mean stress
    double q;       // deviator stress
    double u;       // pore pressure
    double ea;      // axial strain
    double er;      // radial strain
    double ev;      // volumetric strain
    double eLDT;    // LDT average strain
    double eLDT1;   // LDT1 strain
    double eLDT2;   // LDT2 strain
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
    unsigned int DisplayInterval;   // ms — Timer 1: AD acquire + display
    unsigned int ControlInterval;   // ms — Timer 2: control feedback
    unsigned int SaveInterval;      // ms — Timer 3: data file write
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
    // Analog input measurement data (post-filter)
    struct {
        float  raw[AI_MAX_CHANNELS];   // filtered ADC voltages [V]
        double phy[AI_MAX_CHANNELS];   // calibrated physical values
        double param[AI_MAX_CHANNELS]; // derived stress/strain params
        struct {
            double a[NUM_PARAM_MAX];   // quadratic coefficient
            double b[NUM_PARAM_MAX];   // linear coefficient
            double c[NUM_PARAM_MAX];   // offset
        } cal;
        DspFilter dsp;                 // 20Hz-B MA5×MA6 filter state
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

    // Digital filter: DspFilter dsp moved into struct ai above

    // Control state
    int  ControlID;
    int  NumCyclic;
    double TotalStepTime;

    // Amplifier calibration
    int  AmpID;

    // System flags
    struct SystemFlags {
        bool SetBoard;  // AD board (AIO000) successfully opened
        bool HasDA;     // DA board (AIO001) successfully opened
        bool SaveData;
        bool Ctrl;
        bool Cyclic;
    };
    SystemFlags flags;

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
    FILE* fpVoltage;   // raw ADC voltage log  (*_v.tsv)
    FILE* fpPhysical;  // calibrated physical values log (*.tsv)
    FILE* fpParam;     // derived parameters log (*_p.tsv)

    // CAIO board configuration (CONTEC AIO)
    struct AdBoardConfig {
        short  Id;
        short  Channels;
        short  Range;
        float  RangeMax;
        float  RangeMin;
        short  Resolution;
        short  InputMethod;
        short  MemoryType;
        float  SamplingClock;
        long   SamplingTimes;
        float  ScanClock;
        long   LastDataCount;           // actual scan count from last AioGetAiSamplingData
        std::vector<long> Data0;          // raw ADC sample buffer [SamplingTimes * Channels]
    } ad;
    struct DaBoardConfig {
        short  Id;
        short  Channels;
        short  Range;
        float  RangeMax;
        float  RangeMin;
        short  Resolution;
        long   Data[AO_MAX_CHANNELS];
    } da;
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

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
 * A/D Board configuration
 */
struct AdBoardConfig {
    short  Id[2];
    short  Channels[2];
    short  Range[2];
    float  RangeMax[2];
    float  RangeMin[2];
    short  Resolution[2];
    short  InputMethod[2];
    short  MemoryType[2];
    float  SamplingClock[2];
    long   SamplingTimes[2];
    float  ScanClock[2];
    long   Data0[262144];
    long   Data1[262144];
};

/**
 * D/A Board configuration
 */
struct DaBoardConfig {
    short  Id[1];
    short  Channels[1];
    short  Range[1];
    float  RangeMax[1];
    float  RangeMin[1];
    short  Resolution[1];
    long   Data[8];
};

/**
 * Calibration data
 */
struct CalibrationData {
    double a[64];
    double b[64];
    double c[64];
    double DA_a[8];
    double DA_b[8];
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
    unsigned int Interval1;  // Time interval (ms) to display output data
    unsigned int Interval2;  // Time interval (ms) to feed back
    unsigned int Interval3;  // Time interval (ms) to save the data
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
    float AllocatedMemory;
    int   AvSmplNum;
};

/**
 * Error tolerance settings
 */
struct ErrorTolerance {
    double StressCom;   // Compression stress tolerance (kPa)
    double StressExt;   // Extension stress tolerance (kPa)
    double StressA;     // General stress tolerance (kPa)
};

/**
 * Main application context structure
 * Singleton pattern for global state management
 */
struct DigitShowContext {
    // Board configuration
    int NumAD;
    int NumDA;
    AdBoardConfig ad;
    DaBoardConfig da;
    DaChannelAssign daChannel;
    int AdMaxChannels;

    // Sampling and calibration
    SamplingSettings sampling;
    CalibrationData cal;

    // Measurement data
    float  Vout[64];
    float  Vtmp;
    double Phyout[64];
    double Ptmp;
    double CalParam[64];
    float  DAVout[8];

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

    // Control state
    int  ControlID;
    int  NumCyclic;
    double TotalStepTime;

    // Amplifier calibration
    int  AmpID;

    // System flags
    bool FlagSetBoard;
    bool FlagSaveData;
    bool FlagFIFO;
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

    // Memory management
    PVOID  pSmplData0;
    PVOID  pSmplData1;
    HANDLE hHeap0;
    HANDLE hHeap1;

    // File handles
    FILE* FileSaveData0;
    FILE* FileSaveData1;
    FILE* FileSaveData2;

    // Error handling
    long    Ret;
    long    Ret2;
    char    ErrorString[256];
    CString TextString;

    // Event handling
    long AdEvent;
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

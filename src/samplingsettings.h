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

#ifndef __SAMPLINGSETTINGS_H_INCLUDE__
#define __SAMPLINGSETTINGS_H_INCLUDE__

#pragma once

class CSamplingSettings : public CDialog
{
public:
    CSamplingSettings(CWnd* pParent = NULL);

    enum { IDD = IDD_SamplingSettings };

    long    m_TimeInterval1;
    long    m_TimeInterval2;
    long    m_TimeInterval3;
    CString m_AllocatedMemory;
    int     m_AvSmplNum;
    int     m_Channels;
    int     m_EventSamplingTimes;
    CString m_MemoryType;
    float   m_SamplingClock;
    int     m_SavingTime;
    long    m_TotalSamplingTimes;

protected:
    virtual void DoDataExchange(CDataExchange* pDX);
    virtual void OnOK();
    virtual BOOL OnInitDialog();

    afx_msg void OnBUTTONCheck();

    DECLARE_MESSAGE_MAP()
};

#endif // __SAMPLINGSETTINGS_H_INCLUDE__

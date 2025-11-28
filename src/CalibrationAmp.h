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

#ifndef __CALIBRATIONAMP_H_INCLUDE__
#define __CALIBRATIONAMP_H_INCLUDE__

#pragma once

#include "DigitShowBasicDoc.h"

class CCalibrationAmp : public CDialog
{
public:
    CCalibrationAmp(CWnd* pParent = NULL);

    enum { IDD = IDD_CalibrationAmp };

    int   m_AmpNo;
    float m_AmpPB;
    float m_AmpVB;
    float m_AmpVO;
    float m_AmpPO;

protected:
    virtual void DoDataExchange(CDataExchange* pDX);

    afx_msg void OnBUTTONAmpBase();
    afx_msg void OnBUTTONAmpOffset();
    afx_msg void OnBUTTONAmpUpdate();

    DECLARE_MESSAGE_MAP()
};

#endif // __CALIBRATIONAMP_H_INCLUDE__

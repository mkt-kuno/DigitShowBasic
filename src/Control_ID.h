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

#ifndef __CONTROL_ID_H_INCLUDE__
#define __CONTROL_ID_H_INCLUDE__

#pragma once

class CControl_ID : public CDialog
{
public:
    CControl_ID(CWnd* pParent = NULL);

    enum { IDD = IDD_Control_ID };

    int    m_Control_ID;
    double m_esigma0;
    double m_esigma1;
    double m_esigma2;
    double m_esigmaAmp0;
    double m_esigmaAmp1;
    double m_esigmaAmp2;
    double m_esigmaRate0;
    double m_esigmaRate1;
    double m_esigmaRate2;
    int    m_flag0;
    int    m_flag1;
    int    m_flag2;
    double m_K0;
    int    m_Motor;
    int    m_MotorCruch;
    double m_MotorSpeed;
    double m_p;
    double m_sigma0;
    double m_sigma1;
    double m_sigma2;
    double m_sigmaAmp0;
    double m_sigmaAmp1;
    double m_sigmaAmp2;
    double m_sigmaRate0;
    double m_sigmaRate1;
    double m_sigmaRate2;
    double m_strain0;
    double m_strain1;
    double m_strain2;
    double m_strainAmp0;
    double m_strainAmp1;
    double m_strainAmp2;
    double m_strainRate0;
    double m_strainRate1;
    double m_strainRate2;
    int    m_time0;
    int    m_time1;
    int    m_time2;
    double m_u;
    double m_q;

protected:
    virtual void DoDataExchange(CDataExchange* pDX);

private:
    int i;

protected:
    afx_msg void OnBUTTONLoad();
    afx_msg void OnBUTTONUpdate();
    afx_msg void OnBUTTONLoadfromfile();
    afx_msg void OnBUTTONSaveFile();

    DECLARE_MESSAGE_MAP()
};

#endif // __CONTROL_ID_H_INCLUDE__

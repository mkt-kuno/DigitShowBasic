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

#ifndef __CONTROL_LINEARSTRESSPATH_H_INCLUDE__
#define __CONTROL_LINEARSTRESSPATH_H_INCLUDE__

#pragma once

class CControl_LinearStressPath : public CDialog
{
public:
    CControl_LinearStressPath(CWnd* pParent = NULL);

    enum { IDD = IDD_Control_LinearStressPathLoading };

    double m_e_sigma1;
    double m_e_sigma2;
    double m_MotorSpeed;
    double m_sigma_rate;
    double m_sigma1;
    double m_sigma2;

protected:
    virtual void DoDataExchange(CDataExchange* pDX);

    afx_msg void OnBUTTONUpdate();

    DECLARE_MESSAGE_MAP()
};

#endif // __CONTROL_LINEARSTRESSPATH_H_INCLUDE__

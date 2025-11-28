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

#ifndef __CONTROL_CLOADING_H_INCLUDE__
#define __CONTROL_CLOADING_H_INCLUDE__

#pragma once

class CControl_CLoading : public CDialog
{
public:
    CControl_CLoading(CWnd* pParent = NULL);

    enum { IDD = IDD_Control_Cloading };

    int    m_flag0;
    double m_MotorSpeed;
    double m_q_lower;
    double m_q_upper;
    int    m_time0;
    int    m_time1;
    int    m_time2;

protected:
    virtual void DoDataExchange(CDataExchange* pDX);

    afx_msg void OnBUTTONUpdate();
    afx_msg void OnBUTTONReflesh();

    DECLARE_MESSAGE_MAP()
};

#endif // __CONTROL_CLOADING_H_INCLUDE__

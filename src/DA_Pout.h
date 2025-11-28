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

#ifndef __DA_POUT_H_INCLUDE__
#define __DA_POUT_H_INCLUDE__

#pragma once

#include "DigitShowBasicDoc.h"

class CDA_Pout : public CDialog
{
public:
    CDA_Pout(CWnd* pParent = NULL);
    CDigitShowBasicDoc* pDoc2;

    enum { IDD = IDD_DA_Pout };

    double m_DACala00;
    double m_DACala01;
    double m_DACala02;
    double m_DACala03;
    double m_DACala04;
    double m_DACala05;
    double m_DACala06;
    double m_DACala07;
    double m_DACalb00;
    double m_DACalb01;
    double m_DACalb02;
    double m_DACalb03;
    double m_DACalb04;
    double m_DACalb05;
    double m_DACalb06;
    double m_DACalb07;
    double m_DAPvalue00;
    double m_DAPvalue01;
    double m_DAPvalue02;
    double m_DAPvalue03;
    double m_DAPvalue04;
    double m_DAPvalue05;
    double m_DAPvalue06;
    double m_DAPvalue07;
    float  m_DAVout00;
    float  m_DAVout01;
    float  m_DAVout02;
    float  m_DAVout03;
    float  m_DAVout04;
    float  m_DAVout05;
    float  m_DAVout06;
    float  m_DAVout07;

protected:
    virtual void DoDataExchange(CDataExchange* pDX);

    afx_msg void OnBUTTONDAOutput();
    afx_msg void OnBUTTONCalculation00();
    afx_msg void OnBUTTONCalculation01();
    afx_msg void OnBUTTONCalculation02();
    afx_msg void OnBUTTONCalculation03();
    afx_msg void OnBUTTONCalculation04();
    afx_msg void OnBUTTONCalculation05();
    afx_msg void OnBUTTONCalculation06();
    afx_msg void OnBUTTONCalculation07();

    DECLARE_MESSAGE_MAP()
};

#endif // __DA_POUT_H_INCLUDE__

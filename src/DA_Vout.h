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

#ifndef __DA_VOUT_H_INCLUDE__
#define __DA_VOUT_H_INCLUDE__

#pragma once

#include "DigitShowBasicDoc.h"

class CDA_Vout : public CDialog
{
public:
    CDA_Vout(CWnd* pParent = NULL);
    CDigitShowBasicDoc* pDoc;

    enum { IDD = IDD_DA_Vout };

    float m_DAVout01;
    float m_DAVout02;
    float m_DAVout03;
    float m_DAVout04;
    float m_DAVout05;
    float m_DAVout06;
    float m_DAVout07;
    float m_DAVout08;
    float m_DAVout09;
    float m_DAVout10;
    float m_DAVout11;
    float m_DAVout12;
    float m_DAVout13;
    float m_DAVout14;
    float m_DAVout15;
    float m_DAVout16;

protected:
    virtual void DoDataExchange(CDataExchange* pDX);

    afx_msg void OnBUTTONDAVout();
    afx_msg void OnBUTTONReflesh();

    DECLARE_MESSAGE_MAP()
};

#endif // __DA_VOUT_H_INCLUDE__

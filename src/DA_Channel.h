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

#ifndef __DA_CHANNEL_H_INCLUDE__
#define __DA_CHANNEL_H_INCLUDE__

#pragma once

class CDA_Channel : public CDialog
{
public:
    CDA_Channel(CWnd* pParent = NULL);

    enum { IDD = IDD_DA_Channel };

    double m_DA_Cala00;
    double m_DA_Cala01;
    double m_DA_Cala02;
    double m_DA_Cala03;
    double m_DA_Cala04;
    double m_DA_Cala05;
    double m_DA_Cala06;
    double m_DA_Cala07;
    double m_DA_Calb00;
    double m_DA_Calb01;
    double m_DA_Calb02;
    double m_DA_Calb03;
    double m_DA_Calb04;
    double m_DA_Calb05;
    double m_DA_Calb06;
    double m_DA_Calb07;

protected:
    virtual void DoDataExchange(CDataExchange* pDX);
    virtual void OnOK();

    DECLARE_MESSAGE_MAP()
};

#endif // __DA_CHANNEL_H_INCLUDE__

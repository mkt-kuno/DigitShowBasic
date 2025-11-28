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

#ifndef __CONTROL_FILE_H_INCLUDE__
#define __CONTROL_FILE_H_INCLUDE__

#pragma once

class CControl_File : public CDialog
{
public:
    CControl_File(CWnd* pParent = NULL);

    enum { IDD = IDD_Control_File };

    double m_CFPARA0;
    double m_CFPARA1;
    double m_CFPARA2;
    double m_CFPARA3;
    double m_CFPARA4;
    double m_CFPARA5;
    double m_CFPARA6;
    double m_CFPARA7;
    double m_CFPARA8;
    double m_CFPARA9;
    int    m_StepNum;
    int    m_SCFNum;
    int    m_CurNum;
    int    m_CFNum;

protected:
    virtual void DoDataExchange(CDataExchange* pDX);
    virtual BOOL OnInitDialog();

    afx_msg void OnBUTTONUpdate();
    afx_msg void OnBUTTONReadFile();
    afx_msg void OnBUTTONSaveFile();
    afx_msg void OnBUTTONLoad();
    afx_msg void OnCHECKChangeNo();
    afx_msg void OnBUTTONStepDec();
    afx_msg void OnBUTTONStepInc();

    DECLARE_MESSAGE_MAP()
};

#endif // __CONTROL_FILE_H_INCLUDE__

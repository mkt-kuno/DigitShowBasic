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

#ifndef __CALIBRATIONFACTOR_H_INCLUDE__
#define __CALIBRATIONFACTOR_H_INCLUDE__

#pragma once

#include "DigitShowBasicDoc.h"

class CCalibrationFactor : public CDialog
{
public:
    CCalibrationFactor(CWnd* pParent = NULL);
    CDigitShowBasicDoc* pDoc;

    void CF_Load();

    enum { IDD = IDD_Calibration_Factor };

    CString m_CFP00, m_CFP01, m_CFP02, m_CFP03;
    CString m_CFP04, m_CFP05, m_CFP06, m_CFP07;
    CString m_CFP08, m_CFP09, m_CFP10, m_CFP11;
    CString m_CFP12, m_CFP13, m_CFP14, m_CFP15;
    double  m_CFA00, m_CFA01, m_CFA02, m_CFA03;
    double  m_CFA04, m_CFA05, m_CFA06, m_CFA07;
    double  m_CFA08, m_CFA09, m_CFA10, m_CFA11;
    double  m_CFA12, m_CFA13, m_CFA14, m_CFA15;
    double  m_CFB00, m_CFB01, m_CFB02, m_CFB03;
    double  m_CFB04, m_CFB05, m_CFB06, m_CFB07;
    double  m_CFB08, m_CFB09, m_CFB10, m_CFB11;
    double  m_CFB12, m_CFB13, m_CFB14, m_CFB15;
    double  m_CFC00, m_CFC01, m_CFC02, m_CFC03;
    double  m_CFC04, m_CFC05, m_CFC06, m_CFC07;
    double  m_CFC08, m_CFC09, m_CFC10, m_CFC11;
    double  m_CFC12, m_CFC13, m_CFC14, m_CFC15;
    CString m_Channels;
    CString m_C00, m_C01, m_C02, m_C03;
    CString m_C04, m_C05, m_C06, m_C07;
    CString m_C08, m_C09, m_C10, m_C11;
    CString m_C12, m_C13, m_C14, m_C15;

protected:
    virtual void DoDataExchange(CDataExchange* pDX);
    virtual BOOL OnInitDialog();

    afx_msg void OnBUTTONCFUpdate();
    afx_msg void OnBUTTONZero00();
    afx_msg void OnBUTTONZero01();
    afx_msg void OnBUTTONZero02();
    afx_msg void OnBUTTONZero03();
    afx_msg void OnBUTTONZero04();
    afx_msg void OnBUTTONZero05();
    afx_msg void OnBUTTONZero06();
    afx_msg void OnBUTTONZero07();
    afx_msg void OnBUTTONZero08();
    afx_msg void OnBUTTONZero09();
    afx_msg void OnBUTTONZero10();
    afx_msg void OnBUTTONZero11();
    afx_msg void OnBUTTONZero12();
    afx_msg void OnBUTTONZero13();
    afx_msg void OnBUTTONZero14();
    afx_msg void OnBUTTONZero15();
    afx_msg void OnBUTTONAmp00();
    afx_msg void OnBUTTONAmp01();
    afx_msg void OnBUTTONAmp02();
    afx_msg void OnBUTTONAmp03();
    afx_msg void OnBUTTONAmp04();
    afx_msg void OnBUTTONAmp05();
    afx_msg void OnBUTTONAmp06();
    afx_msg void OnBUTTONAmp07();
    afx_msg void OnBUTTONAmp08();
    afx_msg void OnBUTTONAmp09();
    afx_msg void OnBUTTONAmp10();
    afx_msg void OnBUTTONAmp11();
    afx_msg void OnBUTTONAmp12();
    afx_msg void OnBUTTONAmp13();
    afx_msg void OnBUTTONAmp14();
    afx_msg void OnBUTTONAmp15();
    afx_msg void OnBUTTONCFLoadFile();
    afx_msg void OnBUTTONCFSave();
    afx_msg void OnBUTTONChannelChange();

    DECLARE_MESSAGE_MAP()
};

#endif // __CALIBRATIONFACTOR_H_INCLUDE__

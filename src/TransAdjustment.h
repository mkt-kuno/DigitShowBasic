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

#ifndef __TRANSADJUSTMENT_H_INCLUDE__
#define __TRANSADJUSTMENT_H_INCLUDE__

#pragma once

class CTransAdjustment : public CDialog
{
public:
    CTransAdjustment(CWnd* pParent = NULL);

    enum { IDD = IDD_TransAdjustment };

    double m_FinalDisp;
    double m_InitialDisp;
    double m_FinalBullet;
    double m_InitialBullet;

protected:
    virtual void DoDataExchange(CDataExchange* pDX);

    afx_msg void OnBUTTONInitialDisp();
    afx_msg void OnBUTTONEndDisp();
    afx_msg void OnBUTTONInitialBullet();
    afx_msg void OnBUTTONEndBullet();
    afx_msg void OnBUTTONUpdateDisp();
    afx_msg void OnBUTTONUpdateBullet();

    DECLARE_MESSAGE_MAP()
};

#endif // __TRANSADJUSTMENT_H_INCLUDE__

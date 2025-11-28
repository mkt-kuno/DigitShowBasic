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

#ifndef __SPECIMEN_H_INCLUDE__
#define __SPECIMEN_H_INCLUDE__

#pragma once

class CSpecimen : public CDialog
{
public:
    CSpecimen(CWnd* pParent = NULL);

    enum { IDD = IDD_SpecimenData };

    double m_Area0;
    double m_Area1;
    double m_Area2;
    double m_Area3;
    double m_Depth0;
    double m_Depth1;
    double m_Depth2;
    double m_Depth3;
    double m_Gs;
    double m_Height0;
    double m_Height1;
    double m_Height2;
    double m_Height3;
    double m_MembraneE;
    double m_MembraneT;
    double m_RodArea;
    double m_RodWeight;
    double m_Volume0;
    double m_Volume1;
    double m_Volume2;
    double m_Volume3;
    double m_Weight0;
    double m_Weight1;
    double m_Weight2;
    double m_Weight3;
    double m_Width0;
    double m_Width1;
    double m_Width2;
    double m_Width3;
    double m_Diameter0;
    double m_Diameter1;
    double m_Diameter2;
    double m_Diameter3;
    double m_VLDT1_0;
    double m_VLDT1_1;
    double m_VLDT1_2;
    double m_VLDT1_3;
    double m_VLDT2_0;
    double m_VLDT2_1;
    double m_VLDT2_2;
    double m_VLDT2_3;

protected:
    virtual void DoDataExchange(CDataExchange* pDX);

    void Reflesh();

    afx_msg void OnBUTTONSave();
    afx_msg void OnBUTTONUpdate();
    afx_msg void OnBUTTONBeConsol();
    afx_msg void OnBUTTONAfConsolidation();
    afx_msg void OnBUTTONToPresent1();
    afx_msg void OnBUTTONToPresent2();
    afx_msg void OnBUTTONToPresent3();

    DECLARE_MESSAGE_MAP()
};

#endif // __SPECIMEN_H_INCLUDE__

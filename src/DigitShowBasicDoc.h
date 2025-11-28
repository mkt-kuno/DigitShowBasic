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

#ifndef __DIGITSHOWBASICDOC_H_INCLUDE__
#define __DIGITSHOWBASICDOC_H_INCLUDE__

#pragma once

#include "DigitShowContext.h"

class CDigitShowBasicDoc : public CDocument
{
protected:
    CDigitShowBasicDoc();
    DECLARE_DYNCREATE(CDigitShowBasicDoc)

public:
    virtual BOOL OnNewDocument();
    virtual void Serialize(CArchive& ar);

public:
    void Creep2();
    void SaveToFile2();
    void Allocate_Memory();
    void LinearEffectiveStressPath();
    void Stop_Control();
    void Start_Control();
    void Creep();
    void CLoading_Strain();
    void CLoading_Stress();
    void MLoading_Strain();
    void MLoading_Stress();
    void CloseBoard();
    void OpenBoard();
    void SaveToFile();
    void Control_DA();
    void Cal_Param();
    void Cal_Physical();
    void DA_OUTPUT();
    void AD_INPUT();
    virtual ~CDigitShowBasicDoc();

#ifdef _DEBUG
    virtual void AssertValid() const;
    virtual void Dump(CDumpContext& dc) const;
#endif

protected:
    DECLARE_MESSAGE_MAP()
};

#endif // __DIGITSHOWBASICDOC_H_INCLUDE__

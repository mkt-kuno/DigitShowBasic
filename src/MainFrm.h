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

#ifndef __MAINFRM_H_INCLUDE__
#define __MAINFRM_H_INCLUDE__

#pragma once

class CMainFrame : public CFrameWnd
{
protected:
    CMainFrame();
    DECLARE_DYNCREATE(CMainFrame)

public:
    virtual BOOL PreCreateWindow(CREATESTRUCT& cs);

public:
    virtual ~CMainFrame();

#ifdef _DEBUG
    virtual void AssertValid() const;
    virtual void Dump(CDumpContext& dc) const;
#endif

private:
    int nResult;

protected:
    afx_msg void OnBoardSettings();
    afx_msg void OnCalibrationFactor();
    afx_msg void OnSpecimenData();
    afx_msg void OnDAVout();
    afx_msg void OnControlID();
    afx_msg void OnDAPout();
    afx_msg void OnDAChannel();
    afx_msg void OnControlConsolidation();
    afx_msg void OnControlMLoading();
    afx_msg void OnControlSensitivity();
    afx_msg void OnControlCLoading();
    afx_msg void OnControlFile();
    afx_msg void OnSamplingSettings();
    afx_msg void OnControlPreConsolidation();
    afx_msg void OnTransAdjustment();
    afx_msg void OnControlLinearStressPath();
    DECLARE_MESSAGE_MAP()
};

#endif // __MAINFRM_H_INCLUDE__

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

#ifndef __DIGITSHOWBASICVIEW_H_INCLUDE__
#define __DIGITSHOWBASICVIEW_H_INCLUDE__

#pragma once

#include "DigitShowBasic.h"
#include "sys/timeb.h"

class CDigitShowBasicView : public CFormView
{
protected:
    CDigitShowBasicView();
    DECLARE_DYNCREATE(CDigitShowBasicView)

public:
    enum { IDD = IDD_DIGITSHOWBASIC_FORM };
    CString    m_Vout00, m_Vout01, m_Vout02, m_Vout03;
    CString    m_Vout04, m_Vout05, m_Vout06, m_Vout07;
    CString    m_Vout08, m_Vout09, m_Vout10, m_Vout11;
    CString    m_Vout12, m_Vout13, m_Vout14, m_Vout15;
    CString    m_Vout16, m_Vout17, m_Vout18, m_Vout19;
    CString    m_Vout20, m_Vout21, m_Vout22, m_Vout23;
    CString    m_Vout24, m_Vout25, m_Vout26, m_Vout27;
    CString    m_Vout28, m_Vout29, m_Vout30, m_Vout31;
    CString    m_Phyout00, m_Phyout01, m_Phyout02, m_Phyout03;
    CString    m_Phyout04, m_Phyout05, m_Phyout06, m_Phyout07;
    CString    m_Phyout08, m_Phyout09, m_Phyout10, m_Phyout11;
    CString    m_Phyout12, m_Phyout13, m_Phyout14, m_Phyout15;
    CString    m_Phyout16, m_Phyout17, m_Phyout18, m_Phyout19;
    CString    m_Phyout20, m_Phyout21, m_Phyout22, m_Phyout23;
    CString    m_Phyout24, m_Phyout25, m_Phyout26, m_Phyout27;
    CString    m_Phyout28, m_Phyout29, m_Phyout30, m_Phyout31;
    CString    m_Para00, m_Para01, m_Para02, m_Para03;
    CString    m_Para04, m_Para05, m_Para06, m_Para07;
    CString    m_Para08, m_Para09, m_Para10, m_Para11;
    CString    m_Para12, m_Para13, m_Para14, m_Para15;
    int        m_Ctrl_ID;
    CString    m_NowTime;
    long    m_SeqTime;
    long    m_SamplingTime;
    CString    m_FileName;

public:
    CDigitShowBasicDoc* GetDocument();
    struct _timeb StartTime2, NowTime2;
    struct _timeb StepTime0, StepTime1;
    CBrush* m_pEditBrush;
    CBrush* m_pStaticBrush;
    CBrush* m_pDlgBrush;

public:
    virtual BOOL PreCreateWindow(CREATESTRUCT& cs);

protected:
    virtual void DoDataExchange(CDataExchange* pDX);
    virtual void OnInitialUpdate();
    virtual LRESULT DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam);

public:
    void ShowData();
    virtual ~CDigitShowBasicView();

#ifdef _DEBUG
    virtual void AssertValid() const;
    virtual void Dump(CDumpContext& dc) const;
#endif

protected:
    afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
    afx_msg void OnTimer(UINT_PTR nIDEvent);
    afx_msg void OnBUTTONCtrlOff();
    afx_msg void OnBUTTONCtrlOn();
    afx_msg void OnBUTTONStartSave();
    afx_msg void OnBUTTONStopSave();
    afx_msg void OnDestroy();
    afx_msg void OnBUTTONInterceptSave();
    afx_msg void OnBUTTONFIFOStart();
    afx_msg void OnBUTTONFIFOStop();
    afx_msg void OnBUTTONWriteData();
    afx_msg void OnBUTTONSetCtrlID();
    afx_msg void OnBUTTONSetTimeInterval();
    DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG
inline CDigitShowBasicDoc* CDigitShowBasicView::GetDocument()
{
    return (CDigitShowBasicDoc*)m_pDocument;
}
#endif

#endif // __DIGITSHOWBASICVIEW_H_INCLUDE__

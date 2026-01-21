/*
 * DigitShowDST - Direct Shear Test Machine Control Software
 * Copyright (C) 2025 Makoto KUNO, Takuto ISHII
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

/**
 * @file MainFrm.cpp
 * @brief Implementation of CMainFrame class
 *
 * Implements main frame window functionality and menu command handlers.
 * CMainFrame クラスの動作の定義を行います。
 */

#include "StdAfx.h"

#include "AppInfoDlg.h"
#include "Board.hpp"
#include "BoardSettings.h"
#include "CalibrationFactor.h"
#include "Control_File.h"
#include "DA_Channel.h"
#include "DA_Pout.h"
#include "DA_Vout.h"
#include "MainFrm.h"
#include "SamplingSettings.h"
#include "TransAdjustment.h"
#include "resource.h"
#include <spdlog/spdlog.h>

#if defined(_DEBUG) && !defined(__clang__)
#define new DEBUG_NEW
#undef THIS_FILE
static constexpr const char *THIS_FILE = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMainFrame

IMPLEMENT_DYNCREATE(CMainFrame, CFrameWnd)

BEGIN_MESSAGE_MAP_IGNORE_UNUSED_LOCAL_TYPEDEF(CMainFrame, CFrameWnd)
//{{AFX_MSG_MAP(CMainFrame)
ON_COMMAND(ID_APP_ABOUT, &CMainFrame::OnAppAbout)
ON_COMMAND(ID_BoardSettings, &CMainFrame::OnBoardSettings)
ON_COMMAND(ID_Calibration_Factor, &CMainFrame::OnCalibrationFactor)
ON_COMMAND(ID_DA_Vout, &CMainFrame::OnDAVout)
ON_COMMAND(ID_DA_Pout, &CMainFrame::OnDAPout)
ON_COMMAND(ID_DA_Channel, &CMainFrame::OnDAChannel)
ON_COMMAND(ID_Control_File, &CMainFrame::OnControlFile)
ON_COMMAND(ID_SamplingSettings, &CMainFrame::OnSamplingSettings)
ON_COMMAND(ID_TransAdjustment, &CMainFrame::OnTransAdjustment)
//}}AFX_MSG_MAP
END_MESSAGE_MAP_IGNORE_UNUSED_LOCAL_TYPEDEF()

/////////////////////////////////////////////////////////////////////////////

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT &cs)
{
    // TODO: この位置で CREATESTRUCT cs を修正して、Window クラスやスタイルを
    //       修正してください。

    // Get system height and widths- added later
    cs.cy = ::GetSystemMetrics(SM_CYSCREEN);
    cs.cx = ::GetSystemMetrics(SM_CXSCREEN);
    cs.y = 0;
    cs.x = 0;
    // over: Get...added later
    // If previous saved Window_size could not be read,Set default value

    cs.style ^= (LONG)FWS_ADDTOTITLE; // Not showing title of child window in main window

    return CFrameWnd::PreCreateWindow(cs);
}

/////////////////////////////////////////////////////////////////////////////
// CMainFrame クラスの診断

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
    CFrameWnd::AssertValid();
}

void CMainFrame::Dump(CDumpContext &dc) const
{
    CFrameWnd::Dump(dc);
}

#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CMainFrame メッセージ ハンドラ

void CMainFrame::OnAppAbout()
{
    spdlog::info("About dialog menu selected");
    CAppInfoDlg appInfoDlg;
    nResult = appInfoDlg.DoModal();
}

void CMainFrame::OnBoardSettings()
{
    spdlog::info("Board Settings menu selected");
    // TODO: この位置にコマンド ハンドラ用のコードを追加してください
    CBoardSettings BoardSettings;
    nResult = BoardSettings.DoModal(); // Display a device open dialog.
}

void CMainFrame::OnSamplingSettings()
{
    spdlog::info("Sampling Settings menu selected");
    // TODO: この位置にコマンド ハンドラ用のコードを追加してください
    CSamplingSettings SamplingSettings;
    nResult = SamplingSettings.DoModal(); // Display a device open dialog.
}

void CMainFrame::OnCalibrationFactor()
{
    spdlog::info("Calibration Factor menu selected");
    // TODO: この位置にコマンド ハンドラ用のコードを追加してください
    if (board::Flag_SetBoard == FALSE)
    {
        spdlog::warn("Calibration Factor accessed but board not initialized");
        AfxMessageBox(_T("BoardSettings has not been accomplished !"), MB_ICONEXCLAMATION | MB_OK);
    }
    CCalibrationFactor CalibrationFactor;
    nResult = CalibrationFactor.DoModal();
}

void CMainFrame::OnTransAdjustment()
{
    spdlog::info("Trans Adjustment menu selected");
    // TODO: この位置にコマンド ハンドラ用のコードを追加してください
    CTransAdjustment TransAdjustment;
    nResult = TransAdjustment.DoModal();
}

void CMainFrame::OnDAChannel()
{
    spdlog::info("DA Channel menu selected");
    // TODO: この位置にコマンド ハンドラ用のコードを追加してください
    CDA_Channel DA_Channel;
    nResult = DA_Channel.DoModal();
}
void CMainFrame::OnDAVout()
{
    spdlog::info("DA Vout menu selected");
    // TODO: この位置にコマンド ハンドラ用のコードを追加してください
    if (board::Flag_SetBoard == FALSE)
    {
        spdlog::warn("DA Vout accessed but board not initialized");
        AfxMessageBox(_T("BoardSettings has not been accomplished !"), MB_ICONEXCLAMATION | MB_OK);
    }
    CDA_Vout DA_Vout;
    nResult = DA_Vout.DoModal();
}

void CMainFrame::OnDAPout()
{
    spdlog::info("DA Pout menu selected");
    // TODO: この位置にコマンド ハンドラ用のコードを追加してください
    if (board::Flag_SetBoard == FALSE)
    {
        spdlog::warn("DA Pout accessed but board not initialized");
        AfxMessageBox(_T("BoardSettings has not been accomplished !"), MB_ICONEXCLAMATION | MB_OK);
    }
    CDA_Pout DA_Pout;
    nResult = DA_Pout.DoModal();
}

void CMainFrame::OnControlFile()
{
    spdlog::info("Control File menu selected");
    // TODO: この位置にコマンド ハンドラ用のコードを追加してください
    CControl_File Control_File;
    nResult = Control_File.DoModal();
}

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

#include "stdafx.h"
#include "DigitShowBasic.h"
#include "DigitShowBasicDoc.h"

#include "MainFrm.h"
#include "BoardSettings.h"
#include "SamplingSettings.h"
#include "CalibrationFactor.h"
#include "Specimen.h"
#include "TransAdjustment.h"
#include "Control_ID.h"
#include "Control_Sensitivity.h"
#include "Control_PreConsolidation.h"
#include "Control_Consolidation.h"
#include "Control_MLoading.h"
#include "Control_CLoading.h"
#include "Control_LinearStressPath.h"
#include "Control_File.h"
#include "DA_Vout.h"
#include "DA_Pout.h"
#include "DA_Channel.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMainFrame

IMPLEMENT_DYNCREATE(CMainFrame, CFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
    //{{AFX_MSG_MAP(CMainFrame)
    ON_COMMAND(ID_BoardSettings, OnBoardSettings)
    ON_COMMAND(ID_Calibration_Factor, OnCalibrationFactor)
    ON_COMMAND(ID_SpecimenData, OnSpecimenData)
    ON_COMMAND(ID_DA_Vout, OnDAVout)
    ON_COMMAND(ID_Control_ID, OnControlID)
    ON_COMMAND(ID_DA_Pout, OnDAPout)
    ON_COMMAND(ID_DA_Channel, OnDAChannel)
    ON_COMMAND(ID_Control_Consolidation, OnControlConsolidation)
    ON_COMMAND(ID_Control_MLoading, OnControlMLoading)
    ON_COMMAND(ID_Control_Sensitivity, OnControlSensitivity)
    ON_COMMAND(ID_Control_CLoading, OnControlCLoading)
    ON_COMMAND(ID_Control_File, OnControlFile)
    ON_COMMAND(ID_SamplingSettings, OnSamplingSettings)
    ON_COMMAND(ID_Control_PreConsolidation, OnControlPreConsolidation)
    ON_COMMAND(ID_TransAdjustment, OnTransAdjustment)
    ON_COMMAND(ID_Control_LinearStressPath, OnControlLinearStressPath)
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMainFrame クラスの構築/消滅

CMainFrame::CMainFrame()
{

}

CMainFrame::~CMainFrame()
{
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{

    //       修正してください。

    //Get system height and widths- added later
    cs.cy = ::GetSystemMetrics(SM_CYSCREEN);
    cs.cx = ::GetSystemMetrics(SM_CXSCREEN);
    cs.y = 0;
    cs.x = 0;
    //over: Get...added later
    //If previous saved Window_size could not be read,Set default value

    cs.style ^=(LONG)FWS_ADDTOTITLE;
    //Not showing title of child window in main window

    return CFrameWnd::PreCreateWindow(cs);

}

/////////////////////////////////////////////////////////////////////////////
// CMainFrame クラスの診断

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
    CFrameWnd::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
    CFrameWnd::Dump(dc);
}

#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CMainFrame メッセージ ハンドラ

void CMainFrame::OnBoardSettings() 
{

        CBoardSettings BoardSettings;
        nResult = BoardSettings.DoModal();
        // Display a device open dialog.
}

void CMainFrame::OnSamplingSettings() 
{

        CSamplingSettings SamplingSettings;
        nResult = SamplingSettings.DoModal();
        // Display a device open dialog.
}

void CMainFrame::OnCalibrationFactor() 
{
    DigitShowContext* ctx = GetContext();
    if(ctx->FlagSetBoard==FALSE){
        AfxMessageBox("BoardSettings has not been accomplished !",MB_ICONEXCLAMATION | MB_OK );
    }
    CCalibrationFactor CalibrationFactor;
    nResult = CalibrationFactor.DoModal();        
}

void CMainFrame::OnSpecimenData() 
{

    CSpecimen Specimen;
    nResult = Specimen.DoModal();        
}

void CMainFrame::OnTransAdjustment() 
{

    CTransAdjustment TransAdjustment;
    nResult = TransAdjustment.DoModal();        
}

void CMainFrame::OnDAChannel() 
{

    CDA_Channel DA_Channel;
    nResult = DA_Channel.DoModal();    
}
void CMainFrame::OnDAVout() 
{
    DigitShowContext* ctx = GetContext();
    if(ctx->FlagSetBoard==FALSE){
        AfxMessageBox("BoardSettings has not been accomplished !",MB_ICONEXCLAMATION | MB_OK );
    }
    CDA_Vout DA_Vout;
    nResult = DA_Vout.DoModal();        
}

void CMainFrame::OnDAPout() 
{
    DigitShowContext* ctx = GetContext();
    if(ctx->FlagSetBoard==FALSE){
        AfxMessageBox("BoardSettings has not been accomplished !",MB_ICONEXCLAMATION | MB_OK );
    }
    CDA_Pout DA_Pout;
    nResult = DA_Pout.DoModal();    
}

void CMainFrame::OnControlSensitivity() 
{

    CControl_Sensitivity Control_Sensitivity;
    nResult = Control_Sensitivity.DoModal();        
}

void CMainFrame::OnControlID() 
{

    CControl_ID Control_ID;
    nResult = Control_ID.DoModal();    
}

void CMainFrame::OnControlPreConsolidation() 
{

    CControl_PreConsolidation Control_PreConsolidation;
    nResult = Control_PreConsolidation.DoModal();    
}

void CMainFrame::OnControlConsolidation() 
{

    CControl_Consolidation Control_Consolidation;
    nResult = Control_Consolidation.DoModal();    
}

void CMainFrame::OnControlMLoading() 
{

    CControl_MLoading Control_MLoading;
    nResult = Control_MLoading.DoModal();    
}

void CMainFrame::OnControlCLoading() 
{

    CControl_CLoading Control_CLoading;
    nResult = Control_CLoading.DoModal();    
}

void CMainFrame::OnControlLinearStressPath() 
{

    CControl_LinearStressPath Control_LinearStressPath;
    nResult = Control_LinearStressPath.DoModal();    
    
}

void CMainFrame::OnControlFile() 
{

    CControl_File Control_File;
    nResult = Control_File.DoModal();    
}

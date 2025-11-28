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
#include "Specimen.h"
#include "DigitShowBasicDoc.h"
#include "DigitShowContext.h"
#include "math.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CSpecimen::CSpecimen(CWnd* pParent)
    : CDialog(CSpecimen::IDD, pParent)
{
    DigitShowContext* ctx = GetContext();
    auto SpecimenData = &ctx->specimen;

    m_Area0 = SpecimenData->Area[0];
    m_Area1 = SpecimenData->Area[1];
    m_Area2 = SpecimenData->Area[2];
    m_Area3 = SpecimenData->Area[3];
    m_Depth0 = SpecimenData->Depth[0];
    m_Depth1 = SpecimenData->Depth[1];
    m_Depth2 = SpecimenData->Depth[2];
    m_Gs = SpecimenData->Gs;
    m_Height0 = SpecimenData->Height[0];
    m_Height1 = SpecimenData->Height[1];
    m_Height2 = SpecimenData->Height[2];
    m_Height3 = SpecimenData->Height[3];
    m_MembraneE = SpecimenData->MembraneModulus;
    m_MembraneT = SpecimenData->MembraneThickness;
    m_RodArea = SpecimenData->RodArea;
    m_RodWeight = SpecimenData->RodWeight;
    m_Volume0 = SpecimenData->Volume[0];
    m_Volume1 = SpecimenData->Volume[1];
    m_Volume2 = SpecimenData->Volume[2];
    m_Volume3 = SpecimenData->Volume[3];
    m_Weight0 = SpecimenData->Weight[0];
    m_Weight1 = SpecimenData->Weight[1];    
    m_Weight2 = SpecimenData->Weight[2];
    m_Weight3 = SpecimenData->Weight[3];
    m_Width0 = SpecimenData->Width[0];
    m_Width1 = SpecimenData->Width[1];
    m_Width2 = SpecimenData->Width[2];
    m_Width3 = SpecimenData->Width[3];
    m_Depth3 = SpecimenData->Depth[3];    
    m_Diameter0 = SpecimenData->Diameter[0];
    m_Diameter1 = SpecimenData->Diameter[1];
    m_Diameter2 = SpecimenData->Diameter[2];
    m_Diameter3 = SpecimenData->Diameter[3];
    m_VLDT1_0 = SpecimenData->VLDT1[0];
    m_VLDT1_1 = SpecimenData->VLDT1[1];
    m_VLDT1_2 = SpecimenData->VLDT1[2];
    m_VLDT1_3 = SpecimenData->VLDT1[3];
    m_VLDT2_0 = SpecimenData->VLDT2[0];
    m_VLDT2_1 = SpecimenData->VLDT2[1];
    m_VLDT2_2 = SpecimenData->VLDT2[2];
    m_VLDT2_3 = SpecimenData->VLDT2[3];
    //}}AFX_DATA_INIT
}


void CSpecimen::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    //{{AFX_DATA_MAP(CSpecimen)
    DDX_Text(pDX, IDC_EDIT_Area0, m_Area0);
    DDX_Text(pDX, IDC_EDIT_Area1, m_Area1);
    DDX_Text(pDX, IDC_EDIT_Area2, m_Area2);
    DDX_Text(pDX, IDC_EDIT_Area3, m_Area3);
    DDX_Text(pDX, IDC_EDIT_Depth0, m_Depth0);
    DDX_Text(pDX, IDC_EDIT_Depth1, m_Depth1);
    DDX_Text(pDX, IDC_EDIT_Depth2, m_Depth2);
    DDX_Text(pDX, IDC_EDIT_Gs, m_Gs);
    DDX_Text(pDX, IDC_EDIT_Height0, m_Height0);
    DDX_Text(pDX, IDC_EDIT_Height1, m_Height1);
    DDX_Text(pDX, IDC_EDIT_Height2, m_Height2);
    DDX_Text(pDX, IDC_EDIT_Height3, m_Height3);
    DDX_Text(pDX, IDC_EDIT_MembraneE, m_MembraneE);
    DDX_Text(pDX, IDC_EDIT_MembraneT, m_MembraneT);
    DDX_Text(pDX, IDC_EDIT_RodArea, m_RodArea);
    DDX_Text(pDX, IDC_EDIT_RodWeight, m_RodWeight);
    DDX_Text(pDX, IDC_EDIT_Volume0, m_Volume0);
    DDX_Text(pDX, IDC_EDIT_Volume1, m_Volume1);
    DDX_Text(pDX, IDC_EDIT_Volume2, m_Volume2);
    DDX_Text(pDX, IDC_EDIT_Volume3, m_Volume3);
    DDX_Text(pDX, IDC_EDIT_Weight0, m_Weight0);
    DDX_Text(pDX, IDC_EDIT_Weight1, m_Weight1);
    DDX_Text(pDX, IDC_EDIT_Weight2, m_Weight2);
    DDX_Text(pDX, IDC_EDIT_Weight3, m_Weight3);
    DDX_Text(pDX, IDC_EDIT_Width0, m_Width0);
    DDX_Text(pDX, IDC_EDIT_Width1, m_Width1);
    DDX_Text(pDX, IDC_EDIT_Width2, m_Width2);
    DDX_Text(pDX, IDC_EDIT_Width3, m_Width3);
    DDX_Text(pDX, IDC_EDIT_Depth3, m_Depth3);
    DDX_Text(pDX, IDC_EDIT_Diameter0, m_Diameter0);
    DDX_Text(pDX, IDC_EDIT_Diameter1, m_Diameter1);
    DDX_Text(pDX, IDC_EDIT_Diameter2, m_Diameter2);
    DDX_Text(pDX, IDC_EDIT_Diameter3, m_Diameter3);
    DDX_Text(pDX, IDC_EDIT_VLDT1_0, m_VLDT1_0);
    DDX_Text(pDX, IDC_EDIT_VLDT1_1, m_VLDT1_1);
    DDX_Text(pDX, IDC_EDIT_VLDT1_2, m_VLDT1_2);
    DDX_Text(pDX, IDC_EDIT_VLDT1_3, m_VLDT1_3);
    DDX_Text(pDX, IDC_EDIT_VLDT2_0, m_VLDT2_0);
    DDX_Text(pDX, IDC_EDIT_VLDT2_1, m_VLDT2_1);
    DDX_Text(pDX, IDC_EDIT_VLDT2_2, m_VLDT2_2);
    DDX_Text(pDX, IDC_EDIT_VLDT2_3, m_VLDT2_3);
    //}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CSpecimen, CDialog)
    //{{AFX_MSG_MAP(CSpecimen)
    ON_BN_CLICKED(IDC_BUTTON_Save, OnBUTTONSave)
    ON_BN_CLICKED(IDC_BUTTON_Update, OnBUTTONUpdate)
    ON_BN_CLICKED(IDC_BUTTON_BeConsol, OnBUTTONBeConsol)
    ON_BN_CLICKED(IDC_BUTTON_AfConsolidation, OnBUTTONAfConsolidation)
    ON_BN_CLICKED(IDC_BUTTON_ToPresent1, OnBUTTONToPresent1)
    ON_BN_CLICKED(IDC_BUTTON_ToPresent2, OnBUTTONToPresent2)
    ON_BN_CLICKED(IDC_BUTTON_ToPresent3, OnBUTTONToPresent3)
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSpecimen メッセージ ハンドラ

void CSpecimen::OnBUTTONUpdate() 
{
    DigitShowContext* ctx = GetContext();
    auto SpecimenData = &ctx->specimen;

    UpdateData(TRUE);
    if( m_Diameter0 != 0.0){
        m_Width0 = 0.0;
        m_Depth0 = 0.0;
        m_Area0 = 3.141592*pow(m_Diameter0,2.0)/4.0;
    }    
    else    m_Area0 = m_Depth0*m_Width0;
    if( m_Diameter1 != 0.0){
        m_Width1 = 0.0;
        m_Depth1 = 0.0;
        m_Area1 = 3.141592*pow(m_Diameter1,2.0)/4.0;
    }    
    else    m_Area1 = m_Depth1*m_Width1;
    if( m_Diameter2 != 0.0){
        m_Width2 = 0.0;
        m_Depth2 = 0.0;
        m_Area2 = 3.141592*pow(m_Diameter2,2.0)/4.0;
    }
    else    m_Area2 = m_Depth2*m_Width2;    
    if( m_Diameter3 != 0.0){
        m_Width3 = 0.0;
        m_Depth3 = 0.0;
        m_Area3 = 3.141592*pow(m_Diameter3,2.0)/4.0;
    }
    else    m_Area3 = m_Depth3*m_Width3;
    m_Volume0 = m_Area0*m_Height0;
    m_Volume1 = m_Area1*m_Height1;
    m_Volume2 = m_Area2*m_Height2;
    m_Volume3 = m_Area3*m_Height3;

    SpecimenData->Area[0]   = m_Area0;
    SpecimenData->Area[1]   = m_Area1;
    SpecimenData->Area[2]   = m_Area2;
    SpecimenData->Area[3]   = m_Area3;
    SpecimenData->Gs        = m_Gs;
    SpecimenData->Height[0] = m_Height0;
    SpecimenData->Height[1] = m_Height1;    
    SpecimenData->Height[2] = m_Height2;
    SpecimenData->Height[3] = m_Height3;
    SpecimenData->MembraneModulus   = m_MembraneE;
    SpecimenData->MembraneThickness = m_MembraneT;
    SpecimenData->RodArea   = m_RodArea;
    SpecimenData->RodWeight = m_RodWeight;
    SpecimenData->Volume[0] = m_Volume0;
    SpecimenData->Volume[1] = m_Volume1;    
    SpecimenData->Volume[2] = m_Volume2;
    SpecimenData->Volume[3] = m_Volume3;
    SpecimenData->Weight[0] = m_Weight0;
    SpecimenData->Weight[1] = m_Weight1;        
    SpecimenData->Weight[2] = m_Weight2;    
    SpecimenData->Weight[3] = m_Weight3;    
    SpecimenData->Diameter[0] = m_Diameter0;
    SpecimenData->Diameter[1] = m_Diameter1;    
    SpecimenData->Diameter[2] = m_Diameter2;
    SpecimenData->Diameter[3] = m_Diameter3;
    SpecimenData->Depth[0] = m_Depth0;
    SpecimenData->Depth[1] = m_Depth1;
    SpecimenData->Depth[2] = m_Depth2;
    SpecimenData->Depth[3] = m_Depth3;    
    SpecimenData->Width[0] = m_Width0;
    SpecimenData->Width[1] = m_Width1;
    SpecimenData->Width[2] = m_Width2;
    SpecimenData->Width[3] = m_Width3;
    SpecimenData->VLDT1[0] = m_VLDT1_0;
    SpecimenData->VLDT1[1] = m_VLDT1_1;
    SpecimenData->VLDT1[2] = m_VLDT1_2;
    SpecimenData->VLDT1[3] = m_VLDT1_3;
    SpecimenData->VLDT2[0] = m_VLDT2_0;
    SpecimenData->VLDT2[1] = m_VLDT2_1;
    SpecimenData->VLDT2[2] = m_VLDT2_2;
    SpecimenData->VLDT2[3] = m_VLDT2_3;
    UpdateData(FALSE);
}

void CSpecimen::OnBUTTONSave() 
{
    DigitShowContext* ctx = GetContext();
    auto SpecimenData = &ctx->specimen;
    
    OnBUTTONUpdate();

    CString    pFileName;
    FILE    *FileSpcData;
    errno_t err; 

    CFileDialog SpcSaveFile_dlg( FALSE, NULL, "*.spe",  OFN_CREATEPROMPT | OFN_OVERWRITEPROMPT,
        "Specimen Files(*.spe)|*.spe| All Files(*.*)|*.*| |",NULL);

    if (SpcSaveFile_dlg.DoModal()==IDOK)
    {
        pFileName = SpcSaveFile_dlg.GetPathName();    
        if((err = fopen_s(&FileSpcData,(LPCSTR)pFileName , _T("w"))) == 0)
        {
            fprintf(FileSpcData,"%s    %lf    %lf    %lf    %lf\n","Diameter(mm)",
                SpecimenData->Diameter[0],SpecimenData->Diameter[1],SpecimenData->Diameter[2],SpecimenData->Diameter[3]);
            fprintf(FileSpcData,"%s    %lf    %lf    %lf    %lf\n","Width(mm)",
                SpecimenData->Width[0],SpecimenData->Width[1],SpecimenData->Width[2],SpecimenData->Width[3]);
            fprintf(FileSpcData,"%s    %lf    %lf    %lf    %lf\n","Depth(mm)",
                SpecimenData->Depth[0],SpecimenData->Depth[1],SpecimenData->Depth[2],SpecimenData->Depth[3]);
            fprintf(FileSpcData,"%s    %lf    %lf    %lf    %lf\n","Height(mm)",
                SpecimenData->Height[0],SpecimenData->Height[1],SpecimenData->Height[2],SpecimenData->Height[3]);
            fprintf(FileSpcData,"%s    %lf    %lf    %lf    %lf\n","Area(mm^2)",
                SpecimenData->Area[0],SpecimenData->Area[1],SpecimenData->Area[2],SpecimenData->Area[3]);
            fprintf(FileSpcData,"%s    %lf    %lf    %lf    %lf\n","Volume(mm^3)",
                SpecimenData->Volume[0],SpecimenData->Volume[1],SpecimenData->Volume[2],SpecimenData->Volume[3]);
            fprintf(FileSpcData,"%s    %lf    %lf    %lf    %lf\n","Weight(g)",
                SpecimenData->Weight[0],SpecimenData->Weight[1],SpecimenData->Weight[2],SpecimenData->Weight[3]);
            fprintf(FileSpcData,"%s    %lf    %lf    %lf    %lf\n","VLDT1(mm)",
                SpecimenData->VLDT1[0],SpecimenData->VLDT1[1],SpecimenData->VLDT1[2],SpecimenData->VLDT1[3]);
            fprintf(FileSpcData,"%s    %lf    %lf    %lf    %lf\n","VLDT2(mm)",
                SpecimenData->VLDT2[0],SpecimenData->VLDT2[1],SpecimenData->VLDT2[2],SpecimenData->VLDT2[3]);
            fprintf(FileSpcData,"%s    %lf\n","Gs",SpecimenData->Gs);
            fprintf(FileSpcData,"%s    %lf\n","MembraneModulus(kPa)",SpecimenData->MembraneModulus);
            fprintf(FileSpcData,"%s    %lf\n","MembraneThickness(mm)",SpecimenData->MembraneThickness);
            fprintf(FileSpcData,"%s    %lf\n","RodArea(mm^2)",SpecimenData->RodArea);
            fprintf(FileSpcData,"%s    %lf\n","RodWeight(g)",SpecimenData->RodWeight);
            fclose(FileSpcData);
        }
    }    
}

void CSpecimen::OnBUTTONBeConsol() 
{
    DigitShowContext* ctx = GetContext();
    auto SpecimenData = &ctx->specimen;

    SpecimenData->Height[2] = SpecimenData->Height[1]-ctx->Phyout[1];    
    SpecimenData->Volume[2] = SpecimenData->Volume[1]-SpecimenData->Area[1]*ctx->Phyout[1];
    SpecimenData->Area[2]   = SpecimenData->Area[1];
    SpecimenData->Diameter[2] = SpecimenData->Diameter[1]*sqrt(SpecimenData->Area[2]/SpecimenData->Area[1]) ;
    SpecimenData->Depth[2]  = SpecimenData->Depth[1]*sqrt(SpecimenData->Area[2]/SpecimenData->Area[1]);
    SpecimenData->Width[2]  = SpecimenData->Width[1]*sqrt(SpecimenData->Area[2]/SpecimenData->Area[1]);
    SpecimenData->VLDT1[2] = ctx->Phyout[5];
    SpecimenData->VLDT2[2] = ctx->Phyout[6];
    ctx->cal.c[1] = ctx->cal.c[1]- ctx->Phyout[1];
    //---0-adjustment of Displacement transducer---
    ctx->cal.c[4] = ctx->cal.c[4]- ctx->Phyout[4];
    //---0-adjustment of Volume Change ---
    Reflesh();
    OnBUTTONToPresent2();
}

void CSpecimen::OnBUTTONAfConsolidation() 
{
    DigitShowContext* ctx = GetContext();
    auto SpecimenData = &ctx->specimen;

    SpecimenData->Height[3] = SpecimenData->Height[2]-ctx->Phyout[1];    
    SpecimenData->Volume[3] = SpecimenData->Volume[2]-ctx->Phyout[4];
    SpecimenData->Area[3]   = SpecimenData->Volume[3]/SpecimenData->Height[3];
    SpecimenData->Diameter[3] = SpecimenData->Diameter[2]*sqrt(SpecimenData->Area[3]/SpecimenData->Area[2]);
    SpecimenData->Depth[3] = SpecimenData->Depth[2]*sqrt(SpecimenData->Area[3]/SpecimenData->Area[2]);
    SpecimenData->Width[3] = SpecimenData->Width[2]*sqrt(SpecimenData->Area[3]/SpecimenData->Area[2]);    
    SpecimenData->VLDT1[3] = ctx->Phyout[5];
    SpecimenData->VLDT2[3] = ctx->Phyout[6];
    ctx->cal.c[1] = ctx->cal.c[1]- ctx->Phyout[1];
    //---0-adjustment of Displacement transducer---
    ctx->cal.c[4] = ctx->cal.c[4]- ctx->Phyout[4];
    //---0-adjustment of Volume Change ---
    Reflesh();
    OnBUTTONToPresent3();
}

void CSpecimen::OnBUTTONToPresent1() 
{
    DigitShowContext* ctx = GetContext();
    auto SpecimenData = &ctx->specimen;

    UpdateData(TRUE);
    if( m_Diameter1 != 0.0){
        m_Width1 = 0.0;
        m_Depth1 = 0.0;
        m_Area1 = 3.141592*pow(m_Diameter1,2.0)/4.0;
    }    
    else    m_Area1 = m_Depth1*m_Width1;
    m_Volume1 = m_Area1*m_Height1;
    SpecimenData->Diameter[1] = m_Diameter1;
    SpecimenData->Width[1] = m_Width1;
    SpecimenData->Depth[1] = m_Depth1;
    SpecimenData->Height[1] = m_Height1;
    SpecimenData->Area[1] = m_Area1;
    SpecimenData->Volume[1] = m_Volume1;
    SpecimenData->VLDT1[1] = m_VLDT1_1;
    SpecimenData->VLDT2[1] = m_VLDT2_1;
//  -> present one
    SpecimenData->Diameter[0] = SpecimenData->Diameter[1];
    SpecimenData->Width[0] = SpecimenData->Width[1];
    SpecimenData->Depth[0] = SpecimenData->Depth[1];
    SpecimenData->Height[0] = SpecimenData->Height[1];
    SpecimenData->Area[0] = SpecimenData->Area[1];
    SpecimenData->Volume[0] = SpecimenData->Volume[1];
    SpecimenData->VLDT1[0] = SpecimenData->VLDT1[1];
    SpecimenData->VLDT2[0] = SpecimenData->VLDT2[1];
    Reflesh();
}

void CSpecimen::OnBUTTONToPresent2() 
{
    DigitShowContext* ctx = GetContext();
    auto SpecimenData = &ctx->specimen;

    UpdateData(TRUE);
    if( m_Diameter2 != 0.0){
        m_Width2 = 0.0;
        m_Depth2 = 0.0;
        m_Area2 = 3.141592*pow(m_Diameter2,2.0)/4.0;
    }    
    else    m_Area2 = m_Depth2*m_Width2;
    m_Volume2 = m_Area2*m_Height2;
    SpecimenData->Diameter[2] = m_Diameter2;
    SpecimenData->Width[2] = m_Width2;
    SpecimenData->Depth[2] = m_Depth2;
    SpecimenData->Height[2] = m_Height2;
    SpecimenData->Area[2] = m_Area2;
    SpecimenData->Volume[2] = m_Volume2;
    SpecimenData->VLDT1[2] = m_VLDT1_2;
    SpecimenData->VLDT2[2] = m_VLDT2_2;
//  -> present one
    SpecimenData->Diameter[0] = SpecimenData->Diameter[2];
    SpecimenData->Width[0] = SpecimenData->Width[2];
    SpecimenData->Depth[0] = SpecimenData->Depth[2];
    SpecimenData->Height[0] = SpecimenData->Height[2];
    SpecimenData->Area[0] = SpecimenData->Area[2];
    SpecimenData->Volume[0] = SpecimenData->Volume[2];
    SpecimenData->VLDT1[0] = SpecimenData->VLDT1[2];
    SpecimenData->VLDT2[0] = SpecimenData->VLDT2[2];
    Reflesh();
}

void CSpecimen::OnBUTTONToPresent3() 
{
    DigitShowContext* ctx = GetContext();
    auto SpecimenData = &ctx->specimen;

    UpdateData(TRUE);
    if( m_Diameter3 != 0.0){
        m_Width3 = 0.0;
        m_Depth3 = 0.0;
        m_Area3 = 3.141592*pow(m_Diameter3,2.0)/4.0;
    }    
    else    m_Area3 = m_Depth3*m_Width3;
    m_Volume3 = m_Area3*m_Height3;
    SpecimenData->Diameter[3] = m_Diameter3;
    SpecimenData->Width[3] = m_Width3;
    SpecimenData->Depth[3] = m_Depth3;
    SpecimenData->Height[3] = m_Height3;
    SpecimenData->Area[3] = m_Area3;
    SpecimenData->Volume[3] = m_Volume3;
    SpecimenData->VLDT1[3] = m_VLDT1_3;
    SpecimenData->VLDT2[3] = m_VLDT2_3;
//  -> present one
    SpecimenData->Diameter[0] = SpecimenData->Diameter[3];
    SpecimenData->Width[0] = SpecimenData->Width[3];
    SpecimenData->Depth[0] = SpecimenData->Depth[3];
    SpecimenData->Height[0] = SpecimenData->Height[3];
    SpecimenData->Area[0] = SpecimenData->Area[3];
    SpecimenData->Volume[0] = SpecimenData->Volume[3];
    SpecimenData->VLDT1[0] = SpecimenData->VLDT1[3];
    SpecimenData->VLDT2[0] = SpecimenData->VLDT2[3];
    Reflesh();
}

void CSpecimen::Reflesh()
{
    DigitShowContext* ctx = GetContext();
    auto SpecimenData = &ctx->specimen;

    m_Area0 = SpecimenData->Area[0];
    m_Area1 = SpecimenData->Area[1];
    m_Area2 = SpecimenData->Area[2];
    m_Area3 = SpecimenData->Area[3];
    m_Gs = SpecimenData->Gs;
    m_Height0 = SpecimenData->Height[0];
    m_Height1 = SpecimenData->Height[1];
    m_Height2 = SpecimenData->Height[2];
    m_Height3 = SpecimenData->Height[3];
    m_MembraneE = SpecimenData->MembraneModulus;
    m_MembraneT = SpecimenData->MembraneThickness;
    m_Diameter0 = SpecimenData->Diameter[0];
    m_Diameter1 = SpecimenData->Diameter[1];
    m_Diameter2 = SpecimenData->Diameter[2];
    m_Diameter3 = SpecimenData->Diameter[3];
    m_RodArea = SpecimenData->RodArea;
    m_RodWeight = SpecimenData->RodWeight;
    m_Volume0 = SpecimenData->Volume[0];
    m_Volume1 = SpecimenData->Volume[1];
    m_Volume2 = SpecimenData->Volume[2];
    m_Volume3 = SpecimenData->Volume[3];
    m_Weight0 = SpecimenData->Weight[0];
    m_Weight1 = SpecimenData->Weight[1];    
    m_Weight2 = SpecimenData->Weight[2];
    m_Weight3 = SpecimenData->Weight[3];
    m_Depth0 = SpecimenData->Depth[0];
    m_Depth1 = SpecimenData->Depth[1];
    m_Depth2 = SpecimenData->Depth[2];
    m_Depth3 = SpecimenData->Depth[3];    
    m_Width0 = SpecimenData->Width[0];
    m_Width1 = SpecimenData->Width[1];
    m_Width2 = SpecimenData->Width[2];
    m_Width3 = SpecimenData->Width[3];
    m_VLDT1_0 = SpecimenData->VLDT1[0];
    m_VLDT1_1 = SpecimenData->VLDT1[1];
    m_VLDT1_2 = SpecimenData->VLDT1[2];
    m_VLDT1_3 = SpecimenData->VLDT1[3];
    m_VLDT2_0 = SpecimenData->VLDT2[0];
    m_VLDT2_1 = SpecimenData->VLDT2[1];
    m_VLDT2_2 = SpecimenData->VLDT2[2];
    m_VLDT2_3 = SpecimenData->VLDT2[3];
    UpdateData(FALSE);
}

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
#include "Control_File.h"
#include "DigitShowContext.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CControl_File::CControl_File(CWnd* pParent)
    : CDialog(CControl_File::IDD, pParent)
{
    DigitShowContext* ctx = GetContext();
    int curNum = ctx->controlFile.CurrentNum;
    m_CurNum = curNum;
    m_CFNum = ctx->controlFile.Num[curNum];
    m_StepNum = curNum;
    m_SCFNum = ctx->controlFile.Num[curNum];
    m_CFPARA0 = ctx->controlFile.Para[curNum][0];
    m_CFPARA1 = ctx->controlFile.Para[curNum][1];
    m_CFPARA2 = ctx->controlFile.Para[curNum][2];
    m_CFPARA3 = ctx->controlFile.Para[curNum][3];
    m_CFPARA4 = ctx->controlFile.Para[curNum][4];
    m_CFPARA5 = ctx->controlFile.Para[curNum][5];
    m_CFPARA6 = ctx->controlFile.Para[curNum][6];
    m_CFPARA7 = ctx->controlFile.Para[curNum][7];
    m_CFPARA8 = ctx->controlFile.Para[curNum][8];
    m_CFPARA9 = ctx->controlFile.Para[curNum][9];
}

void CControl_File::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    DDX_Text(pDX, IDC_EDIT_CFPARA0, m_CFPARA0);
    DDX_Text(pDX, IDC_EDIT_CFPARA1, m_CFPARA1);
    DDX_Text(pDX, IDC_EDIT_CFPARA2, m_CFPARA2);
    DDX_Text(pDX, IDC_EDIT_CFPARA3, m_CFPARA3);
    DDX_Text(pDX, IDC_EDIT_CFPARA4, m_CFPARA4);
    DDX_Text(pDX, IDC_EDIT_CFPARA5, m_CFPARA5);
    DDX_Text(pDX, IDC_EDIT_CFPARA6, m_CFPARA6);
    DDX_Text(pDX, IDC_EDIT_CFPARA7, m_CFPARA7);
    DDX_Text(pDX, IDC_EDIT_CFPARA8, m_CFPARA8);
    DDX_Text(pDX, IDC_EDIT_CFPARA9, m_CFPARA9);
    DDX_Text(pDX, IDC_EDIT_StepNum, m_StepNum);
    DDV_MinMaxInt(pDX, m_StepNum, 0, 127);
    DDX_Text(pDX, IDC_EDIT_SCFNum, m_SCFNum);
    DDX_Text(pDX, IDC_EDIT_CurNum, m_CurNum);
    DDX_Text(pDX, IDC_EDIT_CFNum, m_CFNum);
}

BEGIN_MESSAGE_MAP(CControl_File, CDialog)
    ON_BN_CLICKED(IDC_BUTTON_Update, OnBUTTONUpdate)
    ON_BN_CLICKED(IDC_BUTTON_ReadFile, OnBUTTONReadFile)
    ON_BN_CLICKED(IDC_BUTTON_SaveFile, OnBUTTONSaveFile)
    ON_BN_CLICKED(IDC_BUTTON_Load, OnBUTTONLoad)
    ON_BN_CLICKED(IDC_CHECK_ChangeNo, OnCHECKChangeNo)
    ON_BN_CLICKED(IDC_BUTTON_StepDec, OnBUTTONStepDec)
    ON_BN_CLICKED(IDC_BUTTON_StepInc, OnBUTTONStepInc)
END_MESSAGE_MAP()

BOOL CControl_File::OnInitDialog()
{
    CDialog::OnInitDialog();
    
    CButton* myBTN1 = (CButton*)GetDlgItem(IDC_BUTTON_StepDec);
    CButton* myBTN2 = (CButton*)GetDlgItem(IDC_BUTTON_StepInc);
    myBTN1->EnableWindow(FALSE);
    myBTN2->EnableWindow(FALSE);
    CButton* chkbox1 = (CButton*)GetDlgItem(IDC_CHECK_ChangeNo);
    chkbox1->SetCheck(0);
    return TRUE;
}

void CControl_File::OnBUTTONLoad()
{
    UpdateData(TRUE);
    DigitShowContext* ctx = GetContext();
    m_SCFNum = ctx->controlFile.Num[m_StepNum];
    m_CFPARA0 = ctx->controlFile.Para[m_StepNum][0];
    m_CFPARA1 = ctx->controlFile.Para[m_StepNum][1];
    m_CFPARA2 = ctx->controlFile.Para[m_StepNum][2];
    m_CFPARA3 = ctx->controlFile.Para[m_StepNum][3];
    m_CFPARA4 = ctx->controlFile.Para[m_StepNum][4];
    m_CFPARA5 = ctx->controlFile.Para[m_StepNum][5];
    m_CFPARA6 = ctx->controlFile.Para[m_StepNum][6];
    m_CFPARA7 = ctx->controlFile.Para[m_StepNum][7];
    m_CFPARA8 = ctx->controlFile.Para[m_StepNum][8];
    m_CFPARA9 = ctx->controlFile.Para[m_StepNum][9];
    UpdateData(FALSE);
}

void CControl_File::OnBUTTONUpdate()
{
    UpdateData(TRUE);
    DigitShowContext* ctx = GetContext();
    ctx->controlFile.Num[m_StepNum] = m_SCFNum;
    ctx->controlFile.Para[m_StepNum][0] = m_CFPARA0;
    ctx->controlFile.Para[m_StepNum][1] = m_CFPARA1;
    ctx->controlFile.Para[m_StepNum][2] = m_CFPARA2;
    ctx->controlFile.Para[m_StepNum][3] = m_CFPARA3;
    ctx->controlFile.Para[m_StepNum][4] = m_CFPARA4;
    ctx->controlFile.Para[m_StepNum][5] = m_CFPARA5;
    ctx->controlFile.Para[m_StepNum][6] = m_CFPARA6;
    ctx->controlFile.Para[m_StepNum][7] = m_CFPARA7;
    ctx->controlFile.Para[m_StepNum][8] = m_CFPARA8;
    ctx->controlFile.Para[m_StepNum][9] = m_CFPARA9;
    UpdateData(FALSE);
}

void CControl_File::OnBUTTONReadFile()
{
    DigitShowContext* ctx = GetContext();
    ctx->controlFile.CurrentNum = 0;
    CString pFileName;
    FILE* FileCtlData;
    errno_t err;

    CFileDialog CtlLoadFile_dlg(TRUE, NULL, "*.ctl", OFN_FILEMUSTEXIST | OFN_HIDEREADONLY,
        "Control Files(*.ctl)|*.ctl| All Files(*.*)|*.*| |", NULL);

    if (CtlLoadFile_dlg.DoModal() == IDOK) {
        pFileName = CtlLoadFile_dlg.GetPathName();
        if ((err = fopen_s(&FileCtlData, (LPCSTR)pFileName, _T("r"))) == 0) {
            for (int i = 0; i < 128; i++) {
                fscanf_s(FileCtlData, _T("%d"), &ctx->controlFile.Num[i]);
                for (int j = 0; j < 10; j++) {
                    fscanf_s(FileCtlData, _T("%lf"), &ctx->controlFile.Para[i][j]);
                }
            }
            fclose(FileCtlData);
        }
    }
    m_CFNum = ctx->controlFile.Num[ctx->controlFile.CurrentNum];
    UpdateData(FALSE);
}

void CControl_File::OnBUTTONSaveFile()
{
    DigitShowContext* ctx = GetContext();
    CString pFileName;
    FILE* FileCtlData;
    errno_t err;

    CFileDialog CtlSaveFile_dlg(FALSE, NULL, "*.ctl", OFN_OVERWRITEPROMPT,
        "Control Files(*.ctl)|*.ctl| All Files(*.*)|*.*| |", NULL);

    if (CtlSaveFile_dlg.DoModal() == IDOK) {
        pFileName = CtlSaveFile_dlg.GetPathName();
        if ((err = fopen_s(&FileCtlData, (LPCSTR)pFileName, _T("w"))) == 0) {
            for (int i = 0; i < 128; i++) {
                fprintf(FileCtlData, "%d    ", ctx->controlFile.Num[i]);
                for (int j = 0; j < 10; j++) {
                    fprintf(FileCtlData, "%lf    ", ctx->controlFile.Para[i][j]);
                }
                fprintf(FileCtlData, "\n");
            }
            fclose(FileCtlData);
        }
    }
}

void CControl_File::OnCHECKChangeNo()
{
    DigitShowContext* ctx = GetContext();
    CButton* myBTN1 = (CButton*)GetDlgItem(IDC_BUTTON_StepDec);
    CButton* myBTN2 = (CButton*)GetDlgItem(IDC_BUTTON_StepInc);
    CButton* chkbox1 = (CButton*)GetDlgItem(IDC_CHECK_ChangeNo);
    if (chkbox1->GetCheck()) {
        if (ctx->controlFile.CurrentNum > 0) {
            myBTN1->EnableWindow(TRUE);
        }
        if (ctx->controlFile.CurrentNum < 127) {
            myBTN2->EnableWindow(TRUE);
        }
    }
    else {
        myBTN1->EnableWindow(FALSE);
        myBTN2->EnableWindow(FALSE);
    }
}

void CControl_File::OnBUTTONStepDec()
{
    DigitShowContext* ctx = GetContext();
    ctx->controlFile.CurrentNum--;
    ctx->NumCyclic = 0;
    ctx->TotalStepTime = 0.0;
    m_CurNum = ctx->controlFile.CurrentNum;
    m_CFNum = ctx->controlFile.Num[ctx->controlFile.CurrentNum];
    CButton* myBTN1 = (CButton*)GetDlgItem(IDC_BUTTON_StepDec);
    CButton* myBTN2 = (CButton*)GetDlgItem(IDC_BUTTON_StepInc);
    myBTN1->EnableWindow(ctx->controlFile.CurrentNum > 0);
    myBTN2->EnableWindow(ctx->controlFile.CurrentNum < 127);
    UpdateData(FALSE);
}

void CControl_File::OnBUTTONStepInc()
{
    DigitShowContext* ctx = GetContext();
    ctx->controlFile.CurrentNum++;
    ctx->NumCyclic = 0;
    ctx->TotalStepTime = 0.0;
    m_CurNum = ctx->controlFile.CurrentNum;
    m_CFNum = ctx->controlFile.Num[ctx->controlFile.CurrentNum];
    CButton* myBTN1 = (CButton*)GetDlgItem(IDC_BUTTON_StepDec);
    CButton* myBTN2 = (CButton*)GetDlgItem(IDC_BUTTON_StepInc);
    myBTN1->EnableWindow(ctx->controlFile.CurrentNum > 0);
    myBTN2->EnableWindow(ctx->controlFile.CurrentNum < 127);
    UpdateData(FALSE);
}

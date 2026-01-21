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
 * @file TransAdjustment.cpp
 * @brief Implementation of transducer adjustment dialog
 *
 * インプリメンテーション ファイル
 */
#include "StdAfx.h"

#include "TransAdjustment.h"
#include "Variables.hpp"
#include "resource.h"

using namespace variables;

/////////////////////////////////////////////////////////////////////////////
// CTransAdjustment ダイアログ

CTransAdjustment::CTransAdjustment(CWnd *pParent /*=NULL*/) : CDialog(CTransAdjustment::IDD, pParent)

{
    //{{AFX_DATA_INIT(CTransAdjustment)

    //}}AFX_DATA_INIT
}

void CTransAdjustment::DoDataExchange(CDataExchange *pDX)
{
    CDialog::DoDataExchange(pDX);
    //{{AFX_DATA_MAP(CTransAdjustment)
    DDX_Text(pDX, IDC_EDIT_FinalDisp, m_FinalDisp);
    DDX_Text(pDX, IDC_EDIT_InitialDisp, m_InitialDisp);
    DDX_Text(pDX, IDC_EDIT_FinalBullet, m_FinalBullet);
    DDX_Text(pDX, IDC_EDIT_InitialBullet, m_InitialBullet);
    //}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP_IGNORE_UNUSED_LOCAL_TYPEDEF(CTransAdjustment, CDialog)
//{{AFX_MSG_MAP(CTransAdjustment)
ON_BN_CLICKED(IDC_BUTTON_InitialDisp, &CTransAdjustment::OnBUTTONInitialDisp)
ON_BN_CLICKED(IDC_BUTTON_EndDisp, &CTransAdjustment::OnBUTTONEndDisp)
ON_BN_CLICKED(IDC_BUTTON_InitialBullet, &CTransAdjustment::OnBUTTONInitialBullet)
ON_BN_CLICKED(IDC_BUTTON_EndBullet, &CTransAdjustment::OnBUTTONEndBullet)
ON_BN_CLICKED(IDC_BUTTON_UpdateDisp, &CTransAdjustment::OnBUTTONUpdateDisp)
ON_BN_CLICKED(IDC_BUTTON_UpdateBullet, &CTransAdjustment::OnBUTTONUpdateBullet)
//}}AFX_MSG_MAP
END_MESSAGE_MAP_IGNORE_UNUSED_LOCAL_TYPEDEF()

/////////////////////////////////////////////////////////////////////////////
// CTransAdjustment メッセージ ハンドラ

void CTransAdjustment::OnBUTTONInitialDisp()
{
    // TODO: この位置にコントロール通知ハンドラ用のコードを追加してください
    m_InitialDisp = variables::Phyout[1];
    UpdateData(FALSE);
    GetDlgItem(IDC_BUTTON_UpdateDisp)->EnableWindow(TRUE);
}

void CTransAdjustment::OnBUTTONEndDisp()
{
    // TODO: この位置にコントロール通知ハンドラ用のコードを追加してください
    m_FinalDisp = variables::Phyout[1];
    UpdateData(FALSE);
    GetDlgItem(IDC_BUTTON_UpdateDisp)->EnableWindow(TRUE);
}

void CTransAdjustment::OnBUTTONUpdateDisp()
{
    // TODO: この位置にコントロール通知ハンドラ用のコードを追加してください
    UpdateData(TRUE);
    variables::Cal_c[1] = variables::Cal_c[1] + (m_InitialDisp - m_FinalDisp);
    GetDlgItem(IDC_BUTTON_UpdateDisp)->EnableWindow(FALSE);
}

void CTransAdjustment::OnBUTTONInitialBullet()
{
    // TODO: この位置にコントロール通知ハンドラ用のコードを追加してください
    m_InitialBullet = variables::Phyout[4];
    UpdateData(FALSE);
    GetDlgItem(IDC_BUTTON_UpdateBullet)->EnableWindow(TRUE);
}

void CTransAdjustment::OnBUTTONEndBullet()
{
    // TODO: この位置にコントロール通知ハンドラ用のコードを追加してください
    m_FinalBullet = variables::Phyout[4];
    UpdateData(FALSE);
    GetDlgItem(IDC_BUTTON_UpdateBullet)->EnableWindow(TRUE);
}

void CTransAdjustment::OnBUTTONUpdateBullet()
{
    // TODO: この位置にコントロール通知ハンドラ用のコードを追加してください
    UpdateData(TRUE);
    variables::Cal_c[4] = variables::Cal_c[4] + (m_InitialBullet - m_FinalBullet);
    GetDlgItem(IDC_BUTTON_UpdateBullet)->EnableWindow(FALSE);
}

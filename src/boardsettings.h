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
 * @file BoardSettings.h
 * @brief Header file for board settings dialog
 *
 * ヘッダー ファイル
 */

#pragma once
#if !defined(AFX_BoardSettings_H__7C23E5A9_7E1B_4821_857B_F8600C2F6EA6__INCLUDED_)
#define AFX_BoardSettings_H__7C23E5A9_7E1B_4821_857B_F8600C2F6EA6__INCLUDED_

#include "DigitShowDSTDoc.h"
#include "resource.h"

/////////////////////////////////////////////////////////////////////////////
/**
 * @class CBoardSettings
 * @brief Dialog class for hardware board configuration
 *
 * Displays and manages A/D and D/A board settings (method, resolution, range, channels).
 * CBoardSettings ダイアログ
 */
class CBoardSettings : public CDialog
{
    // コンストラクション
  public:
    /**
     * @brief Standard constructor
     * @param pParent Pointer to parent window (default NULL)
     */
    CBoardSettings(CWnd *pParent = NULL);

    CDigitShowDSTDoc *pDoc{}; /**< Pointer to document */

    // ダイアログ データ
    //{{AFX_DATA(CBoardSettings)
    enum
    {
        IDD = IDD_BoardSettings /**< Dialog resource ID */
    };
    CStringW m_ADMethod0;     /**< A/D board 0 sampling method */
    CStringW m_ADMethod1;     /**< A/D board 1 sampling method */
    CStringW m_ADResolution0; /**< A/D board 0 resolution (bits) */
    CStringW m_ADResolution1; /**< A/D board 1 resolution (bits) */
    CStringW m_ADRange0;      /**< A/D board 0 input range */
    CStringW m_ADRange1;      /**< A/D board 1 input range */
    CStringW m_ADMaxChannel0; /**< A/D board 0 maximum channel number */
    CStringW m_ADMaxChannel1; /**< A/D board 1 maximum channel number */
    CStringW m_DAMaxChannel0; /**< D/A board 0 maximum channel number */
    CStringW m_DARange0;      /**< D/A board 0 output range */
    CStringW m_DAResolution0; /**< D/A board 0 resolution (bits) */
                              //}}AFX_DATA

    // オーバーライド
    // ClassWizard は仮想関数のオーバーライドを生成します。
    //{{AFX_VIRTUAL(CBoardSettings)
  protected:
    /**
     * @brief Data exchange function for DDX/DDV support
     * @param pDX Pointer to CDataExchange object
     */
    void DoDataExchange(CDataExchange *pDX) override;
    //}}AFX_VIRTUAL

    // インプリメンテーション
  protected:
    // 生成されたメッセージ マップ関数
    //{{AFX_MSG(CBoardSettings)
    /**
     * @brief Initialize dialog with hardware settings
     * @return TRUE if initialization succeeds
     */
    BOOL OnInitDialog() override;
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ は前行の直前に追加の宣言を挿入します。

#endif // !defined(AFX_BoardSettings_H__7C23E5A9_7E1B_4821_857B_F8600C2F6EA6__INCLUDED_)

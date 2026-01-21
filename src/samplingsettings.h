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
 * @file SamplingSettings.h
 * @brief Header file for sampling settings dialog
 *
 * ヘッダー ファイル
 */

#pragma once
#if !defined(AFX_SAMPLINGSETTINGS_H__A100D0D8_4CDC_4617_978A_A28957A25A20__INCLUDED_)
#define AFX_SAMPLINGSETTINGS_H__A100D0D8_4CDC_4617_978A_A28957A25A20__INCLUDED_

#include "resource.h"

/////////////////////////////////////////////////////////////////////////////
/**
 * @class CSamplingSettings
 * @brief Dialog class for data sampling configuration
 *
 * Manages timer intervals, sampling parameters, and memory allocation settings.
 * CSamplingSettings ダイアログ
 */
class CSamplingSettings : public CDialog
{
    // コンストラクション
  public:
    /**
     * @brief Standard constructor
     * @param pParent Pointer to parent window (default NULL)
     */
    CSamplingSettings(CWnd *pParent = NULL);

    // ダイアログ データ
    //{{AFX_DATA(CSamplingSettings)
    enum
    {
        IDD = IDD_SamplingSettings /**< Dialog resource ID */
    };
    int64_t m_TimeInterval1{0}; /**< Timer 1 interval (UI refresh) [ms] */
    int64_t m_TimeInterval2{0}; /**< Timer 2 interval (control) [ms] */
    int64_t m_TimeInterval3{0}; /**< Timer 3 interval (logging) [ms] */
    int m_AvSmplNum{0};         /**< Number of samples for averaging */
    size_t m_Channels{0};       /**< Number of A/D channels */
                                //}}AFX_DATA

    // オーバーライド
    // ClassWizard は仮想関数のオーバーライドを生成します。
    //{{AFX_VIRTUAL(CSamplingSettings)
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
    //{{AFX_MSG(CSamplingSettings)
    /**
     * @brief Check and validate sampling settings
     */
    afx_msg void OnBUTTONCheck();

    /**
     * @brief Handle OK button click
     */
    void OnOK() override;

    /**
     * @brief Initialize dialog with current settings
     * @return TRUE if initialization succeeds
     */
    BOOL OnInitDialog() override;
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ は前行の直前に追加の宣言を挿入します。

#endif // !defined(AFX_SAMPLINGSETTINGS_H__A100D0D8_4CDC_4617_978A_A28957A25A20__INCLUDED_)

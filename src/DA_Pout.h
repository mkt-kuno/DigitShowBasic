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
 * @file DA_Pout.h
 * @brief Header file for D/A physical output dialog
 *
 * ヘッダー ファイル
 */

#pragma once
#if !defined(AFX_DA_POUT_H__0001B6E0_A540_11D5_B813_0020E0636BD5__INCLUDED_)
#define AFX_DA_POUT_H__0001B6E0_A540_11D5_B813_0020E0636BD5__INCLUDED_

#include "DigitShowDSTDoc.h"
#include "resource.h"
#include <array>

/////////////////////////////////////////////////////////////////////////////
/**
 * @class CDA_Pout
 * @brief Dialog class for D/A physical output control
 *
 * Allows manual control of D/A output channels with physical unit conversion.
 * CDA_Pout ダイアログ
 */
class CDA_Pout : public CDialog
{
    // コンストラクション
  public:
    /**
     * @brief Standard constructor
     * @param pParent Pointer to parent window (default NULL)
     */
    CDA_Pout(CWnd *pParent = NULL);

    CDigitShowDSTDoc *pDoc2{}; /**< Pointer to document */
    // ダイアログ データ
    //{{AFX_DATA(CDA_Pout)
    enum
    {
        IDD = IDD_DA_Pout /**< Dialog resource ID */
    };

    // Array size for D/A channels
    static constexpr int CHANNELS_DA_POUT = 8; /**< Number of D/A output channels */

    // D/A output data consolidated into arrays
    std::array<double, CHANNELS_DA_POUT> m_DACala{};   /**< D/A calibration coefficient a for channels 0-7 */
    std::array<double, CHANNELS_DA_POUT> m_DACalb{};   /**< D/A calibration coefficient b for channels 0-7 */
    std::array<double, CHANNELS_DA_POUT> m_DAPvalue{}; /**< D/A physical output value for channels 0-7 */
    std::array<float, CHANNELS_DA_POUT> m_DAVout{};    /**< D/A voltage output [V] for channels 0-7 */
                                                       //}}AFX_DATA

    // オーバーライド
    // ClassWizard は仮想関数のオーバーライドを生成します。
    //{{AFX_VIRTUAL(CDA_Pout)
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
    //{{AFX_MSG(CDA_Pout)
    /** @brief Execute D/A output with current values */
    afx_msg void OnBUTTONDAOutput();

    /** @brief Calculate voltage for channel 0 from physical value */
    afx_msg void OnBUTTONCalculation00();

    /** @brief Calculate voltage for channel 1 from physical value */
    afx_msg void OnBUTTONCalculation01();

    /** @brief Calculate voltage for channel 2 from physical value */
    afx_msg void OnBUTTONCalculation02();

    /** @brief Calculate voltage for channel 3 from physical value */
    afx_msg void OnBUTTONCalculation03();

    /** @brief Calculate voltage for channel 4 from physical value */
    afx_msg void OnBUTTONCalculation04();

    /** @brief Calculate voltage for channel 5 from physical value */
    afx_msg void OnBUTTONCalculation05();

    /** @brief Calculate voltage for channel 6 from physical value */
    afx_msg void OnBUTTONCalculation06();

    /** @brief Calculate voltage for channel 7 from physical value */
    afx_msg void OnBUTTONCalculation07();
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ は前行の直前に追加の宣言を挿入します。

#endif // !defined(AFX_DA_POUT_H__0001B6E0_A540_11D5_B813_0020E0636BD5__INCLUDED_)

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
 * @file CalibrationAmp.h
 * @brief Header file for amplifier calibration dialog
 *
 * ヘッダー ファイル
 */

#pragma once
#if !defined(AFX_CALIBRATIONAMP_H__C9D79E38_8396_494A_8202_AC899D8267E6__INCLUDED_)
#define AFX_CALIBRATIONAMP_H__C9D79E38_8396_494A_8202_AC899D8267E6__INCLUDED_

#include "DigitShowDSTDoc.h"
#include "Variables.hpp"
#include "resource.h"

inline size_t AmpID;

/////////////////////////////////////////////////////////////////////////////
/**
 * @class CCalibrationAmp
 * @brief Dialog class for sensor amplifier calibration
 *
 * Performs two-point calibration for sensor amplifiers (base and offset).
 * CCalibrationAmp ダイアログ
 */
class CCalibrationAmp : public CDialog
{
    // コンストラクション
  public:
    /**
     * @brief Standard constructor
     * @param pParent Pointer to parent window (default NULL)
     */
    CCalibrationAmp(CWnd *pParent = NULL);

    // ダイアログ データ
    //{{AFX_DATA(CCalibrationAmp)
    enum
    {
        IDD = IDD_CalibrationAmp /**< Dialog resource ID */
    };
    size_t m_AmpNo;      /**< Amplifier channel number */
    float m_AmpPB{0.0f}; /**< Physical value at base point */
    float m_AmpVB{0.0f}; /**< Voltage value at base point [V] */
    float m_AmpVO{0.0f}; /**< Voltage value at offset point [V] */
    float m_AmpPO{0.0f}; /**< Physical value at offset point */
                         //}}AFX_DATA

    // オーバーライド
    // ClassWizard は仮想関数のオーバーライドを生成します。
    //{{AFX_VIRTUAL(CCalibrationAmp)
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
    //{{AFX_MSG(CCalibrationAmp)
    /** @brief Initialize dialog */
    BOOL OnInitDialog() override;

    /** @brief Capture base point calibration values */
    afx_msg void OnBUTTONAmpBase();

    /** @brief Capture offset point calibration values */
    afx_msg void OnBUTTONAmpOffset();

    /** @brief Calculate and update calibration coefficients */
    afx_msg void OnBUTTONAmpUpdate();
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ は前行の直前に追加の宣言を挿入します。

#endif // !defined(AFX_CALIBRATIONAMP_H__C9D79E38_8396_494A_8202_AC899D8267E6__INCLUDED_)

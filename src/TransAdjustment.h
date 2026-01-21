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
 * @file TransAdjustment.h
 * @brief Header file for transducer adjustment dialog
 *
 * ヘッダー ファイル
 */

#pragma once
#if !defined(AFX_TRANSADJUSTMENT_H__9C426B86_E15E_42FA_9A0E_A35AF4BFAD55__INCLUDED_)
#define AFX_TRANSADJUSTMENT_H__9C426B86_E15E_42FA_9A0E_A35AF4BFAD55__INCLUDED_

#include "resource.h"

/////////////////////////////////////////////////////////////////////////////
/**
 * @class CTransAdjustment
 * @brief Dialog class for transducer offset adjustment
 *
 * Allows setting initial and final reference values for displacement and bullet transducers.
 * CTransAdjustment ダイアログ
 */
class CTransAdjustment : public CDialog
{
    // コンストラクション
  public:
    /**
     * @brief Standard constructor
     * @param pParent Pointer to parent window (default NULL)
     */
    CTransAdjustment(CWnd *pParent = NULL);

    // ダイアログ データ
    //{{AFX_DATA(CTransAdjustment)
    enum
    {
        IDD = IDD_TransAdjustment /**< Dialog resource ID */
    };
    double m_FinalDisp{0.0};     /**< Final displacement reference value [mm] */
    double m_InitialDisp{0.0};   /**< Initial displacement reference value [mm] */
    double m_FinalBullet{0.0};   /**< Final bullet transducer reference value [mm] */
    double m_InitialBullet{0.0}; /**< Initial bullet transducer reference value [mm] */
                                 //}}AFX_DATA

    // オーバーライド
    // ClassWizard は仮想関数のオーバーライドを生成します。
    //{{AFX_VIRTUAL(CTransAdjustment)
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
    //{{AFX_MSG(CTransAdjustment)
    /** @brief Capture initial displacement reference value */
    afx_msg void OnBUTTONInitialDisp();

    /** @brief Capture final displacement reference value */
    afx_msg void OnBUTTONEndDisp();

    /** @brief Capture initial bullet transducer reference value */
    afx_msg void OnBUTTONInitialBullet();

    /** @brief Capture final bullet transducer reference value */
    afx_msg void OnBUTTONEndBullet();

    /** @brief Update displacement offset in document */
    afx_msg void OnBUTTONUpdateDisp();

    /** @brief Update bullet transducer offset in document */
    afx_msg void OnBUTTONUpdateBullet();
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ は前行の直前に追加の宣言を挿入します。

#endif // !defined(AFX_TRANSADJUSTMENT_H__9C426B86_E15E_42FA_9A0E_A35AF4BFAD55__INCLUDED_)

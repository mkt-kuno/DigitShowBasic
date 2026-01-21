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
 * @file DigitShowDSTView.h
 * @brief Declaration and interface definition for CDigitShowDSTView class
 *
 * CDigitShowDSTView クラスの宣言およびインターフェイスの定義をします。
 */
/////////////////////////////////////////////////////////////////////////////

#pragma once
#if !defined(AFX_DIGITSHOWDSTVIEW_H__44354AEA_B72E_464C_AAD2_A696AA19EF89__INCLUDED_)
#define AFX_DIGITSHOWDSTVIEW_H__44354AEA_B72E_464C_AAD2_A696AA19EF89__INCLUDED_

#include "resource.h"
#include <array>
#include <chrono>

/**
 * @class CDigitShowDSTView
 * @brief Main view class for DigitShowDST application
 *
 * Form view that displays sensor readings, controls timers, and manages UI updates.
 * シリアライズ機能のみから作成します。
 */
class CDigitShowDSTView : public CFormView
{
  protected:
    /**
     * @brief Default constructor (created only from serialization)
     */
    CDigitShowDSTView();
    DECLARE_DYNCREATE(CDigitShowDSTView)

  public:
    //{{AFX_DATA(CDigitShowDSTView)
    enum
    {
        IDD = IDD_DIGITSHOWDST_FORM /**< Dialog resource ID */
    };

    // Array sizes for channel data
    static constexpr int CHANNELS_VOUT = 8;   /**< Number of voltage output channels */
    static constexpr int CHANNELS_PHYOUT = 8; /**< Number of physical output channels */
    static constexpr int CHANNELS_PARA = 14;  /**< Number of calculated parameters */

  private:
    // Resource ID arrays for DDX_Text loops
    static constexpr std::array<int, CHANNELS_VOUT> IDS_VOUT = {IDC_EDIT_Vout00, IDC_EDIT_Vout01, IDC_EDIT_Vout02,
                                                                IDC_EDIT_Vout03, IDC_EDIT_Vout04, IDC_EDIT_Vout05,
                                                                IDC_EDIT_Vout06, IDC_EDIT_Vout07};
    static constexpr std::array<int, CHANNELS_PHYOUT> IDS_PHYOUT = {
        IDC_EDIT_Phyout00, IDC_EDIT_Phyout01, IDC_EDIT_Phyout02, IDC_EDIT_Phyout03,
        IDC_EDIT_Phyout04, IDC_EDIT_Phyout05, IDC_EDIT_Phyout06, IDC_EDIT_Phyout07};
    static constexpr std::array<int, CHANNELS_PARA> IDS_PARA = {
        IDC_EDIT_Para00, IDC_EDIT_Para01, IDC_EDIT_Para02, IDC_EDIT_Para03, IDC_EDIT_Para04,
        IDC_EDIT_Para07, IDC_EDIT_Para08, IDC_EDIT_Para09, IDC_EDIT_Para10, IDC_EDIT_Para11,
        IDC_EDIT_Para12, IDC_EDIT_Para14, IDC_EDIT_Para15, IDC_EDIT_Para13};

    std::chrono::steady_clock::time_point last_tick = std::chrono::steady_clock::now();

  public:
    // Display strings consolidated into arrays
    std::array<CStringW, CHANNELS_VOUT> m_Vout;     /**< Display voltage for channels 0-31 [V] */
    std::array<CStringW, CHANNELS_PHYOUT> m_Phyout; /**< Display physical value for channels 0-31 */
    std::array<CStringW, CHANNELS_PARA> m_Para;     /**< Display calculated parameters 0-15 */

    CStringW m_NowTime;       /**< Current time display string */
    int64_t m_SeqTime{0};     /**< Sequential time counter [s] */
    int64_t m_SamplingTime{}; /**< Sampling time display [ms] */
    CStringW m_FileName;      /**< Data file name display */

    //}}AFX_DATA

    // アトリビュート
  public:
    /**
     * @brief Get pointer to document
     * @return Pointer to CDigitShowDSTDoc
     */
    CDigitShowDSTDoc *GetDocument();

    // オペレーション
  public:
    CBrush *m_pEditBrush;   /**< Brush for edit controls */
    CBrush *m_pStaticBrush; /**< Brush for static controls */
    CBrush *m_pDlgBrush;    /**< Brush for dialog background */
                            // オーバーライド
                            // ClassWizard は仮想関数のオーバーライドを生成します。
                            //{{AFX_VIRTUAL(CDigitShowDSTView)
  public:
    /**
     * @brief Pre-create window initialization
     * @param cs Create structure
     * @return TRUE if creation should proceed
     */
    BOOL PreCreateWindow(CREATESTRUCT &cs) override;

  protected:
    /**
     * @brief Data exchange function for DDX/DDV support
     * @param pDX Pointer to CDataExchange object
     */
    void DoDataExchange(CDataExchange *pDX) override;

    /**
     * @brief Called once after construction (初回のみ呼び出される)
     */
    void OnInitialUpdate() override;

    /**
     * @brief Default window procedure override
     * @param message Window message
     * @param wParam Message parameter
     * @param lParam Message parameter
     * @return Message processing result
     */
    virtual LRESULT DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam) override;
    //}}AFX_VIRTUAL

    // インプリメンテーション
  public:
    /**
     * @brief Update UI display with current data
     */
    void ShowData();

    /**
     * @brief Destructor
     */
    ~CDigitShowDSTView() override;
#ifdef _DEBUG
    /** @brief Assert validity of object state */
    virtual void AssertValid() const override;

    /** @brief Dump object state to diagnostic context */
    virtual void Dump(CDumpContext &dc) const override;
#endif

  protected:
    // 生成されたメッセージ マップ関数
  protected:
    //{{AFX_MSG(CDigitShowDSTView)
    /**
     * @brief Handle control color messages
     * @param pDC Device context
     * @param pWnd Control window
     * @param nCtlColor Control color type
     * @return Brush handle for control
     */
    afx_msg HBRUSH OnCtlColor(CDC *pDC, CWnd *pWnd, UINT nCtlColor);

    /**
     * @brief Handle timer events (UI refresh, control, logging)
     * @param nIDEvent Timer ID
     */
    afx_msg void OnTimer(UINT_PTR nIDEvent);

    /** @brief Turn off control mode */
    afx_msg void OnBUTTONCtrlOff();

    /** @brief Turn on control mode */
    afx_msg void OnBUTTONCtrlOn();

    /** @brief Handle window destruction */
    afx_msg void OnDestroy();

    /** @brief Save intermediate data point */
    afx_msg void OnBUTTONInterceptSave();

    /** @brief Set timer intervals */
    afx_msg void OnBUTTONSetTimeInterval();
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG // DigitShowDSTView.cpp ファイルがデバッグ環境の時使用されます。
inline CDigitShowDSTDoc *CDigitShowDSTView::GetDocument()
{
    return dynamic_cast<CDigitShowDSTDoc *>(m_pDocument);
}
#endif

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ は前行の直前に追加の宣言を挿入します。

#endif // !defined(AFX_DIGITSHOWDSTVIEW_H__44354AEA_B72E_464C_AAD2_A696AA19EF89__INCLUDED_)

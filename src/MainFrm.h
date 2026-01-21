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
 * @file MainFrm.h
 * @brief Declaration and interface definition for CMainFrame class
 *
 * CMainFrame クラスの宣言およびインターフェイスの定義をします。
 */
/////////////////////////////////////////////////////////////////////////////

#pragma once
#if !defined(AFX_MAINFRM_H__AC57187F_9544_4DFA_BC03_5FC6D5E29FB4__INCLUDED_)
#define AFX_MAINFRM_H__AC57187F_9544_4DFA_BC03_5FC6D5E29FB4__INCLUDED_

/**
 * @class CMainFrame
 * @brief Main frame window class for DigitShowDST application
 *
 * Manages menu commands and child dialogs.
 * シリアライズ機能のみから作成します。
 */
class CMainFrame : public CFrameWnd
{

  protected:
    /**
     * @brief Default constructor (created only from serialization)
     */
    CMainFrame() = default;
    DECLARE_DYNCREATE(CMainFrame)

    // アトリビュート
  public:
    // オペレーション
  public:
    // オーバーライド
    // ClassWizard は仮想関数のオーバーライドを生成します。
    //{{AFX_VIRTUAL(CMainFrame)
    /**
     * @brief Pre-create window initialization
     * @param cs Create structure
     * @return TRUE if creation should proceed
     */
    BOOL PreCreateWindow(CREATESTRUCT &cs) override;
    //}}AFX_VIRTUAL

    // インプリメンテーション
  public:
    /**
     * @brief Destructor
     */
    ~CMainFrame() override = default;
#ifdef _DEBUG
    /** @brief Assert validity of object state */
    virtual void AssertValid() const override;

    /** @brief Dump object state to diagnostic context */
    virtual void Dump(CDumpContext &dc) const override;
#endif

    // 生成されたメッセージ マップ関数
  private:
    INT_PTR nResult = 0; /**< Dialog result for modal dialogs */

  protected:
    //{{AFX_MSG(CMainFrame)
    /** @brief Show About dialog */
    afx_msg void OnAppAbout();

    /** @brief Show board settings dialog */
    afx_msg void OnBoardSettings();

    /** @brief Show calibration factor dialog */
    afx_msg void OnCalibrationFactor();

    /** @brief Show D/A voltage output dialog */
    afx_msg void OnDAVout();

    /** @brief Show D/A physical output dialog */
    afx_msg void OnDAPout();

    /** @brief Show D/A channel calibration dialog */
    afx_msg void OnDAChannel();

    /** @brief Show control file dialog */
    afx_msg void OnControlFile();

    /** @brief Show sampling settings dialog */
    afx_msg void OnSamplingSettings();

    /** @brief Show transducer adjustment dialog */
    afx_msg void OnTransAdjustment();

    // Deprecated individual control dialogs (removed):
    // OnControlID, OnControlConsolidation, OnControlMLoading,
    // OnControlCLoading, OnControlPreConsolidation, OnControlLinearStressPath
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ は前行の直前に追加の宣言を挿入します。

#endif // !defined(AFX_MAINFRM_H__AC57187F_9544_4DFA_BC03_5FC6D5E29FB4__INCLUDED_)

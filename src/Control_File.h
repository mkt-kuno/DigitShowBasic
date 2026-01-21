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
 * @file Control_File.h
 * @brief Header file for file-based control script dialog
 *
 * ヘッダー ファイル
 */

#pragma once
#if !defined(AFX_CONTROL_FILE_H__EF6A40AB_D157_4D17_BCC0_884EC690BC71__INCLUDED_)
#define AFX_CONTROL_FILE_H__EF6A40AB_D157_4D17_BCC0_884EC690BC71__INCLUDED_

#include "control/control.hpp"
#include "resource.h"
#include <string>

/////////////////////////////////////////////////////////////////////////////
/**
 * @class CControl_File
 * @brief Dialog class for control script file management
 *
 * Manages loading, editing, and executing .ctl control script files.
 * CControl_File ダイアログ
 */
class CControl_File : public CDialog
{
    // コンストラクション
  public:
    /**
     * @brief Standard constructor
     * @param pParent Pointer to parent window (default NULL)
     */
    CControl_File(CWnd *pParent = NULL);

    // ダイアログ データ
    //{{AFX_DATA(CControl_File)
    enum
    {
        IDD = IDD_Control_File /**< Dialog resource ID */
    };

    size_t m_EditingStepIndex{};                          /**< Editing target step index */
    CStringW m_YamlText{};                                /**< YAML text for the current editing step */
    CStringW m_ValidationText{};                          /**< Validation status message */
    CStringW m_FilePath{};                                /**< Last imported/exported file path */
    CStringW m_ExecLabel{};                               /**< Label for execution section */
    CStringW m_EditLabel{};                               /**< Label for editor section */
    bool m_HasUnsavedEdits{false};                        /**< True when editor has unsaved edits */
    bool m_LastValidationOk{false};                       /**< True when last validation passed */
    static constexpr UINT_PTR kYamlValidationTimerId = 1; /**< Timer ID for debounced validation */
    static constexpr UINT kYamlValidationDelayMs = 400;   /**< Delay before validating after edits */
                                                          //}}AFX_DATA

    // オーバーライド
    // ClassWizard は仮想関数のオーバーライドを生成します。
    //{{AFX_VIRTUAL(CControl_File)
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
    //{{AFX_MSG(CControl_File)
    afx_msg void OnButtonUpdate();
    afx_msg void OnButtonClear();
    afx_msg void OnButtonImport();
    afx_msg void OnButtonExport();
    afx_msg void OnButtonExecPrev();
    afx_msg void OnButtonExecNext();
    afx_msg void OnButtonEditPrev();
    afx_msg void OnButtonEditNext();
    afx_msg void OnYamlChanged();
    afx_msg void OnYamlKillFocus();
    afx_msg HBRUSH OnCtlColor(CDC *pDC, CWnd *pWnd, UINT nCtlColor);
    afx_msg void OnButtonStart();
    afx_msg void OnButtonStop();
    afx_msg void OnButtonPause();
    afx_msg void OnButtonResume();
    afx_msg void OnTimer(UINT_PTR nIDEvent);

    /**
     * @brief Initialize dialog
     * @return TRUE if initialization succeeds
     */
    BOOL OnInitDialog() override;
    void OnOK() override;
    void OnCancel() override;
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()

  private:
    void UpdateExecLabel();
    void UpdateEditLabel();
    void UpdateNavEnabled();
    void LoadStepIntoEditor(size_t index);
    bool ValidateYaml(control::ControlParams &out_params);
    bool ConfirmUpdateWhenActive(size_t target_step);
    bool IsControlRunning() const;
    bool ConfirmStepChangeWhenRunning() const;
    void MarkDirty();
    void SetValidationMessage(const CStringW &msg, bool ok);
    void RestartYamlValidationTimer();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ は前行の直前に追加の宣言を挿入します。

#endif // !defined(AFX_CONTROL_FILE_H__EF6A40AB_D157_4D17_BCC0_884EC690BC71__INCLUDED_)

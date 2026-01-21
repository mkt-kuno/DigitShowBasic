/*
 * DigitShowDST - Direct Shear Test Machine Control Software
 * Copyright (C) 2025 Takuto ISHII
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
 * @file AppInfoDlg.h
 * @brief Application information dialog header
 *
 * Displays git version information and library versions used in the application.
 */

#pragma once

#include "resource.h"
#include <afxwin.h>

/**
 * @class CAppInfoDlg
 * @brief About dialog showing application and library version information
 */
class CAppInfoDlg : public CDialogEx
{
  public:
    /**
     * @brief Constructor
     * @param pParent Parent window pointer (default nullptr)
     */
    explicit CAppInfoDlg(CWnd *pParent = nullptr) noexcept;

    /**
     * @brief Dialog IDD
     */
    enum
    {
        IDD = IDD_AppInfo
    };

  protected:
    /**
     * @brief Initialize dialog
     * @return TRUE if initialization succeeds
     */
    BOOL OnInitDialog() override;

    /**
     * @brief Data exchange for dialog controls
     * @param pDX Data exchange object
     */
    void DoDataExchange(CDataExchange *pDX) override;

    /**
     * @brief Handle Copy Diff button click
     */
    afx_msg void OnCopyDiffToClipboard();

    DECLARE_MESSAGE_MAP()

  private:
    /**
     * @brief Build the complete about text with version information
     * @return Formatted about text
     */
    CString BuildAboutText() const;

    /**
     * @brief Copy text to Windows clipboard
     * @param text Text to copy
     * @return true if successful
     */
    bool CopyToClipboard(const CString &text);
};

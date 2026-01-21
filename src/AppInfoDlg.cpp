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
 * @file AppInfoDlg.cpp
 * @brief Application information dialog implementation
 *
 * Implements the About dialog showing git version and library information.
 */

#include "StdAfx.h"

#include "../generated/git_version.hpp"
#include "AppInfoDlg.h"
#include <sstream>
#include <string>
#include <vector>

// Conditionally include library headers for version detection
#if __has_include(<sqlite3.h>)
#include <sqlite3.h>
#endif

#if __has_include(<zlib.h>)
#include <zlib.h>
#endif

#if __has_include(<spdlog/version.h>)
#include <spdlog/version.h>
#endif

#if __has_include(<fmt/core.h>)
#include <fmt/core.h>
#endif

#if __has_include(<ryml/ryml.hpp>)
#include <ryml/ryml.hpp>
#endif

#if __has_include(<modbus/modbus.h>)
#include <modbus/modbus.h>
#endif

#if defined(_DEBUG) && !defined(__clang__)
#define new DEBUG_NEW
#endif

BEGIN_MESSAGE_MAP_IGNORE_UNUSED_LOCAL_TYPEDEF(CAppInfoDlg, CDialogEx)
ON_BN_CLICKED(IDC_AppInfo_CopyDiff2ClipBoard, &CAppInfoDlg::OnCopyDiffToClipboard)
END_MESSAGE_MAP_IGNORE_UNUSED_LOCAL_TYPEDEF()

CAppInfoDlg::CAppInfoDlg(CWnd *pParent) noexcept : CDialogEx(IDD_AppInfo, pParent)
{
}

void CAppInfoDlg::DoDataExchange(CDataExchange *pDX)
{
    CDialogEx::DoDataExchange(pDX);
}

BOOL CAppInfoDlg::OnInitDialog()
{
    CDialogEx::OnInitDialog();

    // Build and set the about text
    CString aboutText = BuildAboutText();
    SetDlgItemText(IDC_AppInfo_FreeText, aboutText);

    // Enable/disable Copy Diff button based on diff availability
    constexpr auto diffLen = static_cast<int>(std::char_traits<char>::length(git_version::DIFF_STR));
    CWnd *pButton = GetDlgItem(IDC_AppInfo_CopyDiff2ClipBoard);
    if (pButton != nullptr)
    {
        constexpr bool flag = diffLen >= 10;
        pButton->EnableWindow(flag);
    }

    return TRUE;
}

CString CAppInfoDlg::BuildAboutText() const
{
    std::ostringstream oss;

    // Application version information
    oss << "DigitShowDST\r\n";
    oss << "Version: " << git_version::VERSION.data() << "\r\n";
    oss << "Commit: " << git_version::COMMIT_SHORT.data() << "\r\n";
    oss << "Full Commit: " << git_version::COMMIT_FULL.data() << "\r\n";
    oss << "Branch: " << git_version::BRANCH.data() << "\r\n";

    if (git_version::REMOTE_URL.length() > 0)
    {
        oss << "Remote: " << git_version::REMOTE_URL.data() << "\r\n";
    }

    if (git_version::DIRTY)
    {
        oss << "Status: Working directory has uncommitted changes\r\n";
    }
    else
    {
        oss << "Status: Clean\r\n";
    }

    // Library versions
    oss << "\r\n--- Library Versions ---\r\n";

    // SQLite3
#if __has_include(<sqlite3.h>)
    oss << "sqlite3: v" << SQLITE_VERSION << "\r\n";
#endif

    // zlib
#if __has_include(<zlib.h>)
    oss << "zlib: v" << ZLIB_VERSION << "\r\n";
#endif

    // spdlog
#if __has_include(<spdlog/version.h>)
    oss << "spdlog: v" << SPDLOG_VER_MAJOR << "." << SPDLOG_VER_MINOR << "." << SPDLOG_VER_PATCH << "\r\n";
#endif

    // fmt
#if __has_include(<fmt/core.h>)
    oss << "fmt: v" << FMT_VERSION / 10000 << "." << (FMT_VERSION / 100) % 100 << "." << FMT_VERSION % 100 << "\r\n";
#endif

// rapidyaml
#if __has_include(<ryml/ryml.hpp>)
    oss << "rapidyaml: v" << COR_VERSION_MAJOR << "." << COR_VERSION_MINOR << "." << RYML_VERSION_PATCH << "\r\n";
#endif

    // cpp-httplib
#if __has_include(<httplib.h>)
    oss << "cpp-httplib: v\r\n";
#endif

    // Convert to CString
    std::string str = oss.str();
    CString result(str.c_str());
    return result;
}

void CAppInfoDlg::OnCopyDiffToClipboard()
{
    // Get diff string length
    constexpr auto diffLen = static_cast<int>(std::char_traits<char>::length(git_version::DIFF_STR));

    if constexpr (diffLen < 10)
    {
        AfxMessageBox(_T("No diff available to copy."), MB_OK | MB_ICONINFORMATION);
        return;
    }
    else
    {
        // Convert UTF-8 diff to UTF-16 for Windows clipboard
        std::string diffUtf8(git_version::DIFF_STR);

        // Convert to wide string
        int wideLen = MultiByteToWideChar(CP_UTF8, 0, diffUtf8.c_str(), -1, nullptr, 0);
        if (wideLen <= 0)
        {
            AfxMessageBox(_T("Failed to convert diff to clipboard format."), MB_OK | MB_ICONERROR);
            return;
        }

        std::vector<wchar_t> wideStr(static_cast<size_t>(wideLen));
        MultiByteToWideChar(CP_UTF8, 0, diffUtf8.c_str(), -1, wideStr.data(), wideLen);

        CString diffText(wideStr.data());

        if (CopyToClipboard(diffText))
        {
            AfxMessageBox(_T("Diff copied to clipboard."), MB_OK | MB_ICONINFORMATION);
        }
        else
        {
            AfxMessageBox(_T("Failed to copy diff to clipboard."), MB_OK | MB_ICONERROR);
        }
    }
}

bool CAppInfoDlg::CopyToClipboard(const CString &text)
{
    if (!::OpenClipboard(m_hWnd))
    {
        return false;
    }

    EmptyClipboard();

    // Allocate global memory for the text
    SIZE_T textSize = (text.GetLength() + 1) * sizeof(TCHAR);
    HGLOBAL hMem = GlobalAlloc(GMEM_MOVEABLE, textSize);
    if (hMem == nullptr)
    {
        CloseClipboard();
        return false;
    }

    // Copy text to global memory
    LPTSTR pMem = static_cast<LPTSTR>(GlobalLock(hMem));
    if (pMem == nullptr)
    {
        GlobalFree(hMem);
        CloseClipboard();
        return false;
    }

    memcpy(pMem, static_cast<LPCTSTR>(text), textSize);
    GlobalUnlock(hMem);

    // Set clipboard data
#ifdef UNICODE
    UINT format = CF_UNICODETEXT;
#else
    UINT format = CF_TEXT;
#endif

    if (SetClipboardData(format, hMem) == nullptr)
    {
        GlobalFree(hMem);
        CloseClipboard();
        return false;
    }

    CloseClipboard();
    return true;
}

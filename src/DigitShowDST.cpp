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
 * @file DigitShowDST.cpp
 * @brief Implementation of CDigitShowDSTApp application class
 *
 * Implements application initialization and main message loop.
 * アプリケーション用クラスの機能定義を行います。
 */

#include "StdAfx.h"

#include "DigitShowDST.h"
#include "DigitShowDSTDoc.h"
#include "DigitShowDSTView.h"
#include "Logging.hpp"
#include "Variables.hpp"
#include "resource.h"

#include "../generated/git_version.hpp"
#include "ControlConfig.h"
#include "MainFrm.h"
#include <Windows.h>
#include <format>
#include <iostream>
#include <spdlog/spdlog.h>
#include <string>
#include <vector>

#if defined(_DEBUG) && !defined(__clang__)
#define new DEBUG_NEW
#undef THIS_FILE
static constexpr const char *THIS_FILE = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDigitShowDSTApp

BEGIN_MESSAGE_MAP_IGNORE_UNUSED_LOCAL_TYPEDEF(CDigitShowDSTApp, CWinApp)
//{{AFX_MSG_MAP(CDigitShowDSTApp)
// メモ - ClassWizard はこの位置にマッピング用のマクロを追加または削除します。
//        この位置に生成されるコードを編集しないでください。
//}}AFX_MSG_MAP
// 標準のファイル基本ドキュメント コマンド
ON_COMMAND(ID_FILE_NEW, &CWinApp::OnFileNew)
ON_COMMAND(ID_FILE_OPEN, &CWinApp::OnFileOpen)
END_MESSAGE_MAP_IGNORE_UNUSED_LOCAL_TYPEDEF()

/////////////////////////////////////////////////////////////////////////////
// 唯一の CDigitShowDSTApp オブジェクト

CDigitShowDSTApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CDigitShowDSTApp クラスの初期化

BOOL CDigitShowDSTApp::InitInstance()
{
    // Initialize logging first
    logging::initialize();
    spdlog::info("DigitShowDST application starting");

    // CLI hook: --validate-config <path> to validate YAML and exit
    {
        int argc = 0;
        LPWSTR *wargv = ::CommandLineToArgvW(::GetCommandLineW(), &argc);
        if (wargv && argc >= 2)
        {
            std::vector<std::wstring> args;
            args.reserve(static_cast<size_t>(argc));
            for (int i = 0; i < argc; ++i)
                args.emplace_back(wargv[i]);

            auto to_narrow = [](const std::wstring &ws) -> std::string {
                int len = ::WideCharToMultiByte(CP_UTF8, 0, ws.c_str(), -1, nullptr, 0, nullptr, nullptr);
                std::string s;
                if (len > 0)
                {
                    s.resize(static_cast<size_t>(len - 1));
                    ::WideCharToMultiByte(CP_UTF8, 0, ws.c_str(), -1, s.data(), len, nullptr, nullptr);
                }
                return s;
            };

            for (size_t i = 1; i < static_cast<size_t>(argc); ++i)
            {
                std::wstring a = args[i];
                if (a == L"--validate-config" || a == L"/validate-config")
                {
                    if (i + 1 >= static_cast<size_t>(argc))
                    {
                        spdlog::error("Missing config file argument for --validate-config");
                        std::cerr << "Usage: DigitShowDST --validate-config <file.yaml>" << std::endl;
                        ::ExitProcess(2);
                    }
                    std::string path = to_narrow(args[i + 1]);
                    spdlog::info("Validating config file: {}", path);
                    auto result = ControlConfig::ConfigLoader::LoadFromFile(path);
                    if (result.has_value())
                    {
                        spdlog::info("Config validation successful: {}", path);
                        std::cout << "OK: " << path << std::endl;
                        ::ExitProcess(0);
                    }
                    else
                    {
                        spdlog::error("Config validation failed: {}", path);
                        const auto &errs = result.error();
                        for (const auto &e : errs)
                        {
                            spdlog::error("  {}", e.format());
                            std::cerr << e.format() << std::endl;
                        }
                        ::ExitProcess(1);
                    }
                }
            }
        }
        if (wargv)
            ::LocalFree(wargv);
    }

    AfxEnableControlContainer();

    spdlog::debug("MFC initialization starting");

    // 標準的な初期化処理
    // もしこれらの機能を使用せず、実行ファイルのサイズを小さく
    // したければ以下の特定の初期化ルーチンの中から不必要なもの
    // を削除してください。

    // MFC の最新バージョンでは 3D コントロールの明示的な有効化は不要。

    // 設定が保存される下のレジストリ キーを変更します。
    // TODO: この文字列を、会社名または所属など適切なものに
    // 変更してください。
    SetRegistryKey(_T("Local AppWizard-Generated Applications"));

    LoadStdProfileSettings(); // 標準の INI ファイルのオプションをローﾄﾞします (MRU を含む)
    spdlog::debug("Profile settings loaded");

    // アプリケーション用のドキュメント テンプレートを登録します。ドキュメント テンプレート
    //  はドキュメント、フレーム ウィンドウとビューを結合するために機能します。

    CSingleDocTemplate *pDocTemplate = nullptr;
    pDocTemplate = new CSingleDocTemplate(IDR_MAINFRAME, RUNTIME_CLASS(CDigitShowDSTDoc),
                                          RUNTIME_CLASS(CMainFrame), // メイン SDI フレーム ウィンドウ
                                          RUNTIME_CLASS(CDigitShowDSTView));
    AddDocTemplate(pDocTemplate);
    spdlog::debug("Document template registered");

    // DDE、file open など標準のシェル コマンドのコマンドラインを解析します。
    CCommandLineInfo cmdInfo;
    ParseCommandLine(cmdInfo);

    // コマンドラインでディスパッチ コマンドを指定します。
    if (!ProcessShellCommand(cmdInfo))
    {
        spdlog::error("ProcessShellCommand failed");
        return FALSE;
    }

    spdlog::info("Main window initialized");
    // メイン ウィンドウが初期化されたので、表示と更新を行います。
    m_pMainWnd->ShowWindow(SW_SHOW);
    m_pMainWnd->UpdateWindow();

    // Set window title with version information
    std::string titleStr = std::format("DigitShowDST v{} [{}]{}", git_version::VERSION.data(),
                                       git_version::COMMIT_SHORT.data(), git_version::DIRTY ? " dirty" : "");
    CString title(titleStr.c_str());
    m_pMainWnd->SetWindowText(title);

    return TRUE;
}

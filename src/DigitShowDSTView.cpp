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
 * @file DigitShowDSTView.cpp
 * @brief Implementation of CDigitShowDSTView class
 *
 * Implements view functionality including UI updates, timer management,
 * and user interaction handling.
 * CDigitShowDSTView クラスの動作の定義を行います。
 */

#include "StdAfx.h"

#include "../generated/git_version.hpp"
#include "Board.hpp"
#include "Constants.h"
#include "DigitShowDSTDoc.h"
#include "DigitShowDSTView.h"
#include "File.hpp"
#include "Logging.hpp"
#include "SamplingSettings.h"
#include "Variables.hpp"
#include "aio_error_logger.hpp"
#include "aio_wrapper.hpp"
#include "board_control.hpp"
#include "chrono_alias.hpp"
#include "control/control.hpp"
#include "digitshow_operations.hpp"
#include "physical_variables.hpp"
#include "resource.h"
#include "timer.hpp"
#include <chrono>
#include <climits>
#include <cmath>
#include <ctime>
#include <format>
#include <ranges>
#include <spdlog/spdlog.h>
#include <string>

#if defined(_DEBUG) && !defined(__clang__)
#define new DEBUG_NEW
#undef THIS_FILE
static constexpr const char *THIS_FILE = __FILE__;
#endif

using namespace board;
using namespace control;
using namespace file;
using namespace variables;
using std::chrono::microseconds;

/////////////////////////////////////////////////////////////////////////////
// CDigitShowDSTView

IMPLEMENT_DYNCREATE(CDigitShowDSTView, CFormView)

BEGIN_MESSAGE_MAP_IGNORE_UNUSED_LOCAL_TYPEDEF(CDigitShowDSTView, CFormView)
//{{AFX_MSG_MAP(CDigitShowDSTView)
ON_WM_CTLCOLOR()
ON_WM_TIMER()
ON_BN_CLICKED(IDC_BUTTON_CtrlOff, &CDigitShowDSTView::OnBUTTONCtrlOff)
ON_BN_CLICKED(IDC_BUTTON_CtrlOn, &CDigitShowDSTView::OnBUTTONCtrlOn)
ON_WM_DESTROY()
ON_BN_CLICKED(IDC_BUTTON_InterceptSave, &CDigitShowDSTView::OnBUTTONInterceptSave)
ON_BN_CLICKED(IDC_BUTTON_SetTimeInterval, &CDigitShowDSTView::OnBUTTONSetTimeInterval)
//}}AFX_MSG_MAP
END_MESSAGE_MAP_IGNORE_UNUSED_LOCAL_TYPEDEF()

/////////////////////////////////////////////////////////////////////////////
// CDigitShowDSTView クラスの構築/消滅

static constexpr auto BG_COLOR_MOTOR_RGB = (RGB(0, 128, 128));        // Motor Mode Color     #008080
static constexpr auto BG_COLOR_DIRTY_OR_FORKED_RGB = (RGB(48, 0, 0)); // DirtyGit/ForkedRepo  #300000
static constexpr auto BG_COLOR = git_version::DIRTY ? BG_COLOR_DIRTY_OR_FORKED_RGB : BG_COLOR_MOTOR_RGB;

CDigitShowDSTView::CDigitShowDSTView()
    : CFormView(CDigitShowDSTView::IDD), m_Vout{}, m_Phyout{}, m_Para{}, m_NowTime(_T("")),
      m_SamplingTime(timer::TimeInterval_3.count()), m_FileName(_T("")), m_pEditBrush(new CBrush(RGB(255, 255, 255))),
      m_pStaticBrush(new CBrush(BG_COLOR)), m_pDlgBrush(new CBrush(BG_COLOR))
{
}

CDigitShowDSTView::~CDigitShowDSTView()
{
    spdlog::info("{} called", __func__);

    if (!board::Flag_SetBoard)
    {
        spdlog::debug("No boards to close (Flag_SetBoard is false)");
        return;
    }

    // Reset all voltage outputs to 0V before closing boards to stop all devices
    if (const auto result = board_control::ResetAllVoltageOutputs(); !result)
    {
        spdlog::warn("Error resetting voltage outputs: {}", result.error());
    }
    else
    {
        // Synchronize latest_physical_output with reset voltages
        variables::physical::update();
    }

    if (const auto result = board_control::CloseBoards(); !result)
    {
        spdlog::warn("Error closing boards: {}", result.error());
        return;
    }
}

void CDigitShowDSTView::DoDataExchange(CDataExchange *pDX)
{
    CFormView::DoDataExchange(pDX);
    //{{AFX_DATA_MAP(CDigitShowDSTView)
    spdlog::trace("{}", __func__);

    // Use loops for array-based DDX
    for (size_t i = 0; i < CHANNELS_VOUT; ++i)
        DDX_Text(pDX, IDS_VOUT[i], m_Vout[i]);

    for (size_t i = 0; i < CHANNELS_PHYOUT; ++i)
        DDX_Text(pDX, IDS_PHYOUT[i], m_Phyout[i]);

    for (size_t i = 0; i < CHANNELS_PARA; ++i)
        DDX_Text(pDX, IDS_PARA[i], m_Para[i]);

    DDX_Text(pDX, IDC_EDIT_NowTime, m_NowTime);
    DDX_Text(pDX, IDC_EDIT_SeqTime, m_SeqTime);
    DDX_Text(pDX, IDC_EDIT_SamplingTime, m_SamplingTime);
    DDV_MinMaxLongLong(pDX, m_SamplingTime, 100, 86400000);
    DDX_Text(pDX, IDC_EDIT_FileName, m_FileName);
    //}}AFX_DATA_MAP
}

BOOL CDigitShowDSTView::PreCreateWindow(CREATESTRUCT &cs)
{
    // TODO: この位置で CREATESTRUCT cs を修正して Window クラスまたはスタイルを
    //  修正してください。
    return CFormView::PreCreateWindow(cs);
}

/////////////////////////////////////////////////////////////////////////////
// CDigitShowDSTView クラスの診断

#ifdef _DEBUG
void CDigitShowDSTView::AssertValid() const
{
    CFormView::AssertValid();
}

void CDigitShowDSTView::Dump(CDumpContext &dc) const
{
    CFormView::Dump(dc);
}

CDigitShowDSTDoc *CDigitShowDSTView::GetDocument() // 非デバッグ バージョンはインラインです。
{
    ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CDigitShowDSTDoc)));
    return (CDigitShowDSTDoc *)m_pDocument;
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CDigitShowDSTView クラスのメッセージ ハンドラ
void CDigitShowDSTView::OnInitialUpdate()
{
    spdlog::info("{} called", __func__);
    CFormView::OnInitialUpdate();
    GetParentFrame()->RecalcLayout();
    ResizeParentToFit();
    GetDlgItem(IDC_BUTTON_CtrlOff)->EnableWindow(FALSE);
    GetDlgItem(IDC_BUTTON_InterceptSave)->EnableWindow(FALSE);
#pragma warning(push)
#pragma warning(disable : 4946)
    CComboBox *m_Combo2 = reinterpret_cast<CComboBox *>(
        GetDlgItem(IDC_COMBO_SamplingTime)); // NOLINT(cppcoreguidelines-pro-type-reinterpret-cast)
#pragma warning(pop)
    m_Combo2->InsertString(-1, _T("0.05 s"));
    m_Combo2->InsertString(-1, _T("0.1 s")); // 2022.12.14追記。これにより0.05, 0.1sでのサンプリングが可能になる。
    m_Combo2->InsertString(-1, _T("0.2 s"));
    m_Combo2->InsertString(-1, _T("0.5 s"));
    m_Combo2->InsertString(-1, _T("1.0 s"));
    m_Combo2->InsertString(-1, _T("2.0 s"));
    m_Combo2->InsertString(-1, _T("3.0 s"));
    m_Combo2->InsertString(-1, _T("5.0 s"));
    m_Combo2->InsertString(-1, _T("10.0 s"));
    m_Combo2->InsertString(-1, _T("20.0 s"));
    m_Combo2->InsertString(-1, _T("30.0 s"));
    m_Combo2->InsertString(-1, _T("1.0 min"));
    m_Combo2->InsertString(-1, _T("2.0 min"));
    m_Combo2->InsertString(-1, _T("3.0 min"));
    m_Combo2->InsertString(-1, _T("5.0 min"));
    m_Combo2->InsertString(-1, _T("10.0 min"));
    m_Combo2->SetWindowText(_T("1.0 s"));

    if (board::Flag_SetBoard)
    {
        spdlog::warn("Board already initialized, skipping initialize_boards");
        AfxMessageBox(L"Board already initialized", MB_ICONSTOP | MB_OK);
        return;
    }

    constexpr std::array<std::string_view, 2> kAdDeviceNames = {{"AIO000", "AIO002"}};
    constexpr std::array<std::string_view, 1> kDaDeviceNames = {{"AIO001"}};

    if (const auto result = board_control::InitializeBoards(std::span{kAdDeviceNames}, std::span{kDaDeviceNames});
        !result)
    {
        AfxMessageBox(CA2W(std::format("Board initialization failed: {}", result.error()).c_str(), CP_UTF8),
                      MB_ICONSTOP | MB_OK);
        return;
    }
    spdlog::info("Board initialization completed successfully via board_control layer");

    // Initialize synthetic wall-clock anchors once
    if (board::WallClockStart.time_since_epoch().count() == 0)
    {
        board::WallClockStart = std::chrono::system_clock::now();
        board::SteadyClockStart = std::chrono::steady_clock::now();
    }
    if (Flag_SetBoard)
    {
        // Use board_control for initial sampling configuration
        if (auto result = board_control::ConfigureInitialSampling(m_hWnd); !result)
        {
            aio::log_error("OnInitialUpdate: ConfigureInitialSampling", result.error());
            AfxMessageBox(
                CA2W(std::format("Initial sampling configuration failed: {}", result.error()).c_str(), CP_UTF8),
                MB_ICONSTOP | MB_OK);
        }
    }
    SetTimer(timer::kTimerId_UI, static_cast<UINT>(timer::TimeInterval_1.count()), NULL);

    // Start API server if enabled
    auto *pDoc = GetDocument();
    if (pDoc && !pDoc->StartApiServer())
    {
        spdlog::warn("API server failed to start, but continuing application execution");
    }
}

HBRUSH CDigitShowDSTView::OnCtlColor(CDC *pDC, CWnd *pWnd, UINT nCtlColor)
{
    switch (nCtlColor)
    {
    case CTLCOLOR_EDIT:
        pDC->SetBkColor(RGB(255, 255, 255));                       // white background
        pDC->SetTextColor(RGB(0, 0, 0));                           // black for text color in EDIT
        return static_cast<HBRUSH>(m_pEditBrush->GetSafeHandle()); // EDITBOX color
    case CTLCOLOR_STATIC:                                          // Static label properties
        pDC->SetBkMode(TRANSPARENT);
        pDC->SetTextColor(RGB(255, 255, 255));
        return static_cast<HBRUSH>(m_pStaticBrush->GetSafeHandle());
    case CTLCOLOR_DLG: // Setting Dialog Box Color
        pDC->SetTextColor(RGB(0, 128, 128));
        return static_cast<HBRUSH>(m_pDlgBrush->GetSafeHandle());
    default:
        return CFormView::OnCtlColor(pDC, pWnd, nCtlColor);
    }
}
void CDigitShowDSTView::OnDestroy()
{
    CFormView::OnDestroy();
    // TODO: この位置にメッセージ ハンドラ用のコードを追加してください

    // Stop API server
    auto *pDoc = GetDocument();
    if (pDoc)
    {
        pDoc->StopApiServer();
    }

    delete m_pEditBrush;
    delete m_pStaticBrush;
    delete m_pDlgBrush;
}
void CDigitShowDSTView::OnTimer(UINT_PTR nIDEvent)
{
    // TODO: この位置にメッセージ ハンドラ用のコードを追加するかまたはデフォルトの処理を呼び出してください

    auto *pDoc = GetDocument();

    switch (nIDEvent)
    {
    case timer::kTimerId_UI: {
        using namespace std::chrono;
        // Synthetic stable wall-clock
        NowTime = SyntheticNow();
        auto nowTimeT = std::chrono::system_clock::to_time_t(NowTime);
        std::tm tmBuf{};
        localtime_s(&tmBuf, &nowTimeT);
        std::array<char, 64> timeBuf = {};
        std::strftime(timeBuf.data(), timeBuf.size(), "%m/%d  %H:%M:%S", &tmBuf);
        SNowTime = timeBuf.data();

        if (Flag_SaveData)
        {
            // Use steady_clock for relative elapsed time to avoid wall-clock jumps
            auto nowSteady = steady_clock::now();
            SequentTime1 = duration_cast<seconds>(nowSteady - StartSteadyTime);
        }
        if (Flag_SetBoard)
        {
            if (const auto result = digitshow::read_analog_inputs(); !result)
            {
                spdlog::warn("read_analog_inputs failed: {}", result.error());
            }
        }
        calc_physical();
        variables::physical::update();
        ShowData();
        // Update API server with latest sensor data
        pDoc->UpdateApiServerData();
    }
    break;
    case timer::kTimerId_Control: {
        using namespace std::chrono;

        // compute monotonic delta - keep native duration for maximum precision
        auto now = steady_clock::now();
        auto delta = now - last_tick;
        last_tick = now;

        // Defensive clipping for spurious large deltas (e.g., system suspend/resume)
        // Timer is expected ~0.5s, anything > 5s indicates abnormal scheduling
        constexpr auto max_delta = 5s;
        if (delta > max_delta)
        {
            spdlog::warn("Large timer delta observed: {}", seconds_d{delta});
        }

        control::CtrlStepTime = delta;
        spdlog::debug("Control tick: delta={}, CtrlStepTime={}", seconds_d{delta}, seconds_d{control::CtrlStepTime});

        if (Flag_SetBoard)
            control::execute();
        break;
    }
    case timer::kTimerId_Log: {
        spdlog::trace("Log timer tick");
        if (Flag_SetBoard)
        {
            if (const auto result = digitshow::read_analog_inputs(); !result)
            {
                spdlog::warn("read_analog_inputs failed: {}", result.error());
            }
        }
        calc_physical();
        variables::physical::update();
        pDoc->SaveToFile();
        pDoc->FlushWritersIfNeeded();
    }
    break;
    }
    CFormView::OnTimer(nIDEvent);
}

void CDigitShowDSTView::ShowData()
{
    // Format voltage output display strings
    for (size_t i = 0; i < CHANNELS_VOUT; ++i)
    {
        m_Vout[i].Format(_T("%11.4f"), Vout[i]);
    }

    // Format physical output display strings
    for (size_t i = 0; i < CHANNELS_PHYOUT; ++i)
    {
        m_Phyout[i].Format(_T("%11.4f"), Phyout[i]);
    }

    // Format calculated parameter display strings using the latest snapshots
    const auto physical_input = variables::physical::latest_physical_input.load();
    const auto physical_output = variables::physical::latest_physical_output.load();

    // Map each parameter shown in the UI to the corresponding physical snapshot
    m_Para[0].Format(_T("%11.4f"), static_cast<double>(physical_input.shear_stress_kpa()));
    m_Para[1].Format(_T("%11.4f"), static_cast<double>(physical_input.shear_displacement_mm));
    m_Para[2].Format(_T("%11.4f"), static_cast<double>(physical_input.vertical_stress_kpa()));
    m_Para[3].Format(_T("%11.4f"), static_cast<double>(physical_input.normal_displacement_mm()));
    m_Para[4].Format(_T("%11.4f"), static_cast<double>(physical_input.tilt_mm()));
    m_Para[5].Format(_T("%11.4f"), static_cast<double>(physical_output.motor_rpm));
    m_Para[6].Format(_T("%11.4f"), static_cast<double>(physical_output.front_ep_kpa));
    m_Para[7].Format(_T("%11.4f"), static_cast<double>(physical_output.rear_ep_kpa));
    // Raw D/A voltage analog values (these are not in the physical outputs)
    m_Para[8].Format(_T("%11.4f"), static_cast<double>(DAVout[CH_MotorSpeed]));
    m_Para[9].Format(_T("%11.4f"), static_cast<double>(DAVout[CH_EP_Cell_f]));
    m_Para[10].Format(_T("%11.4f"), static_cast<double>(DAVout[CH_EP_Cell_r]));

    m_Para[11].Format(_T("%11d"), std::lround(static_cast<double>(control::current_step_index)));
    m_Para[12].Format(_T("%11.4f"), std::chrono::seconds_d{control::step_elapsed}.count());
    m_Para[13].Format(_T("%11d"), std::lround(static_cast<double>(control::num_cyclic)));

    m_NowTime = SNowTime.c_str();
    m_SeqTime = SequentTime1.count();
    m_SamplingTime = timer::TimeInterval_3.count();
    UpdateData(FALSE);
}

void CDigitShowDSTView::OnBUTTONCtrlOn()
{
    spdlog::info("Control ON button clicked");
    // TODO: Add your control notification handler code here
    if (!Flag_SetBoard)
    {
        spdlog::warn("Control ON clicked but board not initialized");
        return;
    }

    // At this point, Flag_SetBoard is guaranteed to be true
    auto *pDoc = GetDocument();

    // Show file save dialog when starting control
    spdlog::debug("Opening file dialog for data save");
    CFileDialog SaveFile_dlg(FALSE, NULL, _T("*.tsv"), OFN_CREATEPROMPT | OFN_OVERWRITEPROMPT,
                             _T("TSV Files(*.tsv)|*.tsv| All Files(*.*)|*.*| |"), NULL);
    if (SaveFile_dlg.DoModal() != IDOK)
    {
        spdlog::info("File dialog cancelled, control not started");
        return;
    }

    // File for saving the physical data (Unicode-safe)
    CStringW pFileName1 = SaveFile_dlg.GetPathName();
    m_FileName = SaveFile_dlg.GetFileTitle();
    CStringW TmpString = SaveFile_dlg.GetFileExt();
    if (TmpString.IsEmpty())
    {
        pFileName1 += _T(".tsv");
        m_FileName += _T(".tsv");
    }
    else if (TmpString.CompareNoCase(_T("tsv")) != 0)
    {
        CStringW dotExt = _T(".") + TmpString;
        pFileName1.Replace(dotExt, _T(".tsv"));
        m_FileName += _T(".tsv");
    }
    const std::filesystem::path filePath{static_cast<LPCWSTR>(pFileName1)};
    const std::string path_u8{CW2A(pFileName1, CP_UTF8)};
    spdlog::info("Opening data file: {}", path_u8);

    // Open writers through document
    if (!pDoc->OpenSaveWriters(filePath))
    {
        spdlog::error("Failed to open data files: {}", path_u8);
        AfxMessageBox(_T("Failed to open data files."), MB_ICONEXCLAMATION | MB_OK);
        return;
    }

    // Reset flush tracking state
    pDoc->ResetFlushState();

    // Timer starts for logging
    SetTimer(timer::kTimerId_Log, static_cast<UINT>(timer::TimeInterval_3.count()), NULL);
    NowTime = std::chrono::system_clock::now();
    StartTime = SyntheticNow();                         // wall-clock reference for file timestamps
    StartSteadyTime = std::chrono::steady_clock::now(); // monotonic reference for elapsed time
    using namespace std::literals::chrono_literals;
    SequentTime1 = 0s;
    //
    Flag_SaveData = true;
    GetDlgItem(IDC_BUTTON_InterceptSave)->EnableWindow(TRUE);
    //
    if (Flag_SetBoard)
    {
        if (const auto result = digitshow::read_analog_inputs(); !result)
        {
            spdlog::warn("read_analog_inputs failed before first save: {}", result.error());
        }
    }
    calc_physical();
    variables::physical::update();
    pDoc->SaveToFile();

    // Start control timer
    spdlog::info("Starting control timer (interval: {})", timer::TimeInterval_2);
    SetTimer(timer::kTimerId_Control, static_cast<UINT>(timer::TimeInterval_2.count()), NULL);
    last_tick = std::chrono::steady_clock::now();
    control::is_control_running = true;
    GetDlgItem(IDC_BUTTON_CtrlOn)->EnableWindow(FALSE);
    GetDlgItem(IDC_BUTTON_CtrlOff)->EnableWindow(TRUE);
    pDoc->Start_Control();
}

void CDigitShowDSTView::OnBUTTONCtrlOff()
{
    spdlog::info("Control OFF button clicked");
    // TODO: Add your control notification handler code here

    auto *pDoc = GetDocument();

    // Stop saving data
    if (Flag_SaveData)
    {
        KillTimer(timer::kTimerId_Log);
        if (Flag_SetBoard)
        {
            if (const auto result = digitshow::read_analog_inputs(); !result)
            {
                spdlog::warn("read_analog_inputs failed during stop: {}", result.error());
            }
        }
        calc_physical();
        variables::physical::update();
        pDoc->SaveToFile();
        pDoc->FlushWriters(); // Flush before closing
        pDoc->CloseSaveWriters();
        GetDlgItem(IDC_BUTTON_InterceptSave)->EnableWindow(FALSE);
        Flag_SaveData = false;
    }

    // Stop control timer
    KillTimer(timer::kTimerId_Control);
    control::is_control_running = false;
    GetDlgItem(IDC_BUTTON_CtrlOn)->EnableWindow(TRUE);
    GetDlgItem(IDC_BUTTON_CtrlOff)->EnableWindow(FALSE);
    pDoc->Stop_Control();
    spdlog::info("Control timer stopped");
}

void CDigitShowDSTView::OnBUTTONInterceptSave()
{
    // TODO: Add your control notification handler code here
    auto *pDoc = GetDocument();

    if (Flag_SetBoard)
    {
        if (const auto result = digitshow::read_analog_inputs(); !result)
        {
            spdlog::warn("read_analog_inputs failed on intercept save: {}", result.error());
        }
    }
    calc_physical();
    variables::physical::update();
    pDoc->SaveToFile();
}

LRESULT CDigitShowDSTView::DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case AIOM_AIE_DATA_NUM: {
        // Get minimum sampling count via board_control
        auto minCountResult = board_control::GetMinimumSamplingCount();
        if (!minCountResult)
        {
            aio::log_error("DefWindowProc: GetMinimumSamplingCount", minCountResult.error());
            return TRUE;
        }
        const long tmp = *minCountResult;

        // Get sampling data via board_control
        auto dataResult = board_control::GetSamplingData(tmp);
        if (!dataResult)
        {
            aio::log_error("DefWindowProc: GetSamplingData", dataResult.error());
            AfxMessageBox(CA2W(std::format("AioGetAiSamplingData failed: {}", dataResult.error()).c_str(), CP_UTF8),
                          MB_ICONSTOP | MB_OK);
            return TRUE;
        }

        return TRUE;
    }
    case AIOM_AIE_OFERR:
        // Reset and restart via board_control
        if (auto result = board_control::ResetAndRestartSampling(); !result)
        {
            aio::log_error("DefWindowProc: AIOM_AIE_OFERR - ResetAndRestartSampling", result.error());
        }
        AfxMessageBox(_T("FIFO stopped by the over flow, but restarted automatically."), MB_OK | MB_ICONSTOP, 0);
        return TRUE;
    case AIOM_AIE_SCERR:
        AfxMessageBox(_T("FIFO stopped by sampling error."), MB_OK | MB_ICONSTOP, 0);
        return TRUE;
    case AIOM_AIE_ADERR:
        AfxMessageBox(_T("FIFO stopped by the error in A/D convert."), MB_OK | MB_ICONSTOP, 0);
        return TRUE;
    case AIOM_AIE_END:
        AfxMessageBox(_T("FIFO stopped to reach the end."), MB_OK | MB_ICONSTOP, 0);
        return TRUE;
    }
    return CFormView::DefWindowProc(message, wParam, lParam);
}

void CDigitShowDSTView::OnBUTTONSetTimeInterval()
{
    // TODO: この位置にコントロール通知ハンドラ用のコードを追加してください
#pragma warning(push)
#pragma warning(disable : 4946)
    CComboBox *m_Combo1 = reinterpret_cast<CComboBox *>( // NOLINT(cppcoreguidelines-pro-type-reinterpret-cast)
        GetDlgItem(IDC_COMBO_SamplingTime));
#pragma warning(pop)
    CStringW tmp{};
    m_Combo1->GetWindowText(tmp);
    using namespace std::literals::chrono_literals;
    // NOLINTBEGIN(cppcoreguidelines-avoid-magic-numbers)
    if (tmp == "0.05 s")
        timer::TimeInterval_3 = 50ms; // 2022.12.14追記。これにより0.05sでのサンプリングが可能になる。
    if (tmp == "0.1 s")
        timer::TimeInterval_3 = 100ms; // 2022.12.14追記。これにより0.1sでのサンプリングが可能になる。
    if (tmp == "0.2 s")
        timer::TimeInterval_3 = 200ms;
    if (tmp == "0.5 s")
        timer::TimeInterval_3 = 500ms;
    if (tmp == "1.0 s")
        timer::TimeInterval_3 = 1000ms;
    if (tmp == "2.0 s")
        timer::TimeInterval_3 = 2000ms;
    if (tmp == "3.0 s")
        timer::TimeInterval_3 = 3000ms;
    if (tmp == "5.0 s")
        timer::TimeInterval_3 = 5000ms;
    if (tmp == "10.0 s")
        timer::TimeInterval_3 = 10000ms;
    if (tmp == "20.0 s")
        timer::TimeInterval_3 = 20000ms;
    if (tmp == "30.0 s")
        timer::TimeInterval_3 = 30000ms;
    if (tmp == "1.0 min")
        timer::TimeInterval_3 = 60000ms;
    if (tmp == "2.0 min")
        timer::TimeInterval_3 = 120000ms;
    if (tmp == "3.0 min")
        timer::TimeInterval_3 = 180000ms;
    if (tmp == "5.0 min")
        timer::TimeInterval_3 = 300000ms;
    if (tmp == "10.0 min")
        timer::TimeInterval_3 = 600000ms;
    // NOLINTEND(cppcoreguidelines-avoid-magic-numbers)
    if (Flag_SaveData)
    {
        KillTimer(timer::kTimerId_Log);
        SetTimer(timer::kTimerId_Log, static_cast<UINT>(timer::TimeInterval_3.count()), NULL);
    }
}

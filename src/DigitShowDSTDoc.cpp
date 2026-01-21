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
 * @file DigitShowDSTDoc.cpp
 * @brief Implementation of CDigitShowDSTDoc class
 *
 * Implements document functionality including hardware control, data acquisition,
 * control algorithms, and data logging.
 * CDigitShowDSTDoc クラスの動作の定義を行います。
 */

#include "StdAfx.h"

#include "Board.hpp"
#include "Constants.h"
#include "DataConvert.h"
#include "DigitShowDSTDoc.h"
#include "File.hpp"
#include "Logging.hpp"
#include "Variables.hpp"
#include "aio_error_logger.hpp"
#include "aio_wrapper.hpp"
#include "board_control.hpp"
#include "chrono_alias.hpp"
#include "control/control.hpp"
#include "control/measurement.hpp"
#include "control/patterns.hpp"
#include "control/utils.hpp"
#include "digitshow_operations.hpp"
#include "physical_variables.hpp"
#include "resource.h"
#include "timer.hpp"
#include <algorithm>
#include <array>
#include <atomic>
#include <chrono>
#include <cmath>
#include <format>
#include <spdlog/spdlog.h>
#include <utility>

using namespace board;
using namespace control;
using namespace file;
using namespace variables;

/////////////////////////////////////////////////////////////////////////////

IMPLEMENT_DYNCREATE(CDigitShowDSTDoc, CDocument)
BEGIN_MESSAGE_MAP_IGNORE_UNUSED_LOCAL_TYPEDEF(CDigitShowDSTDoc, CDocument)
//{{AFX_MSG_MAP(CDigitShowDSTDoc)
// メモ - ClassWizard はこの位置にマッピング用のマクロを追加または削除します。
//        この位置に生成されるコードを編集しないでください。
//}}AFX_MSG_MAP
END_MESSAGE_MAP_IGNORE_UNUSED_LOCAL_TYPEDEF()

/////////////////////////////////////////////////////////////////////////////

BOOL CDigitShowDSTDoc::OnNewDocument()
{
    if (!CDocument::OnNewDocument())
        return FALSE;

    // Initialize latest_physical_input with initial specimen (present = initial at startup)
    variables::physical::latest_physical_input.store({
        SpecimenData,
        0.0, // shear_force_N
        0.0, // vertical_force_N
        0.0, // shear_displacement_mm
        0.0, // front_vertical_disp_mm
        0.0, // rear_vertical_disp_mm
        0.0, // front_friction_force_N
        0.0, // rear_friction_force_N
    });

    return TRUE;
}

/////////////////////////////////////////////////////////////////////////////

// CDigitShowDSTDoc シリアライゼーション
void CDigitShowDSTDoc::Serialize(CArchive &ar)
{
    if (ar.IsStoring())
    {
        // TODO: この位置に保存用のコードを追加してください。
    }
    else
    {
        // TODO: この位置に読み込み用のコードを追加してください。
    }
}

/////////////////////////////////////////////////////////////////////////////

// CDigitShowDSTDoc file writers management

bool CDigitShowDSTDoc::OpenSaveWriters(const std::filesystem::path &basePath)
{
    using namespace file;

    // Build file paths with appropriate suffixes
    const auto vltPath = std::filesystem::path(basePath).replace_extension("") += "_vlt.tsv";
    const auto phyPath = std::filesystem::path(basePath).replace_extension("tsv");
    const auto paramPath = std::filesystem::path(basePath).replace_extension("") += "_out.tsv";

    // Open voltage writer
    if (!m_vltWriter.open(vltPath, true))
    {
        spdlog::error("Failed to open voltage file: {}",
                      reinterpret_cast<const char *>( // NOLINT(cppcoreguidelines-pro-type-reinterpret-cast)
                          vltPath.u8string().c_str()));
        return false;
    }

    // Open physical writer
    if (!m_phyWriter.open(phyPath, true))
    {
        spdlog::error("Failed to open physical file: {}",
                      reinterpret_cast<const char *>( // NOLINT(cppcoreguidelines-pro-type-reinterpret-cast)
                          phyPath.u8string().c_str()));
        m_vltWriter.close();
        return false;
    }

    // Open parameter writer
    if (!m_paramWriter.open(paramPath, true))
    {
        spdlog::error("Failed to open parameter file: {}",
                      reinterpret_cast<const char *>( // NOLINT(cppcoreguidelines-pro-type-reinterpret-cast)
                          paramPath.u8string().c_str()));
        m_vltWriter.close();
        m_phyWriter.close();
        return false;
    }

    // Reserve scratch buffer (typical line might be ~500-1000 chars)
    m_writeScratch.reserve(1024);

    // Write headers to each file
    // Voltage file header
    m_vltWriter.writeLine(
        "UnixTime(ms)\tCH00_(V)\tCH01_(V)\tCH02_(V)\tCH03_(V)\tCH04_(V)\tCH05_(V)\tCH06_(V)\tCH07_(V)");

    // Physical file header
    m_phyWriter.writeLine("UnixTime(ms)\tShear_load_(N)\tVertical_load_(N)\tShear_disp._(mm)\tV-front-disp._(mm)\tV-"
                          "rear-disp._(mm)\tFront_friction_(N)\tRear_friction_(N)\tCH08");

    // Parameter file header
    m_paramWriter.writeLine("UnixTime(ms)\tTau_(kPa)\tShear_disp._(mm)\tSigma_(kPa)\tV-ave-disp._(mm)\tev_diff/"
                            "2_(mm)\tFront_friction_(N)\tRear_friction_(N)\tRPM\tFront_EP_(kPa)\tRear_EP_(kPa)\tRPM_(V)"
                            "\tFront_EP_(V)\tRear_EP_(V)\tLoop_count\tControl_No\tStep_time_(s)");

    spdlog::info("Opened TSV writers: vlt={}, phy={}, param={}",
                 reinterpret_cast<const char *>( // NOLINT(cppcoreguidelines-pro-type-reinterpret-cast)
                     vltPath.u8string().c_str()),
                 reinterpret_cast<const char *>( // NOLINT(cppcoreguidelines-pro-type-reinterpret-cast)
                     phyPath.u8string().c_str()),
                 reinterpret_cast<const char *>( // NOLINT(cppcoreguidelines-pro-type-reinterpret-cast)
                     paramPath.u8string().c_str()));

    return true;
}

void CDigitShowDSTDoc::CloseSaveWriters() noexcept
{
    m_vltWriter.close();
    m_phyWriter.close();
    m_paramWriter.close();
    spdlog::debug("Closed all TSV writers");
}

void CDigitShowDSTDoc::FlushWriters()
{
    m_vltWriter.flush();
    m_phyWriter.flush();
    m_paramWriter.flush();
    m_lastFlushTime = std::chrono::steady_clock::now();
    m_lastFlushedCurnum = control::current_step_index;
    spdlog::debug("Flushed all TSV writers (current_step_index={})", m_lastFlushedCurnum);
}

void CDigitShowDSTDoc::FlushWritersIfNeeded()
{
    using namespace std::chrono;

    // Skip flush if not initialized (ResetFlushState not called yet)
    if (m_lastFlushTime.time_since_epoch().count() == 0)
    {
        return;
    }

    // Flush if control step number changed
    if (control::current_step_index != m_lastFlushedCurnum)
    {
        spdlog::info("Control step changed ({} -> {}), flushing writers", m_lastFlushedCurnum,
                     control::current_step_index);
        FlushWriters();
        return;
    }

    // Flush if more than 1 minute has elapsed since last flush
    constexpr auto kFlushInterval = 1min;
    const auto now = steady_clock::now();
    if (now - m_lastFlushTime >= kFlushInterval)
    {
        spdlog::debug("Flush interval elapsed, flushing writers");
        FlushWriters();
    }
}

void CDigitShowDSTDoc::ResetFlushState() noexcept
{
    m_lastFlushedCurnum = control::current_step_index;
    m_lastFlushTime = std::chrono::steady_clock::now();
}

/////////////////////////////////////////////////////////////////////////////

// CDigitShowDSTDoc クラスの診断
#ifdef _DEBUG
void CDigitShowDSTDoc::AssertValid() const
{
    CDocument::AssertValid();
}
void CDigitShowDSTDoc::Dump(CDumpContext &dc) const
{
    CDocument::Dump(dc);
}
#endif //_DEBUG

void CDigitShowDSTDoc::SaveToFile()
{
    using namespace file;

    // Save Voltage and Physical Data
    // Synthetic stable wall-clock time for UnixTime(ms)
    const auto syntheticNow = board::SyntheticNow();
    const auto unixTimeMs =
        std::chrono::duration_cast<std::chrono::milliseconds>(syntheticNow.time_since_epoch()).count();

    // Build voltage line
    m_writeScratch.clear();
    std::format_to(std::back_inserter(m_writeScratch), "{}\t", unixTimeMs);
    size_t k = 0;
    for (size_t i = 0; i < dsb::NUMAD; i++)
    {
        for (size_t j = 0; std::cmp_less(j, AdChannels[i] / 2); j++)
        { // 2023.2    1/2を戻してCH１つを実施
            std::format_to(std::back_inserter(m_writeScratch), "{:.6f}\t", Vout[k]);
            k++;
        }
    }
    m_writeScratch.back() = '\n'; // Replace last tab with newline
    m_vltWriter.stream().write(m_writeScratch.data(), static_cast<std::streamsize>(m_writeScratch.size()));

    // Build physical line
    m_writeScratch.clear();
    std::format_to(std::back_inserter(m_writeScratch), "{}\t", unixTimeMs);
    k = 0;
    for (size_t i = 0; i < dsb::NUMAD; i++)
    {
        for (size_t j = 0; std::cmp_less(j, AdChannels[i] / 2); j++)
        {
            std::format_to(std::back_inserter(m_writeScratch), "{:.6f}\t", Phyout[k]);
            k++;
        }
    }
    m_writeScratch.back() = '\n'; // Replace last tab with newline
    m_phyWriter.stream().write(m_writeScratch.data(), static_cast<std::streamsize>(m_writeScratch.size()));

    // Build parameter line using snapshot values (no reliance on CalParam)
    m_writeScratch.clear();
    std::format_to(std::back_inserter(m_writeScratch), "{}\t", unixTimeMs);
    const auto physical_input = variables::physical::latest_physical_input.load();
    const auto physical_output = variables::physical::latest_physical_output.load();
    std::format_to(std::back_inserter(m_writeScratch), "{:.6f}\t", physical_input.shear_stress_kpa());    // param[0]
    std::format_to(std::back_inserter(m_writeScratch), "{:.6f}\t", physical_input.shear_displacement_mm); // param[1]
    std::format_to(std::back_inserter(m_writeScratch), "{:.6f}\t", physical_input.vertical_stress_kpa()); // param[2]
    std::format_to(std::back_inserter(m_writeScratch), "{:.6f}\t",
                   physical_input.normal_displacement_mm());                                  // param[3]
    std::format_to(std::back_inserter(m_writeScratch), "{:.6f}\t", physical_input.tilt_mm()); // param[4]
    std::format_to(std::back_inserter(m_writeScratch), "{:.6f}\t",
                   physical_input.front_friction_force_N);                                                // param[5]
    std::format_to(std::back_inserter(m_writeScratch), "{:.6f}\t", physical_input.rear_friction_force_N); // param[6]
    std::format_to(std::back_inserter(m_writeScratch), "{:.6f}\t", physical_output.motor_rpm);            // param[7]
    std::format_to(std::back_inserter(m_writeScratch), "{:.6f}\t", physical_output.front_ep_kpa);         // param[8]
    std::format_to(std::back_inserter(m_writeScratch), "{:.6f}\t", physical_output.rear_ep_kpa);          // param[9]
    std::format_to(std::back_inserter(m_writeScratch), "{:.6f}\t", DAVout[CH_MotorSpeed]);                // param[10]
    std::format_to(std::back_inserter(m_writeScratch), "{:.6f}\t", DAVout[CH_EP_Cell_f]);                 // param[11]
    std::format_to(std::back_inserter(m_writeScratch), "{:.6f}\t", DAVout[CH_EP_Cell_r]);                 // param[12]
    std::format_to(std::back_inserter(m_writeScratch), "{:.6f}\t",
                   static_cast<double>(control::num_cyclic)); // param[13]
    std::format_to(std::back_inserter(m_writeScratch), "{:.6f}\t",
                   static_cast<double>(control::current_step_index)); // param[14]
    std::format_to(std::back_inserter(m_writeScratch), "{:.6f}\t",
                   std::chrono::seconds_d{control::step_elapsed}.count()); // param[15]
    m_writeScratch.back() = '\n';                                          // Replace last tab with newline
    m_paramWriter.stream().write(m_writeScratch.data(), static_cast<std::streamsize>(m_writeScratch.size()));
}
//--- Control Statements ---
void CDigitShowDSTDoc::Start_Control()
{
    spdlog::info("Control started");
}
void CDigitShowDSTDoc::Stop_Control()
{
    spdlog::info("Control stopped");
    DAVout[CH_Motor] = 0.0f;      // Motor Stop
    DAVout[CH_MotorSpeed] = 0.0f; // Motor Speed->0
    if (const auto result = digitshow::write_analog_outputs(); !result)
    {
        spdlog::warn("Failed to write analog outputs during stop: {}", result.error());
    }
    // Synchronize latest_physical_output with updated DAVout
    variables::physical::update();
    spdlog::debug("Motor stopped, DAVout reset");
}

//--- API Server Methods ---
bool CDigitShowDSTDoc::StartApiServer() noexcept
{
    try
    {
        const auto config = api::ApiServer::load_config("api_config.json");
        if (!config.enabled)
        {
            spdlog::info("API server is disabled in configuration");
            return true; // Not an error, just disabled
        }

        if (m_apiServer.start(config))
        {
            spdlog::info("API server started successfully");
            return true;
        }
        else
        {
            spdlog::error("Failed to start API server");
            return false;
        }
    }
    catch (const std::exception &e)
    {
        spdlog::error("Exception while starting API server: {}", e.what());
        return false;
    }
}

void CDigitShowDSTDoc::StopApiServer() noexcept
{
    if (m_apiServer.is_running())
    {
        m_apiServer.stop();
        spdlog::info("API server stopped");
    }
}

void CDigitShowDSTDoc::UpdateApiServerData() noexcept
{
    if (m_apiServer.is_running())
    {
        const auto input = variables::physical::latest_physical_input.load();
        const auto output = variables::physical::latest_physical_output.load();
        m_apiServer.update_sensor_data(input, output);
    }
}

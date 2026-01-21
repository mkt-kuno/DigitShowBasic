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
 * @file DigitShowDSTDoc.h
 * @brief Declaration and interface definition for CDigitShowDSTDoc class
 *
 * CDigitShowDSTDoc クラスの宣言およびインターフェイスの定義をします。
 */
/////////////////////////////////////////////////////////////////////////////

#pragma once
#if !defined(AFX_DIGITSHOWDSTDOC_H__0F5B25DB_9338_44C6_9841_265C5A221957__INCLUDED_)
#define AFX_DIGITSHOWDSTDOC_H__0F5B25DB_9338_44C6_9841_265C5A221957__INCLUDED_

#include "ApiServer.hpp"
#include "TsvWriter.hpp"
#include <chrono>
#include <cstddef>
#include <filesystem>
#include <string>

/**
 * @class CDigitShowDSTDoc
 * @brief Main document class for DigitShowDST application
 *
 * Manages test data, hardware control, data logging, and test execution.
 * シリアライズ機能のみから作成します。
 */
class CDigitShowDSTDoc : public CDocument
{
  protected:
    /** @brief Default constructor (created only from serialization) */
    CDigitShowDSTDoc() noexcept = default;
    DECLARE_DYNCREATE(CDigitShowDSTDoc)

    // アトリビュート
  public:
    // オペレーション
  public:
    // オーバーライド
    //  ClassWizard は仮想関数のオーバーライドを生成します。
    //{{AFX_VIRTUAL(CDigitShowDSTDoc)
  public:
    /** @brief Create new document */
    BOOL OnNewDocument() override;

    /** @brief Serialize document data */
    void Serialize(CArchive &ar) override;
    //}}AFX_VIRTUAL

    // インプリメンテーション
  public:
    /** @brief Stop all control operations */
    void Stop_Control();

    /** @brief Start control operations */
    void Start_Control();

    /** @brief Save data to file (periodic logging) */
    void SaveToFile();

    /**
     * @brief Open TSV writers for data logging
     * @param basePath Base file path (writers will append _vlt.tsv, .tsv, _out.tsv)
     * @return true if all writers opened successfully, false otherwise
     */
    bool OpenSaveWriters(const std::filesystem::path &basePath);

    /**
     * @brief Close all TSV writers
     */
    void CloseSaveWriters() noexcept;

    /**
     * @brief Flush TSV writers if needed (on control step change or after 1 minute)
     *
     * Should be called periodically during control recording. Flushes when:
     * - Control step number (current_step_index) changes
     * - More than 1 minute has elapsed since last flush
     */
    void FlushWritersIfNeeded();

    /**
     * @brief Force flush all TSV writers to disk
     */
    void FlushWriters();

    /**
     * @brief Reset flush tracking state (should be called when starting recording)
     */
    void ResetFlushState() noexcept;

    /**
     * @brief Start the API server if enabled in configuration
     * @return true if server started successfully or is disabled, false on error
     */
    bool StartApiServer() noexcept;

    /**
     * @brief Stop the API server
     */
    void StopApiServer() noexcept;

    /**
     * @brief Update API server with current sensor data
     * Should be called from Timer 1 (50ms UI refresh)
     */
    void UpdateApiServerData() noexcept;

    /** @brief Destructor */
    ~CDigitShowDSTDoc() override = default;
#ifdef _DEBUG
    virtual void AssertValid() const override;
    virtual void Dump(CDumpContext &dc) const override;
#endif

  protected:
    // 生成されたメッセージ マップ関数
  private:
    /** @brief TSV writer for voltage data (_vlt.tsv) */
    TsvWriter m_vltWriter;

    /** @brief TSV writer for physical data (.tsv) */
    TsvWriter m_phyWriter;

    /** @brief TSV writer for parameter data (_out.tsv) */
    TsvWriter m_paramWriter;

    /** @brief Reusable scratch buffer for formatting to avoid repeated allocations */
    std::string m_writeScratch;

    /** @brief Last control step number when flush was performed */
    std::size_t m_lastFlushedCurnum = 0;

    /** @brief Time point of last flush operation */
    std::chrono::steady_clock::time_point m_lastFlushTime{};

    /** @brief API server for streaming sensor data */
    api::ApiServer m_apiServer;

  protected:
    //{{AFX_MSG(CDigitShowDSTDoc)
    // メモ - ClassWizard はこの位置にメンバ関数を追加または削除します。
    //        この位置に生成されるコードを編集しないでください。
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ は前行の直前に追加の宣言を挿入します。

#endif // !defined(AFX_DIGITSHOWDSTDOC_H__0F5B25DB_9338_44C6_9841_265C5A221957__INCLUDED_)

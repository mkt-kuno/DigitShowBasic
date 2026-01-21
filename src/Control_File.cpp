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
 * @file Control_File.cpp
 * @brief Implementation of control file dialog and script execution
 */
#include "StdAfx.h"

#pragma warning(push)
#pragma warning(disable : 4800 4866)
#include "Control_File.h"
#include "charconv.hpp"
#include "control/json.hpp"
#include "resource.h"
#include <filesystem>
#include <format>
#include <optional>
#include <ryml/ryml.hpp>
#include <ryml/ryml_std.hpp>
#include <spdlog/spdlog.h>
#include <sstream>

#include "ui_helpers.hpp"
#include "version_info.hpp"

namespace
{

std::wstring build_step_label(const control::ControlParams &params)
{
    const auto use = get_use_label(params.pattern);
    if (!params.name.empty())
    {
        return std::format(L"{}\n{}", to_wstring_utf8(params.name), to_wstring_utf8(use));
    }
    return to_wstring_utf8(use);
}

auto emit_step_yaml(const control::ControlParams &params)
{
    ryml::Tree tree = ControlParamsToJsonStep(params);
    std::string yaml_text;
    ryml::emitrs_yaml(tree, &yaml_text);
    return normalize_to_crlf(yaml_text);
}

std::expected<control::ControlParams, std::vector<control::ParseError>> parse_yaml_to_params(const CStringW &text)
{
    const std::string yaml = normalize_to_lf(to_utf8(text));
    if (yaml.empty())
    {
        return std::unexpected(
            std::vector{control::ParseError{control::ParseErrorKind::ValueAbsent, "text", "YAML text is empty"}});
    }

    try
    {
        ryml::Tree tree = ryml::parse_in_arena(ryml::to_csubstr(yaml));
        ryml::ConstNodeRef root = tree.rootref();
        return JsonStepToControlParams(root, 0);
    }
    catch (const std::exception &e)
    {
        spdlog::error("Error parsing control step YAML: {}", e.what());
        return std::unexpected(std::vector{control::ParseError{control::ParseErrorKind::SyntaxError, "text",
                                                               std::format("YAML syntax error: {}", e.what())}});
    }
}

std::wstring format_step_position(size_t idx, size_t total, const control::ControlParams *params)
{
    if (total == 0)
        return L"";
    std::wstring label = std::format(L"{}/{}", idx + 1, total);
    if (params != nullptr)
    {
        const std::wstring desc = build_step_label(*params);
        if (!desc.empty())
        {
            label += L" ";
            label += desc;
        }
    }
    return label;
}
} // namespace

BEGIN_MESSAGE_MAP_IGNORE_UNUSED_LOCAL_TYPEDEF(CControl_File, CDialog)
ON_BN_CLICKED(IDC_BUTTON_Update, &CControl_File::OnButtonUpdate)
ON_BN_CLICKED(IDC_BUTTON_Clear, &CControl_File::OnButtonClear)
ON_BN_CLICKED(IDC_BUTTON_Import, &CControl_File::OnButtonImport)
ON_BN_CLICKED(IDC_BUTTON_Export, &CControl_File::OnButtonExport)
ON_BN_CLICKED(IDC_BUTTON_ExecPrev, &CControl_File::OnButtonExecPrev)
ON_BN_CLICKED(IDC_BUTTON_ExecNext, &CControl_File::OnButtonExecNext)
ON_BN_CLICKED(IDC_BUTTON_EditPrev, &CControl_File::OnButtonEditPrev)
ON_BN_CLICKED(IDC_BUTTON_EditNext, &CControl_File::OnButtonEditNext)
ON_EN_CHANGE(IDC_EDIT_Yaml, &CControl_File::OnYamlChanged)
ON_EN_KILLFOCUS(IDC_EDIT_Yaml, &CControl_File::OnYamlKillFocus)
ON_BN_CLICKED(IDC_BUTTON_Start, &CControl_File::OnButtonStart)
ON_BN_CLICKED(IDC_BUTTON_Stop, &CControl_File::OnButtonStop)
ON_BN_CLICKED(IDC_BUTTON_Pause, &CControl_File::OnButtonPause)
ON_BN_CLICKED(IDC_BUTTON_Resume, &CControl_File::OnButtonResume)
ON_WM_CTLCOLOR()
ON_WM_TIMER()
END_MESSAGE_MAP_IGNORE_UNUSED_LOCAL_TYPEDEF()

CControl_File::CControl_File(CWnd *pParent) : CDialog(CControl_File::IDD, pParent)
{
    if (!control::control_steps.empty())
    {
        m_EditingStepIndex =
            (control::current_step_index < control::control_steps.size()) ? control::current_step_index : 0;
    }
}

void CControl_File::DoDataExchange(CDataExchange *pDX)
{
    CDialog::DoDataExchange(pDX);
    DDX_Text(pDX, IDC_EDIT_Yaml, m_YamlText);
    DDX_Text(pDX, IDC_STATIC_VALIDATION, m_ValidationText);
    DDX_Text(pDX, IDC_STATIC_ExecLabel, m_ExecLabel);
    DDX_Text(pDX, IDC_STATIC_EditLabel, m_EditLabel);
    DDX_Text(pDX, IDC_EDIT_FilePath, m_FilePath);
}

BOOL CControl_File::OnInitDialog()
{
    CDialog::OnInitDialog();

    // Set text limit for YAML editor to allow large multi-line content
    CEdit *pYamlEdit = dynamic_cast<CEdit *>(GetDlgItem(IDC_EDIT_Yaml));
    if (pYamlEdit)
    {
        pYamlEdit->SetLimitText(0); // 0 means unlimited (up to ~2GB)
    }

    if (control::control_steps.empty())
    {
        m_ExecLabel = L"";
        m_EditLabel = L"";
        m_YamlText = L"";
        m_ValidationText = L"Import a file to begin.";
        m_LastValidationOk = false;
        UpdateData(false);
        UpdateNavEnabled();
        GetDlgItem(IDC_BUTTON_Update)->EnableWindow(false);
        GetDlgItem(IDC_BUTTON_Clear)->EnableWindow(false);
        GetDlgItem(IDC_EDIT_Yaml)->EnableWindow(false);
        GetDlgItem(IDC_BUTTON_Export)->EnableWindow(false);
        GetDlgItem(IDC_BUTTON_Start)->EnableWindow(false);
        GetDlgItem(IDC_BUTTON_Stop)->EnableWindow(false);
        GetDlgItem(IDC_BUTTON_Pause)->EnableWindow(false);
        GetDlgItem(IDC_BUTTON_Resume)->EnableWindow(false);
        return true;
    }

    m_EditingStepIndex =
        (control::current_step_index < control::control_steps.size()) ? control::current_step_index : 0;
    LoadStepIntoEditor(m_EditingStepIndex);
    UpdateExecLabel();
    UpdateEditLabel();
    UpdateNavEnabled();
    // Initialize Start/Stop/Pause/Resume button states based on control running state
    if (control::is_control_running)
    {
        GetDlgItem(IDC_BUTTON_Start)->EnableWindow(false);
        GetDlgItem(IDC_BUTTON_Stop)->EnableWindow(true);
        GetDlgItem(IDC_BUTTON_Pause)->EnableWindow(true);
        GetDlgItem(IDC_BUTTON_Resume)->EnableWindow(false);
    }
    else
    {
        GetDlgItem(IDC_BUTTON_Start)->EnableWindow(true);
        GetDlgItem(IDC_BUTTON_Stop)->EnableWindow(false);
        GetDlgItem(IDC_BUTTON_Pause)->EnableWindow(false);
        GetDlgItem(IDC_BUTTON_Resume)->EnableWindow(false);
    }
    return true;
}

void CControl_File::OnButtonImport()
{
    CFileDialog dlg(true, nullptr, nullptr, OFN_FILEMUSTEXIST | OFN_HIDEREADONLY,
                    _T("Config Files (*.json;*.yml;*.yaml)|*.json;*.yml;*.yaml|JSON Files (*.json)|*.json|YAML Files "
                       "(*.yml;*.yaml)|*.yml;*.yaml|All Files (*.*)|*.*||"),
                    nullptr);

    if (dlg.DoModal() != IDOK)
    {
        spdlog::debug("Control script load cancelled by user");
        return;
    }

    const std::wstring wpath = std::wstring(dlg.GetPathName().GetString());
    const std::string path_u8{CW2A(wpath.c_str(), CP_UTF8)};
    m_FilePath = wpath.c_str();

    try
    {
        const auto tree_result = LoadConfigFile(std::filesystem::path(wpath));
        if (!tree_result)
        {
            const auto &err = tree_result.error();
            spdlog::error("Failed to load control config file {}: {}", path_u8, err.format());

            std::wstring error_msg =
                std::format(L"Failed to load control config file.\n\n{}", to_wstring_utf8(err.format()));
            AfxMessageBox(error_msg.c_str(), MB_ICONEXCLAMATION | MB_OK);
            return;
        }

        ryml::ConstNodeRef root = tree_result.value().rootref();
        if (!root.has_child("steps") || !root["steps"].is_seq())
        {
            spdlog::error("Invalid control config format: missing 'steps' array");
            AfxMessageBox(_T("Invalid control config format: missing 'steps' array."), MB_ICONEXCLAMATION | MB_OK);
            return;
        }

        spdlog::debug("Initializing control arrays");
        control::control_steps.clear();
        control::reset_current_step_index();

        int loaded = 0;
        std::vector<std::string> all_errors;

        for (size_t i = 0; const auto &step : root["steps"])
        {

            [[maybe_unused]] auto _ = JsonStepToControlParams(step, i)
                                          .transform([&](const auto &parameters) {
                                              control::control_steps.push_back({.parameters = parameters});
                                              ++loaded;
                                              spdlog::debug("Loaded step {}: {}", control::control_steps.size() - 1,
                                                            get_use_label(parameters.pattern));
                                              return parameters;
                                          })
                                          .transform_error([&](const auto &errors) {
                                              spdlog::warn("Failed to parse step {}: {} error(s)", i, errors.size());
                                              for (const auto &err : errors)
                                              {
                                                  spdlog::warn("  - {}", err.format());
                                                  all_errors.push_back(err.format());
                                              }
                                              return errors;
                                          });
            ++i;
        }

        if (loaded == 0)
        {
            std::wstring error_msg = L"No valid steps found.";
            if (!all_errors.empty())
            {
                error_msg += L"\n\nParse errors:\n";
                for (const auto &err : all_errors)
                {
                    error_msg += to_wstring_utf8(err) + L"\n";
                }
            }
            AfxMessageBox(error_msg.c_str(), MB_ICONWARNING | MB_OK);
            return;
        }

        // Show warnings if some steps had errors
        if (!all_errors.empty())
        {
            std::wstring warning_msg =
                std::format(L"Loaded {} step(s), but {} step(s) had errors:\n\n", loaded, all_errors.size());
            size_t display_count = std::min(all_errors.size(), size_t(5)); // Limit to first 5 errors
            for (size_t i = 0; i < display_count; ++i)
            {
                warning_msg += to_wstring_utf8(all_errors[i]) + L"\n";
            }
            if (all_errors.size() > display_count)
            {
                warning_msg +=
                    std::format(L"... and {} more error(s). Check log for details.", all_errors.size() - display_count);
            }
            AfxMessageBox(warning_msg.c_str(), MB_ICONWARNING | MB_OK);
        }

        m_EditingStepIndex = 0;
        LoadStepIntoEditor(0);
        UpdateExecLabel();
        UpdateEditLabel();
        UpdateNavEnabled();
        // Update Start/Stop/Pause/Resume button states after import
        GetDlgItem(IDC_BUTTON_Start)->EnableWindow(true);
        GetDlgItem(IDC_BUTTON_Stop)->EnableWindow(false);
        GetDlgItem(IDC_BUTTON_Pause)->EnableWindow(false);
        GetDlgItem(IDC_BUTTON_Resume)->EnableWindow(false);
        spdlog::info("Imported {} steps. Current step reset to 1/{}", loaded, loaded);
        SetValidationMessage(std::format(L"Imported {} steps. Current step reset to 1/{}", loaded, loaded).c_str(),
                             true);
    }
    catch (const std::exception &e)
    {
        spdlog::error("Error loading control config: {}", e.what());
        AfxMessageBox(CA2W(std::format("Error loading control config: {}", e.what()).c_str(), CP_UTF8),
                      MB_ICONEXCLAMATION | MB_OK);
    }
}

void CControl_File::OnButtonExport()
{
    if (control::control_steps.empty())
    {
        AfxMessageBox(_T("No steps to export."), MB_ICONEXCLAMATION | MB_OK);
        return;
    }

    if (m_HasUnsavedEdits)
    {
        if (AfxMessageBox(_T("You have unsaved edits. Save them before export?"), MB_YESNO | MB_ICONQUESTION) == IDYES)
        {
            OnButtonUpdate();
            if (m_HasUnsavedEdits)
                return; // save failed
        }
    }

    CFileDialog dlg(false, _T("yml"), _T("*.yml"), OFN_CREATEPROMPT | OFN_OVERWRITEPROMPT,
                    _T("YAML Files (*.yml;*.yaml)|*.yml;*.yaml|JSON Files (*.json)|*.json|All Files (*.*)|*.*||"),
                    nullptr);
    if (dlg.DoModal() != IDOK)
        return;

    const std::wstring wpath = std::wstring(dlg.GetPathName().GetString());
    const auto format = DetectFormat(std::filesystem::path(wpath));
    try
    {
        ryml::Tree tree;
        ryml::NodeRef root = tree.rootref();
        root |= ryml::MAP;
        root["$schema"] << "../schemas/control_script.schema.json";

        // Add version information (git commit hash)
        const auto version = version_info::get_version_string();
        if (!version.empty())
        {
            root["version"] << version;
            spdlog::debug("Added version info to control script: {}", version);
        }

        ryml::NodeRef steps = root["steps"];
        steps |= ryml::SEQ;
        for (const auto &step : control::control_steps)
        {
            ryml::Tree step_tree = ControlParamsToJsonStep(step.parameters);
            ryml::NodeRef new_step = steps.append_child();
            new_step |= ryml::MAP;
            for (const auto &child : step_tree.rootref())
            {
                const auto key = child.key();
                if (child.is_map())
                {
                    ryml::NodeRef dst = new_step[ryml::to_csubstr(key)];
                    dst |= ryml::MAP;
                    for (const auto &grand : child)
                    {
                        dst[grand.key()] << grand.val();
                    }
                }
                else if (child.is_seq())
                {
                    ryml::NodeRef dst = new_step[ryml::to_csubstr(key)];
                    dst |= ryml::SEQ;
                    for (const auto &item : child)
                    {
                        dst.append_child() << item.val();
                    }
                }
                else
                {
                    new_step[ryml::to_csubstr(key)] << child.val();
                }
            }
        }

        if (!SaveConfigFile(std::filesystem::path(wpath), tree, format))
        {
            const auto path_u8 = to_utf8(wpath.c_str());
            spdlog::error("Failed to save control config file: {}", path_u8);
            AfxMessageBox(_T("Failed to save control config."), MB_ICONEXCLAMATION | MB_OK);
            return;
        }

        spdlog::info("Control script saved successfully");
        m_FilePath = wpath.c_str();
        SetValidationMessage(
            std::format(L"Exported {} steps to {}", control::control_steps.size(), m_FilePath.GetString()).c_str(),
            true);
    }
    catch (const std::exception &e)
    {
        spdlog::error("Error saving control config: {}", e.what());
        AfxMessageBox(CA2W(std::format("Error saving control config: {}", e.what()).c_str(), CP_UTF8),
                      MB_ICONEXCLAMATION | MB_OK);
    }
}

void CControl_File::OnButtonExecPrev()
{
    if (control::control_steps.empty() || control::current_step_index == 0)
        return;

    if (!ConfirmStepChangeWhenRunning())
        return;

    if (!control::decrement_current_step_index())
    {
        spdlog::warn("Failed to decrement step index (already at first step)");
        return;
    }
    UpdateExecLabel();
    UpdateNavEnabled();
}

void CControl_File::OnButtonExecNext()
{
    if (control::control_steps.empty() || control::current_step_index + 1 >= control::control_steps.size())
        return;

    if (!ConfirmStepChangeWhenRunning())
        return;

    if (!control::increment_current_step_index())
    {
        spdlog::warn("Failed to increment step index (already at last step)");
        return;
    }
    UpdateExecLabel();
    UpdateNavEnabled();
}

void CControl_File::OnButtonEditPrev()
{
    if (control::control_steps.empty() || m_EditingStepIndex == 0)
        return;

    if (m_HasUnsavedEdits)
    {
        const int res =
            AfxMessageBox(_T("You have unsaved edits. Save before moving?"), MB_YESNOCANCEL | MB_ICONQUESTION);
        if (res == IDCANCEL)
            return;
        if (res == IDYES)
        {
            OnButtonUpdate();
            if (m_HasUnsavedEdits)
                return;
        }
    }

    m_EditingStepIndex--;
    LoadStepIntoEditor(m_EditingStepIndex);
    UpdateEditLabel();
    UpdateNavEnabled();
}

void CControl_File::OnButtonEditNext()
{
    if (control::control_steps.empty() || m_EditingStepIndex + 1 >= control::control_steps.size())
        return;

    if (m_HasUnsavedEdits)
    {
        const int res =
            AfxMessageBox(_T("You have unsaved edits. Save before moving?"), MB_YESNOCANCEL | MB_ICONQUESTION);
        if (res == IDCANCEL)
            return;
        if (res == IDYES)
        {
            OnButtonUpdate();
            if (m_HasUnsavedEdits)
                return;
        }
    }

    m_EditingStepIndex++;
    LoadStepIntoEditor(m_EditingStepIndex);
    UpdateEditLabel();
    UpdateNavEnabled();
}

void CControl_File::OnButtonUpdate()
{
    if (control::control_steps.empty())
        return;

    control::ControlParams parsed{};
    if (!ValidateYaml(parsed))
        return;

    const bool touches_active = (m_EditingStepIndex == control::current_step_index);
    if (touches_active && !ConfirmUpdateWhenActive(m_EditingStepIndex))
        return;

    control::control_steps[m_EditingStepIndex].parameters = parsed;
    m_HasUnsavedEdits = false;
    UpdateEditLabel();
    UpdateExecLabel();
    UpdateNavEnabled();
    SetValidationMessage(L"Update successful.", true);
}

void CControl_File::OnButtonClear()
{
    if (m_YamlText.IsEmpty())
        return;
    if (AfxMessageBox(_T("Clear the editor?"), MB_YESNO | MB_ICONQUESTION) != IDYES)
        return;
    m_YamlText.Empty();
    m_HasUnsavedEdits = true;
    m_LastValidationOk = false;
    SetValidationMessage(L"Editor cleared.", false);
    UpdateData(false);
}

void CControl_File::OnYamlChanged()
{
    UpdateData(true);
    m_HasUnsavedEdits = true;
    m_LastValidationOk = false;
    SetValidationMessage(L"Validating...", false);
    RestartYamlValidationTimer();
}

void CControl_File::OnYamlKillFocus()
{
    KillTimer(kYamlValidationTimerId);
    control::ControlParams parsed{};
    ValidateYaml(parsed);
    m_HasUnsavedEdits = true;
}

void CControl_File::OnOK()
{
    if (m_HasUnsavedEdits)
    {
        const int res =
            AfxMessageBox(_T("You have unsaved edits. Save before closing?"), MB_YESNOCANCEL | MB_ICONQUESTION);
        if (res == IDCANCEL)
            return;
        if (res == IDYES)
        {
            OnButtonUpdate();
            if (m_HasUnsavedEdits)
                return;
        }
    }
    CDialog::OnOK();
}

void CControl_File::OnCancel()
{
    if (m_HasUnsavedEdits)
    {
        const int res =
            AfxMessageBox(_T("You have unsaved edits. Discard and close?"), MB_YESNOCANCEL | MB_ICONQUESTION);
        if (res != IDYES)
            return;
    }
    CDialog::OnCancel();
}

void CControl_File::OnTimer(UINT_PTR nIDEvent)
{
    if (nIDEvent == kYamlValidationTimerId)
    {
        KillTimer(kYamlValidationTimerId);
        control::ControlParams parsed{};
        ValidateYaml(parsed);
        CDialog::OnTimer(nIDEvent);
        return;
    }
    CDialog::OnTimer(nIDEvent);
}

void CControl_File::RestartYamlValidationTimer()
{
    KillTimer(kYamlValidationTimerId);
    SetTimer(kYamlValidationTimerId, kYamlValidationDelayMs, nullptr);
}

bool CControl_File::ValidateYaml(control::ControlParams &out_params)
{
    UpdateData(true);
    return parse_yaml_to_params(m_YamlText)
        .transform([&](const auto &params) {
            out_params = params;
            const std::wstring msg =
                std::format(L"✓ Valid YAML; schema: {}", to_wstring_utf8(get_use_label(out_params.pattern)));
            SetValidationMessage(msg.c_str(), true);
            m_LastValidationOk = true;
            UpdateData(false);
            return true;
        })
        .transform_error([&](const auto &errors) {
            std::wstring error_msg = L"YAML parse failed:\r\n";
            size_t display_count = std::min(errors.size(), size_t(3)); // Limit to first 3 errors
            for (size_t i = 0; i < display_count; ++i)
            {
                error_msg += to_wstring_utf8(errors[i].format()) + L"\r\n";
            }
            if (errors.size() > display_count)
            {
                error_msg += std::format(L"... and {} more error(s)", errors.size() - display_count);
            }
            SetValidationMessage(error_msg.c_str(), false);
            m_LastValidationOk = false;
            UpdateData(false);
            return false;
        })
        .value_or(false);
}

bool CControl_File::ConfirmUpdateWhenActive(size_t target_step)
{
    if (target_step != control::current_step_index)
        return true;
    const int res =
        AfxMessageBox(_T("You are updating the currently active step. Apply changes?"), MB_YESNO | MB_ICONQUESTION);
    return res == IDYES;
}

bool CControl_File::IsControlRunning() const
{
    return control::is_control_running;
}

bool CControl_File::ConfirmStepChangeWhenRunning() const
{
    if (!IsControlRunning())
        return true;
    const int res =
        AfxMessageBox(_T("Control is currently running. Change the execution step?"), MB_YESNO | MB_ICONQUESTION);
    return res == IDYES;
}

void CControl_File::LoadStepIntoEditor(size_t index)
{
    if (index >= control::control_steps.size())
        return;
    const auto &params = control::control_steps[index].parameters;
    m_YamlText = to_wstring_utf8(emit_step_yaml(params)).c_str();
    m_ValidationText = L"";
    m_LastValidationOk = true;
    m_HasUnsavedEdits = false;
    UpdateEditLabel();
    UpdateData(false);
}

void CControl_File::UpdateExecLabel()
{
    if (control::control_steps.empty())
    {
        m_ExecLabel = L"";
    }
    else if (control::current_step_index >= control::control_steps.size())
    {
        // Safety check: prevent out-of-bounds access
        // This can occur when:
        // 1. Control completes the last step and increments beyond bounds
        // 2. Dialog is opened immediately after control completion
        // 3. Race condition between control timer and UI update
        spdlog::warn("UpdateExecLabel: current_step_index={} out of bounds (size={}), control_running={}, "
                     "resetting to 0. This may indicate control completed or a race condition.",
                     control::current_step_index, control::control_steps.size(), control::is_control_running);
        control::reset_current_step_index();
        m_ExecLabel = L"[Step index out of bounds - reset to 0]";
    }
    else
    {
        const auto &params = control::control_steps[control::current_step_index].parameters;
        m_ExecLabel = format_step_position(control::current_step_index, control::control_steps.size(), &params).c_str();
    }
    UpdateData(false);
}

void CControl_File::UpdateEditLabel()
{
    if (control::control_steps.empty())
    {
        m_EditLabel = L"";
    }
    else
    {
        const auto &params = control::control_steps[m_EditingStepIndex].parameters;
        m_EditLabel = format_step_position(m_EditingStepIndex, control::control_steps.size(), &params).c_str();
    }
    UpdateData(false);
}

void CControl_File::UpdateNavEnabled()
{
    const bool has_steps = !control::control_steps.empty();
    const bool exec_has_prev = has_steps && control::current_step_index > 0;
    const bool exec_has_next = has_steps && (control::current_step_index + 1 < control::control_steps.size());
    const bool edit_has_prev = has_steps && m_EditingStepIndex > 0;
    const bool edit_has_next = has_steps && (m_EditingStepIndex + 1 < control::control_steps.size());

    GetDlgItem(IDC_BUTTON_ExecPrev)->EnableWindow(exec_has_prev);
    GetDlgItem(IDC_BUTTON_ExecNext)->EnableWindow(exec_has_next);
    GetDlgItem(IDC_BUTTON_EditPrev)->EnableWindow(edit_has_prev);
    GetDlgItem(IDC_BUTTON_EditNext)->EnableWindow(edit_has_next);

    const bool editable = has_steps;
    GetDlgItem(IDC_EDIT_Yaml)->EnableWindow(editable);
    GetDlgItem(IDC_BUTTON_Update)->EnableWindow(editable && m_LastValidationOk);
    GetDlgItem(IDC_BUTTON_Clear)->EnableWindow(editable);
    GetDlgItem(IDC_BUTTON_Export)->EnableWindow(editable);
}

void CControl_File::MarkDirty()
{
    m_HasUnsavedEdits = true;
}

void CControl_File::SetValidationMessage(const CStringW &msg, bool ok)
{
    m_ValidationText = msg;
    m_LastValidationOk = ok;
    GetDlgItem(IDC_BUTTON_Update)->EnableWindow(ok && !control::control_steps.empty());
    UpdateData(false);
}

HBRUSH CControl_File::OnCtlColor(CDC *pDC, CWnd *pWnd, UINT nCtlColor)
{
    HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);
    if (pWnd->GetDlgCtrlID() == IDC_STATIC_VALIDATION)
    {
        pDC->SetTextColor(m_LastValidationOk ? RGB(0, 128, 0) : RGB(192, 0, 0));
    }
    return hbr;
}

void CControl_File::OnButtonStart()
{
    if (control::control_steps.empty())
        return;

    // Send message to main view to start control (BEFORE advancing step)
    CWnd *pMainWnd = AfxGetMainWnd();
    if (pMainWnd)
    {
        CWnd *pView = pMainWnd->GetDescendantWindow(AFX_IDW_PANE_FIRST, true);
        if (pView)
        {
            pView->SendMessage(WM_COMMAND, MAKEWPARAM(IDC_BUTTON_CtrlOn, BN_CLICKED), 0);
        }
    }

    // Update button states to reflect that control is now running
    GetDlgItem(IDC_BUTTON_Start)->EnableWindow(false);
    GetDlgItem(IDC_BUTTON_Stop)->EnableWindow(true);
    GetDlgItem(IDC_BUTTON_Pause)->EnableWindow(true);
    GetDlgItem(IDC_BUTTON_Resume)->EnableWindow(false);
}

void CControl_File::OnButtonStop()
{
    // Send message to main view to stop control
    CWnd *pMainWnd = AfxGetMainWnd();
    if (pMainWnd)
    {
        CWnd *pView = pMainWnd->GetDescendantWindow(AFX_IDW_PANE_FIRST, true);
        if (pView)
        {
            pView->SendMessage(WM_COMMAND, MAKEWPARAM(IDC_BUTTON_CtrlOff, BN_CLICKED), 0);
        }
    }

    // Update button states to reflect that control is now stopped
    GetDlgItem(IDC_BUTTON_Start)->EnableWindow(true);
    GetDlgItem(IDC_BUTTON_Stop)->EnableWindow(false);
    GetDlgItem(IDC_BUTTON_Pause)->EnableWindow(false);
    GetDlgItem(IDC_BUTTON_Resume)->EnableWindow(false);
}

void CControl_File::OnButtonPause()
{
    // Send message to main view to pause control (same as stop for now)
    CWnd *pMainWnd = AfxGetMainWnd();
    if (pMainWnd)
    {
        CWnd *pView = pMainWnd->GetDescendantWindow(AFX_IDW_PANE_FIRST, true);
        if (pView)
        {
            pView->SendMessage(WM_COMMAND, MAKEWPARAM(IDC_BUTTON_CtrlOff, BN_CLICKED), 0);
        }
    }

    // Update button states: pause means we can resume
    GetDlgItem(IDC_BUTTON_Start)->EnableWindow(false);
    GetDlgItem(IDC_BUTTON_Stop)->EnableWindow(true);
    GetDlgItem(IDC_BUTTON_Pause)->EnableWindow(false);
    GetDlgItem(IDC_BUTTON_Resume)->EnableWindow(true);
}

void CControl_File::OnButtonResume()
{
    // Send message to main view to resume control
    CWnd *pMainWnd = AfxGetMainWnd();
    if (pMainWnd)
    {
        CWnd *pView = pMainWnd->GetDescendantWindow(AFX_IDW_PANE_FIRST, true);
        if (pView)
        {
            pView->SendMessage(WM_COMMAND, MAKEWPARAM(IDC_BUTTON_CtrlOn, BN_CLICKED), 0);
        }
    }

    // Update button states to reflect that control is running again
    GetDlgItem(IDC_BUTTON_Start)->EnableWindow(false);
    GetDlgItem(IDC_BUTTON_Stop)->EnableWindow(true);
    GetDlgItem(IDC_BUTTON_Pause)->EnableWindow(true);
    GetDlgItem(IDC_BUTTON_Resume)->EnableWindow(false);
}

#pragma warning(pop)
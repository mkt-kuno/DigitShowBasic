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
 * @file ui_helpers.hpp
 * @brief Small MFC UI helpers for dialog sizing/labels
 */

#pragma once

#include <afxwin.h> // MFC core and standard components
#include <string_view>

/**
 * @brief Set status text to a static control and expand the dialog so the text is not clipped.
 *
 * - Respects explicit line breaks ("\n").
 * - Expands dialog width to fit the longest line plus padding.
 * - Expands dialog height to fit the number of lines (no word-wrap assumed).
 * - Keeps the control's top-left position; grows to the right and downward.
 *
 * Safe to call repeatedly; if there is enough space, it is a no-op.
 */
inline void set_status_text_and_fit_dialog(CWnd &dlg, const int statusCtrlId, const std::string_view text,
                                           const int pad_px = 12) noexcept
{
    LPCTSTR textW = CA2W(text.data(), CP_UTF8);
    dlg.SetDlgItemText(statusCtrlId, textW);

    CWnd *const pCtrl = dlg.GetDlgItem(statusCtrlId);
    if (pCtrl == nullptr)
        return;

    // Measure text using the control's font
    CClientDC dc(pCtrl);
    CFont *const font = pCtrl->GetFont();
    CFont *pOld = nullptr;
    if (font)
        pOld = dc.SelectObject(font);

    // Split by explicit line breaks
    int pos = 0;
    CStringW line;
    LONG max_line_px = 0;
    int line_count = 0;
    CStringW rest = textW;
    while (!(line = rest.Tokenize(_T("\n"), pos)).IsEmpty() || (pos >= 0 && line_count == 0))
    {
        // Tokenize returns empty for trailing breaks; guard initial empty
        const CSize sz = dc.GetTextExtent(line);
        max_line_px = std::max(max_line_px, sz.cx);
        ++line_count;
        if (pos < 0)
            break;
    }

    if (line_count <= 0)
        line_count = 1;

    TEXTMETRIC tm{};
    dc.GetTextMetrics(&tm);
    const int line_height = tm.tmHeight + tm.tmExternalLeading;
    const int needed_w = max_line_px + pad_px;
    const int needed_h = line_count * line_height + 2; // a little breathing room

    if (pOld)
        dc.SelectObject(pOld);

    // Current rectangles (client coords)
    CRect rcCtrl;
    pCtrl->GetWindowRect(&rcCtrl);
    dlg.ScreenToClient(&rcCtrl);

    CRect rcClient;
    dlg.GetClientRect(&rcClient);

    // Compute deltas
    int delta_w = std::max(0, needed_w - rcCtrl.Width());
    int delta_h = std::max(0, needed_h - rcCtrl.Height());

    if (delta_w == 0 && delta_h == 0)
        return; // Nothing to do

    // Cap by screen work area
    RECT wa{};
    SystemParametersInfo(SPI_GETWORKAREA, 0, &wa, 0);
    const int screen_w = wa.right - wa.left;
    const int screen_h = wa.bottom - wa.top;

    // Translate client growth to window size (add non-client area)
    CRect rcWnd;
    dlg.GetWindowRect(&rcWnd);
    const int nc_w = rcWnd.Width() - rcClient.Width();
    const int nc_h = rcWnd.Height() - rcClient.Height();

    int new_client_w = std::min(rcClient.Width() + delta_w, screen_w - 32); // 32px safety margin
    int new_client_h = std::min(rcClient.Height() + delta_h, screen_h - 32);

    // Compute desired new control rect first (width grow only for now)
    CRect desiredCtrl = rcCtrl;
    desiredCtrl.right = desiredCtrl.left + std::max<LONG>(needed_w, new_client_w - rcCtrl.left - 8);
    desiredCtrl.bottom = desiredCtrl.top + std::max(needed_h, rcCtrl.Height());

    // Check overlap with other sibling controls (client coords)
    bool overlaps = false;
    CWnd *child = dlg.GetWindow(GW_CHILD);
    CRect unionRect = rcCtrl; // track bottom-most rect
    while (child)
    {
        if (child->m_hWnd != pCtrl->m_hWnd)
        {
            CRect r;
            child->GetWindowRect(&r);
            dlg.ScreenToClient(&r);
            unionRect.UnionRect(&unionRect, &r);
            CRect inter;
            if (inter.IntersectRect(&desiredCtrl, &r))
            {
                // Significant vertical overlap? (avoid counting mere touch)
                if (inter.Height() > 4 && inter.Width() > 4)
                {
                    overlaps = true;
                }
            }
        }
        child = child->GetWindow(GW_HWNDNEXT);
    }

    if (overlaps)
    {
        // Move status control below the lowest existing control
        const int margin = 6;
        int shiftY = (unionRect.bottom + margin) - rcCtrl.top;
        desiredCtrl.OffsetRect(0, shiftY);

        // Increase client height to fit if necessary
        int extra_h = (desiredCtrl.bottom + 8) - rcClient.Height();
        if (extra_h > 0)
        {
            new_client_h = std::min(rcClient.Height() + extra_h, screen_h - 32);
        }
    }

    // Apply final dialog size
    const int final_w = std::max(new_client_w + nc_w, rcWnd.Width());
    const int final_h = std::max(new_client_h + nc_h, rcWnd.Height());
    dlg.SetWindowPos(nullptr, 0, 0, final_w, final_h, SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE);

    // Move/resize status control
    pCtrl->MoveWindow(desiredCtrl, TRUE);
}

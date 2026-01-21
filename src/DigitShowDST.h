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
 * @file DigitShowDST.h
 * @brief Main header file for DigitShowDST application
 *
 * DIGITSHOWDST アプリケーションのメイン ヘッダー ファイル
 */

#pragma once
#if !defined(AFX_DIGITSHOWDST_H__022B46E0_9575_4547_950E_44664C9C23DA__INCLUDED_)
#define AFX_DIGITSHOWDST_H__022B46E0_9575_4547_950E_44664C9C23DA__INCLUDED_

#ifndef __AFXWIN_H__
#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h" // メイン シンボル

/////////////////////////////////////////////////////////////////////////////
/**
 * @class CDigitShowDSTApp
 * @brief Main application class for DigitShowDST
 *
 * CDigitShowDSTApp:
 * このクラスの動作の定義に関しては DigitShowDST.cpp ファイルを参照してください。
 */
class CDigitShowDSTApp : public CWinApp
{
  public:
    /**
     * @brief Default constructor
     */
    CDigitShowDSTApp() noexcept = default;

    // オーバーライド
    // ClassWizard は仮想関数のオーバーライドを生成します。
    //{{AFX_VIRTUAL(CDigitShowDSTApp)
  public:
    /**
     * @brief Initialize application instance
     * @return TRUE if initialization succeeds
     */
    BOOL InitInstance() override;
    //}}AFX_VIRTUAL

    // インプリメンテーション
    //{{AFX_MSG(CDigitShowDSTApp)
    // メモ - ClassWizard はこの位置にメンバ関数を追加または削除します。
    //        この位置に生成されるコードを編集しないでください。
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ は前行の直前に追加の宣言を挿入します。

#endif // !defined(AFX_DIGITSHOWDST_H__022B46E0_9575_4547_950E_44664C9C23DA__INCLUDED_)

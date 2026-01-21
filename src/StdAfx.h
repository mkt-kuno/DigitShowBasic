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
 * @file stdafx.h
 * @brief Precompiled header file for standard system includes
 *
 * Contains standard system include files or project-specific include files
 * that are used frequently but changed infrequently.
 * 標準のシステム インクルード ファイル、または参照回数が多く、
 * かつあまり変更されないプロジェクト専用のインクルード ファイルを記述します。
 */

#if !defined(AFX_STDAFX_H__F3DD9DEF_11CF_4968_9BB5_ED24C05E4BF0__INCLUDED_)
#define AFX_STDAFX_H__F3DD9DEF_11CF_4968_9BB5_ED24C05E4BF0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/**
 * @def VC_EXTRALEAN
 * @brief Excludes rarely-used stuff from Windows headers
 */
#define VC_EXTRALEAN

/**
 * @def NOMINMAX
 * @brief Prevents Windows headers from defining min/max macros that break std::min/std::max
 */
#ifndef NOMINMAX
#define NOMINMAX
#endif

#include <afxdisp.h>  // MFC のオートメーション クラス
#include <afxdtctl.h> // MFC の Internet Explorer 4 コモン コントロール サポート
#include <afxext.h>   // MFC の拡張部分
#include <afxwin.h>   // MFC のコアおよび標準コンポーネント
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>      // MFC の Windows コモン コントロール サポート
#endif                   // _AFX_NO_AFXCMN_SUPPORT
#include <afxdialogex.h> // MFC dialog extensions (CDialogEx)

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ は前行の直前に追加の宣言を挿入します。

/*
 * Provide an opt-in message-map wrapper that suppresses compiler warnings
 * for unused local typedefs inside the message map macro expansion.
 * This avoids changing the global behaviour of MFC message map macros.
 */

#if defined(__clang__)
#define BEGIN_MESSAGE_MAP_IGNORE_UNUSED_LOCAL_TYPEDEF(theClass, theBaseClass)                                          \
    _Pragma("clang diagnostic push") _Pragma("clang diagnostic ignored \"-Wunused-local-typedef\"")                    \
        BEGIN_MESSAGE_MAP(theClass, theBaseClass)

#define END_MESSAGE_MAP_IGNORE_UNUSED_LOCAL_TYPEDEF()                                                                  \
    END_MESSAGE_MAP()                                                                                                  \
    _Pragma("clang diagnostic pop")

#elif defined(__GNUC__)
#define BEGIN_MESSAGE_MAP_IGNORE_UNUSED_LOCAL_TYPEDEF(theClass, theBaseClass)                                          \
    _Pragma("GCC diagnostic push") _Pragma("GCC diagnostic ignored \"-Wunused-local-typedef\"")                        \
        BEGIN_MESSAGE_MAP(theClass, theBaseClass)

#define END_MESSAGE_MAP_IGNORE_UNUSED_LOCAL_TYPEDEF()                                                                  \
    END_MESSAGE_MAP()                                                                                                  \
    _Pragma("GCC diagnostic pop")

#else
/* On other compilers (MSVC), just map through to the original macros. */
#define BEGIN_MESSAGE_MAP_IGNORE_UNUSED_LOCAL_TYPEDEF(theClass, theBaseClass) BEGIN_MESSAGE_MAP(theClass, theBaseClass)

#define END_MESSAGE_MAP_IGNORE_UNUSED_LOCAL_TYPEDEF() END_MESSAGE_MAP()
#endif

#endif // !defined(AFX_STDAFX_H__F3DD9DEF_11CF_4968_9BB5_ED24C05E4BF0__INCLUDED_)

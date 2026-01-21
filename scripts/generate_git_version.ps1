#!/usr/bin/env pwsh
<#
.SYNOPSIS
    Generates git_version.hpp with compile-time version information.
.DESCRIPTION
    Collects git metadata (commit, branch, remote, tag, dirty status, diff)
    and produces a C++ header with inline constexpr constants in the git_version namespace.
.PARAMETER OutDir
    Output directory for generated/git_version.hpp
#>
param(
    [Parameter(Mandatory=$true)]
    [string]$OutDir
)

Set-StrictMode -Version Latest
$ErrorActionPreference = "Stop"

# Check git availability
try {
    $null = git --version 2>&1
} catch {
    Write-Error "Git is not available. Build requires git to be installed and in PATH."
    exit 1
}

# Collect git information
try {
    $commitShort = git rev-parse --short HEAD 2>$null
    $commitFull = git rev-parse HEAD 2>$null
    $branch = git rev-parse --abbrev-ref HEAD 2>$null
    $remoteUrl = git config --get remote.origin.url 2>$null
    if (-not $remoteUrl) {
        $remoteUrl = ""
    }
    
    # Try to get a version from git describe
    $version = git describe --tags --always --dirty 2>$null
    if (-not $version) {
        $version = $commitShort
    }
    
    # Check if working directory is dirty
    $statusOutput = git status --porcelain 2>$null
    $isDirty = ($statusOutput -and $statusOutput.Length -gt 0)
    
    # Get diff (truncate to 8192 bytes preserving UTF-8 boundaries)
    $MaxDiffBytes = 8192
    $diffRaw = git diff HEAD 2>$null
    if (-not $diffRaw) {
        $diffRaw = ""
    }
    
    # Convert to UTF-8 bytes and truncate
    $diffBytes = [System.Text.Encoding]::UTF8.GetBytes($diffRaw)
    if ($diffBytes.Length -gt $MaxDiffBytes) {
        # Find a valid UTF-8 boundary
        $truncateAt = $MaxDiffBytes
        while ($truncateAt -gt 0) {
            $byte = $diffBytes[$truncateAt]
            # Check if this is a valid UTF-8 boundary (not a continuation byte)
            if (($byte -band 0xC0) -ne 0x80) {
                break
            }
            $truncateAt--
        }
        $diffBytes = $diffBytes[0..($truncateAt - 1)]
        $diffStr = [System.Text.Encoding]::UTF8.GetString($diffBytes)
    } else {
        $diffStr = $diffRaw
    }
    
    # Escape the diff string for raw string literal
    # Check if delimiter R"===(" and ")===" work (check for occurrences)
    $rawDelimiter = "==="
    if ($diffStr.Contains(")$rawDelimiter")) {
        # Fallback: produce empty diff to avoid malformed source
        Write-Warning "Diff contains raw string delimiter, producing empty DIFF_STR"
        $diffStr = ""
    }
    
} catch {
    Write-Error "Failed to collect git information: $_"
    exit 1
}

# Escape strings for C++
function Escape-CppString {
    param([string]$str)
    $str = $str -replace '\\', '\\'
    $str = $str -replace '"', '\"'
    $str = $str -replace "`n", '\n'
    $str = $str -replace "`r", '\r'
    $str = $str -replace "`t", '\t'
    return $str
}

$commitShortEsc = Escape-CppString $commitShort
$commitFullEsc = Escape-CppString $commitFull
$branchEsc = Escape-CppString $branch
$remoteUrlEsc = Escape-CppString $remoteUrl
$versionEsc = Escape-CppString $version

# Generate header content
$headerContent = @"
/**
 * @file git_version.hpp
 * @brief Auto-generated git version information (DO NOT EDIT)
 * 
 * Generated at build time by scripts/generate_git_version.ps1
 */

#pragma once

#include <string_view>

namespace git_version {

inline constexpr std::string_view VERSION = "$versionEsc";
inline constexpr std::string_view COMMIT_SHORT = "$commitShortEsc";
inline constexpr std::string_view COMMIT_FULL = "$commitFullEsc";
inline constexpr std::string_view BRANCH = "$branchEsc";
inline constexpr std::string_view REMOTE_URL = "$remoteUrlEsc";
inline constexpr bool DIRTY = $($isDirty.ToString().ToLower());

inline constexpr const char DIFF_STR[] = R"$rawDelimiter($diffStr)$rawDelimiter";

} // namespace git_version
"@

# Ensure output directory exists
$outputPath = Join-Path $OutDir "git_version.hpp"
$outputDir = Split-Path $outputPath -Parent
if (-not (Test-Path $outputDir)) {
    New-Item -ItemType Directory -Path $outputDir -Force | Out-Null
}

# Write file as UTF-8
[System.IO.File]::WriteAllText($outputPath, $headerContent, [System.Text.UTF8Encoding]::new($false))

Write-Host "Generated $outputPath"
exit 0

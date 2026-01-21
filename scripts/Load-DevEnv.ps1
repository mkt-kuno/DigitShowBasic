#!/usr/bin/env pwsh
<#
.SYNOPSIS
    Loads development environment variables from .env file and initializes Visual Studio.

.DESCRIPTION
    Reads the .env file in the workspace root and sets environment variables.
    Then launches the Visual Studio Developer Shell to complete the environment setup.
    This script is used by VS Code terminal profiles and build tasks for local development.

.EXAMPLE
    # Called from VS Code terminal profile
    . ${workspaceFolder}\scripts\Load-DevEnv.ps1

    # Now msbuild and all VS tools are available
    msbuild .\DigitShowDST.vcxproj /t:Build /p:Configuration=Release

.NOTES
    Requires: .env file in workspace root with VS_INSTALL_PATH defined
    Author: DigitShowDST Team
#>

Set-StrictMode -Version Latest
$ErrorActionPreference = 'Stop'

# Locate .env file in workspace root
$workspaceRoot = Split-Path $PSScriptRoot -Parent
$envFile = Join-Path $workspaceRoot '.env'

if (-not (Test-Path $envFile)) {
  Write-Error @"
❌ .env file not found at: $envFile

Please create .env from .env.example:
  1. Copy .env.example to .env
  2. Edit .env and set VS_INSTALL_PATH to your Visual Studio installation directory
     Example: C:\Program Files\Microsoft Visual Studio\2022\Community
  3. Restart VS Code terminal

See README.md for more details.
"@ -ErrorAction Stop
}

# Load environment variables from .env
Write-Verbose "Loading environment from: $envFile"
$envVars = @{}

Get-Content $envFile |
Where-Object { $_ -match '^\s*\w+=' -and -not $_.StartsWith('#') } |
ForEach-Object {
  $trimmed = $_.Trim()
  if ($trimmed -match '^(\w+)=(.*)$') {
    $key = $matches[1]
    $value = $matches[2].Trim('"')
    $envVars[$key] = $value
    Set-Item -Path "env:$key" -Value $value
    Write-Verbose "  $key = $value"
  }
}

Write-Host "✓ Loaded environment from .env" -ForegroundColor Green

# Validate required variables
if (-not $env:VS_INSTALL_PATH) {
  Write-Error @"
❌ VS_INSTALL_PATH is not set in .env

Please add the following line to .env:
  VS_INSTALL_PATH=C:\Program Files\Microsoft Visual Studio\2022\Community

Adjust the path to match your Visual Studio installation location.
"@ -ErrorAction Stop
}

# Verify VS installation path exists
if (-not (Test-Path $env:VS_INSTALL_PATH)) {
  Write-Error @"
❌ Visual Studio path does not exist:
  $env:VS_INSTALL_PATH

Please check the VS_INSTALL_PATH in .env and correct it.
You can find your VS installation at:
  C:\Program Files\Microsoft Visual Studio
  C:\Program Files (x86)\Microsoft Visual Studio
"@ -ErrorAction Stop
}

# Launch Visual Studio Developer Shell
$vsDevShell = Join-Path $env:VS_INSTALL_PATH 'Common7\Tools\Launch-VsDevShell.ps1'

if (-not (Test-Path $vsDevShell)) {
  Write-Error @"
❌ Launch-VsDevShell.ps1 not found at:
  $vsDevShell

This indicates the Visual Studio installation at:
  $env:VS_INSTALL_PATH

may be incomplete or corrupted. Please reinstall Visual Studio and try again.
"@ -ErrorAction Stop
}

Write-Verbose "Launching Visual Studio Developer Shell..."
& $vsDevShell -SkipAutomaticLocation

# Verify MSBuild is now available
if (-not (Get-Command msbuild -ErrorAction SilentlyContinue)) {
  Write-Error @"
❌ MSBuild not found after loading VS environment.

This may indicate an issue with your Visual Studio installation.
Please try:
  1. Repair Visual Studio from Control Panel > Programs > Programs and Features
  2. Ensure 'Desktop development with C++' workload is installed
  3. Restart your terminal and try again
"@ -ErrorAction Stop
}

Write-Host "✓ Visual Studio environment ready" -ForegroundColor Green
Write-Host "  MSBuild: $(Get-Command msbuild | Select-Object -ExpandProperty Source)" -ForegroundColor Gray

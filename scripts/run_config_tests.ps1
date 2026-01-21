# Runs config validation over YAML fixtures in test_data/ using the app's --validate-config CLI.
# Heuristic expectations:
#  - Files starting with 'valid_' should pass (exit code 0 and no error markers)
#  - Files starting with 'invalid_' should fail (non-zero exit or known error messages)
# Current app may not yet wire the CLI; in that case, this script reports "SKIPPED".

param(
  [string]$Configuration = 'Release',
  [ValidateSet('Win32', 'x64')]
  [string]$Platform = 'x64'
)

$ErrorActionPreference = 'Stop'
$Root = Split-Path -Parent $MyInvocation.MyCommand.Path
$RepoRoot = Resolve-Path (Join-Path $Root '..')
$Exe = if ($Platform -eq 'x64') { Join-Path $RepoRoot "x64/$Configuration/DigitShowDST.exe" } else { Join-Path $RepoRoot "$Configuration/DigitShowDST.exe" }
$Fixtures = Get-ChildItem -Path (Join-Path $RepoRoot 'test_data') -Filter *.yaml -File -ErrorAction SilentlyContinue

if (-not (Test-Path $Exe)) {
  Write-Warning "Executable not found: $Exe. Build first."
  exit 2
}
if (-not $Fixtures) {
  Write-Warning "No YAML fixtures found under test_data/."
  exit 2
}

$Total = 0; $Passed = 0; $Failed = 0; $Skipped = 0

foreach ($f in $Fixtures) {
  $Total++
  $expectValid = $f.BaseName.StartsWith('valid_')
  $expectInvalid = $f.BaseName.StartsWith('invalid_')
  $args = @('--validate-config', $f.FullName)
  $p = Start-Process -FilePath $Exe -ArgumentList $args -NoNewWindow -PassThru -Wait -RedirectStandardOutput "$env:TEMP\\configtest_$($f.BaseName).out" -RedirectStandardError "$env:TEMP\\configtest_$($f.BaseName).err"
  $out = Get-Content "$env:TEMP\\configtest_$($f.BaseName).out" -Raw -ErrorAction SilentlyContinue
  $err = Get-Content "$env:TEMP\\configtest_$($f.BaseName).err" -Raw -ErrorAction SilentlyContinue

  $hasParserStub = ($out + $err) -match 'YAML parser not yet integrated'
  if ($hasParserStub) {
    Write-Host "SKIP  $($f.Name)  (parser not integrated)" -ForegroundColor Yellow
    $Skipped++
    continue
  }

  $ok = $p.ExitCode -eq 0
  if ($expectValid -and $ok) { $Passed++; Write-Host "PASS  $($f.Name)" -ForegroundColor Green }
  elseif ($expectInvalid -and -not $ok) { $Passed++; Write-Host "PASS  $($f.Name) (invalid as expected)" -ForegroundColor Green }
  else { $Failed++; Write-Host "FAIL  $($f.Name)  (exit=$($p.ExitCode))" -ForegroundColor Red; if ($out) { Write-Host $out }; if ($err) { Write-Host $err } }
}

Write-Host "\nSummary: total=$Total, passed=$Passed, failed=$Failed, skipped=$Skipped"
if ($Failed -gt 0) { exit 1 } else { exit 0 }

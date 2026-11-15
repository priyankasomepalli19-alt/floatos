@echo off
setlocal EnableExtensions
title FloatOS 0.1 ISO Downloader
color 0A
set "DEST=%USERPROFILE%\Downloads\FloatOS"
set "ISO=FloatOS-0.1-x86_64.iso"
set "BASE=https://github.com/priyankasomepalli19-alt/floatos/releases/download/floatos-latest"

if not exist "%DEST%" mkdir "%DEST%"
echo.
echo  FLOATOS 0.1 ISO DOWNLOADER
echo  Destination: %DEST%
echo.

where aria2c.exe >nul 2>&1
if not errorlevel 1 (
    echo Using aria2 for multi-connection download...
    aria2c.exe --continue=true --allow-overwrite=true --auto-file-renaming=false --max-connection-per-server=8 --split=8 --min-split-size=1M --max-tries=10 --retry-wait=3 --dir="%DEST%" --out="%ISO%" "%BASE%/%ISO%"
    if errorlevel 1 goto :download_failed
) else (
    echo aria2 was not found. Using Windows curl with resume and retries...
    curl.exe --location --fail --retry 10 --retry-all-errors --retry-delay 3 --connect-timeout 30 --continue-at - --output "%DEST%\%ISO%" "%BASE%/%ISO%"
    if errorlevel 1 goto :download_failed
)

echo.
echo Downloading SHA-256 checksum...
curl.exe --location --fail --retry 5 --retry-all-errors --retry-delay 2 --output "%DEST%\%ISO%.sha256" "%BASE%/%ISO%.sha256"
if errorlevel 1 goto :checksum_failed

echo.
echo Verifying SHA-256...
powershell.exe -NoLogo -NoProfile -ExecutionPolicy Bypass -Command "$expected = ((Get-Content -LiteralPath '%DEST%\%ISO%.sha256' -Raw) -split '\s+')[0].ToLower(); $actual = (Get-FileHash -LiteralPath '%DEST%\%ISO%' -Algorithm SHA256).Hash.ToLower(); if ($expected -ne $actual) { Write-Error 'SHA-256 mismatch. Do not use this ISO.'; exit 1 }; Write-Host 'SHA-256 verified successfully.' -ForegroundColor Green"
if errorlevel 1 goto :checksum_failed

echo.
echo SUCCESS! FloatOS ISO is ready:
echo %DEST%\%ISO%
start "" "%DEST%"
pause
exit /b 0

:download_failed
echo.
echo Download failed. The release may not be published yet, or the connection was interrupted.
echo Run this file again to resume the download.
pause
exit /b 1

:checksum_failed
echo.
echo Checksum download or verification failed. Do not boot this ISO until it verifies.
pause
exit /b 1

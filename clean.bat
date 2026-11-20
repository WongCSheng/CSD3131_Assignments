@echo off
setlocal

set "ROOT=%~dp0"
set "BIN_DIR=%ROOT%bin"
set "LIB_DIR=%ROOT%lib"

set "CALLER="
for /f "usebackq delims=" %%P in (`powershell -NoProfile -Command ^
    "$p = Get-CimInstance Win32_Process -Filter ('ProcessId=' + $PID);" ^
    "while ($p -and $p.ParentProcessId) {" ^
    "  $p = Get-CimInstance Win32_Process -Filter ('ProcessId=' + $p.ParentProcessId);" ^
    "  if ($p -and $p.Name -notmatch '^(cmd|conhost)(\.exe)?$') { $p.Name; break }" ^
    "}"`) do (
    if not defined CALLER set "CALLER=%%P"
)

if /i "%CALLER%"=="wsl.exe"     goto :from_wsl
if /i "%CALLER%"=="wslhost.exe" goto :from_wsl
if /i "%CALLER%"=="powershell.exe" goto :from_ps
if /i "%CALLER%"=="pwsh.exe"       goto :from_ps

REM Launched from file explorer
echo Launched from file explorer. Cleaning up the build environment...
if exist %BIN_DIR% (
    rmdir /s /q %BIN_DIR%
)
if exist %LIB_DIR%\imgui (
    rmdir /s /q %LIB_DIR%\imgui
)
if exist %LIB_DIR%\stb (
    rmdir /s /q %LIB_DIR%\stb
)
if exist %LIB_DIR%\opencv (
    rmdir /s /q %LIB_DIR%\opencv
)
if exist %LIB_DIR%temp-build (
    rmdir /s /q "%LIB_DIR%temp-build"
)
if exist %LIB_DIR%temp-opencv-src (
    rmdir /s /q "%LIB_DIR%temp-opencv-src"
)
if exist %ROOT%output (
    rmdir /s /q %ROOT%output
)
endlocal
goto :eof

:from_wsl
echo Detected WSL invocation. Cleaning up the build environment...
make clean
if exist bin (
    rmdir bin /s /q
)
if exist lib\imgui (
    rmdir lib\imgui /s /q
)
if exist lib\stb (
    rmdir lib\stb /s /q
)
if exist lib\opencv (
    rmdir lib\opencv /s /q
)
if exist output (
    rmdir output /s /q
)
endlocal
goto :eof

:from_ps
echo Detected PowerShell invocation. Cleaning up the build environment...
if exist %BIN_DIR% (
    rmdir /s /q %BIN_DIR%
)
if exist %LIB_DIR%\imgui (
    rmdir /s /q %LIB_DIR%\imgui
)
if exist %LIB_DIR%\stb (
    rmdir /s /q %LIB_DIR%\stb
)
if exist %LIB_DIR%\opencv (
    rmdir /s /q %LIB_DIR%\opencv
)
if exist %LIB_DIR%temp-build (
    rmdir /s /q "%LIB_DIR%temp-build"
)
if exist %LIB_DIR%temp-opencv-src (
    rmdir /s /q "%LIB_DIR%temp-opencv-src"
)
if exist %ROOT%output (
    rmdir /s /q %ROOT%output
)
endlocal
goto :eof
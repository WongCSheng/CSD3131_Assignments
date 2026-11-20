@echo off
setlocal

set "ROOT=%~dp0"
set "BUILD_DIR=%ROOT%build"
set "DEPENDENCIES_DIR=%ROOT%dependencies"
set "EXE_PATH=%BUILD_DIR%\Greedy_algo_Seam_Carving.exe"
set "OUTPUT_DIR=%ROOT%output"

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

echo Cleaning files...

echo Launched from CMD / Explorer / other (%CALLER%)
if exist %BUILD_DIR% (
    rmdir /s /q %BUILD_DIR%
)
if exist %DEPENDENCIES_DIR% (
    rmdir /s /q %DEPENDENCIES_DIR%
)
if exist %EXE_PATH% (
    del %EXE_PATH%
)
if exist %OUTPUT_DIR% (
    rmdir /s /q %OUTPUT_DIR%
)
if exist %ROOT%temp-build (
    rmdir /s /q "%ROOT%temp-build"
)
if exist %ROOT%temp-opencv-src (
    rmdir /s /q "%ROOT%temp-opencv-src"
)
endlocal
goto :eof

:from_wsl
echo Detected WSL invocation. Cleaning up the build environment...
make clean
if exist build (
    rmdir build /s /q
)
if exist dependencies (
    rmdir dependencies /s /q
)
if exist output (
    rmdir output /s /q
)
if exist temp-build (
    rmdir temp-build /s /q
)
if exist temp-opencv-src (
    rmdir temp-opencv-src /s /q
)
endlocal
goto :eof

:from_ps
echo Detected PowerShell invocation. Cleaning up the build environment...
if exist %BUILD_DIR% (
    rmdir /s /q %BUILD_DIR%
)
if exist %DEPENDENCIES_DIR% (
    rmdir /s /q %DEPENDENCIES_DIR%
)
if exist %EXE_PATH% (
    del %EXE_PATH%
)
if exist %OUTPUT_DIR% (
    rmdir /s /q %OUTPUT_DIR%
)
if exist %ROOT%temp-build (
    rmdir /s /q "%ROOT%temp-build"
)
if exist %ROOT%temp-opencv-src (
    rmdir /s /q "%ROOT%temp-opencv-src"
)
endlocal
goto :eof
@echo off
setlocal

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

:from_wsl
echo Detected WSL invocation. Cleaning up the build environment...
make clean
if exist build (
    rmdir build /s /q
)
if exist dependencies (
    rmdir dependencies /s /q
)
goto :eof

:from_ps
echo Detected PowerShell invocation. Cleaning up the build environment...
if exist build (
    rmdir build /s /q
)
if exist dependencies (
    rmdir dependencies /s /q
)
if exist Greedy_algo_Seam_Carving.exe (
    del Greedy_algo_Seam_Carving.exe
)
goto :eof
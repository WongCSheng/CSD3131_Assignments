@echo off
setlocal 

REM Set the path variables
set "ROOT=%~dp0"
set "EXE_PATH=%ROOT%build\Greedy_algo_Seam_Carving.exe"

REM Get platform that is calling the batch file
set "CALLER="
for /f "usebackq delims=" %%P in (`powershell -NoProfile -Command ^
    "$p = Get-CimInstance Win32_Process -Filter ('ProcessId=' + $PID);" ^
    "while ($p -and $p.ParentProcessId) {" ^
    "  $p = Get-CimInstance Win32_Process -Filter ('ProcessId=' + $p.ParentProcessId);" ^
    "  if ($p -and $p.Name -notmatch '^(cmd|conhost|OpenConsole)(\.exe)?$') { $p.Name; break }" ^
    "}"`) do (
    if not defined CALLER set "CALLER=%%P"
)

REM Detect which command line environment are called
if /i "%CALLER%"=="wsl.exe"     goto :from_wsl
if /i "%CALLER%"=="wslhost.exe" goto :from_wsl
if /i "%CALLER%"=="powershell.exe" goto :from_ps
if /i "%CALLER%"=="pwsh.exe"       goto :from_ps

echo Rebuilding executable...

REM If launched from the file explorer
echo Launched from CMD / Explorer / other (%CALLER%)
g++ -std=c++20 -O2 -Wall -Wextra ^
        "%ROOT%Greedy_algo_Seam_Carving.cpp" ^
        -I"%ROOT%." ^
        -I"%ROOT%dependencies\include" ^
        -L"%ROOT%dependencies\bin" ^
        -L"%ROOT%dependencies" ^
        -lopencv_highgui4140 -lopencv_imgcodecs4140 -lopencv_imgproc4140 -lopencv_core4140 ^
        -o "%EXE_PATH%"
if exist "%ROOT%dependencies\bin\*.dll" (
        xcopy /y /d /q "%ROOT%dependencies\bin\*.dll" "%ROOT%build\" >nul
    )
REM Copy over dll to the build folder if not there
if exist "%QT_DIR%\bin\windeployqt.exe" (
    "%QT_DIR%\bin\windeployqt.exe" --no-translations "%ROOT%build\opencv_highgui4140.dll" >nul
) else if exist "%QT_DIR%\bin\windeployqt6.exe" (
    "%QT_DIR%\bin\windeployqt6.exe" --no-translations "%ROOT%build\opencv_highgui4140.dll" >nul
)
if errorlevel 1 (
    echo Compilation failed!
    exit /b 1
)

REM sets the dependency path
set "PATH=%ROOT%dependencies\bin;%ROOT%dependencies;%PATH%"
echo Running %EXE_PATH%...
"%EXE_PATH%"
endlocal
goto :end

REM if launched from WSL command-line tool
:from_wsl
make clean
make all
endlocal
goto :end

REM if launched from PowerShell command-line tool
:from_ps
g++ -std=c++20 -O2 -Wall -Wextra ^
        "%ROOT%Greedy_algo_Seam_Carving.cpp" ^
        -I"%ROOT%." ^
        -I"%ROOT%dependencies\include" ^
        -L"%ROOT%dependencies\bin" ^
        -L"%ROOT%dependencies" ^
        -lopencv_highgui4140 -lopencv_imgcodecs4140 -lopencv_imgproc4140 -lopencv_core4140 ^
        -o "%EXE_PATH%"
if exist "%ROOT%dependencies\bin\*.dll" (
        xcopy /y /d /q "%ROOT%dependencies\bin\*.dll" "%ROOT%build\" >nul
    )
REM Copy over dll to the build folder if not there
if exist "%QT_DIR%\bin\windeployqt.exe" (
    "%QT_DIR%\bin\windeployqt.exe" --no-translations "%ROOT%build\opencv_highgui4140.dll" >nul
) else if exist "%QT_DIR%\bin\windeployqt6.exe" (
    "%QT_DIR%\bin\windeployqt6.exe" --no-translations "%ROOT%build\opencv_highgui4140.dll" >nul
)
if errorlevel 1 (
    echo Compilation failed!
    exit /b 1
)
REM sets the dependency path
set "PATH=%ROOT%dependencies\bin;%ROOT%dependencies;%PATH%"
echo Running %EXE_PATH%...
"%EXE_PATH%"
endlocal
goto :end

:end
pause
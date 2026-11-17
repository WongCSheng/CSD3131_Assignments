@echo off
setlocal EnableDelayedExpansion

set "ROOT=%~dp0"
set "EXE_PATH=%ROOT%build\Greedy_algo_Seam_Carving.exe"

if not exist "%ROOT%dependencies" (
    mkdir "%ROOT%dependencies"
    echo Downloading and building OpenCV 4.14.0 with MinGW...
    git clone -c advice.detachedHead=false --depth 1 --branch 4.14.0 https://github.com/opencv/opencv.git "%ROOT%temp-opencv-src"

    cmake -G "MinGW Makefiles" -S "%ROOT%temp-opencv-src" -B "%ROOT%temp-build" ^
        -DCMAKE_INSTALL_PREFIX="%ROOT%dependencies" ^
        -DWITH_QT=ON ^
        -DBUILD_LIST=core,imgproc,imgcodecs,highgui ^
        -DOPENCV_INCLUDE_INSTALL_PATH=include ^
        -DOPENCV_LIB_INSTALL_PATH=lib ^
        -DOPENCV_BIN_INSTALL_PATH=bin ^
        -DBUILD_TESTS=OFF -DBUILD_PERF_TESTS=OFF -DBUILD_EXAMPLES=OFF ^
        -DBUILD_opencv_apps=OFF -DBUILD_opencv_python3=OFF -DBUILD_opencv_python2=OFF -DBUILD_JAVA=OFF

    cmake --build "%ROOT%temp-build" --config Release -j
    cmake --install "%ROOT%temp-build" --config Release
    rmdir /s /q "%ROOT%temp-opencv-src" "%ROOT%temp-build"
)

if not exist "%ROOT%build" mkdir "%ROOT%build"

set "CALLER="
for /f "usebackq delims=" %%P in (`powershell -NoProfile -Command ^
    "$p = Get-CimInstance Win32_Process -Filter ('ProcessId=' + $PID);" ^
    "while ($p -and $p.ParentProcessId) {" ^
    "  $p = Get-CimInstance Win32_Process -Filter ('ProcessId=' + $p.ParentProcessId);" ^
    "  if ($p -and $p.Name -notmatch '^(cmd|conhost|OpenConsole)(\.exe)?$') { $p.Name; break }" ^
    "}"`) do (
    if not defined CALLER set "CALLER=%%P"
)

if /i "%CALLER%"=="wsl.exe"     goto :from_wsl
if /i "%CALLER%"=="wslhost.exe" goto :from_wsl
if /i "%CALLER%"=="powershell.exe" goto :from_ps
if /i "%CALLER%"=="pwsh.exe"       goto :from_ps

echo Launched from CMD / Explorer / other (%CALLER%)
goto :eof

:from_wsl
echo Launched from WSL (%CALLER%)
if not exist %EXE_PATH% (
    echo Executable not found. Building the project...
    make all
) else (
    echo Executable found. Running the program...
    make run
)
endlocal
goto :eof

:from_ps
echo Launched from PowerShell (%CALLER%)

if not exist "%EXE_PATH%" (
    echo Executable not found. Compiling with g++...
    g++ -std=c++20 -O2 -Wall -Wextra ^
        "%ROOT%Greedy_algo_Seam_Carving.cpp" ^
        -I"%ROOT%." ^
        -I"%ROOT%dependencies\include" ^
        -L"%ROOT%dependencies\lib" ^
        -L"%ROOT%dependencies" ^
        -lopencv_highgui4140 -lopencv_imgcodecs4140 -lopencv_imgproc4140 -lopencv_core4140 ^
        -o "%EXE_PATH%"

    if errorlevel 1 (
        echo Compilation failed!
        exit /b 1
    )
)

set "PATH=%ROOT%dependencies\bin;%ROOT%dependencies;%PATH%"
echo Running %EXE_PATH%...
"%EXE_PATH%"
endlocal
goto :eof
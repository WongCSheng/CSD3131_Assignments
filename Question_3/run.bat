@echo off
setlocal

if not exist dependencies (
    mkdir dependencies
    echo Downloading and building OpenCV 4.14.0...
    git clone --depth 1 --branch 4.14.0 https://github.com/opencv/opencv.git temp-opencv-src
    git config advice.detachedHead false

    cmake -S temp-opencv-src -B temp-build ^
    -DCMAKE_INSTALL_PREFIX=.\dependencies ^
    -DBUILD_LIST=core,imgproc,imgcodecs,highgui ^
    -DOPENCV_INCLUDE_INSTALL_PATH=include ^
    -DOPENCV_LIB_INSTALL_PATH=lib ^
    -DOPENCV_BIN_INSTALL_PATH=bin ^
    -DBUILD_TESTS=OFF -DBUILD_PERF_TESTS=OFF -DBUILD_EXAMPLES=OFF ^
    -DBUILD_opencv_apps=OFF -DBUILD_opencv_python3=OFF -DBUILD_opencv_python2=OFF -DBUILD_JAVA=OFF

    cmake --build temp-build --config Release -j
    cmake --install temp-build --config Release
    rmdir /s /q temp-opencv-src temp-build

    cmake -S . -B build
    cmake --build build --config Release
)

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

echo Launched from CMD / Explorer / other (%CALLER%)
goto :eof

:from_wsl
echo Launched from WSL (%CALLER%)
if not exist Greedy_algo_Seam_Carving.exe (
    echo Executable not found. Building the project...
    make all
) else (
    echo Executable found. Running the program...
    make run
)
goto :eof

:from_ps
echo Launched from PowerShell (%CALLER%)
if not exist Greedy_algo_Seam_Carving.exe (
    echo Executable not found. Building the project...
    setlocal EnableDelayedExpansion
    for /f "usebackq delims=" %%F in (`pkg-config --cflags --libs opencv4`) do set "OPENCV_FLAGS=%%F"
    g++ -std=c++20 -O2 -Wall -Wextra Greedy_algo_Seam_Carving.cpp !OPENCV_FLAGS! -o Greedy_algo_Seam_Carving.exe
) else (
    echo Executable found. Running the program...
    ./Greedy_algo_Seam_Carving.exe
)
goto :eof
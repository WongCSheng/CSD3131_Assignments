@echo off
setlocal

if not exist lib mkdir lib

set "ROOT=%~dp0"
set "LIB_DIR=%ROOT%lib"

REM ---- ImGui ----
if not exist lib\imgui (
    echo Importing ImGui
    echo Downloading ImGui headers with MinGW...
    git clone -c advice.detachedHead=false --depth 1 --branch v1.93.0 https://github.com/ocornut/imgui.git lib\imgui
    if errorlevel 1 (
        echo ImGui clone failed.
        exit /b 1
    )
)

REM --- GLFW ----
if not exist lib\GLFW (
    echo Importing GLFW
    echo Downloading GLFW for ImGui with MinGW...
    git clone -c advice.detachedHead=false --depth 1 --branch 3.4 https://github.com/glfw/glfw.git "%LIB_DIR%\temp-glfw-src"
    if errorlevel 1 (
        echo GLFW clone failed.
        exit /b 1
    )

    cmake -G "MinGW Makefiles" -S "%LIB_DIR%\temp-glfw-src" -B "%LIB_DIR%\temp-glfw-build" ^
        -DCMAKE_INSTALL_PREFIX="%LIB_DIR%\GLFW" ^
        -DGLFW_BUILD_EXAMPLES=OFF -DGLFW_BUILD_TESTS=OFF -DGLFW_BUILD_DOCS=OFF

    cmake --build "%LIB_DIR%\temp-glfw-build" -j
    cmake --install "%LIB_DIR%\temp-glfw-build"
    rmdir /s /q "%LIB_DIR%\temp-glfw-src" "%LIB_DIR%\temp-glfw-build"
)

REM ---- stb_image (Q1 and Q4) ----
if not exist lib\stb (
    echo Importing STB
    mkdir lib\stb
    for %%H in (stb_image.h stb_image_write.h) do (
        if not exist lib\stb\%%H (
            curl -L -o lib\stb\%%H https://raw.githubusercontent.com/nothings/stb/master/%%H
        )
    )
)

REM ---- OpenCV (Q3) ----
if not exist %LIB_DIR%\opencv (
    echo Importing OpenCV
    mkdir %LIB_DIR%\opencv
    echo Downloading and building OpenCV 4.14.0 with MinGW...
    git clone -c advice.detachedHead=false --depth 1 --branch 4.14.0 https://github.com/opencv/opencv.git "%LIB_DIR%\temp-opencv-src"

    cmake -G "MinGW Makefiles" -S "%LIB_DIR%\temp-opencv-src" -B "%LIB_DIR%\temp-build" ^
        -DCMAKE_INSTALL_PREFIX="%LIB_DIR%\OpenCV" ^
        -DWITH_QT=ON ^
        -DBUILD_LIST=core,imgproc,imgcodecs,highgui ^
        -DOPENCV_INCLUDE_INSTALL_PATH=include ^
        -DOPENCV_LIB_INSTALL_PATH=lib ^
        -DOPENCV_BIN_INSTALL_PATH=bin ^
        -DBUILD_TESTS=OFF -DBUILD_PERF_TESTS=OFF -DBUILD_EXAMPLES=OFF ^
        -DBUILD_opencv_apps=OFF -DBUILD_opencv_python3=OFF -DBUILD_opencv_python2=OFF -DBUILD_JAVA=OFF ^
        -DBUILD_opencv_java_bindings_generator=OFF -DBUILD_opencv_js=OFF

    cmake --build "%LIB_DIR%\temp-build" --config Release -j
    cmake --install "%LIB_DIR%\temp-build" --config Release
    rmdir /s /q "%LIB_DIR%\temp-opencv-src" "%LIB_DIR%\temp-build"
)

REM Build all answers
echo Building question 1 to 4 answers
if not exist bin (
    mkdir bin
    REM ---- Question 1 ----
    g++ -std=c++20 -O2 -Wall -Wextra Question_1/q1.cpp %LIB_DIR%/energy.cpp -o bin/q1

    REM ---- Question 3 ----
    mkdir output
    g++ -std=c++20 -O2 -Wall -Wextra ^
        "%ROOT%Question_3\Greedy_algo_Seam_Carving.cpp" ^
        -I"%ROOT%Question_3\." ^
        -I"%LIB_DIR%\opencv\include" ^
        -L"%LIB_DIR%\opencv\bin" ^
        -L"%LIB_DIR%\opencv" ^
        -lopencv_highgui4140 -lopencv_imgcodecs4140 -lopencv_imgproc4140 -lopencv_core4140 ^
        -o "%ROOT%bin\q3.exe"

    if errorlevel 1 (
        echo Compilation failed!
        exit /b 1
    )
    REM Copy over dll to the build folder if not there
    if exist "%LIB_DIR%\opencv\bin\*.dll" (
            xcopy /y /d /q "%LIB_DIR%\opencv\bin\*.dll" "%ROOT%bin" >nul
        )

    REM ---- Question 4 ----
    g++ -std=c++20 -O2 -Wall -Wextra Question_4/q4.cpp %LIB_DIR%/energy.cpp -o bin/q4
)

echo Build ready.
endlocal
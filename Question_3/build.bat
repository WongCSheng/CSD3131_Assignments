@echo off
setlocal EnableDelayedExpansion

REM Set the path variables
set "ROOT=%~dp0"

echo Importing OpenCV in Question 3

REM Import in dependencies
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
        -DBUILD_opencv_apps=OFF -DBUILD_opencv_python3=OFF -DBUILD_opencv_python2=OFF -DBUILD_JAVA=OFF ^
        -DBUILD_opencv_java_bindings_generator=OFF -DBUILD_opencv_js=OFF

    cmake --build "%ROOT%temp-build" --config Release -j
    cmake --install "%ROOT%temp-build" --config Release
    rmdir /s /q "%ROOT%temp-opencv-src" "%ROOT%temp-build"
)
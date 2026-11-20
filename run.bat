@echo off
setlocal enabledelayedexpansion

set "ROOT=%~dp0"
set "LIB_DIR=%ROOT%lib"

REM If not built yet
if not exist bin (
    echo Output files not detected, building files...
    mkdir bin
    REM ---- Question 1 ----
    g++ -std=c++20 -O2 -Wall -Wextra Question_1\q1.cpp %LIB_DIR%\energy.cpp -o bin\q1

    REM ---- Question 3 ----
    mkdir output
    g++ -std=c++20 -O2 -Wall -Wextra ^
        "%ROOT%Question_3\Greedy_algo_Seam_Carving.cpp" ^
        -I"%ROOT%Question_3\." ^
        -I"%LIB_DIR%\opencv\include" ^
        -L"%LIB_DIR%\opencv\bin" ^
        -L"%LIB_DIR%\opencv" ^
        -lopencv_highgui4140 -lopencv_imgcodecs4140 -lopencv_imgproc4140 -lopencv_core4140 ^
        -o "%EXE_PATH%"
        
    if errorlevel 1 (
        echo Compilation failed!
        exit /b 1
    )
    if exist "%LIB_DIR%\opencv\bin\*.dll" (
            xcopy /y /d /q "%LIB_DIR%\opencv\bin\*.dll" "%ROOT%bin\" >nul
        )
    REM Copy over dll to the build folder if not there
    if exist "%QT_DIR%\bin\windeployqt.exe" (
    "%QT_DIR%\bin\windeployqt.exe" --no-translations "%ROOT%build\opencv_highgui4140.dll" >nul
    ) else if exist "%QT_DIR%\bin\windeployqt6.exe" (
        "%QT_DIR%\bin\windeployqt6.exe" --no-translations "%ROOT%build\opencv_highgui4140.dll" >nul
    )

    REM ---- Question 4 ----
    g++ -std=c++20 -O2 -Wall -Wextra Question_4/q4.cpp %LIB_DIR%/energy.cpp -o bin/q4
)

REM ---- Main program ----
echo Building launcher
set "IM=lib\imgui"
set "KP=lib\koolplot1_2"
g++ -std=c++20 -O2 "%ROOT%main.cpp" ^
    -I"%IM%" -I"%IM%\backends" ^
    -I"%LIB_DIR%\glfw\include" ^
    -I"%LIB_DIR%\stb" ^
    "%IM%\imgui.cpp" "%IM%\imgui_draw.cpp" "%IM%\imgui_tables.cpp" "%IM%\imgui_widgets.cpp" ^
    "%IM%\backends\imgui_impl_glfw.cpp" "%IM%\backends\imgui_impl_opengl3.cpp" ^
    -L"%LIB_DIR%\glfw\lib" ^
    -o "%ROOT%bin\launcher.exe" -lglfw3 -lopengl32 -lgdi32
if errorlevel 1 goto :fail

REM Run from the project root so img\ paths resolve
bin\launcher.exe
goto :eof

REM ---- helper: gather every .cpp in a folder into SRC ----
:collect
for %%F in (%1\*.cpp) do call set "SRC=%%SRC%% %%F"
exit /b 0

:fail
echo.
echo Build failed. See errors above.
exit /b 1
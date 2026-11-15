cmake_minimum_required(VERSION 3.5...4.3)

project(greedy_algo_seam_carving LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Set the directory where custom CMake modules are stored
set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake")

include(ImportDependencies)
import_opencv()

add_executable(Greedy_algo_Seam_Carving
    Greedy_algo_Seam_Carving.cpp
)

target_include_directories(Greedy_algo_Seam_Carving
    PRIVATE
        ${OpenCV_INCLUDE_DIRS}
)

target_link_libraries(Greedy_algo_Seam_Carving
    PRIVATE
        opencv_core
        opencv_imgproc
        opencv_imgcodecs
        opencv_highgui
)

if(WIN32 AND EXISTS "${CMAKE_SOURCE_DIR}/dependencies/bin")
    file(GLOB OPENCV_DLLS "${CMAKE_SOURCE_DIR}/dependencies/bin/*.dll")
    if(OPENCV_DLLS)
        add_custom_command(TARGET Greedy_algo_Seam_Carving POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
                ${OPENCV_DLLS}
                $<TARGET_FILE_DIR:Greedy_algo_Seam_Carving>
        )
    endif()
endif()
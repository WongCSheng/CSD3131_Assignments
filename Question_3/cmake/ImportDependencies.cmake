 if(NOT DEFINED FETCHCONTENT_BASE_DIR)
    set(
        FETCHCONTENT_BASE_DIR
        "${CMAKE_SOURCE_DIR}/dependencies"
        CACHE PATH "Directory for fetched dependency sources and build files"
    )
endif()

include(FetchContent)

function(import_opencv)
    if(TARGET opencv_core)
        return()
    endif()

    set(_deps_dir "${CMAKE_SOURCE_DIR}/dependencies")

    find_package(OpenCV CONFIG QUIET
        COMPONENTS
            core
            imgproc
            imgcodecs
            highgui
        PATHS "${_deps_dir}"
        NO_DEFAULT_PATH
    )

    if(NOT OpenCV_FOUND)
        message(FATAL_ERROR
            "OpenCV was not found in '${_deps_dir}'.\n"
            "Please build and install OpenCV (with core,imgproc,imgcodecs,highgui) into '${_deps_dir}' first."
        )
    endif()

    foreach(module IN ITEMS opencv_core opencv_imgproc opencv_imgcodecs opencv_highgui)
        if(NOT TARGET ${module})
            message(FATAL_ERROR "OpenCV did not define the expected target: ${module}")
        endif()
    endforeach()
endfunction()

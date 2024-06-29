cmake_minimum_required(VERSION 3.10.2)
project(graphics VERSION 0.0.1 )
set(CMAKE_VERBOSE_MAKEFILE ON)

set(CMAKE_CXX_STANDARD 17)
#set(CMAKE_CXX_CLANG_TIDY "clang-tidy;-checks=*")

#set(OpenCV_DIR ${CMAKE_SOURCE_DIR}/opencv_mobile/sdk/native/jni)
add_compile_options(-O2)

#add_subdirectory(uploading_packages)


set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DANDROID_PLATFORM=android-23")

add_subdirectory(opencv)
include_directories(opencv/include)


# Include the Dart SDK include directory
include_directories(${DART_SDK}/include)

#include_directories(../ios)

#find_package(OpenCV REQUIRED)

file (GLOB SRC_FILES
        ../src/graphics.cpp
        ../src/aroundFilter.cpp



)

set (SOURCE_FILES ${CPP_FILES})

add_library(${CMAKE_PROJECT_NAME} SHARED
  ${SRC_FILES}
)


target_link_libraries( ${CMAKE_PROJECT_NAME}

        libopencv_aruco
        libopencv_bgsegm
        libopencv_bioinspired
        libopencv_calib3d
        libopencv_ccalib
        libopencv_core
        libopencv_datasets
        libopencv_dnn
        libopencv_dnn_objdetect
        libopencv_dnn_superres
        libopencv_dpm
        libopencv_face
        libopencv_features2d
        libopencv_flann
        libopencv_fuzzy
        libopencv_gapi
        libopencv_hfs
        libopencv_highgui
        libopencv_imgcodecs
        libopencv_img_hash
        libopencv_imgproc
        libopencv_intensity_transform
        libopencv_line_descriptor
        libopencv_mcc
        libopencv_ml
        libopencv_objdetect
        libopencv_optflow
        libopencv_phase_unwrapping
        libopencv_photo
        libopencv_plot
        libopencv_quality
        libopencv_rapid
        libopencv_reg
        libopencv_rgbd
        libopencv_saliency
        libopencv_shape
        libopencv_stereo
        libopencv_stitching
        libopencv_structured_light
        libopencv_superres
        libopencv_surface_matching
        libopencv_text
        libopencv_tracking
        libopencv_video
        libopencv_videoio
        libopencv_videostab
        libopencv_wechat_qrcode
        libopencv_xfeatures2d
        libopencv_ximgproc
        libopencv_xobjdetect
        libopencv_xphoto
    )

    target_link_libraries( ${CMAKE_PROJECT_NAME}
        ade
        cpufeatures
        IlmImf
        ittnotify
        libjpeg-turbo
        libopenjp2
        libpng
        libprotobuf
        libtiff
        libwebp
        tbb
        tegra_hal
    )



    target_link_libraries(${CMAKE_PROJECT_NAME}   log OpenSLES atomic m z)

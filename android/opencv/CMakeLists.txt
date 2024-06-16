# Include directories
include_directories(${CMAKE_CURRENT_LIST_DIR}/include)

# Define the OpenCV libraries
set(OPENCV_LIBS
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

# Add OpenCV libraries as imported targets
foreach(lib ${OPENCV_LIBS})
    add_library(${lib} STATIC IMPORTED GLOBAL)
    set_target_properties(${lib} PROPERTIES IMPORTED_LOCATION ${CMAKE_CURRENT_LIST_DIR}/lib/${ANDROID_ABI}/${lib}.a)
endforeach()

# Create a variable that lists all OpenCV libraries for convenient linking
set(OPENCV_LIBRARIES ${OPENCV_LIBS})

# Define the list of third-party libraries
set(THIRD_PARTY_LIBS
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

# Add third-party libraries as imported targets
foreach(lib ${THIRD_PARTY_LIBS})
    add_library(${lib} STATIC IMPORTED GLOBAL)
    set_target_properties(${lib} PROPERTIES IMPORTED_LOCATION ${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/lib/arm64-v8a/lib${lib}.a)
endforeach()

# Create a variable that lists all third-party libraries for convenient linking
set(THIRD_PARTY_LIBRARIES ${THIRD_PARTY_LIBS})
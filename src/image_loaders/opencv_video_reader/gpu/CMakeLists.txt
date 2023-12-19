set(SOURCES
    opencv_video_reader_gpu.h
    opencv_video_reader_gpu.cpp
)

include_directories("${PROJECT_SOURCE_DIR}/src")

add_library(opencv_video_reader_gpu ${SOURCES})

find_package(OpenCV REQUIRED)

target_include_directories(opencv_video_reader_gpu PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${OpenCV_INCLUDE_DIRS}
)

target_link_libraries(opencv_video_reader_gpu PUBLIC
    ${OpenCV_LIBS}
)

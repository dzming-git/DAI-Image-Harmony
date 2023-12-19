set(SOURCES
    opencv_video_reader_cpu.h
    opencv_video_reader_cpu.cpp
)

include_directories("${PROJECT_SOURCE_DIR}/src")

add_library(opencv_video_reader_cpu ${SOURCES})

find_package(OpenCV REQUIRED)

target_include_directories(opencv_video_reader_cpu PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${OpenCV_INCLUDE_DIRS}
)

target_link_libraries(opencv_video_reader_cpu PUBLIC
    ${OpenCV_LIBS}
)

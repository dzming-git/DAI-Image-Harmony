set(SOURCES
    local_image_loader.h
    local_image_loader.cpp
)

include_directories("${PROJECT_SOURCE_DIR}/src")

add_library(local_image ${SOURCES})

find_package(OpenCV REQUIRED)

target_include_directories(local_image PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${OpenCV_INCLUDE_DIRS}
)

target_link_libraries(local_image PUBLIC
    ${OpenCV_LIBS}
)

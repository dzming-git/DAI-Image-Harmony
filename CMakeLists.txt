cmake_minimum_required(VERSION 3.10)

project(image_harmony)

find_package(OpenCV REQUIRED)

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
set(CMAKE_CXX_STANDARD 20)

include_directories("${PROJECT_SOURCE_DIR}/include")

set(SOURCES
    main.cpp
    src/http_server.cpp
    src/image_loaders.cpp
    )

set(HEADERS
    include/http_server.h
    include/image_loaders.h
    )

add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})
target_include_directories(${PROJECT_NAME} PRIVATE ${OpenCV_INCLUDE_DIRS})
target_link_libraries(${PROJECT_NAME} ${OpenCV_LIBS})

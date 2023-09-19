cmake_minimum_required(VERSION 3.10)

project(image_harmony)

find_package(OpenCV REQUIRED)

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
set(CMAKE_CXX_STANDARD 20)

include_directories("${PROJECT_SOURCE_DIR}/include")
include_directories("${PROJECT_SOURCE_DIR}/include/image_loaders")
include_directories("${PROJECT_SOURCE_DIR}/include/image_loaders/local_image")

set(SOURCES
    main.cpp
    src/http_server.cpp
    src/image_loaders/image_loader_base.cpp
    src/image_loaders/image_loader_factory.cpp
    src/image_loaders/local_image/local_image_loader.cpp
    )

set(HEADERS
    include/http_server.h
    include/image_loaders/image_loader_base.h
    include/image_loaders/image_loader_factory.h
    include/image_loaders/local_image/local_image_loader.h
    )

add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})
target_include_directories(${PROJECT_NAME} PRIVATE ${OpenCV_INCLUDE_DIRS})
target_link_libraries(${PROJECT_NAME} ${OpenCV_LIBS})

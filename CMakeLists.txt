cmake_minimum_required(VERSION 3.10)

project(image_harmony)

find_package(OpenCV REQUIRED)

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
set(CMAKE_CXX_STANDARD 20)

# 设置HCNetSDK
set(HCNetSDK_INCLUDE_DIRS ${PROJECT_SOURCE_DIR}/thirdparty/HCNetSDK/include)

# 查找HCNetSDK动态链接库
set(HCNetSDK_LIB_DIR ${PROJECT_SOURCE_DIR}/thirdparty/HCNetSDK/lib/)
file(GLOB HCNetSDK_LIBS "${HCNetSDK_LIB_DIR}/*.so*")

set(HCNetSDK_COM_LIB_DIR ${PROJECT_SOURCE_DIR}/thirdparty/HCNetSDK/lib/HCNetSDKCom)
file(GLOB HCNetSDK_COM_LIBS "${HCNetSDK_COM_LIB_DIR}/*.so*")

set(HCNetSDK_LIBS ${HCNetSDK_LIBS} ${HCNetSDK_COM_LIBS})

include_directories("${PROJECT_SOURCE_DIR}/include")
include_directories("${PROJECT_SOURCE_DIR}/include/image_loaders")
include_directories("${PROJECT_SOURCE_DIR}/include/image_loaders/local_image")
include_directories("${PROJECT_SOURCE_DIR}/include/image_loaders/webcamera_hikvision")

set(SOURCES
    main.cpp
    src/http_server.cpp
    src/image_loaders/image_loader_base.cpp
    src/image_loaders/image_loader_factory.cpp
    src/image_loaders/local_image/local_image_loader.cpp
    src/image_loaders/webcamera_hikvision/webcamera_hikvision.cpp
    )

set(HEADERS
    include/http_server.h
    include/image_loaders/image_loader_base.h
    include/image_loaders/image_loader_factory.h
    include/image_loaders/local_image/local_image_loader.h
    include/image_loaders/webcamera_hikvision/webcamera_hikvision.h
    )

add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})

# OpenCV
target_include_directories(${PROJECT_NAME} PRIVATE ${OpenCV_INCLUDE_DIRS})
target_link_libraries(${PROJECT_NAME} ${OpenCV_LIBS})

# HCNetSDK
target_include_directories(${PROJECT_NAME} PRIVATE ${HCNetSDK_INCLUDE_DIRS})
target_link_libraries(${PROJECT_NAME} ${HCNetSDK_LIBS})

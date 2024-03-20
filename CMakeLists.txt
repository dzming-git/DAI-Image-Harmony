# CMake最低版本
cmake_minimum_required(VERSION 3.10)

# 项目名 语言
project(image_harmony C CXX)

# C++版本
set(CMAKE_CXX_STANDARD 20)

# 子模块
add_subdirectory(src/image_loaders/webcamera_hikvision)
add_subdirectory(src/image_loaders/opencv_video_reader/cpu)
add_subdirectory(src/image_loaders/opencv_video_reader/gpu)
add_subdirectory(src/image_loaders/local_image)
add_subdirectory(src/config)
add_subdirectory(src/consul)
add_subdirectory(resources/protos)
add_subdirectory(src/http_server)

# 可执行程序的输出目录
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
execute_process(
  COMMAND ${CMAKE_COMMAND} -E make_directory ${PROJECT_SOURCE_DIR}/bin
)

# 生成文件路径
set(GENERATED_OUT_PATH ${PROJECT_SOURCE_DIR}/generated)

# 查找OpenCV
find_package(OpenCV REQUIRED)

# include
include_directories(${GENERATED_OUT_PATH})
include_directories(${PROJECT_SOURCE_DIR}/src)
include_directories(${OpenCV_INCLUDE_DIRS})

set(SOURCES
    main.cpp
    src/image_loaders/image_loader_base.h
    src/image_loaders/image_loader_base.cpp
    src/image_loaders/image_loader_factory.h
    src/image_loaders/image_loader_factory.cpp
    src/image_loaders/image_loader_controller.h
    src/image_loaders/image_loader_controller.cpp
    src/grpc/servers/grpc_server.h
    src/grpc/servers/grpc_server.cpp
    src/grpc/servers/grpc_server_builder.h
    src/grpc/servers/grpc_server_builder.cpp
    src/grpc/servers/image_harmony/image_harmony_server.h
    src/grpc/servers/image_harmony/image_harmony_server.cpp
    src/grpc/servers/service_coordinator/service_coordinator_server.h
    src/grpc/servers/service_coordinator/service_coordinator_server.cpp
    src/utils/random_utils.h
    src/utils/random_utils.cpp
    src/utils/log.h
    src/utils/log.cpp
    )

add_executable(${PROJECT_NAME} ${SOURCES})

# 子模块
target_link_libraries(${PROJECT_NAME} 
  opencv_video_reader_cpu 
  opencv_video_reader_gpu 
  webcamera_hikvision
  local_image
  config
  consul-cpp
  image_harmony_data_grpc_proto
  service_coordinator_data_grpc_proto
  http_server
  )

# OpenCV
target_include_directories(${PROJECT_NAME} PRIVATE ${OpenCV_INCLUDE_DIRS})
target_link_libraries(${PROJECT_NAME} ${OpenCV_LIBS})

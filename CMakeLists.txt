# CMake最低版本
cmake_minimum_required(VERSION 3.10)

# 项目名 语言
project(image_harmony C CXX)

# C++版本
set(CMAKE_CXX_STANDARD 20)

# 子模块
add_subdirectory(src/image_loaders/webcamera_hikvision)
add_subdirectory(src/image_loaders/local_image)
add_subdirectory(resources/protos)

# 可执行程序的输出目录
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
execute_process(
  COMMAND ${CMAKE_COMMAND} -E make_directory ${PROJECT_SOURCE_DIR}/bin
)

# 生成文件路径
set(GENERATED_OUT_PATH ${PROJECT_SOURCE_DIR}/generated)

# 查找OpenCV
find_package(OpenCV REQUIRED)

# libhv
set(HV_LIBRARY /usr/local/lib/libhv.so)

# yaml-cpp
set(YAMLCPP_LIBRARY /usr/local/lib/libyaml-cpp.a)

# include
include_directories(${GENERATED_OUT_PATH})
include_directories(${PROJECT_SOURCE_DIR}/src)
include_directories(${OpenCV_INCLUDE_DIRS})

set(SOURCES
    main.cpp
    src/http_server/http_server.h
    src/http_server/http_server.cpp
    src/http_server/http_server_builder.h
    src/http_server/http_server_builder.cpp
    src/http_server/http_response_handler/http_response_handler_base.h
    src/http_server/http_response_handler/http_response_handler_base.cpp
    src/http_server/http_response_handler/http_response_handler_factory.h
    src/http_server/http_response_handler/http_response_handler_factory.cpp
    src/http_server/http_response_handler/http_response_handler_test.h
    src/http_server/http_response_handler/http_response_handler_test.cpp
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
    src/consul/consul_client.h
    src/consul/consul_client.cpp
    src/consul/service_info.h
    src/consul/service_info.cpp
    src/config/config.h
    src/config/config.cpp
    src/utils/random_utils.h
    src/utils/random_utils.cpp
    )

add_executable(${PROJECT_NAME} ${SOURCES})

# 子模块
target_link_libraries(${PROJECT_NAME} 
  webcamera_hikvision
  local_image
  image_harmony_data_grpc_proto
  service_coordinator_data_grpc_proto
  )

# local
set(LOCAL_INCLUDE_DIRS "/usr/local/include")
target_include_directories(${PROJECT_NAME} PRIVATE ${HV_INCLUDE_DIRS})

# OpenCV
target_include_directories(${PROJECT_NAME} PRIVATE ${OpenCV_INCLUDE_DIRS})
target_link_libraries(${PROJECT_NAME} ${OpenCV_LIBS})

# linhv
target_link_libraries(${PROJECT_NAME} ${HV_LIBRARY})

# yaml-cpp
target_link_libraries(${PROJECT_NAME} ${YAMLCPP_LIBRARY})


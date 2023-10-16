# CMake最低版本
cmake_minimum_required(VERSION 3.10)

# 项目名 语言
project(image_harmony C CXX)

# C++版本
set(CMAKE_CXX_STANDARD 20)

# 可执行程序的输出目录
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)

# 生成文件的输出目录
set(GENERATED_OUT_PATH ${PROJECT_SOURCE_DIR}/generated)

# 查找OpenCV
find_package(OpenCV REQUIRED)
find_package(Protobuf CONFIG REQUIRED)
find_package(absl CONFIG REQUIRED)
find_package(gRPC CONFIG REQUIRED)
# 设置HCNetSDK包含文件
set(HCNetSDK_INCLUDE_DIRS ${PROJECT_SOURCE_DIR}/thirdparty/HCNetSDK/include)

# 查找HCNetSDK动态链接库
set(HCNetSDK_LIB_DIR ${PROJECT_SOURCE_DIR}/thirdparty/HCNetSDK/lib/)
file(GLOB HCNetSDK_LIBS "${HCNetSDK_LIB_DIR}/*.so*")
set(HCNetSDK_COM_LIB_DIR ${PROJECT_SOURCE_DIR}/thirdparty/HCNetSDK/lib/HCNetSDKCom)
file(GLOB HCNetSDK_COM_LIBS "${HCNetSDK_COM_LIB_DIR}/*.so*")
set(HCNetSDK_LIBS ${HCNetSDK_LIBS} ${HCNetSDK_COM_LIBS})

#查找本地Protobuf模块的库信息，实际上CMake就是在找Protobuf-config.cmake文件
set(protobuf_MODULE_COMPATIBLE TRUE)
find_package(Protobuf CONFIG REQUIRED)
message(STATUS "Using protobuf ${Protobuf_VERSION}")

set(_PROTOBUF_LIBPROTOBUF protobuf::libprotobuf)
set(_REFLECTION gRPC::grpc++_reflection)
if(CMAKE_CROSSCOMPILING)
  find_program(_PROTOBUF_PROTOC protoc)
else()
  set(_PROTOBUF_PROTOC $<TARGET_FILE:protobuf::protoc>)
endif()

# Find gRPC installation
# Looks for gRPCConfig.cmake file installed by gRPC's cmake installation.
#查找本地gRPC模块的库信息
find_package(gRPC CONFIG REQUIRED)
message(STATUS "Using gRPC ${gRPC_VERSION}")

set(_GRPC_GRPCPP gRPC::grpc++)
if(CMAKE_CROSSCOMPILING)
  find_program(_GRPC_CPP_PLUGIN_EXECUTABLE grpc_cpp_plugin)
else()
  set(_GRPC_CPP_PLUGIN_EXECUTABLE $<TARGET_FILE:gRPC::grpc_cpp_plugin>)
endif()


# proto文件生成代码文件

## 设置生成文件路径
set(data_proto ${PROJECT_SOURCE_DIR}/resources/protos/img_trans.proto)
set(data_proto_srcs "${GENERATED_OUT_PATH}/protos/img_trans/img_trans.pb.cc")
set(data_proto_hdrs "${GENERATED_OUT_PATH}/protos/img_trans/img_trans.pb.h")
set(data_grpc_srcs "${GENERATED_OUT_PATH}/protos/img_trans/img_trans.grpc.pb.cc")
set(data_grpc_hdrs "${GENERATED_OUT_PATH}/protos/img_trans/img_trans.grpc.pb.h")

## 生成文件
add_custom_command(
      OUTPUT "${data_proto_srcs}" "${data_proto_hdrs}" "${data_grpc_srcs}" "${data_grpc_hdrs}"
      COMMAND protoc
      ARGS --grpc_out "${GENERATED_OUT_PATH}/protos/img_trans"
        --cpp_out "${GENERATED_OUT_PATH}/protos/img_trans"
        -I "${PROJECT_SOURCE_DIR}/resources/protos"
        --plugin=protoc-gen-grpc=`which grpc_cpp_plugin`
        "${data_proto}"
      DEPENDS "${data_proto}")

#将grpc的四个文件编译成一个库
# data_grpc_proto
add_library(data_grpc_proto
  ${data_grpc_srcs}
  ${data_grpc_hdrs}
  ${data_proto_srcs}
  ${data_proto_hdrs})
#链接库data_grpc_proto，会生成链接文件
target_link_libraries(data_grpc_proto
  ${_REFLECTION}
  ${_GRPC_GRPCPP}
  ${_PROTOBUF_LIBPROTOBUF})

# libhv
set(HV_LIBRARY /usr/local/lib/libhv.so)

# include
include_directories("${GENERATED_OUT_PATH}")
include_directories("${PROJECT_SOURCE_DIR}/include")

set(HEADERS
    include/http_server/http_server.h
    include/http_server/http_server_builder.h
    include/http_server/http_response_handler/http_response_handler_base.h
    include/http_server/http_response_handler/http_response_handler_factory.h
    include/http_server/http_response_handler/http_response_handler_test.h
    include/image_loaders/image_loader_base.h
    include/image_loaders/image_loader_factory.h
    include/image_loaders/image_loader_controller.h
    include/image_loaders/local_image/local_image_loader.h
    include/image_loaders/webcamera_hikvision/webcamera_hikvision.h
    include/grpc/grpc_server.h
    include/grpc/grpc_server_builder.h
    include/grpc/serviecs/img_trans/img_trans.h
    include/consul/consul_client.h
    )

set(SOURCES
    main.cpp
    src/http_server/http_server.cpp
    src/http_server/http_server_builder.cpp
    src/http_server/http_response_handler/http_response_handler_base.cpp
    src/http_server/http_response_handler/http_response_handler_factory.cpp
    src/http_server/http_response_handler/http_response_handler_test.cpp
    src/image_loaders/image_loader_base.cpp
    src/image_loaders/image_loader_factory.cpp
    src/image_loaders/image_loader_controller.cpp
    src/image_loaders/local_image/local_image_loader.cpp
    src/image_loaders/webcamera_hikvision/webcamera_hikvision.cpp
    src/grpc/grpc_server.cpp
    src/grpc/grpc_server_builder.cpp
    src/grpc/serviecs/img_trans/img_trans.cpp
    src/consul/consul_client.cpp
    )

add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})

# local
set(LOCAL_INCLUDE_DIRS "/usr/local/include")
target_include_directories(${PROJECT_NAME} PRIVATE ${HV_INCLUDE_DIRS})

# OpenCV
target_include_directories(${PROJECT_NAME} PRIVATE ${OpenCV_INCLUDE_DIRS})
target_link_libraries(${PROJECT_NAME} ${OpenCV_LIBS})

# HCNetSDK
target_include_directories(${PROJECT_NAME} PRIVATE ${HCNetSDK_INCLUDE_DIRS})
target_link_libraries(${PROJECT_NAME} ${HCNetSDK_LIBS})

# proto
target_link_libraries(${PROJECT_NAME}
  data_grpc_proto
  ${_REFLECTION}
  ${_GRPC_GRPCPP}
  ${_PROTOBUF_LIBPROTOBUF})

# linhv
target_link_libraries(${PROJECT_NAME} ${HV_LIBRARY})

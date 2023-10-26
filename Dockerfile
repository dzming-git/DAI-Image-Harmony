FROM ubuntu:20.04

ENV DEBIAN_FRONTEND=noninteractive
RUN apt update && \
    apt install -y \
        git \
        gcc \
        g++ \
        gdb \
        cmake \
        make \
        build-essential \
        wget \
        unzip \
        net-tools \
        ffmpeg \
        libsm6 \
        libxext6 \
        libgl1 && \
    apt purge -y --auto-remove

# 安装OpenCV

ENV OPENCV_VERSION='4.8.0' DEBIAN_FRONTEND=noninteractive

CMD ["echo", "140.82.114.4", "github.com", ">>", "/etc/hosts"]

RUN \
    wget --progress=dot:giga https://github.com/opencv/opencv/archive/${OPENCV_VERSION}.zip && \
    unzip -q ${OPENCV_VERSION}.zip && \
    rm ${OPENCV_VERSION}.zip && \
    mv opencv-${OPENCV_VERSION} OpenCV && \
    cd /OpenCV && \
    mkdir build && \
    cd build && \
    cmake \
        -D CMAKE_BUILD_TYPE=RELEASE \
        -D OPENCV_DNN_CUDA=OFF \
        -D WITH_CUDA=OFF \
        -D CMAKE_INSTALL_PREFIX=/usr/local \
        .. && \
    make -j4 && \
    make install && \
    ldconfig && \
    rm -rf /OpenCV

# 安装gRPC
# https://github.com/grpc/grpc

ENV GRPC_VERSION='1.58.1'

RUN \
    cd / && \
    git clone https://github.com/grpc/grpc  && \
    cd /grpc && \
    git checkout v${GRPC_VERSION} && \
    git submodule update --init && \
    mkdir -p cmake/build && \
    cd cmake/build && \
    cmake ../.. && \
    make && \
    make install && \
    cd third_party/protobuf/ && \
    make && \
    make install && \
    ldconfig && \
    rm -rf /grpc

# 安装 libhv
# https://github.com/ithewei/libhv

ENV LIBHV_VERSION='1.3.2'

RUN \
    cd / && \
    git clone https://github.com/ithewei/libhv  && \
    cd /libhv && \
    git checkout v${LIBHV_VERSION} && \
    ./configure && \
    make && \
    make install && \
    ldconfig && \
    rm -rf /libhv


# 安装yaml-cpp
# https://github.com/jbeder/yaml-cpp

ENV YAML_CPP_VERSION='0.7.0'

RUN \
    cd / && \
    git clone https://github.com/jbeder/yaml-cpp  && \
    cd /yaml-cpp && \
    git checkout yaml-cpp-${YAML_CPP_VERSION} && \
    mkdir build && \
    cd build && \
    cmake .. && \
    make && \
    make install && \
    ldconfig && \
    rm -rf /yaml-cpp

EXPOSE 5000

COPY . ./workspace
WORKDIR /workspace

# docker build . -t image_harmony:latest

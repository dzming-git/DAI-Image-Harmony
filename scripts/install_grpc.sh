mkdir -p ~/install_tmp
cd ~/install_tmp
git clone  -b v1.60.0 https://github.com/grpc/grpc
cd grpc
git submodule update --init
mkdir -p cmake/build
cd cmake/build
cmake ../..
make
make install
cd third_party/protobuf/
make
make install
ldconfig

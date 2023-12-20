mkdir -p ~/install_tmp
cd ~/install_tmp
git clone -b yaml-cpp-0.7.0 https://github.com/jbeder/yaml-cpp 
cd yaml-cpp
mkdir build
cd build
cmake ..
make
make install
ldconfig

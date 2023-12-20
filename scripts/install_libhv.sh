mkdir -p ~/install_tmp
cd ~/install_tmp
git clone -b 1.3.2 https://github.com/ithewei/libhv 
cd libhv
./configure
make
make install
ldconfig

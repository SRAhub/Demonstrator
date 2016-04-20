#!/bin/bash
apt-get update

# Dependencies
## Git
apt-get install -y git

## CMake
apt-get install -y cmake

## WiringPi
git clone --depth 1 --branch master git://git.drogon.net/wiringPi
cd wiringPi
./build
cd ..
rm -Rf wiringPi

## Armadillo C++
apt-get install -y libblas-dev liblapack-dev libopenblas-dev
wget -O armadillo.tar.gz http://downloads.sourceforge.net/project/arma/armadillo-6.500.5.tar.gz
mkdir armadillo
tar -xzf armadillo.tar.gz -C ./armadillo --strip-components=1
cd armadillo
cmake .
make
make install
### Fixes issues with IWYU (suggesting for example <armadillo_bits/Base_bones.hpp> instead of <armadillo>)
find /usr/include/armadillo_bits -name *.hpp -exec sed -i -e '1i\/\/ IWYU pragma\: private\, include \<armadillo\>' {} ';'
cd ..
rm -Rf armadillo armadillo.tar.gz

## Mantella
git clone --depth 1 --branch master https://github.com/SebastianNiemann/Mantella.git
cd Mantella
cmake .
make
make install
cd ..
rm -Rf Mantella

# Testing
apt-get install -y iwyu
add-apt-repository 'deb http://llvm.org/apt/trusty/ llvm-toolchain-trusty-3.6 main'
wget -O - http://llvm.org/apt/llvm-snapshot.gpg.key | sudo apt-key add -
apt-get update -qq
if [ -f /usr/local/clang-3.5.0/bin/clang-format ]; then
  mv /usr/local/clang-3.5.0/bin/clang-format /usr/local/clang-3.5.0/bin/clang-format-3.5
fi
apt-get install -y clang-format-3.6
update-alternatives --install /usr/bin/clang-format clang-format /usr/bin/clang-format-3.6 100
update-alternatives --set clang-format /usr/bin/clang-format-3.6

# Debugging
apt-get install -y gdb

# Useful development tools
apt-get install -y htop
apt-get install -y vim
apt-get install -y dos2unix

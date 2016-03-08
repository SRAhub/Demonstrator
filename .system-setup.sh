#!/bin/bash

# Updates the system
apt-get update
apt-get upgrade -y

# Installs dependencies
## Installs Clang
apt-get install -y clang
update-alternatives --set cc /usr/bin/clang
update-alternatives --set c++ /usr/bin/clang++

## Installs CMake
apt-get install -y cmake

## Installs WiringPi
git clone --depth 1 --branch master git://git.drogon.net/wiringPi
cd wiringPi
./build
cd ..
rm -Rf wiringPi

## Installs Armadillo C++
apt-get install -y libblas-dev liblapack-dev libopenblas-dev
wget --quiet -O armadillo.tar.gz http://downloads.sourceforge.net/project/arma/armadillo-6.500.5.tar.gz
mkdir armadillo
tar -xzf armadillo.tar.gz -C ./armadillo --strip-components=1
cd armadillo
cmake .
make --quiet
make --quiet install
### Fixes issues with IWYU (suggesting for example <armadillo_bits/Base_bones.hpp> instead of <armadillo>)
find /usr/include/armadillo_bits -name *.hpp -exec sed -i -e '1i\/\/ IWYU pragma\: private\, include \<armadillo\>' {} ';'
cd ..
rm -Rf armadillo armadillo.tar.gz

## Installs Mantella
git clone --depth 1 --branch master https://github.com/SebastianNiemann/Mantella.git
cd Mantella
cmake .
make
make install
cd ..
rm -Rf Mantella

# Testing
sudo apt-get install -y catch
sudo apt-get install -y iwyu
sudo apt-get install -y lcov
## Installs clang-format-3.6
sudo apt-get install -y clang-format-3.6
sudo update-alternatives --install /usr/bin/clang-format clang-format /usr/bin/clang-format-3.6 100
sudo update-alternatives --set clang-format /usr/bin/clang-format-3.6

# Debugging
sudo apt-get install -y gdb
sudo apt-get install -y valgrind

# Useful development tools
sudo apt-get install -y htop
sudo apt-get install -y git
sudo apt-get install -y ccache
sudo apt-get install -y gdb
sudo apt-get install -y dos2unix

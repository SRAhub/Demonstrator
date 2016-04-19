Version 0.10.0
==============

Installation
------------

The recommended way to install the **Demonstrator library together with all dependencies**, is to run `.system-setup.sh`. Vagrant users may also create a preset virtual image, using `vagrant up`.

However, it is also possible to install everything yourself (the following commands are based on Ubuntu 14.04. If you are using another Linux distributions, the names of each package might also differ.):

### Prerequirements

- GCC 4.8.1, Clang 3.3 or any other C++11 feature complete compiler
- [CMake](https://cmake.org/install/) 2.8.7
- [wiringPi](http://wiringpi.com/download-and-install/) 2.24
- [Mantella](http://mantella.info/getting-started/installation/) 0.12.0
  - [Armadillo](http://mantella.info/getting-started/installation/) (Prerequirement of Mantella)

**If you already installed a newer version, this will also work.** If not, the following instructions will guide you step by step throw their installation, assuming a default Ubuntu 14.04 operating system.

``` sh
# Installs CMake
sudo apt-get install cmake

# Installs WiringPi
git clone --depth 1 --branch master git://git.drogon.net/wiringPi
cd wiringPi
sudo ./build

# Installs Armadillo C++
apt-get install -y libblas-dev liblapack-dev libopenblas-dev
wget -O armadillo.tar.gz http://downloads.sourceforge.net/project/arma/armadillo-6.500.5.tar.gz
mkdir armadillo
tar -xzf armadillo.tar.gz -C ./armadillo --strip-components=1
cd armadillo
cmake .
make
sudo make install

# Installs Mantella
git clone --depth 1 --branch master https://github.com/SebastianNiemann/Mantella.git
cd Mantella
cmake .
make
sudo make install
```

### Demonstrator library

``` sh
mkdir build
cd build
cmake ..
make
sudo make install
```

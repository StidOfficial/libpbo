# libpbo
Library for manage PBO file of Bohemia Interactive.

## Prerequisites
yum :
```
yum install gcc-c++ make cmake libstdc++ openssl-devel gtkmm30-devel
```

apt-get :
```
apt-get install build-essential make cmake libstdc++6 libssl-dev libgtkmm-3.0-dev
```

brew :
```
brew install cmake openssl gtkmm3
```

Visual Studio :

Download and install OpenSSL library : https://github.com/openssl/openssl

## Build
```
cmake . -Bbuild
cd build
make
```

Visual Studio :

Open **CMakeLists.txt** with *File* > *Open* > *CMake...*

And build with *CMake* > *Build All*

## Install
```
make install
```

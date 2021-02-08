# Compiling obs-midi

## Prerequisites

You'll need [Qt 5.15.2](https://download.qt.io/official_releases/qt/5.15/),
[CMake](https://cmake.org/download/) and a working [OBS Studio development environment](https://obsproject.com/wiki/install-instructions) installed on your
computer.

## Windows

In cmake-gui, you'll have to set the following variables :

- **QTDIR** (path) : location of the Qt environment suited for your compiler and architecture
- **LIBOBS_INCLUDE_DIR** (path) : location of the libobs subfolder in the source code of OBS Studio
- **LIBOBS_LIB** (filepath) : location of the obs.lib file
- **OBS_FRONTEND_LIB** (filepath) : location of the obs-frontend-api.lib file
- **CMAKE_INSTALL_PREFIX** (path) : Same location as QTDIR

## Linux

On Debian/Ubuntu :

```shell
sudo apt-get install libboost-all-dev
git clone --recursive https://github.com/Palakis/obs-midi.git
cd obs-midi
mkdir build && cd build
cmake -DLIBOBS_INCLUDE_DIR="<path to the libobs sub-folder in obs-studio's source code>" -DCMAKE_INSTALL_PREFIX=/usr -DUSE_UBUNTU_FIX=true ..
make -j4
sudo make install
```

On other linux OS's, use this cmake command instead:

```shell
cmake -DLIBOBS_INCLUDE_DIR="<path to the libobs sub-folder in obs-studio's source code>" -DCMAKE_INSTALL_PREFIX=/usr ..
```

## OS X

As a prerequisite, you will need Xcode for your current OSX version, the Xcode command line tools, and [Homebrew](https://brew.sh/).
Homebrew's setup will guide you in getting your system set up, you should be good to go once Homebrew is successfully up and running.

Use of the macOS CI scripts is recommended. Please note that these
scripts install new software and can change several settings on your system. An
existing obs-studio development environment is not required, as
`install-build-obs-macos.sh` will install it for you. If you already have a
working obs-studio development environment and have built obs-studio, you can
skip that script.

Of course, you're encouraged to dig through the contents of these scripts to
look for issues or specificities.

```shell
git clone --recursive https://github.com/Palakis/obs-midi.git
cd obs-midi
./CI/install-dependencies-macos.sh
./CI/install-build-obs-macos.sh
./CI/build-macos.sh
./CI/package-macos.sh
```

This will result in a ready-to-use `obs-midi.pkg` installer in the `release` subfolder.

## Automated Builds

[![Build Status](https://dev.azure.com/Palakis/obs-midi/_apis/build/status/Palakis.obs-midi?branchName=4.x-current)](https://dev.azure.com/Palakis/obs-midi/_build/latest?definitionId=2&branchName=4.x-current)

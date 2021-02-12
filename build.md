# Compiling obs-midi

## Prerequisites

You'll need [Qt 5.15.2](https://download.qt.io/official_releases/qt/5.15/),
[CMake](https://cmake.org/download/) and a working [OBS Studio development environment](https://obsproject.com/wiki/install-instructions) installed on your
computer.

For Cleanliness and sanity, Do not mix the obs-studio and obs-midi directories.

For Example My Directory structure looks like this


|Directory | Reason|
|-----------|----------|
| Desktop\OBS\Build| (OBS-Studio Build Dir) |
| Desktop\OBS\Deps | (OBS Dependencies from zip file on obs site) |
| Desktop\OBS\midibuild | (Build Directory for obs-midi) |
| Desktop\OBS\obs-studio | (obs studio source code) |
| Desktop\OBS\obs-midi | (obs midi source code) |
| C:\Qt\5.15.2\msvc2019_64 | (QTDIR) |


## Windows

In cmake-gui, you'll have to set the following variables :

| Variable | info | Example Path |
|----------|-------|-----------|
| **QTDIR**                      | (path) : location of the Qt environment suited for your compiler and architecture     | C:/Qt/5.15.2/msvc2019_64|
| **LIBOBS_INCLUDE_DIR**         | (path) : location of the libobs subfolder in the source code of OBS Studio            | C:/Users/User/Desktop/OBS/obs-studio/libobs |
| **LIBOBS_LIB**                 | (filepath) : location of the obs.lib file                                             | C:/Users/User/Desktop/OBS/build/libobs/RelWithDebInfo/obs.lib|
| **OBS_FRONTEND_LIB**           | (filepath) : location of the obs-frontend-api.lib file                                | C:/Users/User/Desktop/OBS/build/UI/obs-frontend-api/RelWithDebInfo/obs-frontend-api.lib|
| **CMAKE_INSTALL_PREFIX**       | (path) : Same location as QTDIR                                                       | C:/Qt/5.15.2/msvc2019_64|

![My CMake Gui Looks like ](https://github.com/cpyarger/obs-midi/blob/master/docs/obs-midi-cmake.PNG?raw=true)

In Visual Studio, Set OBS-Midi as the startup project,

![set obs-midi as startup project ](https://github.com/cpyarger/obs-midi/blob/master/docs/obs-midi-set-as-startup-project.PNG?raw=true)
Change Output Directory to OBS-Studio projects plugin's folder on the general page
![My CMake Gui Looks like ](https://github.com/cpyarger/obs-midi/blob/master/docs/obs-midi-general-page.PNG?raw=true)
Change Debug to obs-studio's exe and exe dir in the debug page
![My CMake Gui Looks like ](https://github.com/cpyarger/obs-midi/blob/master/docs/obs-midi-debug-page.PNG?raw=true)
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

![CI Multiplatform Build](https://github.com/cpyarger/obs-midi/workflows/CI%20Multiplatform%20Build/badge.svg)

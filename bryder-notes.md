# Using concordance

Run installer
Testing with harmony one
###  --dump-config and --dump-firmware also can use --binary
They all worked
### --print-remote-info --verbose
```powershell
& 'C:\Program Files\Concordance\concordance.exe' --verbose --print-remote-info
```
```text
Concordance 1.4
Copyright 2007 Kevin Timmerman and Phil Dibowitz
This software is distributed under the GPLv3.

Requesting identity:  100%             done
Model: Logitech Harmony One
Skin: 54
Firmware Version: 3.4
Firmware Type: 0
Hardware Version: 0.5.0
External Flash: 4 MiB - 1F:C8 Atmel AT49BV322A
Architecture: 12
Protocol: 12
Manufacturer: Harmony Remote 0-3.4.0
Product: Harmony Remote 0-3.4.0
IRL, ORL, FRL: 0, 0, 0
USB VID: 046D
USB PID: C121
USB Ver: 1054
Serial Number: {EEEEEEEE-EEEE-EEEE-EEEE-EEEEEEEEEEEE}
{DF516B9C-21EB-4E82-BA04-CF8F25E92A4B}
{B489E5E9-0226-4209-B81F-FCEFA2D24615}
Config Flash Used: 45% (1758 of 3840 KiB)

Success!
```
### --verbose --dump-safemode
```powershell
ce\concordance.exe: unknown option -- dump-safe-mode
PS C:\Users\xx\harmonyone> & 'C:\Program Files\Concordance\concordance.exe' --verbose --dump-safemode 
Concordance 1.4
Copyright 2007 Kevin Timmerman and Phil Dibowitz
This software is distributed under the GPLv3.

Requesting identity:  100%             done
Reading safemode fw:  100%    64 KiB   done
Success!
```

### --get-time --verbose
```powershell
& 'C:\Program Files\Concordance\concordance.exe' --get-time --verbose
Concordance 1.4
Copyright 2007 Kevin Timmerman and Phil Dibowitz
This software is distributed under the GPLv3.

Requesting identity:  100%             done
Remote time is 2015/03/29 Sun 18:20:02 +0
Success!
```

### Inputting pronto codes

* [ref](https://www.avforums.com/threads/guide-to-getting-pronto-hex-codes-into-your-harmony-remote.1510519/)
* [harmony size](http://members.harmonyremote.com/EasyZapper) use yh4 and enter when you put in the password

when doing something go to troubleshoot and say 'The remote control software didn't open automatically'

* Go to other
* learn ir commands
* select 0 - 9
* learn selected commands
* troubleshoot
* the remote control software didn't open automatically
* Download LearnIr.EZTut

Then you run
```text
--learn-ir LearnIr.EZTut
```

You do one key, H, 'U' upload it, and on to the next key. When you're done you quit.

Then you update remote.
The first phase needs you to do a connectivity test. Then you can get the config
```bash
--connectivity-test Connectivity.something
--write-config theconfig_you_downloaded.
```
## TODOs
* help for --dump-config is wrong. It writes to Update.EZHex not config.EZHex


# msys2

This worked. There were too many missing libraries in ubuntu mingw. 

* https://www.msys2.org/
* https://www.msys2.org/docs/package-management/

Use msys2 mingw64 environment (ie search for msys2 and run up that shell)

```bash
pacman -Syu
pacman -Su
pacman -Sy
pacman -S --needed base-devel mingw-w64-x86_64-toolchain
# installing a package
pacman -S git

# searching for packages
pacman -Ss lzma
```

```bash

pacman -S mingw-w64-x86_64-hidapi
pacman -S mingw-w64-x86_64-libzip
pacman -S mingw-w64-x86_64-nsis
pacman -S mingw-w64-x86_64-lzma
git clone https://github.com/bryder/concordance.git
```


```bash

mkdir /tmp/buildroot
cd concordance/libconcord
mkdir m4; autoreconf --install # excpet no output while this runs for a bit.
autoupdate # because it told me to
autoreconf --install # with the update.
./configure --prefix=/tmp/buildroot
make
make install

cd ../concordance
autoupdate
autoreconf --install # excpet no output while this runs for a bit.

export MINGW_SYSROOT_BIN=C:/msys64/mingw64/bin
export MINGW_SYSROOT=C:/msys64/


./configure --prefix=/tmp/buildroot CFLAGS="-I/tmp/buildroot/include" LDFLAGS="-L/tmp/buildroot/lib -L/tmp/buildroot/bin"
make install

# Had to edit the .nsi a lot
makensis win/msys2_bryder.nsi
# But this does seem to work so far
cd "C:/program files/concordance"
concordance.exe -h 
```
# Building on 20.04 WSL2
the .exe doesn't do anything. Sigh.

Getting to where I built it
```powershell
cd \\wsl$\Ubuntu

```
## Setup
# ubuntu wsl2 20.04 install

```bash

p=(
libusb-dev
cmake
git
autoconf
concordance # for reference basically
libtool
pkg-config

)
sudo apt install "${p[@]}"
```
## getting debian concordance source - but probably won't use it - gonna try git
```bash
sudo sed -i '/deb-src/s/^# //' /etc/apt/sources.list && sudo apt update

apt-get source concordance
cd concordance-1.4
cd libconcord

# see below for more 
```
# Building from git projects


## Get llvm-mingw tool chain
* [llvm mingw bootstrap](https://github.com/mstorsjo/llvm-mingw/releases)

```bash
mkdir llvm-mingw
export LLVM_DIR=$(realpath llvm-mingw)

release=https://github.com/mstorsjo/llvm-mingw/releases/download/20210423/llvm-mingw-20210423-ucrt-ubuntu-18.04-x86_64.tar.xz
  
wget -qO- "${release}" | tar xJvf - --strip 1 -C ./llvm-mingw
# DId not have a good time trying to build libzip with this.
# so switched to the ubuntu one. Might be a mistake
export PATH="$(pwd)/llvm-mingw/bin:$PATH"
export arch=x86_64-w64-mingw32
```

Hmm. Seemed to work - but did use the tar file - not the git repo

## libhidapi

* [github project](https://github.com/libusb/hidapi)
* [gist to cross compile  to mingw - try this out](https://gist.github.com/tresf/5ls ..65ebc689c735129be9eefd26346b9cd)

```bash
cd concordance

git clone https://github.com/libusb/hidapi "$(pwd)/hidapi"
pushd "$(pwd)/hidapi"
mkdir /tmp/buildroot

./bootstrap
./configure --host=${arch} --prefix=/tmp/buildroot
make  
make install
popd
```
Under wsl2 this is expected. 
```text

libtool:   error: Could not determine the host path corresponding to
libtool:   error:   '/home/bryder/concordance/ubuntu/concordance-1.4/llvm-mingw-hidapi/hidapi/windows/.libs'
```


## libzip

THis is proving to be a pain

Research
* https://github.com/msys2/MINGW-packages/tree/master/mingw-w64-libzip
* https://launchpad.net/ubuntu/+source/libzip
* https://packages.msys2.org/package/mingw-w64-x86_64-libzip
* https://libzip.org/libzip-discuss/msg00867.html
* https://github.com/OpenRCT2/OpenRCT2/blob/c95702c469d6058edbb52174d89015802b628db3/CMakeLists_mingw.txt

The below looks like I can do what I want the same way as in ./configure.

Going to try ubuntu mingw - the llvm-mingw releases are a bit touchy - have to fiddle for c++
```bash
export arch=x86_64-w64-mingw32
```
We'll se later on
```bash
sudo apt install libgcrypt-mingw-w64-dev # maybe? - nah
git clone https://github.com/nih-at/libzip/
cd libzip
# the above didn't seem to work - so used this for using ubuntu toolchain
wget https://libzip.org/download/libzip-1.7.3.tar.xz
tar xf libzip-1.7.3.tar.xz
cd libzip-1.7.3

make_mingw_cmakelists(){
    [[ -z "${arch}" ]] && { echo "**ERROR** you must set \$arch" >&2 ; return 1 ; }

cat <<EOF > CMakeLists_mingw.txt
SET(CMAKE_SYSTEM_NAME Windows)

SET(COMPILER_PREFIX ${arch}-)
SET(CMAKE_C_COMPILER \${COMPILER_PREFIX}gcc)
SET(CMAKE_CXX_COMPILER \${COMPILER_PREFIX}c++)
SET(CMAKE_RC_COMPILER \${COMPILER_PREFIX}windres)
SET(CMAKE_PKGCONFIG_EXECUTABLE \${COMPILER_PREFIX}pkg-config)
SET(PKG_CONFIG_EXECUTABLE \${COMPILER_PREFIX}pkg-config)
SET(CMAKE_SYSTEM_PROCESSOR x86)

#SET(TARGET_ENVIRONMENT ${LLVM_DIR}/${arch})
SET(TARGET_ENVIRONMENT /usr/${arch})  # location from the installed ubuntu mingw 

# here is the target environment located
SET(CMAKE_FIND_ROOT_PATH \${TARGET_ENVIRONMENT})

# adjust the default behaviour of the FIND_XXX() commands:
# search headers and libraries in the target environment, search
# programs in the host environment
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)

INCLUDE_DIRECTORIES(${LLVM_DIR}/${arch}})

EOF
}
```
```text


# note lzma is missing as is bzip2
# actually there are a lot more missing libraries.
# and it said the toolchain variable wasn't used
mkdir build
cd build
cmake ../ -DCMAKE_TOOLCHAIN_FILE=../CMakeLists_mingw.txt -DCMAKE_INSTALL_PREFIX=/tmp/buildroot
make install
```

## concordance
```bash
git clone https://github.com/jaymzh/concordance.git
export PKG_CONFIG_PATH=/tmp/buildroot/lib/pkgconfig:/usr/${arch}/lib/pkgconfig

cd concordance/libconcord
mkdir m4; autoreconf --install
./configure --host=${arch} --prefix=/tmp/buildroot  CFLAGS="-I/tmp/buildroot/include" LDFLAGS="-lzip -L/tmp/buildroot/lib -lhidapi"

make
make install

cd ../concordance
mkdir m4; autoreconf --install
./configure --host=${arch} --prefix=/tmp/buildroot  CFLAGS="-I/tmp/buildroot/include" LDFLAGS="-L/tmp/buildroot/lib -L/tmp/buildroot/bin"

make
make install


```

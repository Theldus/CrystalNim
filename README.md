# CrystalNim 💎
A simple Nim game written in C that uses Raylib as the graphics library

## Introduction
CrystalNim is a mathematical game of strategy in which two players take turns
removing crystals from distinct heaps. On each turn, a player must remove at
least one crystal, and may remove any number of crystals provided they all come
from the same heap. The goal of the game is to avoid being the player who must
remove the last crystal.

<p align="center">
	<img align="center" src="https://i.imgur.com/d8HAIs2.gif"
  alt="CrystalNim running on Linux">
	<br>
	<i>CrystalNim running on Linux</i>
</p>

## Motivation & Portability
From time to time I like to rewrite/rethink this game for different platforms
(such as
[terminal](https://github.com/Theldus/nanvix/blob/dev/src/ubin/nim/nim.c),
[PlayStation 2](https://github.com/Theldus/NIM), and
[16-bit bootable code](https://gist.github.com/Theldus/0979781720666c8facf489649d6074f8)),
it serves as a hobby for me, but I felt there was still something missing that
could be visually interesting and that could run on Web and Android, and then I
discovered [Raylib](https://www.raylib.com/).

Raylib is a library written in C for the creation of games, it supports multiple
programming languages and multiple platforms; abstracts everything from the
boring part and allows you to focus on the logic of what you want to create,
did I say that it uses OpenGL?

Therefore, when using raylib, we are able to support several platforms
(CrystalNim supports three: Linux, Web, and Android) at the same time that the 
platform-dependent code is minimal or even non-existent.

#### A Small Personal Note (do not take this too seriously):
I really don't like to develop Mobile or Web, it can be just me, but having to
use heavy IDEs, a NASA computer, and waiting several seconds to generate a
simple apk is quite unacceptable to me, not to mention the final file size,
performance, etc (how can people tolerate this?).

Libraries like raylib and
[rawdrawandroid](https://github.com/cnlohr/rawdrawandroid) that enable us to
write lightweight C code that could run on multiples platforms (8 for raylib,
and only one for rawdrawandroid (but worth mentioning, rawdraw is amazing too))
are a big favor to society, so a huge THANK YOU to their developers.

## Build instructions
Since CrystalNim supports multiple platforms, the build process differs between
them, especially with regard to the toolchain.

### Linux
The build for Linux is the most direct, does not require any extra library or
specific toolchain, just recursiverly clone the project and invoke make:
```bash
git clone --recursive https://github.com/Theldus/CrystalNim
cd CrystalNim
make            # or make PLATFORM=Linux
```
The desktop version of raylib will be compiled (if not already), as well as
CrystalNim.

### Web/HTML5
For the Web builds to work as expected, you need to first download the
Emscripten SDK to some folder of your choice and then compile CrystalNim for
Web.

#### Emscripten SDK Toolchain:
```bash
cd /foo/emsdk-toolchain
git clone https://github.com/emscripten-core/emsdk.git emsdk-git
cd emsdk-git/
./emsdk install latest
./emsdk activate latest
```

#### Build
Once you have downloaded and configured the Emscripten SDK (and you only need to
do it once), you just need to tell CrystalNim where to find the SDK, with the
`EMSDK_PATH` variable and say that you want a Web build, with the `PLATFORM`
variable:
```bash
# (I would recommend putting the line below in your ~/.bashrc)
export EMSDK_PATH=/foo/emsdk-toolchain/emsdk-git/
make PLATFORM=Web
```

### Android
Android is a little more... complicated... but it follows the same pattern:
toolchain setup followed by a CrystalNim build.

<details><summary>Expand Android instructions</summary>

#### 1) JavaSE JDK:
For the Android build, it is necessary to have JavaSE JDK installed, if not,
follow the instructions of your distribution, or follow the instructions below.
If you already have, go to 2).

```bash
cd /foo/JavaJDK/
wget https://download.java.net/java/GA/jdk16/7863447f0ab643c585b9bdebf67c69db/36/GPL/openjdk-16_linux-x64_bin.tar.gz
tar xf openjdk-16_linux-x64_bin.tar.gz

# Set env vars
export PATH=$PATH:$(pwd)/jdk-16/bin
export JAVA_HOME=$(pwd)/jdk-16

# Set them to your ~/.bashrc too
echo "export PATH=\$PATH:$(pwd)/" >> ~/.bashrc
echo "export JAVA_HOME=$(pwd)/jdk-16" >> ~/.bashrc
```

#### 2) Android SDK
- Download and extract the latest 'Command-line tools only' from
[Download Android Studio and SDK tools](https://developer.android.com/studio#command-tools)
(~83 MiB) or alternatively follow as below:
```bash
cd /foo/android-home/
wget https://dl.google.com/android/repository/commandlinetools-linux-6858069_latest.zip
unzip -q command*.zip
```
- sdkmanager expects a wrong order of cmdline-tools directories, so let's create
that order:
```bash
cd /foo/android-home/
mkdir cmdline-tools/latest
mv cmdline-tools/* cmdline-tools/latest/
```
(the error: `mv: cannot move 'cmdline-tools/latest' to a subdirectory of itself,
'cmdline-tools/latest/latest'` is expected)

- Configuring Android SDK:
```bash
cd /foo/android-home/
./cmdline-tools/latest/bin/sdkmanager --update
./cmdline-tools/latest/bin/sdkmanager --list
./cmdline-tools/latest/bin/sdkmanager --install "build-tools;29.0.3"
./cmdline-tools/latest/bin/sdkmanager --install "platform-tools"
./cmdline-tools/latest/bin/sdkmanager --install "platforms;android-29"
```
Please note that "29" is the Android API version, in this case, Android 10.
[Here](https://source.android.com/setup/start/build-numbers) is a comprehensive
list of Android API numbers.

- Configuring Android NDK: Download and extract the latest NDK from 
[NDK Downloads](https://developer.android.com/ndk/downloads/) (~1.10 GiB) or
alternatively follow as below:
```bash
cd /foo/android-home/
wget https://dl.google.com/android/repository/android-ndk-r21d-linux-x86_64.zip
unzip -q android-ndk*.zip
```

#### 3) Build
Now it's time to build CrystalNim for Android environments. First things first,
we should need to set some environment variables, in order to Raylib and
CrystalNim to properly recognize the tools in your system:
```bash
#
# Observations:
# 1) I highly recommend adding the following lines to your ~/.bashrc as well,
# for your convenience.
#
# 2) Adapt the paths according to those used by you, as well as the version
# of NDK and build-tools.
#
export ANDROID_HOME=/foo/android-home
export ANDROID_NDK=/foo/android-home/android-ndk-r21d/
export ANDROID_BUILD_TOOLS=$ANDROID_HOME/build-tools/29.0.3

# Now build CrystalNim
make PLATFORM=Android
```
#### Android notes:
- The instructions above assume the target device Android 10 and arm64, in case
you want another version of Android:
  - Follow the instructions above with the desired API version.
  - When compiling CrystalNim, set the `ANDROID_API_VERSION` variable to the one
you want, if you want Android 8 for example:
```bash
make PLATFORM=Android ANDROID_API_VERSION=26
```

- If you want to use another architecture (arm64 is used by default), just:
```bash
# (will do a build for ARM 32-bit, or more specifically, armeabi-v7a)
make ANDROID_ARCH=arm
```

- If you want to use another screen aspect ratio* (default 19:9), do:
```bash
#
# Valid values:
#
# - AR_16_9 for 16:9
# - AR_19_9 for 19:9
#
make DEVICE_ASPECT_RATIO=AR_16_9
```

\* The reasoning behind this limitation has to do with how the game was designed
and also at Raylib: when I started the game, I set a hardcoded resolution for
the game, instead of defining it as fullscreen (you know... I would have to
calculate all positions based on the current screen size, etc.). It turns out
that this limitation also bit my leg: Raylib does not correctly calculate the
screen coordinates when the aspect ratio of the screen differs from the set
resolution of the game; my workaround was, therefore, to match (horizontally)
the aspect ratio of the mobile screen resolution.

The limitation of all this is that you have to know the aspect ratio of the
screen beforehand. To support other aspect ratios, just modify include/scenes.h
and adjust `SCREEN_WIDTH` to something that reflects your device's screen, ie:
`SCREEN_HEIGHT * (A / R)`, if your device is 18:9, your `SCREEN_WIDTH` will be
`500 * (18/9) = 1000`.

Any help for a generic solution is highly appreciated =).

</details>

### General Notes
All supported platforms can be configured to use an out-of-the-tree Raylib
installation, with the variables `RAYLIB_INC` (folder containing the headers)
and `RAYLIB_LIB` (for the library itself).

Note that when using this option, you always need to specify `RAYLIB_LIB` for
the different platforms you are going to use, since this variable points to the
static library, not to its path. Therefore, the recommended approach is to let
the standard build be done _without_ defining these variables directly. Thus,
CrystalNim's makefiles can set the paths correctly without having to define
them all the time.

## Contributing
CrystalNim is always open to the community and willing to accept contributions,
whether with issues, documentation, testing, new features, bugfixes, typos, and
etc. Welcome aboard.

## License and Authors
CrystalNim is licensed under MIT License. Written by Davidson Francis and
(hopefully) other
[contributors](https://github.com/Theldus/CrystalNim/graphs/contributors).

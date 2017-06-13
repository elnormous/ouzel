<img src="https://github.com/elnormous/ouzel/blob/master/img/ouzel.png" alt="ouzel" width=128>

# Ouzel v0.35

[![Build Status](https://api.travis-ci.org/elnormous/ouzel.svg?branch=master)](https://travis-ci.org/elnormous/ouzel) [![Build Status](https://ci.appveyor.com/api/projects/status/dp8av7iegdjs6xuj?svg=true)](https://ci.appveyor.com/project/elnormous/ouzel)

Ouzel is a C++ game engine mainly targeted for development of 2D games.

Supported platforms:

* Windows 7, 8, 10 
* macOS 10.8+
* Linux
* iOS 8+
* tvOS 9+
* Android 3.0+
* Emscripten ([sample](http://www.ouzelengine.org/samples/))

Supported rendering backends:

* Direct3D 11
* OpenGL 2 and OpenGL 3
* OpenGL ES 2 and OpenGL ES 3
* Metal 

Supported audio backends:

* XAudio 2
* DirectSound
* OpenAL
* OpenSL ES

Windows Phone platform support is under development.

## Features

* Cross-platform (Windows, macOS, iOS, tvOS, Android, Linux, and Emscripten targets supported)
* Multi-threaded (separate threads for rendering, sound, and game)
* 2D and 3D scene management
* GUI helper classes and management
* Multiple side-by-side viewport support
* Bitmap font support
* XInput, DirectInput, IOKit, and Apple GameController gamepad support
* Node animation (including tweening) system
* Particle systems
* Resource caching system
* Localization support via loading string translations and UTF-8 string support
* Audio engine for sound effect playback
* High DPI support on Windows, macOS, and iOS
* Easy to install (just pull the repository and it's subrepositories and build it)

## Example app

The following code will open a 800px x 600px window:

    void ouzelMain(const std::vector<std::string>& args)
    {
        ouzel::Settings settings;
        settings.size = ouzel::Size2(800.0f, 600.0f);
        ouzel::sharedEngine->init(settings);
    }

## Showcase

2D platformer [Bearslayer](http://store.steampowered.com/app/460210) is being developed using Ouzel engine.

[![Bearslayer](https://github.com/elnormous/ouzel/blob/master/img/bearslayer.gif "Bear Slayer")](https://www.youtube.com/watch?v=q-O8-hpvJ5A)

## Compilation

Ouzel uses stb and rapidjson submodules. The following command has to be run after cloning the Ouzel engine:

```
$ git submodule update --init
```

or these commands if you have a really old version of git:

```
$ git submodule init
$ git submodule update
```

Makefiles and Xcode and Visual Studio project files are located in the "build" directory. Makefiles and project files for sample project are located in the "samples" directory.

You will need to download OpenGL (e.g. Mesa), OpenAL drivers, libxcursor, and libxss installed in order to build Ouzel on Linux. To build it for Raspbian pass "platform=raspbian" to "make" as follows:

```
$ make platform=raspbian
```

To build Ouzel with Emscripten, pass "platform=emscripten" to "make" command, but make sure that you have Emscripten SDK installed before doing so:

```
$ make platform=emscripten
```

You can build Android samples and run them on an Android device by executing the following commands in "samples/android" directory (Android SDK and NDK must be installed and added to PATH):

```
$ ant
$ adb shell am start -n org.ouzelengine/org.ouzelengine.MainActivity
```

## System requirements
* Windows 7+ with Visual Studio 2013 or Visual Studio 2015
* macOS 10.10+ with Xcode 7.2+
* Any reasonable new Linux distro (including Raspbian)

## Getting help

You can ask question in the following locations:

* Ouzel Twitter account: https://twitter.com/ouzelengine
* Ouzel server on Discord: https://discord.gg/4sWuJE8
* #ouzel channel on the Freenode network ([Freenode Webchat](http://webchat.freenode.net/?channels=ouzel))
* Author of the Ouzel engine: https://twitter.com/elnormous
* Development roadmap: https://trello.com/b/5tRlUXKR/ouzel-roadmap

## License

Ouzel codebase is licensed under the BSD license. Please refer to the LICENSE file for detailed information.

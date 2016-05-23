<img src="https://github.com/elvman/ouzel/blob/master/img/ouzel.png" alt="ouzel" width=128>

# Ouzel v0.17
Ouzel is a game engine mainly targeted for development of 2D games, but also 3D games can be created with it.

Supported platforms:

* Windows Vista, 7, 8, 10 
* OS X 10.8+
* Linux
* iOS 7+
* tvOS 9+
* Android 2.3+

Supported rendering backends:

* Direct3D 11
* OpenGL 2 and OpenGL 3
* OpenGL ES 2 and OpenGL ES 3
* Metal 

Windows Phone platform support is under development.

## Compilation

Ouzel uses rapidjson and stb submodules. The following commands have to be run after cloning Ouzel engine:

```
$ git submodule init
$ git submodule update
```

Linux and Android Makefiles and Xcode and Visual Studio project files are located in the build directory. Makefiles and project files for sample project are located in samples directory.

## System requirements
* Windows 7+ with Visual Studio 2013 or Visual Studio 2015
* OS X 10.8+ with Xcode 5.0+
* Any reasonable new Linux distro

## Example app

The following code will open a 640px x 480px window:

    ouzel::Engine engine;

    ouzel::AppPtr ouzelMain(const std::vector<std::string>& args)
    {
        ouzel::Settings settings;
        settings.size = ouzel::Size2(640.0f, 480.0f);
        engine.init(settings);
    }

## Showcase

2D platformer [Bear Slayer](http://steamcommunity.com/sharedfiles/filedetails/?id=624656569) is being developed using Ouzel engine.

[![Bear Slayer](https://github.com/elvman/ouzel/blob/master/img/bearslayer.gif "Bear Slayer")](https://www.youtube.com/watch?v=n-c-7E141kI)

## Features

* Cross-platform (Windows, OS X, iOS, tvOS, Android and Linux supported, and Windows Phone support under development)
* Scene management
* GUI helper classes and management
* Bitmap font support
* Xbox 360 gamepad support
* OS X, iOS and tvOS gamepad support
* Node animation (including tweening) system
* Particle systems
* Resource caching system
* Localization support via loading string translations

## Getting help

You can ask question in the following locations:

* Ouzel Twitter account: https://twitter.com/ouzelengine
* Author of the Ouzel engine: https://twitter.com/elvman
* Freenode chat: https://webchat.freenode.net/ #ouzel
* Development roadmap: https://trello.com/b/5tRlUXKR/ouzel-roadmap

## License

Ouzel codebase is licensed under the BSD license. Please refer to the LICENSE file for detailed information.
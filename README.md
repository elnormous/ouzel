<img src="https://github.com/elvman/ouzel/blob/master/img/ouzel.png" alt="ouzel" width=128>

# Ouzel v0.13
Ouzel is a game engine mainly targeted for development of 2D games, but also 3D games can be created with it.

## Supported platforms
Currently Windows 7+ with Direct3D11 backend and OS X 10.8, iOS 7+ and tvOS 9.0+ with OpenGL and Metal backends are supported. Android, Windows Phone and Linux platform support is under development.

## Compilation

Ouzel uses rapidjson and stb submodules. The following commands have to be run after cloning Ouzel engine:

```
$ git submodule init
$ git submodule update
```

Xcode and Visual Studio project files are located in the build directory.

## System requirements
* Windows 7+ with Visual Studio 2013 or Visual Studio 2015
* OS X 10.8+ with Xcode 5.0+

## Example app

The following code will open a 640px x 480px window:

    class Application: public ouzel::Noncopyable, public ouzel::App
    {        
        virtual void begin() override
        {
        }
    };

    ouzel::AppPtr ouzelMain(const std::vector<std::string>& args)
    {
        ouzel::Settings settings;
        settings.size = ouzel::Size2(640.0f, 480.0f);
        ouzel::sharedEngine->init(settings);
        ouzel::sharedEngine->setApp(std::make_shared<Application>());
    }

## Showcase

2D platformer [Bear Slayer](http://steamcommunity.com/sharedfiles/filedetails/?id=624656569) is being developed using Ouzel engine.

[![Bear Slayer](https://github.com/elvman/ouzel/blob/master/img/bearslayer.gif "Bear Slayer")](https://www.youtube.com/watch?v=n-c-7E141kI)

## Features

* Direct3D 11, Metal and OpenGL renderer support
* Cross-platform (Windows, OS X, iOS, tvOS supported, Android and Linux targets under development)
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

Ouzel codebase is licensed under BSD. Please refer to the LICENSE file for detailed information.
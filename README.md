<img src="https://github.com/elvman/ouzel/blob/master/logo/ouzel.png" alt="ouzel" width=128>

# Ouzel v0.6
Ouzel is a game engine mainly targeted for development of 2D games, but also 3D games can be created with it.

## Supported platforms
Currently Windows 7+ with Direct3D11 backend and OS X 10.8, iOS 7+ and tvOS 9.0+ with OpenGL backend are supported. Android, Windows Phone and Linux platform and Metal backend support is under development.

## Compilation

Xcode and Visual Studio project files are located in the build folder.

## System requirements
* Windows 7+ with Visual Studio 2013 or Visual Studio 2015
* OS X 10.8+ with Xcode 5.0+

## Example app

The following code will open a 640px x 480px window:

    class Application: public ouzel::Noncopyable, public ouzel::App
    {
        virtual ouzel::Settings getSettings() override
        {
            ouzel::Settings settings;
            settings.size = ouzel::Size2(640.0f, 480.0f);
        
            return settings;
        }
        
        virtual void begin() override
        {
        }
    };

    ouzel::AppPtr ouzelMain(std::vector<std::string> const& args)
    {
        std::shared_ptr<Application> application = std::make_shared<Application>();
    
        return application;
    }

## Features

* Direct3D and OpenGL renderer support
* Cross-platform (Windows, OS X, iOS, tvOS)
* Scene management
* GUI helper classes and management
* Bitmap font support
* MFi and Xbox 360 gamepad support
* OS X, iOS and tvOS gamepad support
* Node animation (including tweening) system
* Particle systems

## Getting help

You can ask question in the following locations:

* Ouzel Twitter account: https://twitter.com/ouzelengine
* Author of the Ouzel engine: https://twitter.com/elvman
* Freenode chat: https://webchat.freenode.net/ #ouzel
* Development roadmap: https://trello.com/b/5tRlUXKR/ouzel-roadmap

## License

Ouzel codebase is licensed under BSD. Please refer to the LICENSE file for detailed information.
// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#include <cstdlib>
#include <cstdint>
#include <algorithm>
#include "Application.h"
#include "utils/INI.h"
#include "utils/Log.h"
#include "utils/Utils.h"

namespace ouzel
{
    ouzel::Application* sharedApplication = nullptr;

    Application::Application():
        screenSaverEnabled(false)
    {
        sharedApplication = this;
    }

    Application::Application(int aArgc, char* aArgv[]):
        argc(aArgc), argv(aArgv)
    {
        sharedApplication = this;

        for (int i = 0; i < argc; ++i)
        {
            args.push_back(argv[i]);
        }
    }

    Application::Application(const std::vector<std::string>& aArgs):
        args(aArgs)
    {
        sharedApplication = this;
    }

    Application::~Application()
    {
        sharedApplication = nullptr;
    }

    bool Application::init()
    {
        INI settingsIni("settings.ini");
        Settings settings;

        std::string graphicsDriver = settingsIni.getValue("engine", "graphicsDriver");

        if (!graphicsDriver.empty())
        {
            if (graphicsDriver == "default")
            {
                settings.graphicsDriver = ouzel::graphics::Renderer::Driver::DEFAULT;
            }
            else if (graphicsDriver == "empty")
            {
                settings.graphicsDriver = ouzel::graphics::Renderer::Driver::EMPTY;
            }
            else if (graphicsDriver == "opengl")
            {
                settings.graphicsDriver = ouzel::graphics::Renderer::Driver::OPENGL;
            }
            else if (graphicsDriver == "direct3d11")
            {
                settings.graphicsDriver = ouzel::graphics::Renderer::Driver::DIRECT3D11;
            }
            else if (graphicsDriver == "metal")
            {
                settings.graphicsDriver = ouzel::graphics::Renderer::Driver::METAL;
            }
            else
            {
                ouzel::Log(ouzel::Log::Level::WARN) << "Invalid graphics driver specified";
                return false;
            }
        }

        std::string audioDriver = settingsIni.getValue("engine", "audioDriver");

        if (!audioDriver.empty())
        {
            if (audioDriver == "default")
            {
                settings.audioDriver = ouzel::audio::Audio::Driver::DEFAULT;
            }
            else if (audioDriver == "empty")
            {
                settings.audioDriver = ouzel::audio::Audio::Driver::EMPTY;
            }
            else if (audioDriver == "openal")
            {
                settings.audioDriver = ouzel::audio::Audio::Driver::OPENAL;
            }
            else if (audioDriver == "directsound")
            {
                settings.audioDriver = ouzel::audio::Audio::Driver::DIRECTSOUND;
            }
            else if (audioDriver == "xaudio2")
            {
                settings.audioDriver = ouzel::audio::Audio::Driver::XAUDIO2;
            }
            else if (audioDriver == "opensl")
            {
                settings.audioDriver = ouzel::audio::Audio::Driver::OPENSL;
            }
            else
            {
                ouzel::Log(ouzel::Log::Level::WARN) << "Invalid audio driver specified";
            }
        }

        std::string width = settingsIni.getValue("engine", "width");
        if (!width.empty()) settings.size.v[0] = stringToFloat(width);

        std::string height = settingsIni.getValue("engine", "height");
        if (!height.empty()) settings.size.v[1] = stringToFloat(height);

        std::string sampleCount = settingsIni.getValue("engine", "sampleCount");
        if (!sampleCount.empty()) settings.sampleCount = static_cast<uint32_t>(stringToLong(sampleCount));

        std::string textureFilter = settingsIni.getValue("engine", "textureFilter");
        if (!textureFilter.empty())
        {
            if (textureFilter == "point")
            {
                settings.textureFilter = ouzel::graphics::Texture::Filter::POINT;
            }
            else if (textureFilter == "linear")
            {
                settings.textureFilter = ouzel::graphics::Texture::Filter::LINEAR;
            }
            else if (textureFilter == "bilinear")
            {
                settings.textureFilter = ouzel::graphics::Texture::Filter::BILINEAR;
            }
            else if (textureFilter == "trilinear")
            {
                settings.textureFilter = ouzel::graphics::Texture::Filter::TRILINEAR;
            }
            else
            {
                ouzel::Log(ouzel::Log::Level::WARN) << "Invalid texture filter specified";
            }
        }

        std::string maxAnisotropy = settingsIni.getValue("engine", "maxAnisotropy");
        if (!maxAnisotropy.empty()) settings.maxAnisotropy = static_cast<uint32_t>(stringToLong(maxAnisotropy));

        std::string resizable = settingsIni.getValue("engine", "resizable");
        if (!resizable.empty()) settings.resizable = (resizable == "true" || resizable == "1" || resizable == "yes");

        std::string fullscreen = settingsIni.getValue("engine", "fullscreen");
        if (!fullscreen.empty()) settings.fullscreen = (fullscreen == "true" || fullscreen == "1" || fullscreen == "yes");

        std::string verticalSync = settingsIni.getValue("engine", "verticalSync");
        if (!verticalSync.empty()) settings.verticalSync = (verticalSync == "true" || verticalSync == "1" || verticalSync == "yes");

        std::string depth = settingsIni.getValue("engine", "depth");
        if (!depth.empty()) settings.depth = (depth == "true" || depth == "1" || depth == "yes");

        std::string debugRenderer = settingsIni.getValue("engine", "debugRenderer");
        if (!debugRenderer.empty()) settings.debugRenderer = (debugRenderer == "true" || debugRenderer == "1" || debugRenderer == "yes");

        std::string highDpi = settingsIni.getValue("engine", "highDpi");
        if (!highDpi.empty()) settings.highDpi = (highDpi == "true" || highDpi == "1" || highDpi == "yes");

        engine.init(settings);

        return true;
    }

    int Application::run()
    {
        return EXIT_SUCCESS;
    }

    void Application::exit()
    {
        active = false;

        if (sharedEngine)
        {
            sharedEngine->exit();
        }
    }

    bool Application::openURL(const std::string&)
    {
        return false;
    }

    void Application::setScreenSaverEnabled(bool newScreenSaverEnabled)
    {
        screenSaverEnabled = newScreenSaverEnabled;
    }
}

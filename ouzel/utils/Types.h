// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include <memory>

namespace ouzel
{
    class Localization;
    class Language;
    typedef std::shared_ptr<Language> LanguagePtr;

    namespace audio
    {
        class Audio;

        class Sound;
        typedef std::shared_ptr<Sound> SoundPtr;
    }

    namespace scene
    {
        class SceneManager;
        class Sprite;
        class Node;
        class TextDrawable;
    } // namespace scene
} // namespace ouzel

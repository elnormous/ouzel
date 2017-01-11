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

        class SoundData;
        typedef std::shared_ptr<SoundData> SoundDataPtr;
    }

    namespace graphics
    {
        class Renderer;

        class Resource;
        typedef std::shared_ptr<Resource> ResourcePtr;

        class BlendState;
        typedef std::shared_ptr<BlendState> BlendStatePtr;

        class IndexBuffer;
        typedef std::shared_ptr<IndexBuffer> IndexBufferPtr;

        class MeshBuffer;
        typedef std::shared_ptr<MeshBuffer> MeshBufferPtr;

        class Shader;
        typedef std::shared_ptr<Shader> ShaderPtr;

        class Texture;
        typedef std::shared_ptr<Texture> TexturePtr;

        class VertexBuffer;
        typedef std::shared_ptr<VertexBuffer> VertexBufferPtr;
    }

    namespace input
    {
        class Input;

        class Gamepad;
        typedef std::shared_ptr<Gamepad> GamepadPtr;
    }

    namespace scene
    {
        class SceneManager;
        class Sprite;
        class Node;
        class TextDrawable;
    } // namespace scene
} // namespace ouzel

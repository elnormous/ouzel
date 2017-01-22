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
        class BlendStateResource;
        typedef std::shared_ptr<BlendStateResource> BlendStateResourcePtr;

        class IndexBufferResource;
        typedef std::shared_ptr<IndexBufferResource> IndexBufferResourcePtr;

        class MeshBufferResource;
        typedef std::shared_ptr<MeshBufferResource> MeshBufferResourcePtr;

        class ShaderResource;
        typedef std::shared_ptr<ShaderResource> ShaderResourcePtr;

        class TextureResource;
        typedef std::shared_ptr<TextureResource> TextureResourcePtr;

        class VertexBufferResource;
        typedef std::shared_ptr<VertexBufferResource> VertexBufferResourcePtr;
    }

    namespace scene
    {
        class SceneManager;
        class Sprite;
        class Node;
        class TextDrawable;
    } // namespace scene
} // namespace ouzel

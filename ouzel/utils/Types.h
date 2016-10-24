// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include <cstdint>
#include <memory>

namespace ouzel
{
    class Window;
    class EventDispatcher;
    class Cache;
    class Localization;

    class Language;
    typedef std::shared_ptr<Language> LanguagePtr;

    class FileSystem;

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

        class RenderTarget;
        typedef std::shared_ptr<RenderTarget> RenderTargetPtr;

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
        class Scene;
        class Layer;
        class Component;
        class Sprite;
        class SpriteFrame;
        class Node;
        class Camera;
        class ParticleSystem;
        class ShapeDrawable;
        class TextDrawable;
        class Animator;
    } // namespace scene

    namespace gui
    {
        class Widget;
        class Menu;
        class Label;
        class Button;
        class CheckBox;
        class RadioButton;
        class ComboBox;
        class Popup;
        class EditBox;
        class ScrollArea;
        class ScrollBar;
    } // namespace gui
} // namespace ouzel

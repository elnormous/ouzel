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
        typedef Scene* ScenePtr;

        class Layer;
        typedef Layer* LayerPtr;

        class Component;
        typedef Component* ComponentPtr;

        class Sprite;
        typedef Sprite* SpritePtr;

        class SpriteFrame;
        typedef std::shared_ptr<SpriteFrame> SpriteFramePtr;

        class Node;
        typedef Node* NodePtr;
        typedef Node* NodeWeakPtr;

        class Camera;
        typedef Camera* CameraPtr;

        struct ParticleDefinition;
        typedef std::shared_ptr<ParticleDefinition> ParticleDefinitionPtr;

        class ParticleSystem;
        typedef ParticleSystem* ParticleSystemPtr;

        class ShapeDrawable;
        typedef ShapeDrawable* ShapeDrawablePtr;

        class TextDrawable;
        typedef TextDrawable* TextDrawablePtr;

        class Animator;
        typedef std::shared_ptr<Animator> AnimatorPtr;
    } // namespace scene

    namespace gui
    {
        class Widget;
        typedef Widget* WidgetPtr;

        class Menu;
        typedef Menu* MenuPtr;

        class Label;
        typedef Label* LabelPtr;

        class Button;
        typedef Button* ButtonPtr;

        class CheckBox;
        typedef CheckBox* CheckBoxPtr;

        class RadioButton;
        typedef RadioButton* RadioButtonPtr;

        class ComboBox;
        typedef ComboBox* ComboBoxPtr;

        class Popup;
        typedef Popup* PopupPtr;

        class EditBox;
        typedef EditBox* EditBoxPtr;

        class ScrollArea;
        typedef ScrollArea* ScrollAreaPtr;

        class ScrollBar;
        typedef ScrollBar* ScrollBarPtr;
    } // namespace gui
} // namespace ouzel

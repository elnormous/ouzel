// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include <cstdint>
#include <memory>

namespace ouzel
{
    typedef std::shared_ptr<void> VoidPtr;

    class UpdateCallback;
    typedef std::shared_ptr<UpdateCallback> UpdateCallbackPtr;

    class Window;
    typedef std::shared_ptr<Window> WindowPtr;

    class EventDispatcher;
    typedef std::shared_ptr<EventDispatcher> EventDispatcherPtr;

    class Cache;
    typedef std::shared_ptr<Cache> CachePtr;

    class Localization;
    typedef std::shared_ptr<Localization> LocalizationPtr;

    class Language;
    typedef std::shared_ptr<Language> LanguagePtr;

    namespace graphics
    {
        class Renderer;
        typedef std::shared_ptr<Renderer> RendererPtr;

        class BlendState;
        typedef std::shared_ptr<BlendState> BlendStatePtr;
        typedef std::weak_ptr<BlendState> BlendStateWeakPtr;

        class Texture;
        typedef std::shared_ptr<Texture> TexturePtr;
        typedef std::weak_ptr<Texture> TextureWeakPtr;

        class RenderTarget;
        typedef std::shared_ptr<RenderTarget> RenderTargetPtr;
        typedef std::weak_ptr<RenderTarget> RenderTargetWeakPtr;

        class Shader;
        typedef std::shared_ptr<Shader> ShaderPtr;
        typedef std::weak_ptr<Shader> ShaderWeakPtr;

        class MeshBuffer;
        typedef std::shared_ptr<MeshBuffer> MeshBufferPtr;
    }

    class FileSystem;
    typedef std::shared_ptr<FileSystem> FileSystemPtr;

    namespace input
    {
        class Input;
        typedef std::shared_ptr<Input> InputPtr;

        class Gamepad;
        typedef std::shared_ptr<Gamepad> GamepadPtr;
    }

    class EventHandler;
    typedef std::shared_ptr<EventHandler> EventHandlerPtr;

    struct Event;
    typedef std::shared_ptr<Event> EventPtr;
    struct KeyboardEvent;
    typedef std::shared_ptr<KeyboardEvent> KeyboardEventPtr;
    struct MouseEvent;
    typedef std::shared_ptr<MouseEvent> MouseEventPtr;
    struct TouchEvent;
    typedef std::shared_ptr<TouchEvent> TouchEventPtr;
    struct GamepadEvent;
    typedef std::shared_ptr<GamepadEvent> GamepadEventPtr;
    struct WindowEvent;
    typedef std::shared_ptr<WindowEvent> WindowEventPtr;
    struct SystemEvent;
    typedef std::shared_ptr<SystemEvent> SystemEventPtr;
    struct UIEvent;
    typedef std::shared_ptr<UIEvent> UIEventPtr;

    namespace scene
    {
        class SceneManager;
        typedef std::shared_ptr<SceneManager> SceneManagerPtr;

        class Scene;
        typedef std::shared_ptr<Scene> ScenePtr;
        typedef std::weak_ptr<Scene> SceneWeakPtr;

        class Layer;
        typedef std::shared_ptr<Layer> LayerPtr;
        typedef std::weak_ptr<Layer> LayerWeakPtr;

        class Drawable;
        typedef std::shared_ptr<Drawable> DrawablePtr;

        class Sprite;
        typedef std::shared_ptr<Sprite> SpritePtr;

        class NodeContainer;
        typedef std::shared_ptr<NodeContainer> NodeContainerPtr;
        typedef std::weak_ptr<NodeContainer> NodeContainerWeakPtr;

        class Node;
        typedef std::shared_ptr<Node> NodePtr;
        typedef std::weak_ptr<Node> NodeWeakPtr;

        class Camera;
        typedef std::shared_ptr<Camera> CameraPtr;

        struct ParticleDefinition;
        typedef std::shared_ptr<ParticleDefinition> ParticleDefinitionPtr;

        class ParticleSystem;
        typedef std::shared_ptr<ParticleSystem> ParticleSystemPtr;

        class DebugDrawable;
        typedef std::shared_ptr<DebugDrawable> DebugDrawablePtr;

        class Animator;
        typedef std::shared_ptr<Animator> AnimatorPtr;
        typedef std::weak_ptr<Animator> AnimatorWeakPtr;
    } // namespace scene

    namespace gui
    {
        class Widget;
        typedef std::shared_ptr<Widget> WidgetPtr;

        class Menu;
        typedef std::shared_ptr<Menu> MenuPtr;

        class Label;
        typedef std::shared_ptr<Label> LabelPtr;

        class Button;
        typedef std::shared_ptr<Button> ButtonPtr;

        class CheckBox;
        typedef std::shared_ptr<CheckBox> CheckBoxPtr;

        class RadioButton;
        typedef std::shared_ptr<RadioButton> RadioButtonPtr;

        class ComboBox;
        typedef std::shared_ptr<ComboBox> ComboBoxPtr;

        class Popup;
        typedef std::shared_ptr<Popup> PopupPtr;

        class EditBox;
        typedef std::shared_ptr<EditBox> EditBoxPtr;

        class ScrollArea;
        typedef std::shared_ptr<ScrollArea> ScrollAreaPtr;

        class ScrollBar;
        typedef std::shared_ptr<ScrollBar> ScrollBarPtr;
    } // namespace gui
} // namespace ouzel

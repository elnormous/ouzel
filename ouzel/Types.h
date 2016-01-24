// Copyright (C) 2015 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include <cstdint>
#include <memory>

namespace ouzel
{
    typedef std::shared_ptr<void> VoidPtr;
    
    class Engine;
    typedef std::shared_ptr<Engine> EnginePtr;
    
    class App;
    typedef std::shared_ptr<App> AppPtr;
    
    class EventDispatcher;
    typedef std::shared_ptr<EventDispatcher> EventDispatcherPtr;
    
    class Cache;
    typedef std::shared_ptr<Cache> CachePtr;
    
    class Renderer;
    typedef std::shared_ptr<Renderer> RendererPtr;
    
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
    
    class SceneManager;
    typedef std::shared_ptr<SceneManager> SceneManagerPtr;
    
    class FileSystem;
    typedef std::shared_ptr<FileSystem> FileSystemPtr;
    
    class Input;
    typedef std::shared_ptr<Input> InputPtr;
    
    class Gamepad;
    typedef std::shared_ptr<Gamepad> GamepadPtr;
    
    class EventHandler;
    typedef std::shared_ptr<EventHandler> EventHandlerPtr;
    
    class Scene;
    typedef std::shared_ptr<Scene> ScenePtr;
    typedef std::weak_ptr<Scene> SceneWeakPtr;
    
    class Layer;
    typedef std::shared_ptr<Layer> LayerPtr;
    typedef std::weak_ptr<Layer> LayerWeakPtr;
    
    class Sprite;
    typedef std::shared_ptr<Sprite> SpritePtr;
    
    class Node;
    typedef std::shared_ptr<Node> NodePtr;
    typedef std::weak_ptr<Node> NodeWeakPtr;
    
    class Camera;
    typedef std::shared_ptr<Camera> CameraPtr;
    
    class ParticleSystem;
    typedef std::shared_ptr<ParticleSystem> ParticleSystemPtr;
    
    class Widget;
    typedef std::shared_ptr<Widget> WidgetPtr;
    
    class Menu;
    typedef std::shared_ptr<Menu> MenuPtr;
    
    class Label;
    typedef std::shared_ptr<Label> LabelPtr;
    
    class Button;
    typedef std::shared_ptr<Button> ButtonPtr;
    
    class Animator;
    typedef std::shared_ptr<Animator> AnimatorPtr;
    typedef std::weak_ptr<Animator> AnimatorWeakPtr;
}

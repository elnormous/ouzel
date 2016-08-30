// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include <vector>
#include <string>
#include <queue>
#include <set>
#include <memory>
#include <mutex>
#include <atomic>
#include "utils/Types.h"
#include "utils/Noncopyable.h"
#include "math/Rectangle.h"
#include "math/Matrix4.h"
#include "math/Size2.h"
#include "graphics/Color.h"
#include "math/AABB2.h"
#include "graphics/Vertex.h"
#include "graphics/Shader.h"
#include "graphics/BlendState.h"

namespace ouzel
{
    class Engine;

    namespace graphics
    {
        const std::string SHADER_TEXTURE = "shaderTexture";
        const std::string SHADER_COLOR = "shaderColor";

        const std::string BLEND_NO_BLEND = "blendNoBlend";
        const std::string BLEND_ADD = "blendAdd";
        const std::string BLEND_MULTIPLY = "blendMultiply";
        const std::string BLEND_ALPHA = "blendAlpha";

        class MeshBuffer;

        class Renderer: public Noncopyable
        {
            friend Engine;
            friend Window;
        public:
            enum class Driver
            {
                DEFAULT,
                NONE,
                OPENGL,
                DIRECT3D11,
                METAL
            };

            enum class TextureFiltering
            {
                NONE,
                LINEAR,
                BILINEAR,
                TRILINEAR
            };

            enum class DrawMode
            {
                POINT_LIST,
                LINE_LIST,
                LINE_STRIP,
                TRIANGLE_LIST,
                TRIANGLE_STRIP
            };

            virtual ~Renderer();
            virtual void free();

            Driver getDriver() const { return driver; }

            virtual void setClearColor(Color color) { clearColor = color; }
            virtual Color getClearColor() const { return clearColor; }

            virtual bool present();

            const Size2& getSize() const { return size; }
            uint32_t getSampleCount() const { return sampleCount; }
            TextureFiltering getTextureFiltering() const { return textureFiltering; }

            virtual std::vector<Size2> getSupportedResolutions() const;

            virtual BlendStatePtr createBlendState();
            virtual TexturePtr createTexture();
            virtual RenderTargetPtr createRenderTarget();
            virtual ShaderPtr createShader();
            virtual MeshBufferPtr createMeshBuffer();

            bool getRefillDrawQueue() const { return refillDrawQueue; }
            bool addDrawCommand(const std::vector<TexturePtr>& textures,
                                const ShaderPtr& shader,
                                const std::vector<std::vector<float>>& pixelShaderConstants,
                                const std::vector<std::vector<float>>& vertexShaderConstants,
                                const BlendStatePtr& blendState,
                                const MeshBufferPtr& meshBuffer,
                                uint32_t indexCount = 0,
                                DrawMode drawMode = DrawMode::TRIANGLE_LIST,
                                uint32_t startIndex = 0,
                                const RenderTargetPtr& renderTarget = nullptr,
                                bool scissorTestEnabled = false,
                                const Rectangle& scissorTest = Rectangle());
            void flushDrawCommands();

            Vector2 viewToScreenLocation(const Vector2& position);
            Vector2 viewToScreenRelativeLocation(const Vector2& position);
            Vector2 screenToViewLocation(const Vector2& position);

            virtual bool saveScreenshot(const std::string& filename);

            virtual uint32_t getDrawCallCount() const { return drawCallCount; }

            uint32_t getAPIVersion() const { return apiVersion; }
            void setAPIVersion(uint32_t version) { apiVersion = version; }

            bool isReady() const { return ready; }

            void scheduleUpdate(const ResourcePtr& resource);

            virtual bool isNPOTTexturesSupported() const { return npotTexturesSupported; }

        protected:
            Renderer(Driver pDriver = Driver::NONE);
            virtual bool init(const WindowPtr& window,
                              uint32_t newSampleCount,
                              TextureFiltering newTextureFiltering,
                              bool newVerticalSync);

            virtual void setSize(const Size2& newSize);
            virtual void setFullscreen(bool newFullscreen);

            Driver driver;
            Size2 size;
            uint32_t sampleCount = 1; // MSAA sample count
            TextureFiltering textureFiltering = TextureFiltering::NONE;

            Color clearColor;
            uint32_t drawCallCount = 0;

            uint32_t apiVersion = 0;

            bool fullscreen = false;
            bool verticalSync = true;
            bool ready = false;

            struct DrawCommand
            {
                std::vector<TexturePtr> textures;
                ShaderPtr shader;
                std::vector<std::vector<float>> pixelShaderConstants;
                std::vector<std::vector<float>> vertexShaderConstants;
                BlendStatePtr blendState;
                MeshBufferPtr meshBuffer;
                uint32_t indexCount;
                DrawMode drawMode;
                uint32_t startIndex;
                RenderTargetPtr renderTarget;

                bool scissorTestEnabled;
                Rectangle scissorTest;
            };

            std::atomic<bool> activeDrawQueueFinished;
            std::atomic<bool> refillDrawQueue;

            std::vector<DrawCommand> activeDrawQueue;
            std::vector<DrawCommand> drawQueue;

            std::set<ResourcePtr> updateSet;
            std::vector<ResourcePtr> updateQueue;
            std::mutex updateMutex;

            std::queue<std::string> screenshotQueue;
            std::mutex screenshotMutex;

            bool npotTexturesSupported = true;
        };
    } // namespace graphics
} // namespace ouzel

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

        const std::string TEXTURE_WHITE_PIXEL = "textureWhitePixel";

        class MeshBuffer;

        class Renderer: public Noncopyable
        {
            friend Engine;
            friend Window;
        public:
            enum class Driver
            {
                DEFAULT,
                EMPTY,
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

            enum class PixelFormat
            {
                DEFAULT,
                R8_UNORM,
                R8_SNORM,
                R8_UINT,
                R8_SINT,
                R16_UNORM,
                R16_SNORM,
                R16_UINT,
                R16_SINT,
                R16_FLOAT,
                R32_UINT,
                R32_SINT,
                R32_FLOAT,
                R8G8_UNORM,
                R8G8_SNORM,
                R8G8_UINT,
                R8G8_SINT,
                R8G8B8_UNORM,
                R8G8B8_SNORM,
                R8G8B8_UINT,
                R8G8B8_SINT,
                R8G8B8A8_UNORM,
                R8G8B8A8_SNORM,
                R8G8B8A8_UINT,
                R8G8B8A8_SINT,
                R16G16B16A16_UNORM,
                R16G16B16A16_SNORM,
                R16G16B16A16_UINT,
                R16G16B16A16_SINT,
                R16G16B16A16_FLOAT,
                R32G32B16A32_UINT,
                R32G32B16A32_SINT,
                R32G32B16A32_FLOAT,
                R5G5B5A1_UNORM
            };

            virtual ~Renderer();
            virtual void free();

            Driver getDriver() const { return driver; }

            void setClear(bool newClear) { clear = newClear; }
            bool getClear() const { return clear; }

            virtual void setClearColor(Color color) { clearColor = color; }
            virtual Color getClearColor() const { return clearColor; }

            virtual bool present();

            const Size2& getSize() const { return size; }
            uint32_t getSampleCount() const { return sampleCount; }
            TextureFiltering getTextureFiltering() const { return textureFiltering; }

            virtual std::vector<Size2> getSupportedResolutions() const;

            virtual BlendStatePtr createBlendState() = 0;
            virtual TexturePtr createTexture() = 0;
            virtual RenderTargetPtr createRenderTarget() = 0;
            virtual ShaderPtr createShader() = 0;
            virtual MeshBufferPtr createMeshBuffer() = 0;
            virtual IndexBufferPtr createIndexBuffer() = 0;
            virtual VertexBufferPtr createVertexBuffer() = 0;

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
                                bool wireframe = false,
                                bool scissorTestEnabled = false,
                                const Rectangle& scissorTest = Rectangle());
            void flushDrawCommands();

            Vector2 viewToScreenLocation(const Vector2& position);
            Vector2 viewToScreenRelativeLocation(const Vector2& position);
            Vector2 screenToViewLocation(const Vector2& position);

            virtual bool saveScreenshot(const std::string& filename);

            virtual uint32_t getDrawCallCount() const { return drawCallCount; }

            uint16_t getAPIMajorVersion() const { return apiMajorVersion; }
            uint16_t getAPIMinorVersion() const { return apiMinorVersion; }

            void setAPIVersion(uint16_t majorVersion, uint16_t minorVersion)
            {
                apiMajorVersion = majorVersion;
                apiMinorVersion = minorVersion;
            }

            bool isReady() const { return ready; }

            void scheduleUpdate(const ResourcePtr& resource);

            bool isNPOTTexturesSupported() const { return npotTexturesSupported; }
            bool isRenderTargetsFlipped() const { return renderTargetsFlipped; }

        protected:
            Renderer(Driver pDriver);
            virtual bool init(const WindowPtr& window,
                              uint32_t newSampleCount,
                              TextureFiltering newTextureFiltering,
                              PixelFormat newBackBufferFormat,
                              bool newVerticalSync);

            virtual void setSize(const Size2& newSize);
            virtual void setFullscreen(bool newFullscreen);

            Driver driver;
            Size2 size;
            uint32_t sampleCount = 1; // MSAA sample count
            TextureFiltering textureFiltering = TextureFiltering::NONE;
            PixelFormat backBufferFormat;

            uint32_t currentFrame = 0;
            uint32_t frameBufferClearedFrame = 0;

            Color clearColor;
            uint32_t drawCallCount = 0;

            uint16_t apiMajorVersion = 0;
            uint16_t apiMinorVersion = 0;

            std::atomic<bool> clear;

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

                bool wireframe;
                bool scissorTestEnabled;
                Rectangle scissorTest;
            };

            std::atomic<bool> activeDrawQueueFinished;
            std::atomic<bool> refillDrawQueue;

            bool fullscreen = false;
            bool verticalSync = true;
            bool ready = false;
            bool npotTexturesSupported = true;
            bool renderTargetsFlipped = false;

            std::vector<DrawCommand> activeDrawQueue;
            std::vector<DrawCommand> drawQueue;

            std::set<ResourcePtr> updateSet;
            std::mutex updateMutex;

            std::queue<std::string> screenshotQueue;
            std::mutex screenshotMutex;
        };
    } // namespace graphics
} // namespace ouzel

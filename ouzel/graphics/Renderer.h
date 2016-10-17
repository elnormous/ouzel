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
#include "graphics/PixelFormat.h"
#include "graphics/TextureFilter.h"

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

            void setClear(bool newClear) { clear = newClear; }
            bool getClear() const { return clear; }

            virtual void setClearColor(Color color) { clearColor = color; }
            virtual Color getClearColor() const { return clearColor; }

            virtual bool present();

            const Size2& getSize() const { return size; }
            virtual void setSize(const Size2& newSize);
            uint32_t getSampleCount() const { return sampleCount; }
            TextureFilter getTextureFilter() const { return textureFilter; }

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
                                const Rectangle& viewport = Rectangle(0.0f, 0.0f, 1.0f, 1.0f),
                                bool wireframe = false,
                                bool scissorTestEnabled = false,
                                const Rectangle& scissorTest = Rectangle());
            void flushDrawCommands();

            Vector2 convertScreenToNormalizedLocation(const Vector2& position)
            {
                return Vector2(position.x / size.width,
                               1.0f - (position.y / size.height));
            }

            Vector2 convertNormalizedToScreenLocation(const Vector2& position)
            {
                return Vector2(position.x * size.width,
                               (1.0f - position.y) * size.height);
            }

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
            Renderer(Driver aDriver);
            virtual bool init(Window* newWindow,
                              uint32_t newSampleCount,
                              TextureFilter newTextureFilter,
                              PixelFormat newBackBufferFormat,
                              bool newVerticalSync);

            Driver driver;
            Window* window;
            Size2 size;
            uint32_t sampleCount = 1; // MSAA sample count
            TextureFilter textureFilter = TextureFilter::NONE;
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
                Rectangle viewport;
                bool wireframe;
                bool scissorTestEnabled;
                Rectangle scissorTest;
            };

            std::atomic<bool> activeDrawQueueFinished;
            std::atomic<bool> refillDrawQueue;

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

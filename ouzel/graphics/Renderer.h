// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include <cstdint>
#include <vector>
#include <string>
#include <queue>
#include <set>
#include <memory>
#include <mutex>
#include <atomic>
#include <condition_variable>
#include "utils/Types.h"
#include "utils/Noncopyable.h"
#include "math/Rectangle.h"
#include "math/Matrix4.h"
#include "math/Size2.h"
#include "math/Color.h"
#include "math/AABB2.h"
#include "graphics/Vertex.h"
#include "graphics/BlendState.h"
#include "graphics/MeshBuffer.h"
#include "graphics/PixelFormat.h"
#include "graphics/Shader.h"
#include "graphics/Texture.h"

namespace ouzel
{
    class Engine;
    class Window;

    namespace graphics
    {
        const std::string SHADER_TEXTURE = "shaderTexture";
        const std::string SHADER_COLOR = "shaderColor";

        const std::string BLEND_NO_BLEND = "blendNoBlend";
        const std::string BLEND_ADD = "blendAdd";
        const std::string BLEND_MULTIPLY = "blendMultiply";
        const std::string BLEND_ALPHA = "blendAlpha";

        const std::string TEXTURE_WHITE_PIXEL = "textureWhitePixel";

        class Resource;
        class BlendStateResource;
        class TextureResource;
        class ShaderResource;
        class MeshBufferResource;
        class BufferResource;

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

            Driver getDriver() const { return driver; }

            void setClearColorBuffer(bool clear);
            bool getClearColorBuffer() const { return clearColorBuffer; }

            void setClearDepthBuffer(bool clear);
            bool getClearDepthBuffer() const { return clearDepthBuffer; }

            void setClearColor(Color color);
            Color getClearColor() const { return clearColor; }

            virtual bool process();

            const Size2& getSize() const { return size; }
            uint32_t getSampleCount() const { return sampleCount; }
            Texture::Filter getTextureFilter() const { return textureFilter; }

            virtual std::vector<Size2> getSupportedResolutions() const;

            virtual BlendStateResource* createBlendState() = 0;
            virtual TextureResource* createTexture() = 0;
            virtual ShaderResource* createShader() = 0;
            virtual MeshBufferResource* createMeshBuffer() = 0;
            virtual BufferResource* createBuffer() = 0;
            virtual void uploadResource(Resource* resource);
            virtual void deleteResource(Resource* resource);

            bool getRefillDrawQueue() const { return refillDrawQueue; }
            bool addDrawCommand(const std::vector<std::shared_ptr<Texture>>& textures,
                                const std::shared_ptr<Shader>& shader,
                                const std::vector<std::vector<float>>& pixelShaderConstants,
                                const std::vector<std::vector<float>>& vertexShaderConstants,
                                const std::shared_ptr<BlendState>& blendState,
                                const std::shared_ptr<MeshBuffer>& meshBuffer,
                                uint32_t indexCount = 0,
                                DrawMode drawMode = DrawMode::TRIANGLE_LIST,
                                uint32_t startIndex = 0,
                                const std::shared_ptr<Texture>& renderTarget = nullptr,
                                const Rectangle& viewport = Rectangle(0.0f, 0.0f, 1.0f, 1.0f),
                                bool depthWrite = false,
                                bool depthTest = false,
                                bool wireframe = false,
                                bool scissorTestEnabled = false,
                                const Rectangle& scissorTest = Rectangle());
            void flushDrawCommands();

            Vector2 convertScreenToNormalizedLocation(const Vector2& position)
            {
                return Vector2(position.v[0] / size.v[0],
                               1.0f - (position.v[1] / size.v[1]));
            }

            Vector2 convertNormalizedToScreenLocation(const Vector2& position)
            {
                return Vector2(position.v[0] * size.v[0],
                               (1.0f - position.v[1]) * size.v[1]);
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

            bool isNPOTTexturesSupported() const { return npotTexturesSupported; }
            bool isMultisamplingSupported() const { return multisamplingSupported; }

            const Matrix4& getProjectionTransform(bool renderTarget) const
            {
                return renderTarget ? renderTargetProjectionTransform : projectionTransform;
            }

            float getFPS() const { return currentFPS; }
            float getAccumulatedFPS() const { return accumulatedFPS; }

        protected:
            Renderer(Driver aDriver);
            virtual bool init(Window* newWindow,
                              const Size2& newSize,
                              uint32_t newSampleCount,
                              Texture::Filter newTextureFilter,
                              PixelFormat newBackBufferFormat,
                              bool newVerticalSync,
                              bool newDepth);

            virtual void setSize(const Size2& newSize);
            
            struct DrawCommand
            {
                std::vector<TextureResource*> textures;
                ShaderResource* shader;
                std::vector<std::vector<float>> pixelShaderConstants;
                std::vector<std::vector<float>> vertexShaderConstants;
                BlendStateResource* blendState;
                MeshBufferResource* meshBuffer;
                uint32_t indexCount;
                DrawMode drawMode;
                uint32_t startIndex;
                TextureResource* renderTarget;
                Rectangle viewport;
                bool depthWrite;
                bool depthTest;
                bool wireframe;
                bool scissorTestEnabled;
                Rectangle scissorTest;
            };
            
            virtual bool draw(const std::vector<DrawCommand>& drawCommands) = 0;
            virtual bool update();
            virtual bool generateScreenshot(const std::string& filename);

            Driver driver;
            Window* window;

            uint16_t apiMajorVersion = 0;
            uint16_t apiMinorVersion = 0;

            uint32_t currentFrame = 0;
            uint32_t frameBufferClearedFrame = 0;
            uint32_t sampleCount = 1; // MSAA sample count
            Texture::Filter textureFilter = Texture::Filter::NONE;
            PixelFormat backBufferFormat;
            bool depth = false;

            bool verticalSync = true;

            bool npotTexturesSupported = true;
            bool multisamplingSupported = true;

            Matrix4 projectionTransform;
            Matrix4 renderTargetProjectionTransform;

            struct Data
            {
                Size2 size;
                Color clearColor;
                bool clearColorBuffer;
                bool clearDepthBuffer;
            };

            Data uploadData;

            std::mutex resourceMutex;
            std::vector<std::unique_ptr<Resource>> resources;
            std::set<Resource*> resourceUploadSet;
            std::vector<std::unique_ptr<Resource>> resourceDeleteSet;

        private:
            Size2 size;

            Color clearColor;
            uint32_t drawCallCount = 0;

            bool clearColorBuffer = true;
            bool clearDepthBuffer = false;

            std::vector<DrawCommand> drawQueue;
            std::mutex drawQueueMutex;
            std::condition_variable drawQueueCondition;
            bool drawQueueFinished = false;
            std::atomic<bool> refillDrawQueue;

            std::atomic<float> currentFPS;
            std::chrono::steady_clock::time_point previousFrameTime;

            float accumulatedTime = 0.0f;
            float currentAccumulatedFPS = 0.0f;
            std::atomic<float> accumulatedFPS;

            std::queue<std::string> screenshotQueue;
            std::mutex screenshotMutex;
            std::atomic<bool> dirty;
        };
    } // namespace graphics
} // namespace ouzel

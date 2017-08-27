// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include "utils/Noncopyable.hpp"
#include "graphics/Renderer.hpp"
#include "graphics/Vertex.hpp"
#include "graphics/Resource.hpp"
#include "graphics/BlendState.hpp"
#include "graphics/MeshBuffer.hpp"
#include "graphics/Shader.hpp"
#include "graphics/Texture.hpp"

namespace ouzel
{
    namespace graphics
    {
        class BlendStateResource;
        class BufferResource;
        class MeshBufferResource;
        class ShaderResource;
        class TextureResource;
        
        class RenderDevice: public Noncopyable
        {
            friend Renderer;
            friend BlendState;
            friend Buffer;
            friend MeshBuffer;
            friend Shader;
            friend Texture;
        public:
            virtual ~RenderDevice();

            Renderer::Driver getDriver() const { return driver; }

            virtual void setClearColorBuffer(bool clear);
            bool getClearColorBuffer() const { return clearColorBuffer; }

            virtual void setClearDepthBuffer(bool clear);
            bool getClearDepthBuffer() const { return clearDepthBuffer; }

            virtual void setClearColor(Color color);
            Color getClearColor() const { return clearColor; }

            virtual void setClearDepth(float newClearDepth);
            float getClearDepth() const { return clearDepth; }

            virtual bool process();

            const Size2& getSize() const { return size; }
            uint32_t getSampleCount() const { return sampleCount; }
            Texture::Filter getTextureFilter() const { return textureFilter; }
            uint32_t getMaxAnisotropy() const { return maxAnisotropy; }

            virtual std::vector<Size2> getSupportedResolutions() const;

            bool getRefillQueue() const { return refillQueue; }

            struct DrawCommand
            {
                std::vector<TextureResource*> textures;
                ShaderResource* shader;
                std::vector<std::vector<float>> pixelShaderConstants;
                std::vector<std::vector<float>> vertexShaderConstants;
                BlendStateResource* blendState;
                MeshBufferResource* meshBuffer;
                uint32_t indexCount;
                Renderer::DrawMode drawMode;
                uint32_t startIndex;
                TextureResource* renderTarget;
                Rectangle viewport;
                bool depthWrite;
                bool depthTest;
                bool wireframe;
                bool scissorTest;
                Rectangle scissorRectangle;
                Renderer::CullMode cullMode;
            };

            bool addDrawCommand(const DrawCommand& drawCommand);
            void flushCommands();

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

            uint32_t getDrawCallCount() const { return drawCallCount; }

            uint16_t getAPIMajorVersion() const { return apiMajorVersion; }
            uint16_t getAPIMinorVersion() const { return apiMinorVersion; }

            bool isNPOTTexturesSupported() const { return npotTexturesSupported; }
            bool isMultisamplingSupported() const { return multisamplingSupported; }
            bool isAnisotropicFilteringSupported() const { return anisotropicFilteringSupported; }
            bool isRenderTargetsSupported() const { return renderTargetsSupported; }

            const Matrix4& getProjectionTransform(bool renderTarget) const
            {
                return renderTarget ? renderTargetProjectionTransform : projectionTransform;
            }

            float getFPS() const { return currentFPS; }
            float getAccumulatedFPS() const { return accumulatedFPS; }
            
            void executeOnRenderThread(const std::function<void(void)>& func);

        protected:
            RenderDevice(Renderer::Driver aDriver);

            virtual bool init(Window* newWindow,
                              const Size2& newSize,
                              uint32_t newSampleCount,
                              Texture::Filter newTextureFilter,
                              uint32_t newMaxAnisotropy,
                              bool newVerticalSync,
                              bool newDepth,
                              bool newDebugRenderer);

            virtual void setSize(const Size2& newSize);

            virtual BlendStateResource* createBlendState() = 0;
            virtual TextureResource* createTexture() = 0;
            virtual ShaderResource* createShader() = 0;
            virtual MeshBufferResource* createMeshBuffer() = 0;
            virtual BufferResource* createBuffer() = 0;
            virtual void deleteResource(Resource* resource);

            virtual bool draw(const std::vector<DrawCommand>& drawCommands) = 0;
            virtual bool generateScreenshot(const std::string& filename);

            Renderer::Driver driver;

            Window* window = nullptr;

            uint16_t apiMajorVersion = 0;
            uint16_t apiMinorVersion = 0;

            uint32_t currentFrame = 0;
            uint32_t frameBufferClearedFrame = 0;
            uint32_t sampleCount = 1; // MSAA sample count
            Texture::Filter textureFilter = Texture::Filter::POINT;
            uint32_t maxAnisotropy = 1;

            bool verticalSync = true;
            bool depth = false;
            bool debugRenderer = false;

            bool npotTexturesSupported = true;
            bool multisamplingSupported = true;
            bool anisotropicFilteringSupported = true;
            bool renderTargetsSupported = true;

            Matrix4 projectionTransform;
            Matrix4 renderTargetProjectionTransform;

            Size2 size;
            Color clearColor;
            float clearDepth = 1.0;
            bool clearColorBuffer = true;
            bool clearDepthBuffer = false;

            std::mutex resourceMutex;
            std::vector<std::unique_ptr<Resource>> resources;
            std::vector<std::unique_ptr<Resource>> resourceDeleteSet;

        private:
            void executeAll();

            uint32_t drawCallCount = 0;

            std::vector<DrawCommand> drawQueue;
            std::mutex drawQueueMutex;
            std::condition_variable queueCondition;
            bool queueFinished = false;
            std::atomic<bool> refillQueue;

            std::atomic<float> currentFPS;
            std::chrono::steady_clock::time_point previousFrameTime;

            float accumulatedTime = 0.0f;
            float currentAccumulatedFPS = 0.0f;
            std::atomic<float> accumulatedFPS;
            
            std::queue<std::function<void(void)>> executeQueue;
            std::mutex executeMutex;
        };
    } // namespace graphics
} // namespace ouzel

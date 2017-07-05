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
#include "utils/Noncopyable.h"
#include "math/Rectangle.h"
#include "math/Matrix4.h"
#include "math/Size2.h"
#include "math/Color.h"
#include "graphics/Vertex.h"
#include "graphics/BlendState.h"
#include "graphics/MeshBuffer.h"
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
        class BufferResource;
        class MeshBufferResource;
        class ShaderResource;
        class TextureResource;

        class Renderer: public Noncopyable
        {
            friend Engine;
            friend Window;
            friend BlendState;
            friend Buffer;
            friend MeshBuffer;
            friend Shader;
            friend Texture;
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

            enum class CullMode
            {
                NONE,
                FRONT,
                BACK
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
            uint32_t getMaxAnisotropy() const { return maxAnisotropy; }

            virtual std::vector<Size2> getSupportedResolutions() const;

            bool getRefillDrawQueue() const { return refillDrawQueue; }
            bool addDrawCommand(const std::vector<std::shared_ptr<Texture>>& textures,
                                const std::shared_ptr<Shader>& shader,
                                const std::vector<std::vector<float>>& pixelShaderConstants,
                                const std::vector<std::vector<float>>& vertexShaderConstants,
                                const std::shared_ptr<BlendState>& blendState,
                                const std::shared_ptr<MeshBuffer>& meshBuffer,
                                uint32_t indexCount,
                                DrawMode drawMode,
                                uint32_t startIndex,
                                const std::shared_ptr<Texture>& renderTarget,
                                const Rectangle& viewport,
                                bool depthWrite,
                                bool depthTest,
                                bool wireframe,
                                bool scissorTest,
                                const Rectangle& scissorRectangle,
                                CullMode cullMode);
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

            bool saveScreenshot(const std::string& filename);

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

        protected:
            Renderer(Driver aDriver);
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
            virtual void uploadResource(Resource* resource);
            virtual void deleteResource(Resource* resource);

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
                bool scissorTest;
                Rectangle scissorRectangle;
                CullMode cullMode;
            };

            virtual bool draw(const std::vector<DrawCommand>& drawCommands) = 0;
            virtual bool upload();
            virtual bool generateScreenshot(const std::string& filename);

            Driver driver;
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

            bool dirty = false;
            Size2 size;
            Color clearColor;
            bool clearColorBuffer = true;
            bool clearDepthBuffer = false;
            std::mutex uploadMutex;

            std::mutex resourceMutex;
            std::vector<std::unique_ptr<Resource>> resources;
            std::set<Resource*> resourceUploadSet;
            std::vector<std::unique_ptr<Resource>> resourceDeleteSet;

        private:
            uint32_t drawCallCount = 0;

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
        };
    } // namespace graphics
} // namespace ouzel

// Copyright (C) 2018 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include "graphics/Renderer.hpp"
#include "graphics/Vertex.hpp"
#include "graphics/RenderResource.hpp"
#include "graphics/BlendState.hpp"
#include "graphics/Buffer.hpp"
#include "graphics/MeshBuffer.hpp"
#include "graphics/Shader.hpp"
#include "graphics/Texture.hpp"
#include "thread/Condition.hpp"
#include "thread/Mutex.hpp"

namespace ouzel
{
    namespace graphics
    {
        class RenderDevice
        {
            friend Renderer;
            friend BlendState;
            friend Buffer;
            friend MeshBuffer;
            friend Shader;
            friend Texture;
        public:
            virtual ~RenderDevice();

            RenderDevice(const RenderDevice&) = delete;
            RenderDevice& operator=(const RenderDevice&) = delete;

            RenderDevice(RenderDevice&&) = delete;
            RenderDevice& operator=(RenderDevice&&) = delete;

            inline Renderer::Driver getDriver() const { return driver; }

            virtual void setClearColorBuffer(bool clear);
            inline bool getClearColorBuffer() const { return clearColorBuffer; }

            virtual void setClearDepthBuffer(bool clear);
            inline bool getClearDepthBuffer() const { return clearDepthBuffer; }

            virtual void setClearColor(Color color);
            inline Color getClearColor() const { return clearColor; }

            virtual void setClearDepth(float newClearDepth);
            inline float getClearDepth() const { return clearDepth; }

            virtual bool process();

            inline const Size2& getSize() const { return size; }
            inline uint32_t getSampleCount() const { return sampleCount; }
            inline Texture::Filter getTextureFilter() const { return textureFilter; }
            inline uint32_t getMaxAnisotropy() const { return maxAnisotropy; }

            virtual std::vector<Size2> getSupportedResolutions() const;

            inline bool getRefillQueue() const { return refillQueue; }

            struct Command
            {
                enum Type
                {
                    SET_RENDER_TARGET,
                    CLEAR,
                    BLIT,
                    COMPUTE,
                    SET_CULL_MODE,
                    SET_FILL_MODE,
                    SET_SCISSOR_TEST,
                    SET_VIEWPORT,
                    SET_DEPTH_STATE,
                    SET_PIPELINE_STATE,
                    DRAW,
                    PUSH_DEBUG_MARKER,
                    POP_DEBUG_MARKER,
                    INIT_BLEND_STATE,
                    INIT_BUFFER,
                    SET_BUFFER_DATA,
                    INIT_MESH_BUFFER,
                    INIT_SHADER,
                    SET_SHADER_CONSTANTS,
                    INIT_TEXTURE,
                    SET_TEXTURE_DATA,
                    SET_TEXTURE_FLAGS,
                    SET_TEXTURES
                };

                Command(Type initType):
                    type(initType)
                {
                }

                Type type;
            };

            struct SetRenderTargetCommand: public Command
            {
                SetRenderTargetCommand(TextureResource* initRenderTarget):
                    Command(Command::Type::SET_RENDER_TARGET),
                    renderTarget(initRenderTarget)
                {
                }

                TextureResource* renderTarget;
            };

            struct ClearCommand: public Command
            {
                ClearCommand(TextureResource* initRenderTarget):
                    Command(Command::Type::CLEAR),
                    renderTarget(initRenderTarget)
                {
                }

                TextureResource* renderTarget;
            };

            // TODO: implement
            struct BlitCommand: public Command
            {
                BlitCommand(TextureResource* initSourceTexture,
                            TextureResource* initTargetTexture):
                    Command(Command::Type::BLIT),
                    sourceTexture(initSourceTexture),
                    targetTexture(initTargetTexture)
                {
                }

                TextureResource* sourceTexture;
                TextureResource* targetTexture;
            };

            // TODO: implement
            struct ComputeCommand: public Command
            {
                ComputeCommand(ShaderResource* initShader):
                    Command(Command::Type::COMPUTE),
                    shader(initShader)
                {
                }

                ShaderResource* shader;
            };

            struct SetCullModeCommad: public Command
            {
                SetCullModeCommad(Renderer::CullMode initCullMode):
                    Command(Command::Type::SET_CULL_MODE),
                    cullMode(initCullMode)
                {
                }

                Renderer::CullMode cullMode;
            };

            struct SetFillModeCommad: public Command
            {
                SetFillModeCommad(Renderer::FillMode initFillMode):
                    Command(Command::Type::SET_FILL_MODE),
                    fillMode(initFillMode)
                {
                }

                Renderer::FillMode fillMode;
            };

            struct SetScissorTestCommand: public Command
            {
                SetScissorTestCommand(bool initEnabled,
                                      const Rect& initRectangle):
                    Command(Command::Type::SET_SCISSOR_TEST),
                    enabled(initEnabled),
                    rectangle(initRectangle)
                {
                }

                bool enabled;
                Rect rectangle;
            };

            struct SetViewportCommand: public Command
            {
                SetViewportCommand(const Rect& initViewport):
                    Command(Command::Type::SET_VIEWPORT),
                    viewport(initViewport)
                {
                }

                Rect viewport;
            };

            struct SetDepthStateCommand: public Command
            {
                SetDepthStateCommand(bool initDepthTest,
                                     bool initDepthWrite):
                    Command(Command::Type::SET_DEPTH_STATE),
                    depthTest(initDepthTest),
                    depthWrite(initDepthWrite)
                {
                }

                bool depthTest;
                bool depthWrite;
            };

            struct SetPipelineStateCommand: public Command
            {
                SetPipelineStateCommand(BlendStateResource* initBlendState,
                                        ShaderResource* initShader):
                    Command(Command::Type::SET_PIPELINE_STATE),
                    blendState(initBlendState),
                    shader(initShader)
                {
                }

                BlendStateResource* blendState;
                ShaderResource* shader;
            };

            struct DrawCommand: public Command
            {
                DrawCommand(MeshBufferResource* initMeshBuffer,
                            uint32_t initIndexCount,
                            Renderer::DrawMode initDrawMode,
                            uint32_t initStartIndex):
                    Command(Command::Type::DRAW),
                    meshBuffer(initMeshBuffer),
                    indexCount(initIndexCount),
                    drawMode(initDrawMode),
                    startIndex(initStartIndex)
                {
                }

                MeshBufferResource* meshBuffer;
                uint32_t indexCount;
                Renderer::DrawMode drawMode;
                uint32_t startIndex;
            };

            struct PushDebugMarkerCommand: public Command
            {
                PushDebugMarkerCommand(const std::string& initName):
                    Command(Command::Type::PUSH_DEBUG_MARKER),
                    name(initName)
                {
                }

                std::string name;
            };

            struct PopDebugMarkerCommand: public Command
            {
                PopDebugMarkerCommand():
                    Command(Command::Type::POP_DEBUG_MARKER)
                {
                }
            };

            struct InitBlendStateCommand: public Command
            {
                InitBlendStateCommand(BlendStateResource* initBlendState,
                                      bool initEnableBlending,
                                      BlendState::Factor initColorBlendSource, BlendState::Factor initColorBlendDest,
                                      BlendState::Operation initColorOperation,
                                      BlendState::Factor initAlphaBlendSource, BlendState::Factor initAlphaBlendDest,
                                      BlendState::Operation initAlphaOperation,
                                      uint8_t initColorMask):
                    Command(Command::Type::INIT_BLEND_STATE),
                    blendState(initBlendState),
                    enableBlending(initEnableBlending),
                    colorBlendSource(initColorBlendSource),
                    colorBlendDest(initColorBlendDest),
                    colorOperation(initColorOperation),
                    alphaBlendSource(initAlphaBlendSource),
                    alphaBlendDest(initAlphaBlendDest),
                    alphaOperation(initAlphaOperation),
                    colorMask(initColorMask)
                {
                }

                BlendStateResource* blendState;
                bool enableBlending;
                BlendState::Factor colorBlendSource;
                BlendState::Factor colorBlendDest;
                BlendState::Operation colorOperation;
                BlendState::Factor alphaBlendSource;
                BlendState::Factor alphaBlendDest;
                BlendState::Operation alphaOperation;
                uint8_t colorMask;
            };

            // TODO: implement
            struct InitBufferCommand: public Command
            {
                InitBufferCommand():
                    Command(Command::Type::INIT_BUFFER)
                {
                }
            };

            // TODO: implement
            struct SetBufferDataCommand: public Command
            {
                SetBufferDataCommand():
                    Command(Command::Type::SET_BUFFER_DATA)
                {
                }
            };

            // TODO: implement
            struct InitMeshBufferCommand: public Command
            {
                InitMeshBufferCommand():
                    Command(Command::Type::INIT_MESH_BUFFER)
                {
                }
            };

            struct InitShaderCommand: public Command
            {
                InitShaderCommand(ShaderResource* initShader,
                                  const std::vector<uint8_t>& initPixelShader,
                                  const std::vector<uint8_t>& initVertexShader,
                                  const std::set<Vertex::Attribute::Usage>& initVertexAttributes,
                                  const std::vector<Shader::ConstantInfo>& initPixelShaderConstantInfo,
                                  const std::vector<Shader::ConstantInfo>& initVertexShaderConstantInfo,
                                  uint32_t initPixelShaderDataAlignment,
                                  uint32_t initVertexShaderDataAlignment,
                                  const std::string& initPixelShaderFunction,
                                  const std::string& initVertexShaderFunction):
                    Command(Command::Type::INIT_SHADER),
                    shader(initShader),
                    pixelShader(initPixelShader),
                    vertexShader(initVertexShader),
                    vertexAttributes(initVertexAttributes),
                    pixelShaderConstantInfo(initPixelShaderConstantInfo),
                    vertexShaderConstantInfo(initVertexShaderConstantInfo),
                    pixelShaderDataAlignment(initPixelShaderDataAlignment),
                    vertexShaderDataAlignment(initVertexShaderDataAlignment),
                    pixelShaderFunction(initPixelShaderFunction),
                    vertexShaderFunction(initVertexShaderFunction)
                {
                }

                ShaderResource* shader;
                std::vector<uint8_t> pixelShader;
                std::vector<uint8_t> vertexShader;
                std::set<Vertex::Attribute::Usage> vertexAttributes;
                std::vector<Shader::ConstantInfo> pixelShaderConstantInfo;
                std::vector<Shader::ConstantInfo> vertexShaderConstantInfo;
                uint32_t pixelShaderDataAlignment;
                uint32_t vertexShaderDataAlignment;
                std::string pixelShaderFunction;
                std::string vertexShaderFunction;
            };

            struct SetShaderConstantsCommand: public Command
            {
                SetShaderConstantsCommand(std::vector<std::vector<float>> initPixelShaderConstants,
                                          std::vector<std::vector<float>> initVertexShaderConstants):
                    Command(Command::Type::SET_SHADER_CONSTANTS),
                    pixelShaderConstants(initPixelShaderConstants),
                    vertexShaderConstants(initVertexShaderConstants)
                {
                }

                std::vector<std::vector<float>> pixelShaderConstants;
                std::vector<std::vector<float>> vertexShaderConstants;
            };

//            INIT_TEXTURE
//            SET_TEXTURE_DATA
//            SET_TEXTURE_FLAGS
//
            struct SetTexturesCommand: public Command
            {
                SetTexturesCommand(TextureResource* initTextures[Texture::LAYERS]):
                    Command(Command::Type::SET_TEXTURES)
                {
                    for (uint32_t i = 0; i < Texture::LAYERS; ++i)
                        textures[i] = initTextures[i];
                }

                TextureResource* textures[Texture::LAYERS];
            };

            bool addCommand(std::unique_ptr<Command>&& command);
            void flushCommands();

            Vector2 convertScreenToNormalizedLocation(const Vector2& position)
            {
                return Vector2(position.x / size.width,
                               1.0F - (position.y / size.height));
            }

            Vector2 convertNormalizedToScreenLocation(const Vector2& position)
            {
                return Vector2(position.x * size.width,
                               (1.0F - position.y) * size.height);
            }

            inline uint32_t getDrawCallCount() const { return drawCallCount; }

            inline uint16_t getAPIMajorVersion() const { return apiMajorVersion; }
            inline uint16_t getAPIMinorVersion() const { return apiMinorVersion; }

            inline bool isNPOTTexturesSupported() const { return npotTexturesSupported; }
            inline bool isMultisamplingSupported() const { return multisamplingSupported; }
            inline bool isAnisotropicFilteringSupported() const { return anisotropicFilteringSupported; }
            inline bool isRenderTargetsSupported() const { return renderTargetsSupported; }

            const Matrix4& getProjectionTransform(bool renderTarget) const
            {
                return renderTarget ? renderTargetProjectionTransform : projectionTransform;
            }

            inline float getFPS() const { return currentFPS; }
            inline float getAccumulatedFPS() const { return accumulatedFPS; }

            void executeOnRenderThread(const std::function<void(void)>& func);

        protected:
            explicit RenderDevice(Renderer::Driver initDriver);

            virtual bool init(Window* newWindow,
                              const Size2& newSize,
                              uint32_t newSampleCount,
                              Texture::Filter newTextureFilter,
                              uint32_t newMaxAnisotropy,
                              bool newVerticalSync,
                              bool newDepth,
                              bool newDebugRenderer);

            void executeAll();
            virtual void setSize(const Size2& newSize);

            virtual BlendStateResource* createBlendState() = 0;
            virtual TextureResource* createTexture() = 0;
            virtual ShaderResource* createShader() = 0;
            virtual MeshBufferResource* createMeshBuffer() = 0;
            virtual BufferResource* createBuffer() = 0;
            virtual void deleteResource(RenderResource* resource);

            virtual bool processCommands(const std::vector<std::unique_ptr<Command>>& commands) = 0;
            virtual bool generateScreenshot(const std::string& filename);

            Renderer::Driver driver;

            Window* window = nullptr;

            uint16_t apiMajorVersion = 0;
            uint16_t apiMinorVersion = 0;

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

            Mutex resourceMutex;
            std::vector<std::unique_ptr<RenderResource>> resources;
            std::vector<std::unique_ptr<RenderResource>> resourceDeleteSet;

            uint32_t drawCallCount = 0;

            std::vector<std::unique_ptr<Command>> commandBuffers[2];
            std::vector<std::unique_ptr<Command>>* fillBuffer = &commandBuffers[0];
            std::vector<std::unique_ptr<Command>>* renderBuffer = &commandBuffers[1];

            Mutex commandQueueMutex;
            Condition commandQueueCondition;

            bool queueFinished = false;
            std::atomic<bool> refillQueue;

            std::atomic<float> currentFPS;
            std::chrono::steady_clock::time_point previousFrameTime;

            float accumulatedTime = 0.0F;
            float currentAccumulatedFPS = 0.0F;
            std::atomic<float> accumulatedFPS;

            std::queue<std::function<void(void)>> executeQueue;
            Mutex executeMutex;
        };
    } // namespace graphics
} // namespace ouzel

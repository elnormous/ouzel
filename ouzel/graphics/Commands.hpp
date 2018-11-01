// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#pragma once

#include "graphics/BlendState.hpp"
#include "graphics/Buffer.hpp"
#include "graphics/RenderTarget.hpp"
#include "graphics/Shader.hpp"
#include "graphics/Texture.hpp"

namespace ouzel
{
    namespace graphics
    {
        class Command
        {
        public:
            enum Type
            {
                PRESENT,
                INIT_RENDER_TARGET,
                DELETE_RENDER_TARGET,
                SET_RENDER_TARGET_PARAMETERS,
                SET_RENDER_TARGET,
                CLEAR_RENDER_TARGET,
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
                DELETE_BLEND_STATE,
                INIT_BUFFER,
                DELETE_BUFFER,
                SET_BUFFER_DATA,
                INIT_SHADER,
                DELETE_SHADER,
                SET_SHADER_CONSTANTS,
                INIT_TEXTURE,
                DELETE_TEXTURE,
                SET_TEXTURE_DATA,
                SET_TEXTURE_PARAMETERS,
                SET_TEXTURES
            };

            explicit Command(Type initType):
                type(initType)
            {
            }

            virtual ~Command() {}

            const Type type;
        };

        class PresentCommand: public Command
        {
        public:
            PresentCommand(): Command(Command::Type::PRESENT) {}
        };

        class InitRenderTargetCommand: public Command
        {
        public:
            InitRenderTargetCommand(uint64_t initRenderTarget,
                                    uint32_t initWidth,
                                    uint32_t initHeight,
                                    uint32_t initSampleCount,
                                    PixelFormat initPixelFormat):
                Command(Command::Type::INIT_RENDER_TARGET),
                renderTarget(initRenderTarget),
                width(initWidth),
                height(initHeight),
                sampleCount(initSampleCount),
                pixelFormat(initPixelFormat)
            {
            }

            uint64_t renderTarget;
            uint32_t width;
            uint32_t height;
            uint32_t sampleCount;
            PixelFormat pixelFormat;
        };

        class DeleteRenderTargetCommand: public Command
        {
        public:
            explicit DeleteRenderTargetCommand(uint64_t initRenderTarget):
                Command(Command::Type::DELETE_RENDER_TARGET),
                renderTarget(initRenderTarget)
            {
            }

            uint64_t renderTarget;
        };

        class SetRenderTargetParametersCommand: public Command
        {
        public:
            SetRenderTargetParametersCommand(uint64_t initRenderTarget,
                                             bool initClearColorBuffer,
                                             bool initClearDepthBuffer,
                                             Color initClearColor,
                                             float initClearDepth):
                Command(Command::Type::SET_RENDER_TARGET_PARAMETERS),
                renderTarget(initRenderTarget),
                clearColorBuffer(initClearColorBuffer),
                clearDepthBuffer(initClearDepthBuffer),
                clearColor(initClearColor),
                clearDepth(initClearDepth)
            {
            }

            uint64_t renderTarget;
            bool clearColorBuffer;
            bool clearDepthBuffer;
            Color clearColor;
            float clearDepth;
        };

        class SetRenderTargetCommand: public Command
        {
        public:
            explicit SetRenderTargetCommand(uint64_t initRenderTarget):
                Command(Command::Type::SET_RENDER_TARGET),
                renderTarget(initRenderTarget)
            {
            }

            uint64_t renderTarget;
        };

        class ClearRenderTargetCommand: public Command
        {
        public:
            explicit ClearRenderTargetCommand(uint64_t initRenderTarget):
                Command(Command::Type::CLEAR_RENDER_TARGET),
                renderTarget(initRenderTarget)
            {
            }

            uint64_t renderTarget;
        };

        // TODO: implement
        class BlitCommand: public Command
        {
        public:
            BlitCommand(uint64_t initSourceTexture,
                        uint32_t initSourceLevel,
                        uint32_t initSourceX,
                        uint32_t initSourceY,
                        uint32_t initSourceWidth,
                        uint32_t initSourceHeight,
                        uint64_t initDestinationTexture,
                        uint32_t initDestinationLevel,
                        uint32_t initDestinationX,
                        uint32_t initDestinationY):
                Command(Command::Type::BLIT),
                sourceTexture(initSourceTexture),
                sourceLevel(initSourceLevel),
                sourceX(initSourceX),
                sourceY(initSourceY),
                sourceWidth(initSourceWidth),
                sourceHeight(initSourceHeight),
                destinationTexture(initDestinationTexture),
                destinationLevel(initDestinationLevel),
                destinationX(initDestinationX),
                destinationY(initDestinationY)
            {
            }

            uint64_t sourceTexture;
            uint32_t sourceLevel;
            uint32_t sourceX;
            uint32_t sourceY;
            uint32_t sourceWidth;
            uint32_t sourceHeight;
            uint64_t destinationTexture;
            uint32_t destinationLevel;
            uint32_t destinationX;
            uint32_t destinationY;
        };

        // TODO: implement
        class ComputeCommand: public Command
        {
        public:
            explicit ComputeCommand(uint64_t initShader):
                Command(Command::Type::COMPUTE),
                shader(initShader)
            {
            }

            uint64_t shader;
        };

        class SetCullModeCommad: public Command
        {
        public:
            explicit SetCullModeCommad(Renderer::CullMode initCullMode):
                Command(Command::Type::SET_CULL_MODE),
                cullMode(initCullMode)
            {
            }

            Renderer::CullMode cullMode;
        };

        class SetFillModeCommad: public Command
        {
        public:
            explicit SetFillModeCommad(Renderer::FillMode initFillMode):
                Command(Command::Type::SET_FILL_MODE),
                fillMode(initFillMode)
            {
            }

            Renderer::FillMode fillMode;
        };

        class SetScissorTestCommand: public Command
        {
        public:
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

        class SetViewportCommand: public Command
        {
        public:
            explicit SetViewportCommand(const Rect& initViewport):
                Command(Command::Type::SET_VIEWPORT),
                viewport(initViewport)
            {
            }

            Rect viewport;
        };

        class SetDepthStateCommand: public Command
        {
        public:
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

        class SetPipelineStateCommand: public Command
        {
        public:
            SetPipelineStateCommand(uint64_t initBlendState,
                                    uint64_t initShader):
                Command(Command::Type::SET_PIPELINE_STATE),
                blendState(initBlendState),
                shader(initShader)
            {
            }

            uint64_t blendState;
            uint64_t shader;
        };

        class DrawCommand: public Command
        {
        public:
            DrawCommand(uint64_t initIndexBuffer,
                        uint32_t initIndexCount,
                        uint32_t initIndexSize,
                        uint64_t initVertexBuffer,
                        Renderer::DrawMode initDrawMode,
                        uint32_t initStartIndex):
                Command(Command::Type::DRAW),
                indexBuffer(initIndexBuffer),
                indexCount(initIndexCount),
                indexSize(initIndexSize),
                vertexBuffer(initVertexBuffer),
                drawMode(initDrawMode),
                startIndex(initStartIndex)
            {
            }

            uint64_t indexBuffer;
            uint32_t indexCount;
            uint32_t indexSize;
            uint64_t vertexBuffer;
            Renderer::DrawMode drawMode;
            uint32_t startIndex;
        };

        class PushDebugMarkerCommand: public Command
        {
        public:
            explicit PushDebugMarkerCommand(const std::string& initName):
                Command(Command::Type::PUSH_DEBUG_MARKER),
                name(initName)
            {
            }

            std::string name;
        };

        class PopDebugMarkerCommand: public Command
        {
        public:
            PopDebugMarkerCommand():
                Command(Command::Type::POP_DEBUG_MARKER)
            {
            }
        };

        class InitBlendStateCommand: public Command
        {
        public:
            InitBlendStateCommand(uint64_t initBlendState,
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

            uint64_t blendState;
            bool enableBlending;
            BlendState::Factor colorBlendSource;
            BlendState::Factor colorBlendDest;
            BlendState::Operation colorOperation;
            BlendState::Factor alphaBlendSource;
            BlendState::Factor alphaBlendDest;
            BlendState::Operation alphaOperation;
            uint8_t colorMask;
        };

        class DeleteBlendStateCommand: public Command
        {
        public:
            DeleteBlendStateCommand(uint64_t initBlendState):
                Command(Command::Type::DELETE_BLEND_STATE),
                blendState(initBlendState)
            {
            }

            uint64_t blendState;
        };

        class InitBufferCommand: public Command
        {
        public:
            InitBufferCommand(uint64_t initBuffer,
                              Buffer::Usage initUsage,
                              uint32_t initFlags,
                              const std::vector<uint8_t>& initData,
                              uint32_t initSize):
                Command(Command::Type::INIT_BUFFER),
                buffer(initBuffer),
                usage(initUsage),
                flags(initFlags),
                data(initData),
                size(initSize)
            {
            }

            uint64_t buffer;
            Buffer::Usage usage;
            uint32_t flags;
            std::vector<uint8_t> data;
            uint32_t size;
        };

        class DeleteBufferCommand: public Command
        {
        public:
            DeleteBufferCommand(uint64_t initBuffer):
                Command(Command::Type::DELETE_BUFFER),
                buffer(initBuffer)
            {
            }

            uint64_t buffer;
        };

        class SetBufferDataCommand: public Command
        {
        public:
            SetBufferDataCommand(uint64_t initBuffer,
                                 const std::vector<uint8_t>& initData):
                Command(Command::Type::SET_BUFFER_DATA),
                buffer(initBuffer),
                data(initData)
            {
            }

            uint64_t buffer;
            std::vector<uint8_t> data;
        };

        class InitShaderCommand: public Command
        {
        public:
            InitShaderCommand(uint64_t initShader,
                              const std::vector<uint8_t>& initFragmentShader,
                              const std::vector<uint8_t>& initVertexShader,
                              const std::set<Vertex::Attribute::Usage>& initVertexAttributes,
                              const std::vector<Shader::ConstantInfo>& initFragmentShaderConstantInfo,
                              const std::vector<Shader::ConstantInfo>& initVertexShaderConstantInfo,
                              uint32_t initFragmentShaderDataAlignment,
                              uint32_t initVertexShaderDataAlignment,
                              const std::string& initFragmentShaderFunction,
                              const std::string& initVertexShaderFunction):
                Command(Command::Type::INIT_SHADER),
                shader(initShader),
                fragmentShader(initFragmentShader),
                vertexShader(initVertexShader),
                vertexAttributes(initVertexAttributes),
                fragmentShaderConstantInfo(initFragmentShaderConstantInfo),
                vertexShaderConstantInfo(initVertexShaderConstantInfo),
                fragmentShaderDataAlignment(initFragmentShaderDataAlignment),
                vertexShaderDataAlignment(initVertexShaderDataAlignment),
                fragmentShaderFunction(initFragmentShaderFunction),
                vertexShaderFunction(initVertexShaderFunction)
            {
            }

            uint64_t shader;
            std::vector<uint8_t> fragmentShader;
            std::vector<uint8_t> vertexShader;
            std::set<Vertex::Attribute::Usage> vertexAttributes;
            std::vector<Shader::ConstantInfo> fragmentShaderConstantInfo;
            std::vector<Shader::ConstantInfo> vertexShaderConstantInfo;
            uint32_t fragmentShaderDataAlignment;
            uint32_t vertexShaderDataAlignment;
            std::string fragmentShaderFunction;
            std::string vertexShaderFunction;
        };

        class DeleteShaderCommand: public Command
        {
        public:
            DeleteShaderCommand(uint64_t initShader):
                Command(Command::Type::DELETE_SHADER),
                shader(initShader)
            {
            }

            uint64_t shader;
        };

        class SetShaderConstantsCommand: public Command
        {
        public:
            SetShaderConstantsCommand(std::vector<std::vector<float>> initFragmentShaderConstants,
                                      std::vector<std::vector<float>> initVertexShaderConstants):
                Command(Command::Type::SET_SHADER_CONSTANTS),
                fragmentShaderConstants(initFragmentShaderConstants),
                vertexShaderConstants(initVertexShaderConstants)
            {
            }

            std::vector<std::vector<float>> fragmentShaderConstants;
            std::vector<std::vector<float>> vertexShaderConstants;
        };

        class InitTextureCommand: public Command
        {
        public:
            InitTextureCommand(uint64_t initTexture,
                               const std::vector<Texture::Level>& initLevels,
                               uint32_t initFlags,
                               uint32_t initSampleCount,
                               PixelFormat initPixelFormat):
                Command(Command::Type::INIT_TEXTURE),
                texture(initTexture),
                levels(initLevels),
                flags(initFlags),
                sampleCount(initSampleCount),
                pixelFormat(initPixelFormat)
            {
            }

            uint64_t texture;
            std::vector<Texture::Level> levels;
            uint32_t flags;
            uint32_t sampleCount;
            PixelFormat pixelFormat;
        };

        class DeleteTextureCommand: public Command
        {
        public:
            DeleteTextureCommand(uint64_t initTexture):
                Command(Command::Type::DELETE_TEXTURE),
                texture(initTexture)
            {
            }

            uint64_t texture;
        };

        class SetTextureDataCommand: public Command
        {
        public:
            SetTextureDataCommand(uint64_t initTexture,
                                  const std::vector<Texture::Level>& initLevels):
                Command(Command::Type::SET_TEXTURE_DATA),
                texture(initTexture),
                levels(initLevels)
            {
            }

            uint64_t texture;
            std::vector<Texture::Level> levels;
        };

        class SetTextureParametersCommand: public Command
        {
        public:
            SetTextureParametersCommand(uint64_t initTexture,
                                        Texture::Filter initFilter,
                                        Texture::Address initAddressX,
                                        Texture::Address initAddressY,
                                        uint32_t initMaxAnisotropy):
                Command(Command::Type::SET_TEXTURE_PARAMETERS),
                texture(initTexture),
                filter(initFilter),
                addressX(initAddressX),
                addressY(initAddressY),
                maxAnisotropy(initMaxAnisotropy)
            {
            }

            uint64_t texture;
            Texture::Filter filter;
            Texture::Address addressX;
            Texture::Address addressY;
            uint32_t maxAnisotropy;
        };

        class SetTexturesCommand: public Command
        {
        public:
            explicit SetTexturesCommand(uint64_t initTextures[Texture::LAYERS]):
                Command(Command::Type::SET_TEXTURES)
            {
                for (uint32_t i = 0; i < Texture::LAYERS; ++i)
                    textures[i] = initTextures[i];
            }

            uint64_t textures[Texture::LAYERS];
        };
    } // namespace graphics
} // namespace ouzel

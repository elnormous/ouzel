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
        class RenderResource;

        class Command
        {
        public:
            enum Type
            {
                PRESENT,
                INIT_RENDER_TARGET,
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
                INIT_BUFFER,
                SET_BUFFER_DATA,
                INIT_SHADER,
                SET_SHADER_CONSTANTS,
                INIT_TEXTURE,
                SET_TEXTURE_DATA,
                SET_TEXTURE_PARAMETERS,
                SET_TEXTURES
            };

            Command(Type initType):
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
            InitRenderTargetCommand(RenderResource* initRenderTarget,
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

            RenderResource* renderTarget;
            uint32_t width;
            uint32_t height;
            uint32_t sampleCount;
            PixelFormat pixelFormat;
        };

        class SetRenderTargetParametersCommand: public Command
        {
        public:
            SetRenderTargetParametersCommand(RenderResource* initRenderTarget,
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

            RenderResource* renderTarget;
            bool clearColorBuffer;
            bool clearDepthBuffer;
            Color clearColor;
            float clearDepth;
        };

        class SetRenderTargetCommand: public Command
        {
        public:
            SetRenderTargetCommand(RenderResource* initRenderTarget):
                Command(Command::Type::SET_RENDER_TARGET),
                renderTarget(initRenderTarget)
            {
            }

            RenderResource* renderTarget;
        };

        class ClearRenderTargetCommand: public Command
        {
        public:
            ClearRenderTargetCommand(RenderResource* initRenderTarget):
                Command(Command::Type::CLEAR_RENDER_TARGET),
                renderTarget(initRenderTarget)
            {
            }

            RenderResource* renderTarget;
        };

        // TODO: implement
        class BlitCommand: public Command
        {
        public:
            BlitCommand(RenderResource* initSourceTexture,
                        RenderResource* initDestinationTexture,
                        uint32_t initSourceX,
                        uint32_t initSourceY,
                        uint32_t initSourceWidth,
                        uint32_t initSourceHeight,
                        uint32_t initDestinationX,
                        uint32_t initDestinationY):
                Command(Command::Type::BLIT),
                sourceTexture(initSourceTexture),
                destinationTexture(initDestinationTexture),
                sourceX(initSourceX),
                sourceY(initSourceY),
                sourceWidth(initSourceWidth),
                sourceHeight(initSourceHeight),
                destinationX(initDestinationX),
                destinationY(initDestinationY)
            {
            }

            RenderResource* sourceTexture;
            RenderResource* destinationTexture;
            uint32_t sourceX;
            uint32_t sourceY;
            uint32_t sourceWidth;
            uint32_t sourceHeight;
            uint32_t destinationX;
            uint32_t destinationY;
        };

        // TODO: implement
        class ComputeCommand: public Command
        {
        public:
            ComputeCommand(RenderResource* initShader):
                Command(Command::Type::COMPUTE),
                shader(initShader)
            {
            }

            RenderResource* shader;
        };

        class SetCullModeCommad: public Command
        {
        public:
            SetCullModeCommad(Renderer::CullMode initCullMode):
                Command(Command::Type::SET_CULL_MODE),
                cullMode(initCullMode)
            {
            }

            Renderer::CullMode cullMode;
        };

        class SetFillModeCommad: public Command
        {
        public:
            SetFillModeCommad(Renderer::FillMode initFillMode):
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
            SetViewportCommand(const Rect& initViewport):
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
            SetPipelineStateCommand(RenderResource* initBlendState,
                                    RenderResource* initShader):
                Command(Command::Type::SET_PIPELINE_STATE),
                blendState(initBlendState),
                shader(initShader)
            {
            }

            RenderResource* blendState;
            RenderResource* shader;
        };

        class DrawCommand: public Command
        {
        public:
            DrawCommand(RenderResource* initIndexBuffer,
                        uint32_t initIndexCount,
                        uint32_t initIndexSize,
                        RenderResource* initVertexBuffer,
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

            RenderResource* indexBuffer;
            uint32_t indexCount;
            uint32_t indexSize;
            RenderResource* vertexBuffer;
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
            InitBlendStateCommand(RenderResource* initBlendState,
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

            RenderResource* blendState;
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
        class InitBufferCommand: public Command
        {
        public:
            InitBufferCommand(RenderResource* initBuffer,
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

            RenderResource* buffer;
            Buffer::Usage usage;
            uint32_t flags;
            std::vector<uint8_t> data;
            uint32_t size;
        };

        // TODO: implement
        class SetBufferDataCommand: public Command
        {
        public:
            SetBufferDataCommand(RenderResource* initBuffer,
                                 const std::vector<uint8_t>& initData):
                Command(Command::Type::SET_BUFFER_DATA),
                buffer(initBuffer),
                data(initData)
            {
            }

            RenderResource* buffer;
            std::vector<uint8_t> data;
        };

        class InitShaderCommand: public Command
        {
        public:
            InitShaderCommand(RenderResource* initShader,
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

            RenderResource* shader;
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
            InitTextureCommand(RenderResource* initTexture,
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

            RenderResource* texture;
            std::vector<Texture::Level> levels;
            uint32_t flags;
            uint32_t sampleCount;
            PixelFormat pixelFormat;
        };

        class SetTextureDataCommand: public Command
        {
        public:
            SetTextureDataCommand(RenderResource* initTexture,
                                  const std::vector<Texture::Level>& initLevels):
                Command(Command::Type::SET_TEXTURE_DATA),
                texture(initTexture),
                levels(initLevels)
            {
            }

            RenderResource* texture;
            std::vector<Texture::Level> levels;
        };

        class SetTextureParametersCommand: public Command
        {
        public:
            SetTextureParametersCommand(RenderResource* initTexture,
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

            RenderResource* texture;
            Texture::Filter filter;
            Texture::Address addressX;
            Texture::Address addressY;
            uint32_t maxAnisotropy;
        };

        class SetTexturesCommand: public Command
        {
        public:
            SetTexturesCommand(RenderResource* initTextures[Texture::LAYERS]):
                Command(Command::Type::SET_TEXTURES)
            {
                for (uint32_t i = 0; i < Texture::LAYERS; ++i)
                    textures[i] = initTextures[i];
            }

            RenderResource* textures[Texture::LAYERS];
        };
    } // namespace graphics
} // namespace ouzel

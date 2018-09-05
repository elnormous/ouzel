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
        struct Command
        {
            enum Type
            {
                INIT_RENDER_TARGET,
                SET_RENDER_TARGET_PARAMETERS,
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

        struct InitRenderTargetCommand: public Command
        {
            InitRenderTargetCommand(RenderTarget* initRenderTarget,
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

            RenderTarget* renderTarget;
            uint32_t width;
            uint32_t height;
            uint32_t sampleCount;
            PixelFormat pixelFormat;
        };

        struct SetRenderTargetParametersCommand: public Command
        {
            SetRenderTargetParametersCommand(RenderTarget* initRenderTarget,
                                             uint32_t initWidth,
                                             uint32_t initHeight,
                                             uint32_t initSampleCount,
                                             PixelFormat initPixelFormat):
                Command(Command::Type::SET_RENDER_TARGET_PARAMETERS),
                renderTarget(initRenderTarget),
                width(initWidth),
                height(initHeight),
                sampleCount(initSampleCount),
                pixelFormat(initPixelFormat)
            {
            }

            RenderTarget* renderTarget;
            uint32_t width;
            uint32_t height;
            uint32_t sampleCount;
            PixelFormat pixelFormat;
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
            DrawCommand(BufferResource* initIndexBuffer,
                        uint32_t initIndexCount,
                        uint32_t initIndexSize,
                        BufferResource* initVertexBuffer,
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

            BufferResource* indexBuffer;
            uint32_t indexCount;
            uint32_t indexSize;
            BufferResource* vertexBuffer;
            Renderer::DrawMode drawMode;
            uint32_t startIndex;
        };

        struct PushDebugMarkerCommand: public Command
        {
            explicit PushDebugMarkerCommand(const std::string& initName):
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
            InitBufferCommand(BufferResource* initBuffer,
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

            BufferResource* buffer;
            Buffer::Usage usage;
            uint32_t flags;
            std::vector<uint8_t> data;
            uint32_t size;
        };

        // TODO: implement
        struct SetBufferDataCommand: public Command
        {
            SetBufferDataCommand(BufferResource* initBuffer,
                                 const std::vector<uint8_t>& initData):
                Command(Command::Type::SET_BUFFER_DATA),
                buffer(initBuffer),
                data(initData)
            {
            }

            BufferResource* buffer;
            std::vector<uint8_t> data;
        };

        struct InitShaderCommand: public Command
        {
            InitShaderCommand(ShaderResource* initShader,
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

            ShaderResource* shader;
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

        struct SetShaderConstantsCommand: public Command
        {
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

        struct InitTextureCommand: public Command
        {
            InitTextureCommand(TextureResource* initTexture,
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

            TextureResource* texture;
            std::vector<Texture::Level> levels;
            uint32_t flags;
            uint32_t sampleCount;
            PixelFormat pixelFormat;
        };

        struct SetTextureDataCommand: public Command
        {
            SetTextureDataCommand(TextureResource* initTexture,
                                  const std::vector<Texture::Level>& initLevels):
                Command(Command::Type::SET_TEXTURE_DATA),
                texture(initTexture),
                levels(initLevels)
            {
            }

            TextureResource* texture;
            std::vector<Texture::Level> levels;
        };

        struct SetTextureParametersCommand: public Command
        {
            SetTextureParametersCommand(TextureResource* initTexture,
                                        Texture::Filter initFilter,
                                        Texture::Address initAddressX,
                                        Texture::Address initAddressY,
                                        uint32_t initMaxAnisotropy,
                                        bool initClearColorBuffer,
                                        bool initClearDepthBuffer,
                                        Color initClearColor,
                                        float initClearDepth):
                Command(Command::Type::SET_TEXTURE_PARAMETERS),
                texture(initTexture),
                filter(initFilter),
                addressX(initAddressX),
                addressY(initAddressY),
                maxAnisotropy(initMaxAnisotropy),
                clearColorBuffer(initClearColorBuffer),
                clearDepthBuffer(initClearDepthBuffer),
                clearColor(initClearColor),
                clearDepth(initClearDepth)
            {
            }

            TextureResource* texture;
            Texture::Filter filter;
            Texture::Address addressX;
            Texture::Address addressY;
            uint32_t maxAnisotropy;
            bool clearColorBuffer;
            bool clearDepthBuffer;
            Color clearColor;
            float clearDepth;
        };

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
    } // namespace graphics
} // namespace ouzel

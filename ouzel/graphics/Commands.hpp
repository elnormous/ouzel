// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_GRAPHICS_COMMANDS_HPP
#define OUZEL_GRAPHICS_COMMANDS_HPP

#include <queue>
#include <string>
#include "graphics/BlendState.hpp"
#include "graphics/Buffer.hpp"
#include "graphics/DepthStencilState.hpp"
#include "graphics/DrawMode.hpp"
#include "graphics/RasterizerState.hpp"
#include "graphics/RenderTarget.hpp"
#include "graphics/Shader.hpp"
#include "graphics/Texture.hpp"
#include "math/Rect.hpp"

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
                DELETE_RESOURCE,
                INIT_RENDER_TARGET,
                ADD_RENDER_TARGET_COLOR_TEXTURE,
                REMOVE_RENDER_TARGET_COLOR_TEXTURE,
                SET_RENDER_TARGET_DEPTH_TEXTURE,
                SET_RENDER_TARGET,
                CLEAR_RENDER_TARGET,
                BLIT,
                COMPUTE,
                SET_CULL_MODE,
                SET_FILL_MODE,
                SET_SCISSOR_TEST,
                SET_VIEWPORT,
                INIT_DEPTH_STENCIL_STATE,
                SET_DEPTH_STENCIL_STATE,
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

        class DeleteResourceCommand: public Command
        {
        public:
            DeleteResourceCommand(uintptr_t initResource):
                Command(Command::Type::DELETE_RESOURCE),
                resource(initResource)
            {}

            uintptr_t resource;
        };

        class InitRenderTargetCommand: public Command
        {
        public:
            explicit InitRenderTargetCommand(uintptr_t initRenderTarget):
                Command(Command::Type::INIT_RENDER_TARGET),
                renderTarget(initRenderTarget)
            {
            }

            uintptr_t renderTarget;
        };

        class AddRenderTargetColorTextureCommand: public Command
        {
        public:
            AddRenderTargetColorTextureCommand(uintptr_t initRenderTarget,
                                               uintptr_t initTexture):
                Command(Command::Type::ADD_RENDER_TARGET_COLOR_TEXTURE),
                renderTarget(initRenderTarget),
                texture(initTexture)
            {
            }

            uintptr_t renderTarget;
            uintptr_t texture;
        };

        class RemoveRenderTargetColorTextureCommand: public Command
        {
        public:
            RemoveRenderTargetColorTextureCommand(uintptr_t initRenderTarget,
                                                  uintptr_t initTexture):
            Command(Command::Type::REMOVE_RENDER_TARGET_COLOR_TEXTURE),
            renderTarget(initRenderTarget),
            texture(initTexture)
            {
            }

            uintptr_t renderTarget;
            uintptr_t texture;
        };

        class SetRenderTargetDepthTextureCommand: public Command
        {
        public:
            SetRenderTargetDepthTextureCommand(uintptr_t initRenderTarget,
                                               uintptr_t initTexture):
                Command(Command::Type::SET_RENDER_TARGET_DEPTH_TEXTURE),
                renderTarget(initRenderTarget),
                texture(initTexture)
            {
            }

            uintptr_t renderTarget;
            uintptr_t texture;
        };

        class SetRenderTargetCommand: public Command
        {
        public:
            explicit SetRenderTargetCommand(uintptr_t initRenderTarget):
                Command(Command::Type::SET_RENDER_TARGET),
                renderTarget(initRenderTarget)
            {
            }

            uintptr_t renderTarget;
        };

        class ClearRenderTargetCommand: public Command
        {
        public:
            explicit ClearRenderTargetCommand(bool initClearColorBuffer,
                                              bool initClearDepthBuffer,
                                              bool initClearStencilBuffer,
                                              Color initClearColor,
                                              float initClearDepth,
                                              uint32_t initClearStencil):
                Command(Command::Type::CLEAR_RENDER_TARGET),
                clearColorBuffer(initClearColorBuffer),
                clearDepthBuffer(initClearDepthBuffer),
                clearStencilBuffer(initClearStencilBuffer),
                clearColor(initClearColor),
                clearDepth(initClearDepth),
                clearStencil(initClearStencil)
            {
            }

            bool clearColorBuffer;
            bool clearDepthBuffer;
            bool clearStencilBuffer;
            Color clearColor;
            float clearDepth;
            uint32_t clearStencil;
        };

        // TODO: implement
        class BlitCommand: public Command
        {
        public:
            BlitCommand(uintptr_t initSourceTexture,
                        uint32_t initSourceLevel,
                        uint32_t initSourceX,
                        uint32_t initSourceY,
                        uint32_t initSourceWidth,
                        uint32_t initSourceHeight,
                        uintptr_t initDestinationTexture,
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

            uintptr_t sourceTexture;
            uint32_t sourceLevel;
            uint32_t sourceX;
            uint32_t sourceY;
            uint32_t sourceWidth;
            uint32_t sourceHeight;
            uintptr_t destinationTexture;
            uint32_t destinationLevel;
            uint32_t destinationX;
            uint32_t destinationY;
        };

        // TODO: implement
        class ComputeCommand: public Command
        {
        public:
            explicit ComputeCommand(uintptr_t initShader):
                Command(Command::Type::COMPUTE),
                shader(initShader)
            {
            }

            uintptr_t shader;
        };

        class SetCullModeCommad: public Command
        {
        public:
            explicit SetCullModeCommad(CullMode initCullMode):
                Command(Command::Type::SET_CULL_MODE),
                cullMode(initCullMode)
            {
            }

            CullMode cullMode;
        };

        class SetFillModeCommad: public Command
        {
        public:
            explicit SetFillModeCommad(FillMode initFillMode):
                Command(Command::Type::SET_FILL_MODE),
                fillMode(initFillMode)
            {
            }

            FillMode fillMode;
        };

        class SetScissorTestCommand: public Command
        {
        public:
            SetScissorTestCommand(bool initEnabled,
                                  const Rect<float>& initRectangle):
                Command(Command::Type::SET_SCISSOR_TEST),
                enabled(initEnabled),
                rectangle(initRectangle)
            {
            }

            bool enabled;
            Rect<float> rectangle;
        };

        class SetViewportCommand: public Command
        {
        public:
            explicit SetViewportCommand(const Rect<float>& initViewport):
                Command(Command::Type::SET_VIEWPORT),
                viewport(initViewport)
            {
            }

            Rect<float> viewport;
        };

        class InitDepthStencilStateCommand: public Command
        {
        public:
            InitDepthStencilStateCommand(uintptr_t initDepthStencilState,
                                         bool initDepthTest,
                                         bool initDepthWrite,
                                         DepthStencilState::CompareFunction initCompareFunction,
                                         bool initStencilEnabled,
                                         uint32_t initStencilReadMask,
                                         uint32_t initStencilWriteMask,
                                         DepthStencilState::StencilDescriptor initFrontFaceStencil,
                                         DepthStencilState::StencilDescriptor initBackFaceStencil):
                Command(Command::Type::INIT_DEPTH_STENCIL_STATE),
                depthStencilState(initDepthStencilState),
                depthTest(initDepthTest),
                depthWrite(initDepthWrite),
                compareFunction(initCompareFunction),
                stencilEnabled(initStencilEnabled),
                stencilReadMask(initStencilReadMask),
                stencilWriteMask(initStencilWriteMask),
                frontFaceStencil(initFrontFaceStencil),
                backFaceStencil(initBackFaceStencil)
            {
            }

            uintptr_t depthStencilState;
            bool depthTest;
            bool depthWrite;
            DepthStencilState::CompareFunction compareFunction;
            bool stencilEnabled;
            uint32_t stencilReadMask;
            uint32_t stencilWriteMask;
            DepthStencilState::StencilDescriptor frontFaceStencil;
            DepthStencilState::StencilDescriptor backFaceStencil;
        };

        class SetDepthStencilStateCommand: public Command
        {
        public:
            SetDepthStencilStateCommand(uintptr_t initDepthStencilState):
                Command(Command::Type::SET_DEPTH_STENCIL_STATE),
                depthStencilState(initDepthStencilState)
            {
            }

            uintptr_t depthStencilState;
        };

        class SetPipelineStateCommand: public Command
        {
        public:
            SetPipelineStateCommand(uintptr_t initBlendState,
                                    uintptr_t initShader):
                Command(Command::Type::SET_PIPELINE_STATE),
                blendState(initBlendState),
                shader(initShader)
            {
            }

            uintptr_t blendState;
            uintptr_t shader;
        };

        class DrawCommand: public Command
        {
        public:
            DrawCommand(uintptr_t initIndexBuffer,
                        uint32_t initIndexCount,
                        uint32_t initIndexSize,
                        uintptr_t initVertexBuffer,
                        DrawMode initDrawMode,
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

            uintptr_t indexBuffer;
            uint32_t indexCount;
            uint32_t indexSize;
            uintptr_t vertexBuffer;
            DrawMode drawMode;
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
            InitBlendStateCommand(uintptr_t initBlendState,
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

            uintptr_t blendState;
            bool enableBlending;
            BlendState::Factor colorBlendSource;
            BlendState::Factor colorBlendDest;
            BlendState::Operation colorOperation;
            BlendState::Factor alphaBlendSource;
            BlendState::Factor alphaBlendDest;
            BlendState::Operation alphaOperation;
            uint8_t colorMask;
        };

        class InitBufferCommand: public Command
        {
        public:
            InitBufferCommand(uintptr_t initBuffer,
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

            uintptr_t buffer;
            Buffer::Usage usage;
            uint32_t flags;
            std::vector<uint8_t> data;
            uint32_t size;
        };

        class SetBufferDataCommand: public Command
        {
        public:
            SetBufferDataCommand(uintptr_t initBuffer,
                                 const std::vector<uint8_t>& initData):
                Command(Command::Type::SET_BUFFER_DATA),
                buffer(initBuffer),
                data(initData)
            {
            }

            uintptr_t buffer;
            std::vector<uint8_t> data;
        };

        class InitShaderCommand: public Command
        {
        public:
            InitShaderCommand(uintptr_t initShader,
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

            uintptr_t shader;
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
            InitTextureCommand(uintptr_t initTexture,
                               const std::vector<Texture::Level>& initLevels,
                               Texture::Dimensions initDimensions,
                               uint32_t initFlags,
                               uint32_t initSampleCount,
                               PixelFormat initPixelFormat):
                Command(Command::Type::INIT_TEXTURE),
                texture(initTexture),
                levels(initLevels),
                dimensions(initDimensions),
                flags(initFlags),
                sampleCount(initSampleCount),
                pixelFormat(initPixelFormat)
            {
            }

            uintptr_t texture;
            std::vector<Texture::Level> levels;
            Texture::Dimensions dimensions;
            uint32_t flags;
            uint32_t sampleCount;
            PixelFormat pixelFormat;
        };

        class SetTextureDataCommand: public Command
        {
        public:
            SetTextureDataCommand(uintptr_t initTexture,
                                  const std::vector<Texture::Level>& initLevels):
                Command(Command::Type::SET_TEXTURE_DATA),
                texture(initTexture),
                levels(initLevels)
            {
            }

            uintptr_t texture;
            std::vector<Texture::Level> levels;
        };

        class SetTextureParametersCommand: public Command
        {
        public:
            SetTextureParametersCommand(uintptr_t initTexture,
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

            uintptr_t texture;
            Texture::Filter filter;
            Texture::Address addressX;
            Texture::Address addressY;
            uint32_t maxAnisotropy;
        };

        class SetTexturesCommand: public Command
        {
        public:
            explicit SetTexturesCommand(uintptr_t initTextures[Texture::LAYERS]):
                Command(Command::Type::SET_TEXTURES)
            {
                for (uint32_t i = 0; i < Texture::LAYERS; ++i)
                    textures[i] = initTextures[i];
            }

            uintptr_t textures[Texture::LAYERS];
        };

        class CommandBuffer
        {
        public:
            CommandBuffer() {}
            CommandBuffer(const std::string& initName):
                name(initName)
            {
            }

            inline const std::string& getName() const { return name; }

            inline bool isEmpty() const { return commands.empty(); }

            inline void pushCommand(std::unique_ptr<Command>&& command)
            {
                commands.push(std::move(command));
            }

            inline std::unique_ptr<Command> popCommand()
            {
                std::unique_ptr<Command> result = std::move(commands.front());
                commands.pop();
                return result;
            }

            inline const std::queue<std::unique_ptr<Command>>& getCommands() const
            {
                return commands;
            }

        private:
            std::string name;
            std::queue<std::unique_ptr<Command>> commands;
        };
    } // namespace graphics
} // namespace ouzel

#endif // OUZEL_GRAPHICS_COMMANDS_HPP

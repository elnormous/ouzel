// Copyright 2015-2019 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_GRAPHICS_COMMANDS_HPP
#define OUZEL_GRAPHICS_COMMANDS_HPP

#include <queue>
#include <string>
#include "graphics/BlendFactor.hpp"
#include "graphics/BlendOperation.hpp"
#include "graphics/Buffer.hpp"
#include "graphics/CubeFace.hpp"
#include "graphics/DepthStencilState.hpp"
#include "graphics/DrawMode.hpp"
#include "graphics/RasterizerState.hpp"
#include "graphics/Shader.hpp"
#include "graphics/StencilOperation.hpp"
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
                STOP,
                RESIZE,
                PRESENT,
                DELETE_RESOURCE,
                INIT_RENDER_TARGET,
                SET_RENDER_TARGET,
                CLEAR_RENDER_TARGET,
                BLIT,
                COMPUTE,
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

            virtual ~Command() = default;

            const Type type;
        };

        class StopCommand final: public Command
        {
        public:
            StopCommand(): Command(Command::Type::STOP) {}
        };

        class ResizeCommand final: public Command
        {
        public:
            ResizeCommand(const Size2U& initSize):
                Command(Command::Type::RESIZE),
                size(initSize)
            {}

            Size2U size;
        };

        class PresentCommand final: public Command
        {
        public:
            PresentCommand(): Command(Command::Type::PRESENT) {}
        };

        class DeleteResourceCommand final: public Command
        {
        public:
            DeleteResourceCommand(uintptr_t initResource):
                Command(Command::Type::DELETE_RESOURCE),
                resource(initResource)
            {}

            uintptr_t resource;
        };

        class InitRenderTargetCommand final: public Command
        {
        public:
            InitRenderTargetCommand(uintptr_t initRenderTarget,
                                    const std::set<uintptr_t>& initColorTextures,
                                    uintptr_t initDepthTexture):
                Command(Command::Type::INIT_RENDER_TARGET),
                renderTarget(initRenderTarget),
                colorTextures(initColorTextures),
                depthTexture(initDepthTexture)
            {
            }

            uintptr_t renderTarget;
            std::set<uintptr_t> colorTextures;
            uintptr_t depthTexture;
        };

        class SetRenderTargetCommand final: public Command
        {
        public:
            explicit SetRenderTargetCommand(uintptr_t initRenderTarget):
                Command(Command::Type::SET_RENDER_TARGET),
                renderTarget(initRenderTarget)
            {
            }

            uintptr_t renderTarget;
        };

        class ClearRenderTargetCommand final: public Command
        {
        public:
            ClearRenderTargetCommand(bool initClearColorBuffer,
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
        class BlitCommand final: public Command
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
        class ComputeCommand final: public Command
        {
        public:
            explicit ComputeCommand(uintptr_t initShader):
                Command(Command::Type::COMPUTE),
                shader(initShader)
            {
            }

            uintptr_t shader;
        };

        class SetScissorTestCommand final: public Command
        {
        public:
            SetScissorTestCommand(bool initEnabled,
                                  const RectF& initRectangle):
                Command(Command::Type::SET_SCISSOR_TEST),
                enabled(initEnabled),
                rectangle(initRectangle)
            {
            }

            bool enabled;
            RectF rectangle;
        };

        class SetViewportCommand final: public Command
        {
        public:
            explicit SetViewportCommand(const RectF& initViewport):
                Command(Command::Type::SET_VIEWPORT),
                viewport(initViewport)
            {
            }

            RectF viewport;
        };

        class InitDepthStencilStateCommand final: public Command
        {
        public:
            InitDepthStencilStateCommand(uintptr_t initDepthStencilState,
                                         bool initDepthTest,
                                         bool initDepthWrite,
                                         DepthStencilState::CompareFunction initCompareFunction,
                                         bool initStencilEnabled,
                                         uint32_t initStencilReadMask,
                                         uint32_t initStencilWriteMask,
                                         const DepthStencilState::StencilDescriptor& initFrontFaceStencil,
                                         const DepthStencilState::StencilDescriptor& initBackFaceStencil):
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

        class SetDepthStencilStateCommand final: public Command
        {
        public:
            SetDepthStencilStateCommand(uintptr_t initDepthStencilState,
                                        uint32_t initStencilReferenceValue):
                Command(Command::Type::SET_DEPTH_STENCIL_STATE),
                depthStencilState(initDepthStencilState),
                stencilReferenceValue(initStencilReferenceValue)
            {
            }

            uintptr_t depthStencilState;
            uint32_t stencilReferenceValue;
        };

        class SetPipelineStateCommand final: public Command
        {
        public:
            SetPipelineStateCommand(uintptr_t initBlendState,
                                    uintptr_t initShader,
                                    CullMode initCullMode,
                                    FillMode initFillMode):
                Command(Command::Type::SET_PIPELINE_STATE),
                blendState(initBlendState),
                shader(initShader),
                cullMode(initCullMode),
                fillMode(initFillMode)
            {
            }

            uintptr_t blendState;
            uintptr_t shader;
            CullMode cullMode;
            FillMode fillMode;
        };

        class DrawCommand final: public Command
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

        class PushDebugMarkerCommand final: public Command
        {
        public:
            explicit PushDebugMarkerCommand(const std::string& initName):
                Command(Command::Type::PUSH_DEBUG_MARKER),
                name(initName)
            {
            }

            std::string name;
        };

        class PopDebugMarkerCommand final: public Command
        {
        public:
            PopDebugMarkerCommand():
                Command(Command::Type::POP_DEBUG_MARKER)
            {
            }
        };

        class InitBlendStateCommand final: public Command
        {
        public:
            InitBlendStateCommand(uintptr_t initBlendState,
                                  bool initEnableBlending,
                                  BlendFactor initColorBlendSource,
                                  BlendFactor initColorBlendDest,
                                  BlendOperation initColorOperation,
                                  BlendFactor initAlphaBlendSource,
                                  BlendFactor initAlphaBlendDest,
                                  BlendOperation initAlphaOperation,
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
            BlendFactor colorBlendSource;
            BlendFactor colorBlendDest;
            BlendOperation colorOperation;
            BlendFactor alphaBlendSource;
            BlendFactor alphaBlendDest;
            BlendOperation alphaOperation;
            uint8_t colorMask;
        };

        class InitBufferCommand final: public Command
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

        class SetBufferDataCommand final: public Command
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

        class InitShaderCommand final: public Command
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

        class SetShaderConstantsCommand final: public Command
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

        class InitTextureCommand final: public Command
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

        class SetTextureDataCommand final: public Command
        {
        public:
            SetTextureDataCommand(uintptr_t initTexture,
                                  const std::vector<Texture::Level>& initLevels):
                Command(Command::Type::SET_TEXTURE_DATA),
                texture(initTexture),
                levels(initLevels)
            {
            }

            SetTextureDataCommand(uintptr_t initTexture,
                                  const std::vector<Texture::Level>& initLevels,
                                  CubeFace initFace):
                Command(Command::Type::SET_TEXTURE_DATA),
                texture(initTexture),
                levels(initLevels),
                face(initFace)
            {
            }

            uintptr_t texture;
            std::vector<Texture::Level> levels;
            CubeFace face;
        };

        class SetTextureParametersCommand final: public Command
        {
        public:
            SetTextureParametersCommand(uintptr_t initTexture,
                                        Filter initFilter,
                                        Texture::Address initAddressX,
                                        Texture::Address initAddressY,
                                        Texture::Address initAddressZ,
                                        Color initBorderColor,
                                        uint32_t initMaxAnisotropy):
                Command(Command::Type::SET_TEXTURE_PARAMETERS),
                texture(initTexture),
                filter(initFilter),
                addressX(initAddressX),
                addressY(initAddressY),
                addressZ(initAddressZ),
                borderColor(initBorderColor),
                maxAnisotropy(initMaxAnisotropy)
            {
            }

            uintptr_t texture;
            Filter filter;
            Texture::Address addressX;
            Texture::Address addressY;
            Texture::Address addressZ;
            Color borderColor;
            uint32_t maxAnisotropy;
        };

        class SetTexturesCommand final: public Command
        {
        public:
            explicit SetTexturesCommand(const std::vector<uintptr_t>& initTextures):
                Command(Command::Type::SET_TEXTURES),
                textures(initTextures)
            {
            }

            std::vector<uintptr_t> textures;
        };

        class CommandBuffer final
        {
        public:
            CommandBuffer() = default;
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

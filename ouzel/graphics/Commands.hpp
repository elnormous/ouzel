// Copyright 2015-2019 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_GRAPHICS_COMMANDS_HPP
#define OUZEL_GRAPHICS_COMMANDS_HPP

#include <queue>
#include <string>
#include "graphics/BlendFactor.hpp"
#include "graphics/BlendOperation.hpp"
#include "graphics/BufferType.hpp"
#include "graphics/CompareFunction.hpp"
#include "graphics/CubeFace.hpp"
#include "graphics/DataType.hpp"
#include "graphics/DrawMode.hpp"
#include "graphics/Flags.hpp"
#include "graphics/PixelFormat.hpp"
#include "graphics/RasterizerState.hpp"
#include "graphics/SamplerFilter.hpp"
#include "graphics/SamplerAddressMode.hpp"
#include "graphics/StencilOperation.hpp"
#include "graphics/TextureType.hpp"
#include "graphics/Vertex.hpp"
#include "math/Color.hpp"
#include "math/Rect.hpp"

namespace ouzel
{
    namespace graphics
    {
        class Command
        {
        public:
            enum class Type
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
            explicit ResizeCommand(const Size2U& initSize):
                Command(Command::Type::RESIZE),
                size(initSize)
            {}

            const Size2U size;
        };

        class PresentCommand final: public Command
        {
        public:
            PresentCommand(): Command(Command::Type::PRESENT) {}
        };

        class DeleteResourceCommand final: public Command
        {
        public:
            explicit DeleteResourceCommand(uintptr_t initResource):
                Command(Command::Type::DELETE_RESOURCE),
                resource(initResource)
            {}

            const uintptr_t resource;
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

            const uintptr_t renderTarget;
            const std::set<uintptr_t> colorTextures;
            const uintptr_t depthTexture;
        };

        class SetRenderTargetCommand final: public Command
        {
        public:
            explicit SetRenderTargetCommand(uintptr_t initRenderTarget):
                Command(Command::Type::SET_RENDER_TARGET),
                renderTarget(initRenderTarget)
            {
            }

            const uintptr_t renderTarget;
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

            const bool clearColorBuffer;
            const bool clearDepthBuffer;
            const bool clearStencilBuffer;
            const Color clearColor;
            const float clearDepth;
            const uint32_t clearStencil;
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

            const uintptr_t sourceTexture;
            const uint32_t sourceLevel;
            const uint32_t sourceX;
            const uint32_t sourceY;
            const uint32_t sourceWidth;
            const uint32_t sourceHeight;
            const uintptr_t destinationTexture;
            const uint32_t destinationLevel;
            const uint32_t destinationX;
            const uint32_t destinationY;
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

            const uintptr_t shader;
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

            const bool enabled;
            const RectF rectangle;
        };

        class SetViewportCommand final: public Command
        {
        public:
            explicit SetViewportCommand(const RectF& initViewport):
                Command(Command::Type::SET_VIEWPORT),
                viewport(initViewport)
            {
            }

            const RectF viewport;
        };

        class InitDepthStencilStateCommand final: public Command
        {
        public:
            InitDepthStencilStateCommand(uintptr_t initDepthStencilState,
                                         bool initDepthTest,
                                         bool initDepthWrite,
                                         CompareFunction initCompareFunction,
                                         bool initStencilEnabled,
                                         uint32_t initStencilReadMask,
                                         uint32_t initStencilWriteMask,
                                         StencilOperation initFrontFaceStencilFailureOperation,
                                         StencilOperation initFrontFaceStencilDepthFailureOperation,
                                         StencilOperation initFrontFaceStencilPassOperation,
                                         CompareFunction initFrontFaceStencilCompareFunction,
                                         StencilOperation initBackFaceStencilFailureOperation,
                                         StencilOperation initBackFaceStencilDepthFailureOperation,
                                         StencilOperation initBackFaceStencilPassOperation,
                                         CompareFunction initBackFaceStencilCompareFunction):
                Command(Command::Type::INIT_DEPTH_STENCIL_STATE),
                depthStencilState(initDepthStencilState),
                depthTest(initDepthTest),
                depthWrite(initDepthWrite),
                compareFunction(initCompareFunction),
                stencilEnabled(initStencilEnabled),
                stencilReadMask(initStencilReadMask),
                stencilWriteMask(initStencilWriteMask),
                frontFaceStencilFailureOperation(initFrontFaceStencilFailureOperation),
                frontFaceStencilDepthFailureOperation(initFrontFaceStencilDepthFailureOperation),
                frontFaceStencilPassOperation(initFrontFaceStencilPassOperation),
                frontFaceStencilCompareFunction(initFrontFaceStencilCompareFunction),
                backFaceStencilFailureOperation(initBackFaceStencilFailureOperation),
                backFaceStencilDepthFailureOperation(initBackFaceStencilDepthFailureOperation),
                backFaceStencilPassOperation(initBackFaceStencilPassOperation),
                backFaceStencilCompareFunction(initBackFaceStencilCompareFunction)
            {
            }

            const uintptr_t depthStencilState;
            const bool depthTest;
            const bool depthWrite;
            const CompareFunction compareFunction;
            const bool stencilEnabled;
            const uint32_t stencilReadMask;
            const uint32_t stencilWriteMask;
            const StencilOperation frontFaceStencilFailureOperation;
            const StencilOperation frontFaceStencilDepthFailureOperation;
            const StencilOperation frontFaceStencilPassOperation;
            const CompareFunction frontFaceStencilCompareFunction;
            const StencilOperation backFaceStencilFailureOperation;
            const StencilOperation backFaceStencilDepthFailureOperation;
            const StencilOperation backFaceStencilPassOperation;
            const CompareFunction backFaceStencilCompareFunction;
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

            const uintptr_t depthStencilState;
            const uint32_t stencilReferenceValue;
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

            const uintptr_t blendState;
            const uintptr_t shader;
            const CullMode cullMode;
            const FillMode fillMode;
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

            const uintptr_t indexBuffer;
            const uint32_t indexCount;
            const uint32_t indexSize;
            const uintptr_t vertexBuffer;
            const DrawMode drawMode;
            const uint32_t startIndex;
        };

        class PushDebugMarkerCommand final: public Command
        {
        public:
            explicit PushDebugMarkerCommand(const std::string& initName):
                Command(Command::Type::PUSH_DEBUG_MARKER),
                name(initName)
            {
            }

            const std::string name;
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

            const uintptr_t blendState;
            const bool enableBlending;
            const BlendFactor colorBlendSource;
            const BlendFactor colorBlendDest;
            const BlendOperation colorOperation;
            const BlendFactor alphaBlendSource;
            const BlendFactor alphaBlendDest;
            const BlendOperation alphaOperation;
            const uint8_t colorMask;
        };

        class InitBufferCommand final: public Command
        {
        public:
            InitBufferCommand(uintptr_t initBuffer,
                              BufferType initBufferType,
                              uint32_t initFlags,
                              const std::vector<uint8_t>& initData,
                              uint32_t initSize):
                Command(Command::Type::INIT_BUFFER),
                buffer(initBuffer),
                bufferType(initBufferType),
                flags(initFlags),
                data(initData),
                size(initSize)
            {
            }

            const uintptr_t buffer;
            const BufferType bufferType;
            const uint32_t flags;
            const std::vector<uint8_t> data;
            const uint32_t size;
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

            const uintptr_t buffer;
            const std::vector<uint8_t> data;
        };

        class InitShaderCommand final: public Command
        {
        public:
            InitShaderCommand(uintptr_t initShader,
                              const std::vector<uint8_t>& initFragmentShader,
                              const std::vector<uint8_t>& initVertexShader,
                              const std::set<Vertex::Attribute::Usage>& initVertexAttributes,
                              const std::vector<std::pair<std::string, DataType>>& initFragmentShaderConstantInfo,
                              const std::vector<std::pair<std::string, DataType>>& initVertexShaderConstantInfo,
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

            const uintptr_t shader;
            const std::vector<uint8_t> fragmentShader;
            const std::vector<uint8_t> vertexShader;
            const std::set<Vertex::Attribute::Usage> vertexAttributes;
            const std::vector<std::pair<std::string, DataType>> fragmentShaderConstantInfo;
            const std::vector<std::pair<std::string, DataType>> vertexShaderConstantInfo;
            const uint32_t fragmentShaderDataAlignment;
            const uint32_t vertexShaderDataAlignment;
            const std::string fragmentShaderFunction;
            const std::string vertexShaderFunction;
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

            const std::vector<std::vector<float>> fragmentShaderConstants;
            const std::vector<std::vector<float>> vertexShaderConstants;
        };

        class InitTextureCommand final: public Command
        {
        public:
            InitTextureCommand(uintptr_t initTexture,
                               const std::vector<std::pair<Size2U, std::vector<uint8_t>>>& initLevels,
                               TextureType initTextureType,
                               uint32_t initFlags,
                               uint32_t initSampleCount,
                               PixelFormat initPixelFormat):
                Command(Command::Type::INIT_TEXTURE),
                texture(initTexture),
                levels(initLevels),
                textureType(initTextureType),
                flags(initFlags),
                sampleCount(initSampleCount),
                pixelFormat(initPixelFormat)
            {
            }

            const uintptr_t texture;
            const std::vector<std::pair<Size2U, std::vector<uint8_t>>> levels;
            const TextureType textureType;
            const uint32_t flags;
            const uint32_t sampleCount;
            const PixelFormat pixelFormat;
        };

        class SetTextureDataCommand final: public Command
        {
        public:
            SetTextureDataCommand(uintptr_t initTexture,
                                  const std::vector<std::pair<Size2U, std::vector<uint8_t>>>& initLevels):
                Command(Command::Type::SET_TEXTURE_DATA),
                texture(initTexture),
                levels(initLevels),
                face(CubeFace::PositiveX)
            {
            }

            SetTextureDataCommand(uintptr_t initTexture,
                                  const std::vector<std::pair<Size2U, std::vector<uint8_t>>>& initLevels,
                                  CubeFace initFace):
                Command(Command::Type::SET_TEXTURE_DATA),
                texture(initTexture),
                levels(initLevels),
                face(initFace)
            {
            }

            const uintptr_t texture;
            const std::vector<std::pair<Size2U, std::vector<uint8_t>>> levels;
            const CubeFace face;
        };

        class SetTextureParametersCommand final: public Command
        {
        public:
            SetTextureParametersCommand(uintptr_t initTexture,
                                        SamplerFilter initFilter,
                                        SamplerAddressMode initAddressX,
                                        SamplerAddressMode initAddressY,
                                        SamplerAddressMode initAddressZ,
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

            const uintptr_t texture;
            const SamplerFilter filter;
            const SamplerAddressMode addressX;
            const SamplerAddressMode addressY;
            const SamplerAddressMode addressZ;
            const Color borderColor;
            const uint32_t maxAnisotropy;
        };

        class SetTexturesCommand final: public Command
        {
        public:
            explicit SetTexturesCommand(const std::vector<uintptr_t>& initTextures):
                Command(Command::Type::SET_TEXTURES),
                textures(initTextures)
            {
            }

            const std::vector<uintptr_t> textures;
        };

        class CommandBuffer final
        {
        public:
            CommandBuffer() = default;
            explicit CommandBuffer(const std::string& initName):
                name(initName)
            {
            }

            inline const std::string& getName() const noexcept { return name; }

            inline auto isEmpty() const { return commands.empty(); }

            inline void pushCommand(std::unique_ptr<Command> command)
            {
                commands.push(std::move(command));
            }

            inline std::unique_ptr<Command> popCommand()
            {
                auto result = std::move(commands.front());
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

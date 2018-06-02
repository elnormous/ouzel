// Copyright (C) 2018 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include <cassert>
#include "graphics/RenderResource.hpp"
#include "graphics/BlendState.hpp"
#include "graphics/Buffer.hpp"
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

            const Type type;
        };

        struct InitRenderTargetCommand: public Command
        {
            InitRenderTargetCommand(TextureResource* initRenderTarget):
                Command(Command::Type::INIT_RENDER_TARGET),
                renderTarget(initRenderTarget)
            {
            }

            TextureResource* renderTarget;
        };

        struct SetRenderTargetParametersCommand: public Command
        {
            SetRenderTargetParametersCommand(TextureResource* initRenderTarget):
                Command(Command::Type::SET_RENDER_TARGET_PARAMETERS),
                renderTarget(initRenderTarget)
            {
            }

            TextureResource* renderTarget;
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
            InitTextureCommand(TextureResource* initTexture):
                Command(Command::Type::INIT_TEXTURE),
                texture(initTexture)
            {
            }

            TextureResource* texture;
        };

        struct SetTextureDataCommand: public Command
        {
            SetTextureDataCommand(TextureResource* initTexture):
                Command(Command::Type::SET_TEXTURE_DATA),
                texture(initTexture)
            {
            }

            TextureResource* texture;
        };

        struct SetTextureParametersCommand: public Command
        {
            SetTextureParametersCommand(TextureResource* initTexture):
                Command(Command::Type::SET_TEXTURE_PARAMETERS),
                texture(initTexture)
            {
            }

            TextureResource* texture;
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

        class CommandBuffer
        {
        public:
            CommandBuffer() {}
            CommandBuffer(const CommandBuffer&) = delete;
            CommandBuffer(CommandBuffer&& other)
            {
                buffer = other.buffer;
                capacity = other.capacity;
                count = other.count;
                size = other.size;
                position = other.position;
                current = other.current;

                other.buffer = nullptr;
                other.capacity = 0;
                other.count = 0;
                other.size = 0;
                other.position = 0;
                other.current = 0;
            }
            CommandBuffer& operator=(const CommandBuffer&) = delete;
            CommandBuffer& operator=(CommandBuffer&& other)
            {
                if (&other != this)
                {
                    if (buffer)
                    {
                        deleteCommands();
                        delete [] buffer;
                    }

                    buffer = other.buffer;
                    capacity = other.capacity;
                    count = other.count;
                    size = other.size;
                    position = other.position;
                    current = other.current;

                    other.buffer = nullptr;
                    other.capacity = 0;
                    other.count = 0;
                    other.size = 0;
                    other.position = 0;
                    other.current = 0;
                }

                return *this;
            }

            ~CommandBuffer()
            {
                if (buffer)
                {
                    deleteCommands();
                    delete [] buffer;
                }
            }

            template <class T> void push(const T& command)
            {
                static_assert(std::is_base_of<Command, T>::value, "Not derived from Command");

                size_t offset = size;
                if (offset % alignof(Command*) != 0) offset += alignof(Command*) - (offset % alignof(Command*));

                if (capacity < offset + sizeof(T))
                {
                    capacity *= 2;
                    if (capacity < offset + sizeof(T)) capacity = offset + sizeof(T);
                    uint8_t* newBuffer = new uint8_t[capacity];
                    if (buffer)
                    {
                        moveCommands(newBuffer);
                        deleteCommands();
                        delete [] buffer;
                    }
                    buffer = newBuffer;
                }

                size = offset + sizeof(T);
                ++count;
                new (buffer + offset) T(command);
            }

            Command* front() const
            {
                if (current >= count) return nullptr;

                size_t offset = position;
                if (offset % alignof(Command*) != 0) offset += alignof(Command*) - (offset % alignof(Command*));
                Command* command = reinterpret_cast<Command*>(buffer + offset);

                return command;
            }

            void pop()
            {
                if (current >= count) return;

                if (position % alignof(Command*) != 0) position += alignof(Command*) - (position % alignof(Command*));
                Command* command = reinterpret_cast<Command*>(buffer + position);

                switch (command->type)
                {
                    case Command::Type::INIT_RENDER_TARGET: position += deleteCommand(static_cast<InitRenderTargetCommand*>(command)); break;
                    case Command::Type::SET_RENDER_TARGET_PARAMETERS: position += deleteCommand(static_cast<SetRenderTargetParametersCommand*>(command)); break;
                    case Command::Type::SET_RENDER_TARGET: position += deleteCommand(static_cast<SetRenderTargetCommand*>(command)); break;
                    case Command::Type::CLEAR: position += deleteCommand(static_cast<ClearCommand*>(command)); break;
                    case Command::Type::SET_CULL_MODE: position += deleteCommand(static_cast<SetCullModeCommad*>(command)); break;
                    case Command::Type::SET_FILL_MODE: position += deleteCommand(static_cast<SetFillModeCommad*>(command)); break;
                    case Command::Type::SET_SCISSOR_TEST: position += deleteCommand(static_cast<SetScissorTestCommand*>(command)); break;
                    case Command::Type::SET_VIEWPORT: position += deleteCommand(static_cast<SetViewportCommand*>(command)); break;
                    case Command::Type::SET_DEPTH_STATE: position += deleteCommand(static_cast<SetDepthStateCommand*>(command)); break;
                    case Command::Type::SET_PIPELINE_STATE: position += deleteCommand(static_cast<SetPipelineStateCommand*>(command)); break;
                    case Command::Type::DRAW: position += deleteCommand(static_cast<DrawCommand*>(command)); break;
                    case Command::Type::PUSH_DEBUG_MARKER: position += deleteCommand(static_cast<PushDebugMarkerCommand*>(command)); break;
                    case Command::Type::POP_DEBUG_MARKER: position += deleteCommand(static_cast<PopDebugMarkerCommand*>(command)); break;
                    case Command::Type::INIT_BLEND_STATE: position += deleteCommand(static_cast<InitBlendStateCommand*>(command)); break;
                    case Command::Type::INIT_BUFFER: position += deleteCommand(static_cast<InitBufferCommand*>(command)); break;
                    case Command::Type::SET_BUFFER_DATA: position += deleteCommand(static_cast<SetBufferDataCommand*>(command)); break;
                    case Command::Type::INIT_SHADER: position += deleteCommand(static_cast<InitShaderCommand*>(command)); break;
                    case Command::Type::SET_SHADER_CONSTANTS: position += deleteCommand(static_cast<SetShaderConstantsCommand*>(command)); break;
                    case Command::Type::INIT_TEXTURE: position += deleteCommand(static_cast<InitTextureCommand*>(command)); break;
                    case Command::Type::SET_TEXTURE_DATA: position += deleteCommand(static_cast<SetTextureDataCommand*>(command)); break;
                    case Command::Type::SET_TEXTURE_PARAMETERS: position += deleteCommand(static_cast<SetTextureParametersCommand*>(command)); break;
                    case Command::Type::SET_TEXTURES: position += deleteCommand(static_cast<SetTexturesCommand*>(command)); break;
                    default: break;
                }

                ++current;

                if (current == count)
                {
                    count = 0;
                    size = 0;
                    position = 0;
                    current = 0;
                }
            }

        private:
            template<class T> uint32_t deleteCommand(T* command)
            {
                (void)command; // silence the unreferenced parameter warning in Visual Studio
                command->~T();
                return sizeof(*command);
            }

            template<class T> uint32_t moveCommand(T* command, void* newPointer)
            {
                (void)command; // silence the unreferenced parameter warning in Visual Studio
                new (newPointer) T(std::move(*command));
                return sizeof(*command);
            }

            void moveCommands(uint8_t* newBuffer)
            {
                size_t offset = position;

                for (uint32_t i = current; i < count; ++i)
                {
                    if (offset % alignof(Command*) != 0) offset += alignof(Command*) - (offset % alignof(Command*));
                    Command* command = reinterpret_cast<Command*>(buffer + offset);

                    switch (command->type)
                    {
                        case Command::Type::INIT_RENDER_TARGET: offset += moveCommand(static_cast<InitRenderTargetCommand*>(command), newBuffer + offset); break;
                        case Command::Type::SET_RENDER_TARGET_PARAMETERS: offset += moveCommand(static_cast<SetRenderTargetParametersCommand*>(command), newBuffer + offset); break;
                        case Command::Type::SET_RENDER_TARGET: offset += moveCommand(static_cast<SetRenderTargetCommand*>(command), newBuffer + offset); break;
                        case Command::Type::CLEAR: offset += moveCommand(static_cast<ClearCommand*>(command), newBuffer + offset); break;
                        case Command::Type::SET_CULL_MODE: offset += moveCommand(static_cast<SetCullModeCommad*>(command), newBuffer + offset); break;
                        case Command::Type::SET_FILL_MODE: offset += moveCommand(static_cast<SetFillModeCommad*>(command), newBuffer + offset); break;
                        case Command::Type::SET_SCISSOR_TEST: offset += moveCommand(static_cast<SetScissorTestCommand*>(command), newBuffer + offset); break;
                        case Command::Type::SET_VIEWPORT: offset += moveCommand(static_cast<SetViewportCommand*>(command), newBuffer + offset); break;
                        case Command::Type::SET_DEPTH_STATE: offset += moveCommand(static_cast<SetDepthStateCommand*>(command), newBuffer + offset); break;
                        case Command::Type::SET_PIPELINE_STATE: offset += moveCommand(static_cast<SetPipelineStateCommand*>(command), newBuffer + offset); break;
                        case Command::Type::DRAW: offset += moveCommand(static_cast<DrawCommand*>(command), newBuffer + offset); break;
                        case Command::Type::PUSH_DEBUG_MARKER: offset += moveCommand(static_cast<PushDebugMarkerCommand*>(command), newBuffer + offset); break;
                        case Command::Type::POP_DEBUG_MARKER: offset += moveCommand(static_cast<PopDebugMarkerCommand*>(command), newBuffer + offset); break;
                        case Command::Type::INIT_BLEND_STATE: offset += moveCommand(static_cast<InitBlendStateCommand*>(command), newBuffer + offset); break;
                        case Command::Type::INIT_BUFFER: offset += moveCommand(static_cast<InitBufferCommand*>(command), newBuffer + offset); break;
                        case Command::Type::SET_BUFFER_DATA: offset += moveCommand(static_cast<SetBufferDataCommand*>(command), newBuffer + offset); break;
                        case Command::Type::INIT_SHADER: offset += moveCommand(static_cast<InitShaderCommand*>(command), newBuffer + offset); break;
                        case Command::Type::SET_SHADER_CONSTANTS: offset += moveCommand(static_cast<SetShaderConstantsCommand*>(command), newBuffer + offset); break;
                        case Command::Type::INIT_TEXTURE: offset += moveCommand(static_cast<InitTextureCommand*>(command), newBuffer + offset); break;
                        case Command::Type::SET_TEXTURE_DATA: offset += moveCommand(static_cast<SetTextureDataCommand*>(command), newBuffer + offset); break;
                        case Command::Type::SET_TEXTURE_PARAMETERS: offset += moveCommand(static_cast<SetTextureParametersCommand*>(command), newBuffer + offset); break;
                        case Command::Type::SET_TEXTURES: offset += moveCommand(static_cast<SetTexturesCommand*>(command), newBuffer + offset); break;
                        default: assert(false);
                    }
                }
            }

            void deleteCommands()
            {
                size_t offset = position;

                for (uint32_t i = current; i < count; ++i)
                {
                    if (offset % alignof(Command*) != 0) offset += alignof(Command*) - (offset % alignof(Command*));
                    Command* command = reinterpret_cast<Command*>(buffer + offset);

                    switch (command->type)
                    {
                        case Command::Type::INIT_RENDER_TARGET: offset += deleteCommand(static_cast<InitRenderTargetCommand*>(command)); break;
                        case Command::Type::SET_RENDER_TARGET_PARAMETERS: offset += deleteCommand(static_cast<SetRenderTargetParametersCommand*>(command)); break;
                        case Command::Type::SET_RENDER_TARGET: offset += deleteCommand(static_cast<SetRenderTargetCommand*>(command)); break;
                        case Command::Type::CLEAR: offset += deleteCommand(static_cast<ClearCommand*>(command)); break;
                        case Command::Type::SET_CULL_MODE: offset += deleteCommand(static_cast<SetCullModeCommad*>(command)); break;
                        case Command::Type::SET_FILL_MODE: offset += deleteCommand(static_cast<SetFillModeCommad*>(command)); break;
                        case Command::Type::SET_SCISSOR_TEST: offset += deleteCommand(static_cast<SetScissorTestCommand*>(command)); break;
                        case Command::Type::SET_VIEWPORT: offset += deleteCommand(static_cast<SetViewportCommand*>(command)); break;
                        case Command::Type::SET_DEPTH_STATE: offset += deleteCommand(static_cast<SetDepthStateCommand*>(command)); break;
                        case Command::Type::SET_PIPELINE_STATE: offset += deleteCommand(static_cast<SetPipelineStateCommand*>(command)); break;
                        case Command::Type::DRAW: offset += deleteCommand(static_cast<DrawCommand*>(command)); break;
                        case Command::Type::PUSH_DEBUG_MARKER: offset += deleteCommand(static_cast<PushDebugMarkerCommand*>(command)); break;
                        case Command::Type::POP_DEBUG_MARKER: offset += deleteCommand(static_cast<PopDebugMarkerCommand*>(command)); break;
                        case Command::Type::INIT_BLEND_STATE: offset += deleteCommand(static_cast<InitBlendStateCommand*>(command)); break;
                        case Command::Type::INIT_BUFFER: offset += deleteCommand(static_cast<InitBufferCommand*>(command)); break;
                        case Command::Type::SET_BUFFER_DATA: offset += deleteCommand(static_cast<SetBufferDataCommand*>(command)); break;
                        case Command::Type::INIT_SHADER: offset += deleteCommand(static_cast<InitShaderCommand*>(command)); break;
                        case Command::Type::SET_SHADER_CONSTANTS: offset += deleteCommand(static_cast<SetShaderConstantsCommand*>(command)); break;
                        case Command::Type::INIT_TEXTURE: offset += deleteCommand(static_cast<InitTextureCommand*>(command)); break;
                        case Command::Type::SET_TEXTURE_DATA: offset += deleteCommand(static_cast<SetTextureDataCommand*>(command)); break;
                        case Command::Type::SET_TEXTURE_PARAMETERS: offset += deleteCommand(static_cast<SetTextureParametersCommand*>(command)); break;
                        case Command::Type::SET_TEXTURES: offset += deleteCommand(static_cast<SetTexturesCommand*>(command)); break;
                        default: assert(false);
                    }
                }
            }

            uint8_t* buffer = nullptr;
            size_t capacity = 0;
            size_t size = 0; // write position
            size_t position = 0; // read postion
            uint32_t count = 0;
            uint32_t current = 0;
        };
    } // namespace graphics
} // namespace ouzel

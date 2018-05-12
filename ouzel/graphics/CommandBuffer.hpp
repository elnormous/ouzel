// Copyright (C) 2018 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include "graphics/RenderResource.hpp"
#include "graphics/BlendState.hpp"
#include "graphics/Buffer.hpp"
#include "graphics/MeshBuffer.hpp"
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
            InitBufferCommand(BufferResource* initBuffer):
                Command(Command::Type::INIT_BUFFER),
                buffer(initBuffer)
            {
            }

            BufferResource* buffer;
        };

        // TODO: implement
        struct SetBufferDataCommand: public Command
        {
            SetBufferDataCommand(BufferResource* initBuffer):
                Command(Command::Type::SET_BUFFER_DATA),
                buffer(initBuffer)
            {
            }

            BufferResource* buffer;
        };

        // TODO: implement
        struct InitMeshBufferCommand: public Command
        {
            InitMeshBufferCommand(MeshBuffer* initMeshBuffer):
                Command(Command::Type::INIT_MESH_BUFFER),
                meshBuffer(initMeshBuffer)
            {
            }

            MeshBuffer* meshBuffer;
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

        struct SetTextureFlagsCommand: public Command
        {
            SetTextureFlagsCommand(TextureResource* initTexture):
                Command(Command::Type::SET_TEXTURE_FLAGS),
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
                size = other.size;

                other.buffer = nullptr;
                other.capacity = 0;
                other.size = 0;
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
                    size = other.size;

                    other.buffer = nullptr;
                    other.capacity = 0;
                    other.size = 0;
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

            template <class T> void addCommand(const T& command)
            {
                static_assert(std::is_base_of<Command, T>::value, "Not derived from Command");

                if (capacity < size + sizeof(T))
                {
                    capacity *= 2;
                    if (capacity < size + sizeof(T)) capacity = size + sizeof(T);
                    uint8_t* newBuffer = new uint8_t[capacity];
                    if (buffer)
                    {
                        moveCommands(newBuffer);
                        deleteCommands();
                        delete [] buffer;
                    }
                    buffer = newBuffer;
                }

                size_t offset = size;
                size += sizeof(T);
                new (buffer + offset) T(command);
            }

            void clear()
            {
                if (buffer) deleteCommands();
                size = 0;
            }

            uint32_t getSize() const
            {
                return size;
            }

            const uint8_t* getData() const
            {
                return buffer;
            }

        private:
            void moveCommands(uint8_t* newBuffer)
            {
                for (uint32_t offset = 0; offset < size;)
                {
                    Command* command = reinterpret_cast<Command*>(buffer + offset);

                    switch (command->type)
                    {
                        case Command::Type::SET_RENDER_TARGET:
                        {
                            SetRenderTargetCommand* newSetRenderTargetCommand = reinterpret_cast<SetRenderTargetCommand*>(newBuffer + offset);
                            new (newSetRenderTargetCommand) SetRenderTargetCommand(std::move(*static_cast<SetRenderTargetCommand*>(command)));
                            offset += sizeof(SetRenderTargetCommand);
                            break;
                        }

                        case Command::Type::CLEAR:
                        {
                            ClearCommand* newClearCommand = reinterpret_cast<ClearCommand*>(newBuffer + offset);
                            new (newClearCommand) ClearCommand(std::move(*static_cast<ClearCommand*>(command)));
                            offset += sizeof(ClearCommand);
                            break;
                        }

                        case Command::Type::SET_CULL_MODE:
                        {
                            SetCullModeCommad* newSetCullModeCommad = reinterpret_cast<SetCullModeCommad*>(newBuffer + offset);
                            new (newSetCullModeCommad) SetCullModeCommad(std::move(*static_cast<SetCullModeCommad*>(command)));
                            offset += sizeof(SetCullModeCommad);
                            break;
                        }

                        case Command::Type::SET_FILL_MODE:
                        {
                            SetFillModeCommad* newSetFillModeCommad = reinterpret_cast<SetFillModeCommad*>(newBuffer + offset);
                            new (newSetFillModeCommad) SetFillModeCommad(std::move(*static_cast<SetFillModeCommad*>(command)));
                            offset += sizeof(SetFillModeCommad);
                            break;
                        }

                        case Command::Type::SET_SCISSOR_TEST:
                        {
                            SetScissorTestCommand* newSetScissorTestCommand = reinterpret_cast<SetScissorTestCommand*>(newBuffer + offset);
                            new (newSetScissorTestCommand) SetScissorTestCommand(std::move(*static_cast<SetScissorTestCommand*>(command)));
                            offset += sizeof(SetScissorTestCommand);
                            break;
                        }

                        case Command::Type::SET_VIEWPORT:
                        {
                            SetViewportCommand* newSetViewportCommand = reinterpret_cast<SetViewportCommand*>(newBuffer + offset);
                            new (newSetViewportCommand) SetViewportCommand(std::move(*static_cast<SetViewportCommand*>(command)));
                            offset += sizeof(SetViewportCommand);
                            break;
                        }

                        case Command::Type::SET_DEPTH_STATE:
                        {
                            SetDepthStateCommand* newSetDepthStateCommand = reinterpret_cast<SetDepthStateCommand*>(newBuffer + offset);
                            new (newSetDepthStateCommand) SetDepthStateCommand(std::move(*static_cast<SetDepthStateCommand*>(command)));
                            offset += sizeof(SetDepthStateCommand);
                            break;
                        }

                        case Command::Type::SET_PIPELINE_STATE:
                        {
                            SetPipelineStateCommand* newSetPipelineStateCommand = reinterpret_cast<SetPipelineStateCommand*>(newBuffer + offset);
                            new (newSetPipelineStateCommand) SetPipelineStateCommand(std::move(*static_cast<SetPipelineStateCommand*>(command)));
                            offset += sizeof(SetPipelineStateCommand);
                            break;
                        }

                        case Command::Type::DRAW:
                        {
                            DrawCommand* newDrawCommand = reinterpret_cast<DrawCommand*>(newBuffer + offset);
                            new (newDrawCommand) DrawCommand(std::move(*static_cast<DrawCommand*>(command)));
                            offset += sizeof(DrawCommand);
                            break;
                        }

                        case Command::Type::PUSH_DEBUG_MARKER:
                        {
                            PushDebugMarkerCommand* newPushDebugMarkerCommand = reinterpret_cast<PushDebugMarkerCommand*>(newBuffer + offset);
                            new (newPushDebugMarkerCommand) PushDebugMarkerCommand(std::move(*static_cast<PushDebugMarkerCommand*>(command)));
                            offset += sizeof(PushDebugMarkerCommand);
                            break;
                        }

                        case Command::Type::POP_DEBUG_MARKER:
                        {
                            PopDebugMarkerCommand* newPopDebugMarkerCommand = reinterpret_cast<PopDebugMarkerCommand*>(newBuffer + offset);
                            new (newPopDebugMarkerCommand) PopDebugMarkerCommand(std::move(*static_cast<PopDebugMarkerCommand*>(command)));
                            offset += sizeof(PopDebugMarkerCommand);
                            break;
                        }

                        case Command::Type::INIT_BLEND_STATE:
                        {
                            InitBlendStateCommand* newInitBlendStateCommand = reinterpret_cast<InitBlendStateCommand*>(newBuffer + offset);
                            new (newInitBlendStateCommand) InitBlendStateCommand(std::move(*static_cast<InitBlendStateCommand*>(command)));
                            offset += sizeof(InitBlendStateCommand);
                            break;
                        }

                        case Command::Type::INIT_BUFFER:
                        {
                            InitBufferCommand* newInitBufferCommand = reinterpret_cast<InitBufferCommand*>(newBuffer + offset);
                            new (newInitBufferCommand) InitBufferCommand(std::move(*static_cast<InitBufferCommand*>(command)));
                            offset += sizeof(InitBufferCommand);
                            break;
                        }

                        case Command::Type::SET_BUFFER_DATA:
                        {
                            SetBufferDataCommand* newSetBufferDataCommand = reinterpret_cast<SetBufferDataCommand*>(newBuffer + offset);
                            new (newSetBufferDataCommand) SetBufferDataCommand(std::move(*static_cast<SetBufferDataCommand*>(command)));
                            offset += sizeof(SetBufferDataCommand);
                            break;
                        }

                        case Command::Type::INIT_MESH_BUFFER:
                        {
                            InitMeshBufferCommand* newInitMeshBufferCommand = reinterpret_cast<InitMeshBufferCommand*>(newBuffer + offset);
                            new (newInitMeshBufferCommand) InitMeshBufferCommand(std::move(*static_cast<InitMeshBufferCommand*>(command)));
                            offset += sizeof(InitMeshBufferCommand);
                            break;
                        }

                        case Command::Type::INIT_SHADER:
                        {
                            InitShaderCommand* newInitShaderCommand = reinterpret_cast<InitShaderCommand*>(newBuffer + offset);
                            new (newInitShaderCommand) InitShaderCommand(std::move(*static_cast<InitShaderCommand*>(command)));
                            offset += sizeof(InitShaderCommand);
                            break;
                        }

                        case Command::Type::SET_SHADER_CONSTANTS:
                        {
                            SetShaderConstantsCommand* newSetShaderConstantsCommand = reinterpret_cast<SetShaderConstantsCommand*>(newBuffer + offset);
                            new (newSetShaderConstantsCommand) SetShaderConstantsCommand(std::move(*static_cast<SetShaderConstantsCommand*>(command)));
                            offset += sizeof(SetShaderConstantsCommand);
                            break;
                        }

                        case Command::Type::INIT_TEXTURE:
                        {
                            InitTextureCommand* newInitTextureCommand = reinterpret_cast<InitTextureCommand*>(newBuffer + offset);
                            new (newInitTextureCommand) InitTextureCommand(std::move(*static_cast<InitTextureCommand*>(command)));
                            offset += sizeof(InitTextureCommand);
                            break;
                        }

                        case Command::Type::SET_TEXTURE_DATA:
                        {
                            SetTextureDataCommand* newSetTextureDataCommand = reinterpret_cast<SetTextureDataCommand*>(newBuffer + offset);
                            new (newSetTextureDataCommand) SetTextureDataCommand(std::move(*static_cast<SetTextureDataCommand*>(command)));
                            offset += sizeof(SetTextureDataCommand);
                            break;
                        }

                        case Command::Type::SET_TEXTURE_FLAGS:
                        {
                            SetTextureFlagsCommand* newSetTextureFlagsCommand = reinterpret_cast<SetTextureFlagsCommand*>(newBuffer + offset);
                            new (newSetTextureFlagsCommand) SetTextureFlagsCommand(std::move(*static_cast<SetTextureFlagsCommand*>(command)));
                            offset += sizeof(SetTextureFlagsCommand);
                            break;
                        }

                        case Command::Type::SET_TEXTURES:
                        {
                            SetTexturesCommand* newSetTexturesCommand = reinterpret_cast<SetTexturesCommand*>(newBuffer + offset);
                            new (newSetTexturesCommand) SetTexturesCommand(std::move(*static_cast<SetTexturesCommand*>(command)));
                            offset += sizeof(SetTexturesCommand);
                            break;
                        }

                        default: break;
                    }
                }
            }

            void deleteCommands()
            {
                for (uint32_t offset = 0; offset < size;)
                {
                    Command* command = reinterpret_cast<Command*>(buffer + offset);

                    switch (command->type)
                    {
                        case Command::Type::SET_RENDER_TARGET:
                        {
                            static_cast<SetRenderTargetCommand*>(command)->~SetRenderTargetCommand();
                            offset += sizeof(SetRenderTargetCommand);
                            break;
                        }

                        case Command::Type::CLEAR:
                        {
                            static_cast<ClearCommand*>(command)->~ClearCommand();
                            offset += sizeof(ClearCommand);
                            break;
                        }

                        case Command::Type::SET_CULL_MODE:
                        {
                            static_cast<SetCullModeCommad*>(command)->~SetCullModeCommad();
                            offset += sizeof(SetCullModeCommad);
                            break;
                        }

                        case Command::Type::SET_FILL_MODE:
                        {
                            static_cast<SetFillModeCommad*>(command)->~SetFillModeCommad();
                            offset += sizeof(SetFillModeCommad);
                            break;
                        }

                        case Command::Type::SET_SCISSOR_TEST:
                        {
                            static_cast<SetScissorTestCommand*>(command)->~SetScissorTestCommand();
                            offset += sizeof(SetScissorTestCommand);
                            break;
                        }

                        case Command::Type::SET_VIEWPORT:
                        {
                            static_cast<SetViewportCommand*>(command)->~SetViewportCommand();
                            offset += sizeof(SetViewportCommand);
                            break;
                        }

                        case Command::Type::SET_DEPTH_STATE:
                        {
                            static_cast<SetDepthStateCommand*>(command)->~SetDepthStateCommand();
                            offset += sizeof(SetDepthStateCommand);
                            break;
                        }

                        case Command::Type::SET_PIPELINE_STATE:
                        {
                            static_cast<SetPipelineStateCommand*>(command)->~SetPipelineStateCommand();
                            offset += sizeof(SetPipelineStateCommand);
                            break;
                        }

                        case Command::Type::DRAW:
                        {
                            static_cast<DrawCommand*>(command)->~DrawCommand();
                            offset += sizeof(DrawCommand);
                            break;
                        }

                        case Command::Type::PUSH_DEBUG_MARKER:
                        {
                            static_cast<PushDebugMarkerCommand*>(command)->~PushDebugMarkerCommand();
                            offset += sizeof(PushDebugMarkerCommand);
                            break;
                        }

                        case Command::Type::POP_DEBUG_MARKER:
                        {
                            static_cast<PopDebugMarkerCommand*>(command)->~PopDebugMarkerCommand();
                            offset += sizeof(PopDebugMarkerCommand);
                            break;
                        }

                        case Command::Type::INIT_BLEND_STATE:
                        {
                            static_cast<InitBlendStateCommand*>(command)->~InitBlendStateCommand();
                            offset += sizeof(InitBlendStateCommand);
                            break;
                        }

                        case Command::Type::INIT_BUFFER:
                        {
                            static_cast<InitBufferCommand*>(command)->~InitBufferCommand();
                            offset += sizeof(InitBufferCommand);
                            break;
                        }

                        case Command::Type::SET_BUFFER_DATA:
                        {
                            static_cast<SetBufferDataCommand*>(command)->~SetBufferDataCommand();
                            offset += sizeof(SetBufferDataCommand);
                            break;
                        }

                        case Command::Type::INIT_MESH_BUFFER:
                        {
                            static_cast<InitMeshBufferCommand*>(command)->~InitMeshBufferCommand();
                            offset += sizeof(InitMeshBufferCommand);
                            break;
                        }

                        case Command::Type::INIT_SHADER:
                        {
                            static_cast<InitShaderCommand*>(command)->~InitShaderCommand();
                            offset += sizeof(InitShaderCommand);
                            break;
                        }

                        case Command::Type::SET_SHADER_CONSTANTS:
                        {
                            static_cast<SetShaderConstantsCommand*>(command)->~SetShaderConstantsCommand();
                            offset += sizeof(SetShaderConstantsCommand);
                            break;
                        }

                        case Command::Type::INIT_TEXTURE:
                        {
                            static_cast<InitTextureCommand*>(command)->~InitTextureCommand();
                            offset += sizeof(InitTextureCommand);
                            break;
                        }

                        case Command::Type::SET_TEXTURE_DATA:
                        {
                            static_cast<SetTextureDataCommand*>(command)->~SetTextureDataCommand();
                            offset += sizeof(SetTextureDataCommand);
                            break;
                        }

                        case Command::Type::SET_TEXTURE_FLAGS:
                        {
                            static_cast<SetTextureFlagsCommand*>(command)->~SetTextureFlagsCommand();
                            offset += sizeof(SetTextureFlagsCommand);
                            break;
                        }

                        case Command::Type::SET_TEXTURES:
                        {
                            static_cast<SetTexturesCommand*>(command)->~SetTexturesCommand();
                            offset += sizeof(SetTexturesCommand);
                            break;
                        }

                        default: break;
                    }
                }
            }

            uint8_t* buffer = nullptr;
            uint32_t capacity = 0;
            uint32_t size = 0;
        };
    } // namespace graphics
} // namespace ouzel

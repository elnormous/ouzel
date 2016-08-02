// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "DebugDrawable.h"
#include "core/CompileConfig.h"
#include "core/Engine.h"
#include "graphics/Renderer.h"
#include "graphics/MeshBuffer.h"
#include "core/Cache.h"
#include "Layer.h"
#include "Camera.h"
#include "math/MathUtils.h"
#include "utils/Utils.h"

namespace ouzel
{
    namespace scene
    {
        DebugDrawable::DebugDrawable()
        {
            shader = sharedEngine->getCache()->getShader(graphics::SHADER_COLOR);
            blendState = sharedEngine->getCache()->getBlendState(graphics::BLEND_NO_BLEND);
        }

        void DebugDrawable::draw(const Matrix4& projectionMatrix, const Matrix4& transformMatrix, const graphics::Color& drawColor, const NodePtr& currentNode)
        {
            Drawable::draw(projectionMatrix, transformMatrix, drawColor, currentNode);

            if (shader)
            {
                Matrix4 modelViewProj = projectionMatrix * transformMatrix;
                float colorVector[] = { drawColor.getR(), drawColor.getG(), drawColor.getB(), drawColor.getA() };

                for (const DrawCommand& drawCommand : drawCommands)
                {
                    std::vector<std::vector<float>> pixelShaderConstants(1);
                    pixelShaderConstants[0] = { std::begin(colorVector), std::end(colorVector) };

                    std::vector<std::vector<float>> vertexShaderConstants(1);
                    vertexShaderConstants[0] = { std::begin(modelViewProj.m), std::end(modelViewProj.m) };

                    sharedEngine->getRenderer()->draw(std::vector<graphics::TexturePtr>(),
                                                      shader,
                                                      pixelShaderConstants,
                                                      vertexShaderConstants,
                                                      blendState,
                                                      drawCommand.mesh, 0, drawCommand.mode);
                }
            }
        }

        void DebugDrawable::clear()
        {
            boundingBox = AABB2();

            drawCommands.clear();
        }

        void DebugDrawable::point(const Vector2& position, const graphics::Color& color)
        {
            std::vector<uint16_t> indices = {0};

            std::vector<graphics::VertexPC> vertices = {
                graphics::VertexPC(Vector3(position), color)
            };

            DrawCommand command;

            command.mode = graphics::Renderer::DrawMode::POINT_LIST;
            command.mesh = sharedEngine->getRenderer()->createMeshBuffer();
            command.mesh->initFromBuffer(indices.data(), sizeof(uint16_t),
                                         static_cast<uint32_t>(indices.size()), false,
                                         vertices.data(), graphics::VertexPC::ATTRIBUTES,
                                         static_cast<uint32_t>(vertices.size()), false);

            drawCommands.push_back(command);

            boundingBox.insertPoint(position);
        }

        void DebugDrawable::line(const Vector2& start, const Vector2& finish, const graphics::Color& color)
        {
            std::vector<uint16_t> indices = {0, 1};

            std::vector<graphics::VertexPC> vertices = {
                graphics::VertexPC(Vector3(start), color),
                graphics::VertexPC(Vector3(finish), color)
            };

            DrawCommand command;

            command.mode = graphics::Renderer::DrawMode::LINE_STRIP;
            command.mesh = sharedEngine->getRenderer()->createMeshBuffer();
            command.mesh->initFromBuffer(indices.data(), sizeof(uint16_t),
                                         static_cast<uint32_t>(indices.size()), false,
                                         vertices.data(), graphics::VertexPC::ATTRIBUTES,
                                         static_cast<uint32_t>(vertices.size()), false);

            drawCommands.push_back(command);

            boundingBox.insertPoint(start);
            boundingBox.insertPoint(finish);
        }

        void DebugDrawable::circle(const Vector2& position, float radius, const graphics::Color& color, bool fill, uint32_t segments)
        {
            if (segments < 3)
            {
                return;
            }

            std::vector<uint16_t> indices;
            std::vector<graphics::VertexPC> vertices;

            if (fill)
            {
                vertices.push_back(graphics::VertexPC(Vector3(position), color));
            }

            for(uint32_t i = 0; i <= segments; ++i)
            {
                vertices.push_back(graphics::VertexPC(Vector3((position.x + radius * cosf(i * TAU / static_cast<float>(segments))),
                                                    (position.y + radius * sinf(i * TAU / static_cast<float>(segments))),
                                                    0.0f), color));
            }

            DrawCommand command;

            if (fill)
            {
                command.mode = graphics::Renderer::DrawMode::TRIANGLE_STRIP;

                for(uint16_t i = 1; i <= segments; ++i)
                {
                    indices.push_back(i);

                    if (i & 1)
                    {
                        indices.push_back(0); // center
                    }
                }

                indices.push_back(1);
            }
            else
            {
                command.mode = graphics::Renderer::DrawMode::LINE_STRIP;

                for(uint16_t i = 0; i <= segments; ++i)
                {
                    indices.push_back(i);
                }
            }

            command.mesh = sharedEngine->getRenderer()->createMeshBuffer();
            command.mesh->initFromBuffer(indices.data(), sizeof(uint16_t),
                                         static_cast<uint32_t>(indices.size()), false,
                                         vertices.data(), graphics::VertexPC::ATTRIBUTES,
                                         static_cast<uint32_t>(vertices.size()), false);

            drawCommands.push_back(command);

            boundingBox.insertPoint(Vector2(position.x - radius, position.y - radius));
            boundingBox.insertPoint(Vector2(position.x + radius, position.y + radius));
        }

        void DebugDrawable::rectangle(const Rectangle& rectangle, const graphics::Color& color, bool fill)
        {
            std::vector<uint16_t> indices;

            std::vector<graphics::VertexPC> vertices = {
                graphics::VertexPC(Vector3(rectangle.left(), rectangle.bottom(), 0.0f), color),
                graphics::VertexPC(Vector3(rectangle.right(), rectangle.bottom(), 0.0f), color),
                graphics::VertexPC(Vector3(rectangle.left(), rectangle.top(), 0.0f),  color),
                graphics::VertexPC(Vector3(rectangle.right(), rectangle.top(), 0.0f),  color)
            };

            DrawCommand command;

            if (fill)
            {
                command.mode = graphics::Renderer::DrawMode::TRIANGLE_LIST;
                indices.assign({0, 1, 2, 1, 3, 2});
            }
            else
            {
                command.mode = graphics::Renderer::DrawMode::LINE_STRIP;
                indices.assign({0, 1, 3, 2, 0});
            }

            command.mesh = sharedEngine->getRenderer()->createMeshBuffer();
            command.mesh->initFromBuffer(indices.data(), sizeof(uint16_t),
                                         static_cast<uint32_t>(indices.size()), false,
                                         vertices.data(), graphics::VertexPC::ATTRIBUTES,
                                         static_cast<uint32_t>(vertices.size()), false);

            drawCommands.push_back(command);

            boundingBox.insertPoint(Vector2(rectangle.x, rectangle.y));
            boundingBox.insertPoint(Vector2(rectangle.x + rectangle.width, rectangle.y + rectangle.height));
        }

        void DebugDrawable::triangle(const Vector2 (&positions)[3], const graphics::Color& color, bool fill)
        {
            std::vector<uint16_t> indices;
            std::vector<graphics::VertexPC> vertices;

            for(uint16_t i = 0; i < 3; ++i)
            {
                indices.push_back(i);

                vertices.push_back(graphics::VertexPC(positions[i], color));
                boundingBox.insertPoint(positions[i]);
            }

            DrawCommand command;

            if (fill)
            {
                command.mode = graphics::Renderer::DrawMode::TRIANGLE_LIST;
            }
            else
            {
                command.mode = graphics::Renderer::DrawMode::LINE_STRIP;
                indices.push_back(0);
                boundingBox.insertPoint(positions[0]);
            }

            command.mesh = sharedEngine->getRenderer()->createMeshBuffer();
            command.mesh->initFromBuffer(indices.data(), sizeof(uint16_t),
                                         static_cast<uint32_t>(indices.size()), false,
                                         vertices.data(), graphics::VertexPC::ATTRIBUTES,
                                         static_cast<uint32_t>(vertices.size()), false);

            drawCommands.push_back(command);
        }

    } // namespace scene
} // namespace ouzel

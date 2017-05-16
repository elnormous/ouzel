// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "ShapeDrawable.h"
#include "core/Engine.h"
#include "graphics/Renderer.h"
#include "graphics/MeshBufferResource.h"
#include "graphics/BufferResource.h"
#include "Camera.h"
#include "utils/Utils.h"

namespace ouzel
{
    namespace scene
    {
        ShapeDrawable::ShapeDrawable()
        {
            shader = sharedEngine->getCache()->getShader(graphics::SHADER_COLOR);
            blendState = sharedEngine->getCache()->getBlendState(graphics::BLEND_ALPHA);

            indexBuffer = std::make_shared<graphics::Buffer>();
            indexBuffer->init(graphics::Buffer::Usage::INDEX);

            vertexBuffer = std::make_shared<graphics::Buffer>();
            vertexBuffer->init(graphics::Buffer::Usage::VERTEX);

            meshBuffer = std::make_shared<graphics::MeshBuffer>();
            meshBuffer->init(sizeof(uint16_t), indexBuffer, ouzel::graphics::VertexPC::ATTRIBUTES, vertexBuffer);
        }

        void ShapeDrawable::draw(const Matrix4& transformMatrix,
                                 const Color& drawColor,
                                 const Matrix4& renderViewProjection,
                                 const std::shared_ptr<graphics::Texture>& renderTarget,
                                 const Rectangle& renderViewport,
                                 bool depthWrite,
                                 bool depthTest,
                                 bool wireframe,
                                 bool scissorTest,
                                 const Rectangle& scissorRectangle)
        {
            Component::draw(transformMatrix,
                            drawColor,
                            renderViewProjection,
                            renderTarget,
                            renderViewport,
                            depthWrite,
                            depthTest,
                            wireframe,
                            scissorTest,
                            scissorRectangle);

            if (dirty)
            {
                indexBuffer->setData(indices.data(), static_cast<uint32_t>(getVectorSize(indices)));
                vertexBuffer->setData(vertices.data(), static_cast<uint32_t>(getVectorSize(vertices)));
                dirty = false;
            }

            Matrix4 modelViewProj = renderViewProjection * transformMatrix;
            float colorVector[] = {drawColor.normR(), drawColor.normG(), drawColor.normB(), drawColor.normA()};

            for (const DrawCommand& drawCommand : drawCommands)
            {
                std::vector<std::vector<float>> pixelShaderConstants(1);
                pixelShaderConstants[0] = {std::begin(colorVector), std::end(colorVector)};

                std::vector<std::vector<float>> vertexShaderConstants(1);
                vertexShaderConstants[0] = {std::begin(modelViewProj.m), std::end(modelViewProj.m)};

                sharedEngine->getRenderer()->addDrawCommand(std::vector<std::shared_ptr<graphics::Texture>>(),
                                                            shader,
                                                            pixelShaderConstants,
                                                            vertexShaderConstants,
                                                            blendState,
                                                            meshBuffer,
                                                            drawCommand.indexCount,
                                                            drawCommand.mode,
                                                            drawCommand.startIndex,
                                                            renderTarget,
                                                            renderViewport,
                                                            depthWrite,
                                                            depthTest,
                                                            wireframe,
                                                            scissorTest,
                                                            scissorRectangle);
            }
        }

        void ShapeDrawable::clear()
        {
            boundingBox.reset();

            drawCommands.clear();
            indices.clear();
            vertices.clear();

            dirty = true;
        }

        void ShapeDrawable::line(const Vector2& start, const Vector2& finish, const Color& color, float thickness)
        {
            if (thickness <= 0.0f) return;

            DrawCommand command;
            command.startIndex = static_cast<uint32_t>(indices.size());
            command.mode = graphics::Renderer::DrawMode::TRIANGLE_LIST;
            command.indexCount = 6;

            uint16_t startVertex = static_cast<uint16_t>(vertices.size());

            Vector2 off = finish - start;
            float angle = off.getAngle();
            float sinAngle = sinf(angle);
            float cosAngle = cosf(angle);

            float halfThickness = thickness / 2.0f;

            vertices.push_back(graphics::VertexPC(start + Vector2(-halfThickness * cosAngle + halfThickness * sinAngle,
                                                                  -halfThickness * cosAngle - halfThickness * sinAngle),
                                                  color));

            vertices.push_back(graphics::VertexPC(finish + Vector2(halfThickness * cosAngle + halfThickness * sinAngle,
                                                                   -halfThickness * cosAngle + halfThickness * sinAngle),
                                                  color));

            vertices.push_back(graphics::VertexPC(start + Vector2(-halfThickness * cosAngle - halfThickness * sinAngle,
                                                                  halfThickness * cosAngle - halfThickness * sinAngle),
                                                  color));

            vertices.push_back(graphics::VertexPC(finish + Vector2(halfThickness * cosAngle - halfThickness * sinAngle,
                                                                   halfThickness * cosAngle + halfThickness * sinAngle),
                                                  color));

            indices.push_back(startVertex + 0);
            indices.push_back(startVertex + 1);
            indices.push_back(startVertex + 2);
            indices.push_back(startVertex + 1);
            indices.push_back(startVertex + 3);
            indices.push_back(startVertex + 2);

            boundingBox.insertPoint(vertices[startVertex + 0].position);
            boundingBox.insertPoint(vertices[startVertex + 1].position);
            boundingBox.insertPoint(vertices[startVertex + 2].position);
            boundingBox.insertPoint(vertices[startVertex + 3].position);

            drawCommands.push_back(command);

            dirty = true;
        }

        void ShapeDrawable::circle(const Vector2& position,
                                   float radius,
                                   const Color& color,
                                   bool fill,
                                   uint32_t segments,
                                   float thickness)
        {
            if (segments < 3) return;
            if (fill && thickness <= 0.0f) return;

            DrawCommand command;
            command.mode = graphics::Renderer::DrawMode::TRIANGLE_STRIP;
            command.indexCount = 0;
            command.startIndex = static_cast<uint32_t>(indices.size());

            uint16_t startVertex = static_cast<uint16_t>(vertices.size());

            if (fill)
            {
                vertices.push_back(graphics::VertexPC(position, color)); // center

                for (uint32_t i = 0; i <= segments; ++i)
                {
                    vertices.push_back(graphics::VertexPC(Vector3((position.v[0] + radius * cosf(i * TAU / static_cast<float>(segments))),
                                                                  (position.v[1] + radius * sinf(i * TAU / static_cast<float>(segments))),
                                                                  0.0f), color));
                }

                for (uint16_t i = 0; i < segments; ++i)
                {
                    indices.push_back(startVertex + i + 1);
                    ++command.indexCount;

                    indices.push_back(startVertex); // center
                    ++command.indexCount;
                }

                indices.push_back(startVertex + 1);
                ++command.indexCount;
            }
            else
            {
                float halfThickness = thickness / 2.0f;

                for (uint32_t i = 0; i <= segments; ++i)
                {
                    vertices.push_back(graphics::VertexPC(Vector3((position.v[0] + (radius - halfThickness) * cosf(i * TAU / static_cast<float>(segments))),
                                                                  (position.v[1] + (radius - halfThickness) * sinf(i * TAU / static_cast<float>(segments))),
                                                                  0.0f), color));

                    vertices.push_back(graphics::VertexPC(Vector3((position.v[0] + (radius + halfThickness) * cosf(i * TAU / static_cast<float>(segments))),
                                                                  (position.v[1] + (radius + halfThickness) * sinf(i * TAU / static_cast<float>(segments))),
                                                                  0.0f), color));
                }

                for (uint16_t i = 0; i < segments; ++i)
                {
                    if (i < segments - 1)
                    {
                        indices.push_back(startVertex + i * 2 + 0);
                        indices.push_back(startVertex + i * 2 + 1);
                        indices.push_back(startVertex + i * 2 + 3);

                        indices.push_back(startVertex + i * 2 + 3);
                        indices.push_back(startVertex + i * 2 + 2);
                        indices.push_back(startVertex + i * 2 + 0);
                    }
                    else
                    {
                        indices.push_back(startVertex + i * 2 + 0);
                        indices.push_back(startVertex + i * 2 + 1);
                        indices.push_back(startVertex + 1);

                        indices.push_back(startVertex + 1);
                        indices.push_back(startVertex + 0);
                        indices.push_back(startVertex + i * 2 + 0);
                    }

                    command.indexCount += 6;
                }
            }

            drawCommands.push_back(command);

            boundingBox.insertPoint(position - Vector2(radius, radius));
            boundingBox.insertPoint(position + Vector2(radius, radius));

            dirty = true;
        }

        void ShapeDrawable::rectangle(const Rectangle& rectangle,
                                      const Color& color,
                                      bool fill,
                                      float thickness)
        {
            if (fill && thickness <= 0.0f) return;

            DrawCommand command;
            command.mode = graphics::Renderer::DrawMode::TRIANGLE_LIST;
            command.startIndex = static_cast<uint32_t>(indices.size());

            uint16_t startVertex = static_cast<uint16_t>(vertices.size());

            if (fill)
            {
                vertices.push_back(graphics::VertexPC(Vector3(rectangle.left(), rectangle.bottom(), 0.0f), color));
                vertices.push_back(graphics::VertexPC(Vector3(rectangle.right(), rectangle.bottom(), 0.0f), color));
                vertices.push_back(graphics::VertexPC(Vector3(rectangle.right(), rectangle.top(), 0.0f), color));
                vertices.push_back(graphics::VertexPC(Vector3(rectangle.left(), rectangle.top(), 0.0f), color));

                command.indexCount = 6;

                indices.push_back(startVertex + 0);
                indices.push_back(startVertex + 1);
                indices.push_back(startVertex + 3);
                indices.push_back(startVertex + 1);
                indices.push_back(startVertex + 2);
                indices.push_back(startVertex + 3);
            }
            else
            {
                float halfThickness = thickness / 2.0f;

                // left bottom
                vertices.push_back(graphics::VertexPC(Vector3(rectangle.left() - halfThickness, rectangle.bottom() - halfThickness, 0.0f), color));
                vertices.push_back(graphics::VertexPC(Vector3(rectangle.left() + halfThickness, rectangle.bottom() + halfThickness, 0.0f), color));

                // right bottom
                vertices.push_back(graphics::VertexPC(Vector3(rectangle.right() + halfThickness, rectangle.bottom() - halfThickness, 0.0f), color));
                vertices.push_back(graphics::VertexPC(Vector3(rectangle.right() - halfThickness, rectangle.bottom() + halfThickness, 0.0f), color));

                // right top
                vertices.push_back(graphics::VertexPC(Vector3(rectangle.right() + halfThickness, rectangle.top() + halfThickness, 0.0f), color));
                vertices.push_back(graphics::VertexPC(Vector3(rectangle.right() - halfThickness, rectangle.top() - halfThickness, 0.0f), color));

                // left top
                vertices.push_back(graphics::VertexPC(Vector3(rectangle.left() - halfThickness, rectangle.top() + halfThickness, 0.0f), color));
                vertices.push_back(graphics::VertexPC(Vector3(rectangle.left() + halfThickness, rectangle.top() - halfThickness, 0.0f), color));

                command.indexCount = 24;
                // bottom
                indices.push_back(startVertex + 0);
                indices.push_back(startVertex + 2);
                indices.push_back(startVertex + 1);

                indices.push_back(startVertex + 2);
                indices.push_back(startVertex + 3);
                indices.push_back(startVertex + 1);

                // right
                indices.push_back(startVertex + 2);
                indices.push_back(startVertex + 4);
                indices.push_back(startVertex + 3);

                indices.push_back(startVertex + 4);
                indices.push_back(startVertex + 5);
                indices.push_back(startVertex + 3);

                // top
                indices.push_back(startVertex + 4);
                indices.push_back(startVertex + 6);
                indices.push_back(startVertex + 5);

                indices.push_back(startVertex + 6);
                indices.push_back(startVertex + 7);
                indices.push_back(startVertex + 5);

                // left
                indices.push_back(startVertex + 6);
                indices.push_back(startVertex + 0);
                indices.push_back(startVertex + 7);

                indices.push_back(startVertex + 0);
                indices.push_back(startVertex + 1);
                indices.push_back(startVertex + 7);
            }

            drawCommands.push_back(command);

            boundingBox.insertPoint(rectangle.bottomLeft());
            boundingBox.insertPoint(rectangle.topRight());

            dirty = true;
        }

        void ShapeDrawable::triangle(const Vector2 (&positions)[3],
                                     const Color& color,
                                     bool fill,
                                     float thickness)
        {
            if (fill && thickness <= 0.0f) return;

            DrawCommand command;
            command.indexCount = 3;
            command.startIndex = static_cast<uint32_t>(indices.size());

            uint16_t startVertex = static_cast<uint16_t>(vertices.size());

            for (uint16_t i = 0; i < 3; ++i)
            {
                indices.push_back(startVertex + i);
                vertices.push_back(graphics::VertexPC(positions[i], color));
                boundingBox.insertPoint(positions[i]);
            }

            if (fill)
            {
                command.mode = graphics::Renderer::DrawMode::TRIANGLE_LIST;
            }
            else
            {
                command.mode = graphics::Renderer::DrawMode::LINE_STRIP;
                indices.push_back(startVertex);
                ++command.indexCount;
            }

            drawCommands.push_back(command);

            dirty = true;
        }

        void ShapeDrawable::polygon(const std::vector<Vector2>& edges,
                                    const Color& color,
                                    bool fill,
                                    float thickness)
        {
            if (edges.size() < 3) return;
            if (fill && thickness <= 0.0f) return;

            DrawCommand command;
            command.startIndex = static_cast<uint32_t>(indices.size());

            uint16_t startVertex = static_cast<uint16_t>(vertices.size());

            for (uint16_t i = 0; i < edges.size(); ++i)
            {
                vertices.push_back(graphics::VertexPC(edges[i], color));
                boundingBox.insertPoint(edges[i]);
            }

            if (fill)
            {
                command.mode = graphics::Renderer::DrawMode::TRIANGLE_LIST;
                command.indexCount = static_cast<uint32_t>(edges.size() - 2) * 3;

                for (uint16_t i = 1; i < edges.size() - 1; ++i)
                {
                    indices.push_back(startVertex);
                    indices.push_back(startVertex + i);
                    indices.push_back(startVertex + i + 1);
                }
            }
            else
            {
                command.mode = graphics::Renderer::DrawMode::LINE_STRIP;
                command.indexCount = static_cast<uint32_t>(edges.size()) + 1;

                for (uint16_t i = 0; i < edges.size(); ++i)
                {
                    indices.push_back(startVertex + i);
                }

                indices.push_back(startVertex);
            }

            drawCommands.push_back(command);

            dirty = true;
        }

        static std::vector<uint32_t> pascalsTriangleRow(uint32_t row)
        {
            std::vector<uint32_t> ret;
            ret.push_back(1);
            for (uint32_t i = 0; i < row; ++i)
            {
                ret.push_back(ret[i] * (row - i) / (i + 1));
            }
            return ret;
        }

        void ShapeDrawable::curve(const std::vector<Vector2>& controlPoints,
                                  const Color& color,
                                  uint32_t segments,
                                  float thickness)
        {
            if (controlPoints.size() < 2) return;
            if (thickness <= 0.0f) return;

            DrawCommand command;
            command.mode = graphics::Renderer::DrawMode::LINE_STRIP;
            command.indexCount = 0;
            command.startIndex = static_cast<uint32_t>(indices.size());

            uint16_t startVertex = static_cast<uint16_t>(vertices.size());

            if (controlPoints.size() == 2)
            {
                for (uint16_t i = 0; i < controlPoints.size(); ++i)
                {
                    indices.push_back(startVertex + static_cast<uint16_t>(command.indexCount));
                    ++command.indexCount;
                    vertices.push_back(graphics::VertexPC(controlPoints[i], color));
                    boundingBox.insertPoint(controlPoints[i]);
                }
            }
            else
            {
                std::vector<uint32_t> binomialCoefficients = pascalsTriangleRow(static_cast<uint32_t>(controlPoints.size() - 1));

                for (uint32_t segment = 0; segment < segments; ++segment)
                {
                    float t = static_cast<float>(segment) / static_cast<float>(segments - 1);

                    graphics::VertexPC vertex(Vector3(), color);

                    for (uint16_t n = 0; n < controlPoints.size(); ++n)
                    {
                        vertex.position += static_cast<float>(binomialCoefficients[n]) * powf(t, n) *
                                           powf(1.0f - t, static_cast<float>(controlPoints.size() - n - 1)) * controlPoints[n];
                    }

                    indices.push_back(startVertex + static_cast<uint16_t>(command.indexCount));
                    ++command.indexCount;
                    vertices.push_back(vertex);
                    boundingBox.insertPoint(vertex.position);
                }
            }

            drawCommands.push_back(command);

            dirty = true;
        }
    } // namespace scene
} // namespace ouzel

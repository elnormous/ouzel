// Ouzel by Elviss Strazdins

#include <cassert>
#include <cmath>
#include "ShapeRenderer.hpp"
#include "Camera.hpp"
#include "../core/Engine.hpp"
#include "../graphics/Graphics.hpp"
#include "../utils/Utils.hpp"

namespace ouzel::scene
{
    ShapeRenderer::ShapeRenderer():
        shader{engine->getCache().getShader(shaderColor)},
        blendState{engine->getCache().getBlendState(blendAlpha)},
        indexBuffer{*engine->getGraphics(),
                    graphics::BufferType::index,
                    graphics::Flags::dynamic},
        vertexBuffer{*engine->getGraphics(),
                     graphics::BufferType::vertex,
                     graphics::Flags::dynamic}
    {
    }

    void ShapeRenderer::draw(const Matrix<float, 4>& transformMatrix,
                             float opacity,
                             const Matrix<float, 4>& renderViewProjection,
                             bool wireframe)
    {
        Component::draw(transformMatrix,
                        opacity,
                        renderViewProjection,
                        wireframe);

        if (dirty)
        {
            if (!indices.empty()) indexBuffer.setData(indices.data(), static_cast<std::uint32_t>(getVectorSize(indices)));
            if (!vertices.empty()) vertexBuffer.setData(vertices.data(), static_cast<std::uint32_t>(getVectorSize(vertices)));
            dirty = false;
        }

        const auto modelViewProj = renderViewProjection * transformMatrix;
        const auto colorVector = {1.0F, 1.0F, 1.0F, opacity};

        for (const DrawCommand& drawCommand : drawCommands)
        {
            std::vector<std::vector<float>> fragmentShaderConstants(1);
            fragmentShaderConstants[0] = {std::begin(colorVector), std::end(colorVector)};

            std::vector<std::vector<float>> vertexShaderConstants(1);
            vertexShaderConstants[0] = {std::begin(modelViewProj.m), std::end(modelViewProj.m)};

            engine->getGraphics()->setPipelineState(blendState->getResource(),
                                                    shader->getResource(),
                                                    graphics::CullMode::none,
                                                    wireframe ? graphics::FillMode::wireframe : graphics::FillMode::solid);
            engine->getGraphics()->setShaderConstants(fragmentShaderConstants,
                                                      vertexShaderConstants);
            engine->getGraphics()->draw(indexBuffer.getResource(),
                                        drawCommand.indexCount,
                                        sizeof(std::uint16_t),
                                        vertexBuffer.getResource(),
                                        drawCommand.mode,
                                        drawCommand.startIndex);
        }
    }

    void ShapeRenderer::clear()
    {
        boundingBox.reset();

        drawCommands.clear();
        indices.clear();
        vertices.clear();

        dirty = true;
    }

    void ShapeRenderer::line(const Vector<float, 2>& start, const Vector<float, 2>& finish, Color color, float thickness)
    {
        assert(thickness >= 0.0F);

        DrawCommand command;
        command.startIndex = static_cast<std::uint32_t>(indices.size());

        const auto startVertex = static_cast<std::uint16_t>(vertices.size());

        if (thickness == 0.0F)
        {
            command.mode = graphics::DrawMode::lineList;

            vertices.emplace_back(Vector<float, 3>{start}, color, Vector<float, 2>{}, Vector<float, 3>{0.0F, 0.0F, -1.0F});
            vertices.emplace_back(Vector<float, 3>{finish}, color, Vector<float, 2>{}, Vector<float, 3>{0.0F, 0.0F, -1.0F});

            command.indexCount = 2;

            indices.push_back(startVertex + 0);
            indices.push_back(startVertex + 1);

            boundingBox.insertPoint(Vector<float, 3>{start});
            boundingBox.insertPoint(Vector<float, 3>{finish});
        }
        else
        {
            command.mode = graphics::DrawMode::triangleList;

            const Vector<float, 2> tangent = (finish - start).normalized();
            const Vector<float, 2> normal{-tangent.v[1], tangent.v[0]};

            const float halfThickness = thickness / 2.0F;

            vertices.emplace_back(Vector<float, 3>{start - tangent * halfThickness - normal * halfThickness},
                                  color, Vector<float, 2>{}, Vector<float, 3>{0.0F, 0.0F, -1.0F});
            vertices.emplace_back(Vector<float, 3>{finish + tangent * halfThickness - normal * halfThickness},
                                  color, Vector<float, 2>{}, Vector<float, 3>{0.0F, 0.0F, -1.0F});
            vertices.emplace_back(Vector<float, 3>{start - tangent * halfThickness + normal * halfThickness},
                                  color, Vector<float, 2>{}, Vector<float, 3>{0.0F, 0.0F, -1.0F});
            vertices.emplace_back(Vector<float, 3>{finish + tangent * halfThickness + normal * halfThickness},
                                  color, Vector<float, 2>{}, Vector<float, 3>{0.0F, 0.0F, -1.0F});

            command.indexCount = 6;

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
        }

        drawCommands.push_back(command);

        dirty = true;
    }

    void ShapeRenderer::circle(const Vector<float, 2>& position,
                               float radius,
                               Color color,
                               bool fill,
                               std::uint32_t segments,
                               float thickness)
    {
        assert(radius >= 0.0F);
        assert(segments >= 3);
        assert(thickness >= 0.0F);

        DrawCommand command;
        command.startIndex = static_cast<std::uint32_t>(indices.size());

        const auto startVertex = static_cast<std::uint16_t>(vertices.size());

        if (fill)
        {
            command.mode = graphics::DrawMode::triangleStrip;

            vertices.emplace_back(Vector<float, 3>{position}, color, Vector<float, 2>{}, Vector<float, 3>{0.0F, 0.0F, -1.0F}); // center

            for (std::uint32_t i = 0; i <= segments; ++i)
                vertices.emplace_back(Vector<float, 3>{position.v[0] + radius * std::cos(i * tau<float> / static_cast<float>(segments)), position.v[1] + radius * std::sin(i * tau<float> / static_cast<float>(segments)), 0.0F},
                                      color, Vector<float, 2>{}, Vector<float, 3>{0.0F, 0.0F, -1.0F});

            command.indexCount = segments * 2 + 1;

            for (std::uint16_t i = 0; i < segments; ++i)
            {
                indices.push_back(startVertex + i + 1);
                indices.push_back(startVertex); // center
            }

            indices.push_back(startVertex + 1);

            boundingBox.insertPoint(Vector<float, 3>{position - Vector<float, 2>{radius, radius}});
            boundingBox.insertPoint(Vector<float, 3>{position + Vector<float, 2>{radius, radius}});
        }
        else
        {
            if (thickness == 0.0F)
            {
                command.mode = graphics::DrawMode::lineStrip;

                for (std::uint32_t i = 0; i <= segments; ++i)
                {
                    vertices.emplace_back(Vector<float, 3>{position.v[0] + radius * std::cos(i * tau<float> / static_cast<float>(segments)), position.v[1] + radius * std::sin(i * tau<float> / static_cast<float>(segments)), 0.0F},
                                          color, Vector<float, 2>{}, Vector<float, 3>{0.0F, 0.0F, -1.0F});
                }

                command.indexCount = segments + 1;

                for (std::uint16_t i = 0; i < segments; ++i)
                    indices.push_back(startVertex + i);

                indices.push_back(startVertex);

                boundingBox.insertPoint(Vector<float, 3>(position - Vector<float, 2>{radius, radius}));
                boundingBox.insertPoint(Vector<float, 3>(position + Vector<float, 2>{radius, radius}));
            }
            else
            {
                command.mode = graphics::DrawMode::triangleStrip;

                const float halfThickness = thickness / 2.0F;

                for (std::uint32_t i = 0; i <= segments; ++i)
                {
                    vertices.emplace_back(Vector<float, 3>{position.v[0] + (radius - halfThickness) * std::cos(i * tau<float> / static_cast<float>(segments)), position.v[1] + (radius - halfThickness) * std::sin(i * tau<float> / static_cast<float>(segments)), 0.0F},
                                          color, Vector<float, 2>{}, Vector<float, 3>{0.0F, 0.0F, -1.0F});

                    vertices.emplace_back(Vector<float, 3>{position.v[0] + (radius + halfThickness) * std::cos(i * tau<float> / static_cast<float>(segments)), position.v[1] + (radius + halfThickness) * std::sin(i * tau<float> / static_cast<float>(segments)), 0.0F},
                                          color, Vector<float, 2>{}, Vector<float, 3>{0.0F, 0.0F, -1.0F});
                }

                for (const graphics::Vertex& vertex : vertices)
                    boundingBox.insertPoint(vertex.position);

                command.indexCount = segments * 6;

                for (std::uint16_t i = 0; i < segments; ++i)
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
                }
            }
        }

        drawCommands.push_back(command);

        dirty = true;
    }

    void ShapeRenderer::rectangle(const Rect<float>& rectangle,
                                  Color color,
                                  bool fill,
                                  float thickness)
    {
        assert(thickness >= 0.0F);

        DrawCommand command;
        command.startIndex = static_cast<std::uint32_t>(indices.size());

        const auto startVertex = static_cast<std::uint16_t>(vertices.size());

        if (fill)
        {
            command.mode = graphics::DrawMode::triangleList;

            vertices.emplace_back(Vector<float, 3>{rectangle.left(), rectangle.bottom(), 0.0F},
                                  color, Vector<float, 2>{}, Vector<float, 3>{0.0F, 0.0F, -1.0F});
            vertices.emplace_back(Vector<float, 3>{rectangle.right(), rectangle.bottom(), 0.0F},
                                  color, Vector<float, 2>{}, Vector<float, 3>{0.0F, 0.0F, -1.0F});
            vertices.emplace_back(Vector<float, 3>{rectangle.right(), rectangle.top(), 0.0F},
                                  color, Vector<float, 2>{}, Vector<float, 3>{0.0F, 0.0F, -1.0F});
            vertices.emplace_back(Vector<float, 3>{rectangle.left(), rectangle.top(), 0.0F},
                                  color, Vector<float, 2>{}, Vector<float, 3>{0.0F, 0.0F, -1.0F});

            command.indexCount = 6;

            indices.push_back(startVertex + 0);
            indices.push_back(startVertex + 1);
            indices.push_back(startVertex + 3);
            indices.push_back(startVertex + 1);
            indices.push_back(startVertex + 2);
            indices.push_back(startVertex + 3);

            boundingBox.insertPoint(Vector<float, 3>{rectangle.bottomLeft()});
            boundingBox.insertPoint(Vector<float, 3>{rectangle.topRight()});
        }
        else
        {
            if (thickness == 0.0F)
            {
                command.mode = graphics::DrawMode::lineStrip;

                // left bottom
                vertices.emplace_back(Vector<float, 3>{rectangle.left(), rectangle.bottom(), 0.0F},
                                      color, Vector<float, 2>{}, Vector<float, 3>{0.0F, 0.0F, -1.0F});

                // right bottom
                vertices.emplace_back(Vector<float, 3>{rectangle.right(), rectangle.bottom(), 0.0F},
                                      color, Vector<float, 2>{}, Vector<float, 3>{0.0F, 0.0F, -1.0F});

                // right top
                vertices.emplace_back(Vector<float, 3>{rectangle.right(), rectangle.top(), 0.0F},
                                      color, Vector<float, 2>{}, Vector<float, 3>{0.0F, 0.0F, -1.0F});

                // left top
                vertices.emplace_back(Vector<float, 3>{rectangle.left(), rectangle.top(), 0.0F},
                                      color, Vector<float, 2>{}, Vector<float, 3>{0.0F, 0.0F, -1.0F});

                command.indexCount = 5;

                indices.push_back(startVertex + 0);
                indices.push_back(startVertex + 1);
                indices.push_back(startVertex + 2);
                indices.push_back(startVertex + 3);
                indices.push_back(startVertex + 0);

                boundingBox.insertPoint(Vector<float, 3>{rectangle.bottomLeft()});
                boundingBox.insertPoint(Vector<float, 3>{rectangle.topRight()});
            }
            else
            {
                command.mode = graphics::DrawMode::triangleList;

                const float halfThickness = thickness / 2.0F;

                // left bottom
                vertices.emplace_back(Vector<float, 3>{rectangle.left() - halfThickness, rectangle.bottom() - halfThickness, 0.0F},
                                      color, Vector<float, 2>{}, Vector<float, 3>{0.0F, 0.0F, -1.0F});
                vertices.emplace_back(Vector<float, 3>{rectangle.left() + halfThickness, rectangle.bottom() + halfThickness, 0.0F},
                                      color, Vector<float, 2>{}, Vector<float, 3>{0.0F, 0.0F, -1.0F});

                // right bottom
                vertices.emplace_back(Vector<float, 3>{rectangle.right() + halfThickness, rectangle.bottom() - halfThickness, 0.0F},
                                      color, Vector<float, 2>{}, Vector<float, 3>{0.0F, 0.0F, -1.0F});
                vertices.emplace_back(Vector<float, 3>{rectangle.right() - halfThickness, rectangle.bottom() + halfThickness, 0.0F},
                                      color, Vector<float, 2>{}, Vector<float, 3>{0.0F, 0.0F, -1.0F});

                // right top
                vertices.emplace_back(Vector<float, 3>{rectangle.right() + halfThickness, rectangle.top() + halfThickness, 0.0F},
                                      color, Vector<float, 2>{}, Vector<float, 3>{0.0F, 0.0F, -1.0F});
                vertices.emplace_back(Vector<float, 3>{rectangle.right() - halfThickness, rectangle.top() - halfThickness, 0.0F},
                                      color, Vector<float, 2>{}, Vector<float, 3>{0.0F, 0.0F, -1.0F});

                // left top
                vertices.emplace_back(Vector<float, 3>{rectangle.left() - halfThickness, rectangle.top() + halfThickness, 0.0F},
                                      color, Vector<float, 2>{}, Vector<float, 3>{0.0F, 0.0F, -1.0F});
                vertices.emplace_back(Vector<float, 3>{rectangle.left() + halfThickness, rectangle.top() - halfThickness, 0.0F},
                                      color, Vector<float, 2>{}, Vector<float, 3>{0.0F, 0.0F, -1.0F});

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

                boundingBox.insertPoint(Vector<float, 3>{rectangle.bottomLeft() - Vector<float, 2>{halfThickness, halfThickness}});
                boundingBox.insertPoint(Vector<float, 3>{rectangle.topRight() + Vector<float, 2>{halfThickness, halfThickness}});
            }
        }

        drawCommands.push_back(command);

        dirty = true;
    }

    void ShapeRenderer::polygon(const std::vector<Vector<float, 2>>& edges,
                                Color color,
                                bool fill,
                                float thickness)
    {
        assert(edges.size() >= 3);
        assert(thickness >= 0.0F);

        DrawCommand command;
        command.startIndex = static_cast<std::uint32_t>(indices.size());

        const auto startVertex = static_cast<std::uint16_t>(vertices.size());

        if (fill)
        {
            command.mode = graphics::DrawMode::triangleList;

            for (std::uint16_t i = 0; i < edges.size(); ++i)
                vertices.emplace_back(Vector<float, 3>{edges[i]}, color, Vector<float, 2>{}, Vector<float, 3>{0.0F, 0.0F, -1.0F});

            command.indexCount = static_cast<std::uint32_t>(edges.size() - 2) * 3;

            for (std::uint16_t i = 1; i < edges.size() - 1; ++i)
            {
                indices.push_back(startVertex);
                indices.push_back(startVertex + i);
                indices.push_back(startVertex + i + 1);
            }

            for (std::uint16_t i = 0; i < edges.size(); ++i)
                boundingBox.insertPoint(Vector<float, 3>(edges[i]));
        }
        else
        {
            if (thickness == 0.0F)
            {
                command.mode = graphics::DrawMode::lineStrip;

                for (std::uint16_t i = 0; i < edges.size(); ++i)
                    vertices.emplace_back(Vector<float, 3>{edges[i]}, color, Vector<float, 2>{}, Vector<float, 3>{0.0F, 0.0F, -1.0F});

                command.indexCount = static_cast<std::uint32_t>(edges.size()) + 1;

                for (std::uint16_t i = 0; i < edges.size(); ++i)
                    indices.push_back(startVertex + i);

                indices.push_back(startVertex);

                for (std::uint16_t i = 0; i < edges.size(); ++i)
                    boundingBox.insertPoint(Vector<float, 3>(edges[i]));
            }
            else
            {
                // TODO: implement
            }
        }

        drawCommands.push_back(command);

        dirty = true;
    }

    namespace
    {
        std::vector<std::uint32_t> pascalsTriangleRow(std::uint32_t row)
        {
            std::vector<std::uint32_t> ret;
            ret.push_back(1);
            for (std::uint32_t i = 0; i < row; ++i)
                ret.push_back(ret[i] * (row - i) / (i + 1));

            return ret;
        }
    }

    void ShapeRenderer::curve(const std::vector<Vector<float, 2>>& controlPoints,
                              Color color,
                              std::uint32_t segments,
                              float thickness)
    {
        assert(controlPoints.size() >= 2);
        assert(thickness >= 0.0F);

        DrawCommand command;
        command.startIndex = static_cast<std::uint32_t>(indices.size());

        const auto startVertex = static_cast<std::uint16_t>(vertices.size());

        if (thickness == 0.0F)
        {
            command.mode = graphics::DrawMode::lineStrip;
            command.indexCount = 0;

            if (controlPoints.size() == 2)
            {
                for (std::uint16_t i = 0; i < controlPoints.size(); ++i)
                {
                    indices.push_back(startVertex + static_cast<std::uint16_t>(command.indexCount));
                    ++command.indexCount;
                    vertices.emplace_back(Vector<float, 3>{controlPoints[i]}, color, Vector<float, 2>{}, Vector<float, 3>{0.0F, 0.0F, -1.0F});
                    boundingBox.insertPoint(Vector<float, 3>{controlPoints[i]});
                }
            }
            else
            {
                const std::vector<std::uint32_t> binomialCoefficients = pascalsTriangleRow(static_cast<std::uint32_t>(controlPoints.size() - 1));

                for (std::uint32_t segment = 0; segment < segments; ++segment)
                {
                    const auto t = static_cast<float>(segment) / static_cast<float>(segments - 1);
                    Vector<float, 2> position{};

                    for (std::uint16_t n = 0; n < controlPoints.size(); ++n)
                        position += static_cast<float>(binomialCoefficients[n]) *
                            std::pow(t, static_cast<float>(n)) *
                            std::pow(1.0F - t, static_cast<float>(controlPoints.size() - n - 1)) *
                            controlPoints[n];

                    graphics::Vertex vertex(Vector<float, 3>{position}, color, Vector<float, 2>{}, Vector<float, 3>{0.0F, 0.0F, -1.0F});

                    indices.push_back(startVertex + static_cast<std::uint16_t>(command.indexCount));
                    ++command.indexCount;
                    vertices.push_back(vertex);
                    boundingBox.insertPoint(vertex.position);
                }
            }
        }
        else
        {
            // TODO: implement
        }

        drawCommands.push_back(command);

        dirty = true;
    }
}

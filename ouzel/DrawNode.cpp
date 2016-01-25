// Copyright (C) 2015 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "DrawNode.h"
#include "Engine.h"
#include "Renderer.h"
#include "Cache.h"
#include "Layer.h"
#include "Camera.h"

namespace ouzel
{
    DrawNode::DrawNode()
    {
        _shader = Engine::getInstance()->getCache()->getShader(SHADER_COLOR);
        
#ifdef OUZEL_PLATFORM_WINDOWS
        _uniModelViewProj = 0;
#else
        _uniModelViewProj = _shader->getVertexShaderConstantId("modelViewProj");
#endif
    }
    
    void DrawNode::draw()
    {
        Node::draw();
        
        LayerPtr layer = _layer.lock();
        
        if (_shader && layer)
        {
            Engine::getInstance()->getRenderer()->activateShader(_shader);
            
            Matrix4 modelViewProj = layer->getProjection() * layer->getCamera()->getTransform() * _transform;
            
            _shader->setVertexShaderConstant(_uniModelViewProj, { modelViewProj });
            
            for (const DrawCommand& drawCommand : _drawCommands)
            {
                Engine::getInstance()->getRenderer()->drawMeshBuffer(drawCommand.mesh, 0, drawCommand.mode);
            }
        }
    }
    
    void DrawNode::clear()
    {
        _boundingBox = AABB2();
        
        _drawCommands.clear();
    }
    
    void DrawNode::point(const Vector2& position, const Color& color)
    {
        std::vector<uint16_t> indices = {0};
        
        std::vector<VertexPC> vertices = {
            VertexPC(Vector3(position), color)
        };
        
        DrawCommand command;
        
        command.mode = Renderer::DrawMode::POINT_LIST;
        command.mesh = Engine::getInstance()->getRenderer()->createMeshBuffer(indices.data(), sizeof(uint16_t),
                                                                              static_cast<uint32_t>(indices.size()), false,
                                                                              vertices.data(), sizeof(VertexPC),
                                                                              static_cast<uint32_t>(vertices.size()), false,
                                                                              VertexPC::ATTRIBUTES);
        
        _drawCommands.push_back(command);
    }
    
    void DrawNode::line(const Vector2& start, const Vector2& finish, const Color& color)
    {
        std::vector<uint16_t> indices = {0, 1};
        
        std::vector<VertexPC> vertices = {
            VertexPC(Vector3(start), color),
            VertexPC(Vector3(finish), color)
        };
        
        DrawCommand command;

        command.mode = Renderer::DrawMode::LINE_STRIP;
        command.mesh = Engine::getInstance()->getRenderer()->createMeshBuffer(indices.data(), sizeof(uint16_t),
                                                                              static_cast<uint32_t>(indices.size()), false,
                                                                              vertices.data(), sizeof(VertexPC),
                                                                              static_cast<uint32_t>(vertices.size()), false,
                                                                              VertexPC::ATTRIBUTES);
        
        _drawCommands.push_back(command);
    }
    
    void DrawNode::circle(const Vector2& position, float radius, const Color& color, bool fill)
    {
        
    }
    
    void DrawNode::rectangle(const Rectangle& rectangle, const Color& color, bool fill)
    {
        std::vector<VertexPC> vertices = {
            VertexPC(Vector3(rectangle.left(), rectangle.bottom(), 0.0f), color),
            VertexPC(Vector3(rectangle.right(), rectangle.bottom(), 0.0f), color),
            VertexPC(Vector3(rectangle.left(), rectangle.top(), 0.0f),  color),
            VertexPC(Vector3(rectangle.right(), rectangle.top(), 0.0f),  color)
        };
        
        DrawCommand command;
        
        if (fill)
        {
            std::vector<uint16_t> indices = {0, 1, 2, 1, 3, 2};
            
            command.mode = Renderer::DrawMode::TRIANGLE_LIST;
            command.mesh = Engine::getInstance()->getRenderer()->createMeshBuffer(indices.data(), sizeof(uint16_t),
                                                                                  static_cast<uint32_t>(indices.size()), false,
                                                                                  vertices.data(), sizeof(VertexPC),
                                                                                  static_cast<uint32_t>(vertices.size()), false,
                                                                                  VertexPC::ATTRIBUTES);
        }
        else
        {
            std::vector<uint16_t> indices = {0, 1, 3, 2, 0};
            
            command.mode = Renderer::DrawMode::LINE_STRIP;
            command.mesh = Engine::getInstance()->getRenderer()->createMeshBuffer(indices.data(), sizeof(uint16_t),
                                                                                  static_cast<uint32_t>(indices.size()), false,
                                                                                  vertices.data(), sizeof(VertexPC),
                                                                                  static_cast<uint32_t>(vertices.size()), false,
                                                                                  VertexPC::ATTRIBUTES);
        }
        
        _drawCommands.push_back(command);
    }
}

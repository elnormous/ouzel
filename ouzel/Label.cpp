// Copyright (C) 2015 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "Label.h"
#include "Engine.h"
#include "Renderer.h"
#include "Layer.h"
#include "Camera.h"
#include "BMFont.h"
#include "Cache.h"

namespace ouzel
{
    std::shared_ptr<Label> Label::create(const std::string& font, const std::string& text, const Vector2& textAnchor)
    {
        std::shared_ptr<Label> result = std::make_shared<Label>();
        
        if (!result->init(font, text, textAnchor))
        {
            result.reset();
        }
        
        return result;
    }
    
    Label::Label()
    {
        _shader = Engine::getInstance()->getCache()->getShader(SHADER_TEXTURE);
        
#ifdef OUZEL_PLATFORM_WINDOWS
        _uniModelViewProj = 0;
#else
        _uniModelViewProj = _shader->getVertexShaderConstantId("modelViewProj");
#endif
    }
    
    Label::~Label()
    {
        
    }
    
    bool Label::init(const std::string& font, const std::string& text, const Vector2& textAnchor)
    {
        BMFont bmFont;
        bmFont.loadFont(font);
        
        _texture = bmFont.getTexture();
        
        if (!_texture)
        {
            return false;
        }
        
        _meshBuffer = bmFont.createMeshBuffer(text, Color(255, 255, 255, 255), textAnchor);
        
        return true;
    }
    
    void Label::draw()
    {
        Widget::draw();
        
        LayerPtr layer = _layer.lock();
        
        if (_shader && _texture && layer)
        {
            Engine::getInstance()->getRenderer()->activateTexture(_texture, 0);
            Engine::getInstance()->getRenderer()->activateShader(_shader);
            
            Matrix4 modelViewProj = layer->getProjection() * layer->getCamera()->getTransform() * _transform;
            
            _shader->setVertexShaderConstant(_uniModelViewProj, { modelViewProj });

            Engine::getInstance()->getRenderer()->drawMeshBuffer(_meshBuffer);
        }
    }
}

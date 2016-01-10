// Copyright (C) 2015 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "Label.h"
#include "Engine.h"
#include "Renderer.h"
#include "Layer.h"
#include "Camera.h"
#include "BMFont.h"

namespace ouzel
{
    Label::Label(std::string const& font, std::string const& text)
    {
        BMFont bmFont;
        bmFont.loadFont(font);
        
        _texture = bmFont.getTexture();
        
        _meshBuffer = bmFont.createMeshBuffer(text);
        
        _shader = Engine::getInstance()->getRenderer()->getShader(SHADER_TEXTURE);
        
#ifdef OUZEL_PLATFORM_WINDOWS
        _uniModelViewProj = 0;
#else
        _uniModelViewProj = _shader->getVertexShaderConstantId("modelViewProj");
#endif
    }
    
    Label::~Label()
    {
        
    }
    
    void Label::draw()
    {
        Widget::draw();
        
        std::shared_ptr<Layer> layer = _layer.lock();
        
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

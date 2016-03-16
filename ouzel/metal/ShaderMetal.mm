// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "Engine.h"
#include "RendererMetal.h"
#include "FileSystem.h"
#include "ShaderMetal.h"
#include "Utils.h"

namespace ouzel
{
    namespace video
    {
        ShaderMetal::ShaderMetal():
            Shader()
        {
        }

        ShaderMetal::~ShaderMetal()
        {
            
        }
        
        bool ShaderMetal::initFromBuffers(const uint8_t* fragmentShader, uint32_t fragmentShaderSize, const uint8_t* vertexShader, uint32_t vertexShaderSize, uint32_t vertexAttributes)
        {
            if (!Shader::initFromBuffers(fragmentShader, fragmentShaderSize, vertexShader, vertexShaderSize, vertexAttributes))
            {
                return false;
            }
            
            return true;
        }
        
        uint32_t ShaderMetal::getPixelShaderConstantId(const std::string& name)
        {
            return 0;
        }
        
        bool ShaderMetal::setPixelShaderConstant(uint32_t index, const std::vector<Vector3>& vectors)
        {
            return true;
        }
        
        bool ShaderMetal::setPixelShaderConstant(uint32_t index, const std::vector<Vector4>& vectors)
        {
            return true;
        }
        
        bool ShaderMetal::setPixelShaderConstant(uint32_t index, const std::vector<Matrix4>& matrices)
        {
            return true;
        }
        
        uint32_t ShaderMetal::getVertexShaderConstantId(const std::string& name)
        {
            return 0;
        }
        
        bool ShaderMetal::setVertexShaderConstant(uint32_t index, const std::vector<Vector3>& vectors)
        {
            return true;
        }
        
        bool ShaderMetal::setVertexShaderConstant(uint32_t index, const std::vector<Vector4>& vectors)
        {
            return true;
        }
        
        bool ShaderMetal::setVertexShaderConstant(uint32_t index, const std::vector<Matrix4>& matrices)
        {
            return true;
        }
    } // namespace video
} // namespace ouzel

// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "Engine.h"
#include "RendererMetal.h"
#include "FileSystem.h"
#include "ShaderMetal.h"
#include "RendererMetal.h"
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
            destroy();
        }

        void ShaderMetal::destroy()
        {
            if (_vertexShader) [_vertexShader release];
            if (_fragmentShader) [_fragmentShader release];
        }

        bool ShaderMetal::initFromBuffers(const uint8_t* fragmentShader,
                                          uint32_t fragmentShaderSize,
                                          const uint8_t* vertexShader,
                                          uint32_t vertexShaderSize,
                                          uint32_t vertexAttributes,
                                          const std::string& fragmentShaderFunction,
                                          const std::string& vertexShaderFunction)
        {
            if (!Shader::initFromBuffers(fragmentShader, fragmentShaderSize, vertexShader, vertexShaderSize, vertexAttributes, fragmentShaderFunction, vertexShaderFunction))
            {
                return false;
            }

            destroy();

            std::shared_ptr<RendererMetal> rendererMetal = std::static_pointer_cast<RendererMetal>(sharedEngine->getRenderer());

            NSError* err = Nil;

            dispatch_data_t fragmentShaderData = dispatch_data_create(fragmentShader, fragmentShaderSize, NULL, DISPATCH_DATA_DESTRUCTOR_DEFAULT);
            id<MTLLibrary> fragmentShaderLibrary = [rendererMetal->getDevice() newLibraryWithData:fragmentShaderData error:&err];

            if (err != Nil)
            {
                log("Failed to load fragment shader");
                return false;
            }

            _fragmentShader = [fragmentShaderLibrary newFunctionWithName:[NSString stringWithUTF8String:fragmentShaderFunction.c_str()]];

            [fragmentShaderLibrary release];

            dispatch_data_t vertexShaderData = dispatch_data_create(vertexShader, vertexShaderSize, NULL, DISPATCH_DATA_DESTRUCTOR_DEFAULT);
            id<MTLLibrary> vertexShaderLibrary = [rendererMetal->getDevice() newLibraryWithData:vertexShaderData error:&err];

            if (err != Nil)
            {
                log("Failed to load vertex shader");
                return false;
            }

            _vertexShader = [vertexShaderLibrary newFunctionWithName:[NSString stringWithUTF8String:vertexShaderFunction.c_str()]];

            [vertexShaderLibrary release];

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

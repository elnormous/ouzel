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
        static const size_t BufferSize = 1024 * 1024;

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
            if (_vertexShader)
            {
                [_vertexShader release];
                _vertexShader = Nil;
            }
            if (_pixelShader)
            {
                [_pixelShader release];
                _pixelShader = Nil;
            }
            if (_vertexDescriptor)
            {
                [_vertexDescriptor release];
                _vertexDescriptor = Nil;
            }
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

            NSUInteger offset = 0;
            
            _vertexDescriptor = [MTLVertexDescriptor new];

            if (vertexAttributes & VERTEX_POSITION)
            {
                _vertexDescriptor.attributes[0].format = MTLVertexFormatFloat3;
                _vertexDescriptor.attributes[0].offset = offset;
                _vertexDescriptor.attributes[0].bufferIndex = 0;
                offset += 3 * sizeof(float);
            }

            if (vertexAttributes & VERTEX_COLOR)
            {
                _vertexDescriptor.attributes[1].format = MTLVertexFormatUChar4;
                _vertexDescriptor.attributes[1].offset = offset;
                _vertexDescriptor.attributes[1].bufferIndex = 0;
                offset += 4 * sizeof(uint8_t);
            }

            if (vertexAttributes & VERTEX_NORMAL)
            {
                _vertexDescriptor.attributes[1].format = MTLVertexFormatFloat3;
                _vertexDescriptor.attributes[1].offset = offset;
                _vertexDescriptor.attributes[1].bufferIndex = 0;
                offset += 3 * sizeof(float);
            }

            if (vertexAttributes & VERTEX_TEXCOORD0)
            {
                _vertexDescriptor.attributes[2].format = MTLVertexFormatFloat2;
                _vertexDescriptor.attributes[2].offset = offset;
                _vertexDescriptor.attributes[2].bufferIndex = 0;
                offset += 2 * sizeof(float);
            }

            if (vertexAttributes & VERTEX_TEXCOORD1)
            {
                _vertexDescriptor.attributes[2].format = MTLVertexFormatFloat2;
                _vertexDescriptor.attributes[2].offset = offset;
                _vertexDescriptor.attributes[2].bufferIndex = 0;
                offset += 2 * sizeof(float);
            }

            _vertexDescriptor.layouts[0].stride = offset;
            _vertexDescriptor.layouts[0].stepRate = 1;
            _vertexDescriptor.layouts[0].stepFunction = MTLVertexStepFunctionPerVertex;

            NSError* err = Nil;

            dispatch_data_t fragmentShaderData = dispatch_data_create(fragmentShader, fragmentShaderSize, NULL, DISPATCH_DATA_DESTRUCTOR_DEFAULT);
            id<MTLLibrary> fragmentShaderLibrary = [rendererMetal->getDevice() newLibraryWithData:fragmentShaderData error:&err];
            [fragmentShaderData release];

            if (err != Nil)
            {
                if (fragmentShaderLibrary) [fragmentShaderLibrary release];
                log("Failed to load fragment shader");
                return false;
            }

            _pixelShader = [fragmentShaderLibrary newFunctionWithName:[NSString stringWithUTF8String:fragmentShaderFunction.c_str()]];

            [fragmentShaderLibrary release];

            if (!_pixelShader)
            {
                log("Failed to get function from shader");
                return false;
            }

            dispatch_data_t vertexShaderData = dispatch_data_create(vertexShader, vertexShaderSize, NULL, DISPATCH_DATA_DESTRUCTOR_DEFAULT);
            id<MTLLibrary> vertexShaderLibrary = [rendererMetal->getDevice() newLibraryWithData:vertexShaderData error:&err];
            [vertexShaderData release];

            if (err != Nil)
            {
                if (vertexShaderLibrary) [vertexShaderLibrary release];
                log("Failed to load vertex shader");
                return false;
            }

            _vertexShader = [vertexShaderLibrary newFunctionWithName:[NSString stringWithUTF8String:vertexShaderFunction.c_str()]];

            [vertexShaderLibrary release];

            if (!_vertexShader)
            {
                log("Failed to get function from shader");
                return false;
            }

            if (!createPixelShaderConstantBuffer(BufferSize))
            {
                return false;
            }

            if (!createVertexShaderConstantBuffer(BufferSize))
            {
                return false;
            }

            return true;
        }

        uint32_t ShaderMetal::getPixelShaderConstantId(const std::string& name)
        {
            log("getPixelShaderConstantId not available for Metal");
            return 0;
        }

        bool ShaderMetal::setPixelShaderConstant(uint32_t index, const std::vector<Vector3>& vectors)
        {
            uint32_t size = index + vectorDataSize(vectors);
            if (size > _pixelShaderData.size())
            {
                if (_pixelShaderConstantBuffer) [_pixelShaderConstantBuffer release];
                createPixelShaderConstantBuffer(size);
            }

            memcpy(_pixelShaderData.data() + index, vectors.data(), vectorDataSize(vectors));
            return uploadData(_pixelShaderConstantBuffer, _pixelShaderData.data(), _pixelShaderData.size());
        }

        bool ShaderMetal::setPixelShaderConstant(uint32_t index, const std::vector<Vector4>& vectors)
        {
            uint32_t size = index + vectorDataSize(vectors);
            if (size > _pixelShaderData.size())
            {
                if (_pixelShaderConstantBuffer) [_pixelShaderConstantBuffer release];
                createPixelShaderConstantBuffer(size);
            }

            memcpy(_pixelShaderData.data() + index, vectors.data(), vectorDataSize(vectors));
            return uploadData(_pixelShaderConstantBuffer, _pixelShaderData.data(), _pixelShaderData.size());
        }

        bool ShaderMetal::setPixelShaderConstant(uint32_t index, const std::vector<Matrix4>& matrices)
        {
            uint32_t size = index + vectorDataSize(matrices);
            if (size > _pixelShaderData.size())
            {
                if (_pixelShaderConstantBuffer) [_pixelShaderConstantBuffer release];
                createPixelShaderConstantBuffer(size);
            }

            memcpy(_pixelShaderData.data() + index, matrices.data(), vectorDataSize(matrices));
            return uploadData(_pixelShaderConstantBuffer, _pixelShaderData.data(), _pixelShaderData.size());
        }

        uint32_t ShaderMetal::getVertexShaderConstantId(const std::string& name)
        {
            log("getVertexShaderConstantId not available for Metal");
            return 0;
        }

        bool ShaderMetal::setVertexShaderConstant(uint32_t index, const std::vector<Vector3>& vectors)
        {
            uint32_t size = index + vectorDataSize(vectors);
            if (size > _vertexShaderData.size())
            {
                if (_vertexShaderConstantBuffer) [_vertexShaderConstantBuffer release];
                createVertexShaderConstantBuffer(size);
            }

            memcpy(_vertexShaderData.data() + index, vectors.data(), vectorDataSize(vectors));
            return uploadData(_vertexShaderConstantBuffer, _vertexShaderData.data(), _vertexShaderData.size());
        }

        bool ShaderMetal::setVertexShaderConstant(uint32_t index, const std::vector<Vector4>& vectors)
        {
            uint32_t size = index + vectorDataSize(vectors);
            if (size > _vertexShaderData.size())
            {
                if (_vertexShaderConstantBuffer) [_vertexShaderConstantBuffer release];
                createVertexShaderConstantBuffer(size);
            }

            memcpy(_vertexShaderData.data() + index, vectors.data(), vectorDataSize(vectors));
            return uploadData(_vertexShaderConstantBuffer, _vertexShaderData.data(), _vertexShaderData.size());
        }

        bool ShaderMetal::setVertexShaderConstant(uint32_t index, const std::vector<Matrix4>& matrices)
        {
            uint32_t size = index + vectorDataSize(matrices);
            if (size > _vertexShaderData.size())
            {
                if (_vertexShaderConstantBuffer) [_vertexShaderConstantBuffer release];
                createVertexShaderConstantBuffer(size);
            }

            memcpy(_vertexShaderData.data() + index, matrices.data(), vectorDataSize(matrices));
            return uploadData(_vertexShaderConstantBuffer, _vertexShaderData.data(), _vertexShaderData.size());
        }

        bool ShaderMetal::createPixelShaderConstantBuffer(uint32_t size)
        {
            std::shared_ptr<RendererMetal> rendererMetal = std::static_pointer_cast<RendererMetal>(sharedEngine->getRenderer());

            _pixelShaderConstantBuffer = [rendererMetal->getDevice() newBufferWithLength:size
                                                                                 options:MTLResourceCPUCacheModeWriteCombined];

            if (_pixelShaderConstantBuffer == Nil)
            {
                log("Failed to create Metal index buffer");
                return false;
            }

            _pixelShaderData.resize(size);

            return true;
        }

        bool ShaderMetal::createVertexShaderConstantBuffer(uint32_t size)
        {
            std::shared_ptr<RendererMetal> rendererMetal = std::static_pointer_cast<RendererMetal>(sharedEngine->getRenderer());

            _vertexShaderConstantBuffer = [rendererMetal->getDevice() newBufferWithLength:size
                                                                                  options:MTLResourceCPUCacheModeWriteCombined];

            if (_vertexShaderConstantBuffer == Nil)
            {
                log("Failed to create Metal constant buffer");
                return false;
            }

            _vertexShaderData.resize(size);

            return true;
        }

        bool ShaderMetal::uploadData(MTLBufferPtr buffer, const void* data, uint32_t size)
        {
            memcpy([buffer contents], data, size);

            return true;
        }
    } // namespace video
} // namespace ouzel

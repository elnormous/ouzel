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

        bool ShaderMetal::initFromBuffers(const uint8_t* pixelShader,
                                          uint32_t pixelShaderSize,
                                          const uint8_t* vertexShader,
                                          uint32_t vertexShaderSize,
                                          uint32_t vertexAttributes,
                                          const std::string& pixelShaderFunction,
                                          const std::string& vertexShaderFunction)
        {
            if (!Shader::initFromBuffers(pixelShader, pixelShaderSize, vertexShader, vertexShaderSize, vertexAttributes, pixelShaderFunction, vertexShaderFunction))
            {
                return false;
            }

            destroy();

            std::shared_ptr<RendererMetal> rendererMetal = std::static_pointer_cast<RendererMetal>(sharedEngine->getRenderer());

            uint32_t index = 0;
            NSUInteger offset = 0;
            
            _vertexDescriptor = [MTLVertexDescriptor new];

            if (vertexAttributes & VERTEX_POSITION)
            {
                _vertexDescriptor.attributes[index].format = MTLVertexFormatFloat3;
                _vertexDescriptor.attributes[index].offset = offset;
                _vertexDescriptor.attributes[index].bufferIndex = 0;
                index++;
                offset += 3 * sizeof(float);
            }

            if (vertexAttributes & VERTEX_COLOR)
            {
                _vertexDescriptor.attributes[index].format = MTLVertexFormatUChar4Normalized;
                _vertexDescriptor.attributes[index].offset = offset;
                _vertexDescriptor.attributes[index].bufferIndex = 0;
                index++;
                offset += 4 * sizeof(uint8_t);
            }

            if (vertexAttributes & VERTEX_NORMAL)
            {
                _vertexDescriptor.attributes[index].format = MTLVertexFormatFloat3;
                _vertexDescriptor.attributes[index].offset = offset;
                _vertexDescriptor.attributes[index].bufferIndex = 0;
                index++;
                offset += 3 * sizeof(float);
            }

            if (vertexAttributes & VERTEX_TEXCOORD0)
            {
                _vertexDescriptor.attributes[index].format = MTLVertexFormatFloat2;
                _vertexDescriptor.attributes[index].offset = offset;
                _vertexDescriptor.attributes[index].bufferIndex = 0;
                index++;
                offset += 2 * sizeof(float);
            }

            if (vertexAttributes & VERTEX_TEXCOORD1)
            {
                _vertexDescriptor.attributes[index].format = MTLVertexFormatFloat2;
                _vertexDescriptor.attributes[index].offset = offset;
                _vertexDescriptor.attributes[index].bufferIndex = 0;
                index++;
                offset += 2 * sizeof(float);
            }

            _vertexDescriptor.layouts[0].stride = offset;
            _vertexDescriptor.layouts[0].stepRate = 1;
            _vertexDescriptor.layouts[0].stepFunction = MTLVertexStepFunctionPerVertex;

            NSError* err = Nil;

            dispatch_data_t pixelShaderData = dispatch_data_create(pixelShader, pixelShaderSize, NULL, DISPATCH_DATA_DESTRUCTOR_DEFAULT);
            id<MTLLibrary> pixelShaderLibrary = [rendererMetal->getDevice() newLibraryWithData:pixelShaderData error:&err];
            [pixelShaderData release];

            if (err != Nil)
            {
                if (pixelShaderLibrary) [pixelShaderLibrary release];
                log("Failed to load pixel shader");
                return false;
            }

            _pixelShader = [pixelShaderLibrary newFunctionWithName:[NSString stringWithUTF8String:pixelShaderFunction.c_str()]];

            [pixelShaderLibrary release];

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

            if (!createPixelShaderConstantBuffer(sizeof(Matrix4)))
            {
                return false;
            }

            if (!createVertexShaderConstantBuffer(sizeof(Matrix4)))
            {
                return false;
            }

            return true;
        }

        bool ShaderMetal::setPixelShaderConstantInfo(const std::vector<ConstantInfo>& constantInfo)
        {
            Shader::setPixelShaderConstantInfo(constantInfo);

            _pixelShaderConstantLocations.reserve(constantInfo.size());

            uint32_t offset = 0;

            for (const ConstantInfo& info : _pixelShaderConstantInfo)
            {
                _pixelShaderConstantLocations.push_back(offset);
                offset += info.size;
            }

            if (offset > _pixelShaderData.size())
            {
                if (_pixelShaderConstantBuffer) [_pixelShaderConstantBuffer release];
                createPixelShaderConstantBuffer(offset);
            }

            return true;
        }

        bool ShaderMetal::setVertexShaderConstantInfo(const std::vector<ConstantInfo>& constantInfo)
        {
            Shader::setVertexShaderConstantInfo(constantInfo);

            _vertexShaderConstantLocations.reserve(constantInfo.size());

            uint32_t offset = 0;

            for (const ConstantInfo& info : _vertexShaderConstantInfo)
            {
                _vertexShaderConstantLocations.push_back(offset);
                offset += info.size;
            }

            if (offset > _vertexShaderData.size())
            {
                if (_vertexShaderConstantBuffer) [_vertexShaderConstantBuffer release];
                createVertexShaderConstantBuffer(offset);
            }

            return true;
        }

        void ShaderMetal::nextBuffers()
        {
            _pixelShaderConstantBufferOffset += _pixelShaderData.size();
            _pixelShaderConstantBufferOffset = (_pixelShaderConstantBufferOffset / 256 + 1) * 256;

            if (BufferSize - _pixelShaderConstantBufferOffset < 256)
            {
                _pixelShaderConstantBufferOffset = 0;
            }

            _vertexShaderConstantBufferOffset += _vertexShaderData.size();
            _vertexShaderConstantBufferOffset = (_vertexShaderConstantBufferOffset / 256 + 1) * 256;

            if (BufferSize - _vertexShaderConstantBufferOffset < 256)
            {
                _vertexShaderConstantBufferOffset = 0;
            }
        }

        bool ShaderMetal::setPixelShaderConstant(uint32_t index, const std::vector<Vector3>& vectors)
        {
            if (index >= _pixelShaderConstantLocations.size()) return false;

            uint32_t location = _pixelShaderConstantLocations[index];
            memcpy(_pixelShaderData.data() + location, vectors.data(), vectorDataSize(vectors));

            return uploadData(_pixelShaderConstantBuffer,
                              _pixelShaderConstantBufferOffset,
                              _pixelShaderData.data(),
                              _pixelShaderData.size());
        }

        bool ShaderMetal::setPixelShaderConstant(uint32_t index, const std::vector<Vector4>& vectors)
        {
            if (index >= _pixelShaderConstantLocations.size()) return false;

            uint32_t location = _pixelShaderConstantLocations[index];
            memcpy(_pixelShaderData.data() + location, vectors.data(), vectorDataSize(vectors));

            return uploadData(_pixelShaderConstantBuffer,
                              _pixelShaderConstantBufferOffset,
                              _pixelShaderData.data(),
                              _pixelShaderData.size());
        }

        bool ShaderMetal::setPixelShaderConstant(uint32_t index, const std::vector<Matrix4>& matrices)
        {
            if (index >= _pixelShaderConstantLocations.size()) return false;

            uint32_t location = _pixelShaderConstantLocations[index];
            memcpy(_pixelShaderData.data() + location, matrices.data(), vectorDataSize(matrices));

            return uploadData(_pixelShaderConstantBuffer,
                              _pixelShaderConstantBufferOffset,
                              _pixelShaderData.data(),
                              _pixelShaderData.size());
        }

        bool ShaderMetal::setVertexShaderConstant(uint32_t index, const std::vector<Vector3>& vectors)
        {
            if (index >= _vertexShaderConstantLocations.size()) return false;

            uint32_t location = _vertexShaderConstantLocations[index];
            memcpy(_vertexShaderData.data() + location, vectors.data(), vectorDataSize(vectors));

            return uploadData(_vertexShaderConstantBuffer,
                              _vertexShaderConstantBufferOffset,
                              _vertexShaderData.data(),
                              _vertexShaderData.size());
        }

        bool ShaderMetal::setVertexShaderConstant(uint32_t index, const std::vector<Vector4>& vectors)
        {
            if (index >= _vertexShaderConstantLocations.size()) return false;

            uint32_t location = _vertexShaderConstantLocations[index];
            memcpy(_vertexShaderData.data() + location, vectors.data(), vectorDataSize(vectors));

            return uploadData(_vertexShaderConstantBuffer,
                              _vertexShaderConstantBufferOffset,
                              _vertexShaderData.data(),
                              _vertexShaderData.size());
        }

        bool ShaderMetal::setVertexShaderConstant(uint32_t index, const std::vector<Matrix4>& matrices)
        {
            if (index >= _vertexShaderConstantLocations.size()) return false;

            uint32_t location = _vertexShaderConstantLocations[index];
            memcpy(_vertexShaderData.data() + location, matrices.data(), vectorDataSize(matrices));

            return uploadData(_vertexShaderConstantBuffer,
                              _vertexShaderConstantBufferOffset,
                              _vertexShaderData.data(),
                              _vertexShaderData.size());
        }

        bool ShaderMetal::createPixelShaderConstantBuffer(uint32_t size)
        {
            std::shared_ptr<RendererMetal> rendererMetal = std::static_pointer_cast<RendererMetal>(sharedEngine->getRenderer());

            _pixelShaderConstantBuffer = [rendererMetal->getDevice() newBufferWithLength:BufferSize
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

            _vertexShaderConstantBuffer = [rendererMetal->getDevice() newBufferWithLength:BufferSize
                                                                                  options:MTLResourceCPUCacheModeWriteCombined];

            if (_vertexShaderConstantBuffer == Nil)
            {
                log("Failed to create Metal constant buffer");
                return false;
            }

            _vertexShaderData.resize(size);

            return true;
        }

        bool ShaderMetal::uploadData(MTLBufferPtr buffer, uint32_t offset, const void* data, uint32_t size)
        {
            char* contents = static_cast<char*>([buffer contents]);
            memcpy((contents + offset), data, size);

            return true;
        }
    } // namespace video
} // namespace ouzel

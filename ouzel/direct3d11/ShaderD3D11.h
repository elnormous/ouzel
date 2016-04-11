// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include <d3d11.h>
#include "CompileConfig.h"
#include "Shader.h"

namespace ouzel
{
    namespace video
    {
        class RendererD3D11;

        class ShaderD3D11: public Shader
        {
            friend RendererD3D11;
        public:
            virtual ~ShaderD3D11();

            virtual bool initFromBuffers(const uint8_t* fragmentShader,
                                         uint32_t fragmentShaderSize,
                                         const uint8_t* vertexShader,
                                         uint32_t vertexShaderSize,
                                         uint32_t vertexAttributes,
                                         const std::string& fragmentShaderFunction = "",
                                         const std::string& vertexShaderFunction = "") override;

            virtual ID3D11PixelShader* getPixelShader() const { return _pixelShader; }
            virtual ID3D11VertexShader* getVertexShader() const { return _vertexShader; }

            virtual ID3D11Buffer* getPixelShaderConstantBuffer() const { return _pixelShaderConstantBuffer; }
            virtual ID3D11Buffer* getVertexShaderConstantBuffer() const { return _vertexShaderConstantBuffer; }
            virtual ID3D11InputLayout* getInputLayout() const { return _inputLayout;  }

            virtual uint32_t getPixelShaderConstantId(const std::string& name) override;
            virtual bool setPixelShaderConstant(uint32_t index, const std::vector<Vector3>& vectors);
            virtual bool setPixelShaderConstant(uint32_t index, const std::vector<Vector4>& vectors);
            virtual bool setPixelShaderConstant(uint32_t index, const std::vector<Matrix4>& matrices);

            virtual uint32_t getVertexShaderConstantId(const std::string& name) override;
            virtual bool setVertexShaderConstant(uint32_t index, const std::vector<Vector3>& vectors);
            virtual bool setVertexShaderConstant(uint32_t index, const std::vector<Vector4>& vectors);
            virtual bool setVertexShaderConstant(uint32_t index, const std::vector<Matrix4>& matrices);

        protected:
            ShaderD3D11();

            void destroy();
            bool createPixelShaderConstantBuffer(uint32_t size);
            bool createVertexShaderConstantBuffer(uint32_t size);

            bool uploadData(ID3D11Buffer* buffer, const void* data, uint32_t size);

            ID3D11PixelShader* _pixelShader = nullptr;
            ID3D11VertexShader* _vertexShader = nullptr;
            ID3D11InputLayout* _inputLayout = nullptr;

            ID3D11Buffer* _pixelShaderConstantBuffer = nullptr;
            std::vector<char> _pixelShaderData;

            ID3D11Buffer* _vertexShaderConstantBuffer = nullptr;
            std::vector<char> _vertexShaderData;
        };
    } // namespace video
} // namespace ouzel

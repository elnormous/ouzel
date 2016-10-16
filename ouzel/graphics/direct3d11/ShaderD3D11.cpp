// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#include <fstream>
#include "ShaderD3D11.h"
#include "core/Engine.h"
#include "RendererD3D11.h"
#include "files/FileSystem.h"
#include "utils/Log.h"

namespace ouzel
{
    namespace graphics
    {
        ShaderD3D11::ShaderD3D11()
        {
        }

        ShaderD3D11::~ShaderD3D11()
        {
            if (pixelShader)
            {
                pixelShader->Release();
            }

            if (vertexShader)
            {
                vertexShader->Release();
            }

            if (inputLayout)
            {
                inputLayout->Release();
            }

            if (pixelShaderConstantBuffer)
            {
                pixelShaderConstantBuffer->Release();
            }

            if (vertexShaderConstantBuffer)
            {
                vertexShaderConstantBuffer->Release();
            }
        }

        void ShaderD3D11::free()
        {
            Shader::free();

            pixelShaderConstantLocations.clear();
            vertexShaderConstantLocations.clear();

            if (pixelShader)
            {
                pixelShader->Release();
                pixelShader = nullptr;
            }

            if (vertexShader)
            {
                vertexShader->Release();
                vertexShader = nullptr;
            }

            if (inputLayout)
            {
                inputLayout->Release();
                inputLayout = nullptr;
            }

            if (pixelShaderConstantBuffer)
            {
                pixelShaderConstantBuffer->Release();
                pixelShaderConstantBuffer = nullptr;
            }

            if (vertexShaderConstantBuffer)
            {
                vertexShaderConstantBuffer->Release();
                vertexShaderConstantBuffer = nullptr;
            }
        }

        bool ShaderD3D11::uploadBuffer(ID3D11Buffer* buffer, const void* data, uint32_t size)
        {
            RendererD3D11* rendererD3D11 = static_cast<RendererD3D11*>(sharedEngine->getRenderer());

            D3D11_MAPPED_SUBRESOURCE mappedSubresource;
            HRESULT hr = rendererD3D11->getContext()->Map(buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedSubresource);
            if (FAILED(hr))
            {
                Log(Log::Level::ERR) << "Failed to lock Direct3D 11 buffer";
                return false;
            }

            memcpy(mappedSubresource.pData, data, size);

            rendererD3D11->getContext()->Unmap(buffer, 0);

            return true;
        }

        bool ShaderD3D11::upload()
        {
            if (uploadData.dirty)
            {
                RendererD3D11* rendererD3D11 = static_cast<RendererD3D11*>(sharedEngine->getRenderer());

                if (!pixelShader)
                {
                    HRESULT hr = rendererD3D11->getDevice()->CreatePixelShader(uploadData.pixelShaderData.data(), uploadData.pixelShaderData.size(), NULL, &pixelShader);
                    if (FAILED(hr))
                    {
                        Log(Log::Level::ERR) << "Failed to create a Direct3D 11 pixel shader";
                        return false;
                    }
                }

                if (!vertexShader)
                {
                    HRESULT hr = rendererD3D11->getDevice()->CreateVertexShader(uploadData.vertexShaderData.data(), uploadData.vertexShaderData.size(), NULL, &vertexShader);
                    if (FAILED(hr))
                    {
                        Log(Log::Level::ERR) << "Failed to create a Direct3D 11 vertex shader";
                        return false;
                    }

                    std::vector<D3D11_INPUT_ELEMENT_DESC> vertexInputElements;

                    UINT offset = 0;

                    if (uploadData.vertexAttributes & VERTEX_POSITION)
                    {
                        vertexInputElements.push_back({ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, offset, D3D11_INPUT_PER_VERTEX_DATA, 0 });
                        offset += 3 * sizeof(float);
                    }

                    if (uploadData.vertexAttributes & VERTEX_COLOR)
                    {
                        vertexInputElements.push_back({ "COLOR", 0, DXGI_FORMAT_R8G8B8A8_UNORM, 0, offset, D3D11_INPUT_PER_VERTEX_DATA, 0 });
                        offset += 4 * sizeof(uint8_t);
                    }

                    if (uploadData.vertexAttributes & VERTEX_NORMAL)
                    {
                        vertexInputElements.push_back({ "NORMAL", 0, DXGI_FORMAT_R32G32_FLOAT, 0, offset, D3D11_INPUT_PER_VERTEX_DATA, 0 });
                        offset += 3 * sizeof(float);
                    }

                    if (uploadData.vertexAttributes & VERTEX_TEXCOORD0)
                    {
                        vertexInputElements.push_back({ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, offset, D3D11_INPUT_PER_VERTEX_DATA, 0 });
                        offset += 2 * sizeof(float);
                    }

                    if (uploadData.vertexAttributes & VERTEX_TEXCOORD1)
                    {
                        vertexInputElements.push_back({ "TEXCOORD", 1, DXGI_FORMAT_R32G32_FLOAT, 0, offset, D3D11_INPUT_PER_VERTEX_DATA, 0 });
                        offset += 2 * sizeof(float);
                    }

                    hr = rendererD3D11->getDevice()->CreateInputLayout(
                        vertexInputElements.data(),
                        static_cast<UINT>(vertexInputElements.size()),
                        uploadData.vertexShaderData.data(),
                        uploadData.vertexShaderData.size(),
                        &inputLayout);

                    if (FAILED(hr))
                    {
                        Log(Log::Level::ERR) << "Failed to create Direct3D 11 input layout for vertex shader";
                        return false;
                    }
                }

                pixelShaderConstantLocations.clear();
                pixelShaderConstantLocations.reserve(uploadData.pixelShaderConstantInfo.size());

                pixelShaderConstantSize = 0;

                for (const ConstantInfo& info : uploadData.pixelShaderConstantInfo)
                {
                    pixelShaderConstantLocations.push_back({ pixelShaderConstantSize, info.size });
                    pixelShaderConstantSize += info.size;
                }

                if (pixelShaderConstantBuffer) pixelShaderConstantBuffer->Release();

                D3D11_BUFFER_DESC pixelShaderConstantBufferDesc;
                memset(&pixelShaderConstantBufferDesc, 0, sizeof(pixelShaderConstantBufferDesc));

                pixelShaderConstantBufferDesc.ByteWidth = static_cast<UINT>(pixelShaderConstantSize);
                pixelShaderConstantBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
                pixelShaderConstantBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
                pixelShaderConstantBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

                HRESULT hr = rendererD3D11->getDevice()->CreateBuffer(&pixelShaderConstantBufferDesc, nullptr, &pixelShaderConstantBuffer);
                if (FAILED(hr))
                {
                    Log(Log::Level::ERR) << "Failed to create Direct3D 11 constant buffer";
                    return false;
                }

                vertexShaderConstantSize = 0;

                for (const ConstantInfo& info : uploadData.vertexShaderConstantInfo)
                {
                    vertexShaderConstantLocations.push_back({ vertexShaderConstantSize, info.size });
                    vertexShaderConstantSize += info.size;
                }

                if (vertexShaderConstantBuffer) vertexShaderConstantBuffer->Release();

                D3D11_BUFFER_DESC vertexShaderConstantBufferDesc;
                memset(&vertexShaderConstantBufferDesc, 0, sizeof(vertexShaderConstantBufferDesc));

                vertexShaderConstantBufferDesc.ByteWidth = static_cast<UINT>(vertexShaderConstantSize);
                vertexShaderConstantBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
                vertexShaderConstantBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
                vertexShaderConstantBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

                hr = rendererD3D11->getDevice()->CreateBuffer(&vertexShaderConstantBufferDesc, nullptr, &vertexShaderConstantBuffer);
                if (FAILED(hr))
                {
                    Log(Log::Level::ERR) << "Failed to create Direct3D 11 constant buffer";
                    return false;
                }

                uploadData.dirty = false;
            }

            return true;
        }
    } // namespace graphics
} // namespace ouzel

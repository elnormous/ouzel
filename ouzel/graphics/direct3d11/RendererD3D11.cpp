// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "RendererD3D11.h"
#include "core/Engine.h"
#include "TextureD3D11.h"
#include "ShaderD3D11.h"
#include "MeshBufferD3D11.h"
#include "IndexBufferD3D11.h"
#include "VertexBufferD3D11.h"
#include "RenderTargetD3D11.h"
#include "utils/Log.h"
#include "TexturePSD3D11.h"
#include "TextureVSD3D11.h"
#include "ColorPSD3D11.h"
#include "ColorVSD3D11.h"
#include "scene/Camera.h"
#include "core/Cache.h"
#include "events/EventDispatcher.h"
#include "BlendStateD3D11.h"
#include "core/windows/WindowWin.h"
#include "stb_image_write.h"

using namespace ouzel;

namespace ouzel
{
    namespace graphics
    {
        RendererD3D11::RendererD3D11():
            Renderer(Driver::DIRECT3D11), dirty(false)
        {
            apiMajorVersion = 11;
            apiMinorVersion = 0;
            memset(&rasterizerStates, 0, sizeof(rasterizerStates));
            memset(&resourceViews, 0, sizeof(resourceViews));
            memset(&samplerStates, 0, sizeof(samplerStates));
        }

        RendererD3D11::~RendererD3D11()
        {
            if (depthStencilState)
            {
                depthStencilState->Release();
            }

            for (uint32_t i = 0; i < 4; ++i)
            {
                if (rasterizerStates[i])
                {
                    rasterizerStates[i]->Release();
                }
            }

            if (samplerState)
            {
                samplerState->Release();
            }

            if (renderTargetView)
            {
                renderTargetView->Release();
            }

            if (backBuffer)
            {
                backBuffer->Release();
            }

            if (swapChain)
            {
                swapChain->Release();
            }

            if (adapter)
            {
                adapter->Release();
            }
        }

        void RendererD3D11::free()
        {
            Renderer::free();

            if (depthStencilState)
            {
                depthStencilState->Release();
                depthStencilState = nullptr;
            }

            for (uint32_t i = 0; i < 4; ++i)
            {
                if (rasterizerStates[i])
                {
                    rasterizerStates[i]->Release();
                    rasterizerStates[i] = nullptr;
                }
            }

            if (samplerState)
            {
                samplerState->Release();
                samplerState = nullptr;
            }

            if (renderTargetView)
            {
                renderTargetView->Release();
                renderTargetView = nullptr;
            }

            if (backBuffer)
            {
                backBuffer->Release();
                backBuffer= nullptr;
            }

            if (swapChain)
            {
                swapChain->Release();
                swapChain = nullptr;
            }

            if (adapter)
            {
                adapter->Release();
                adapter = nullptr;
            }
        }

        bool RendererD3D11::init(const WindowPtr& newWindow,
                                 uint32_t newSampleCount,
                                 TextureFilter newTextureFilter,
                                 PixelFormat newBackBufferFormat,
                                 bool newVerticalSync)
        {
            std::lock_guard<std::mutex> lock(dataMutex);

            if (!Renderer::init(newWindow, newSampleCount, newTextureFilter, newBackBufferFormat, newVerticalSync))
            {
                return false;
            }

            free();

            UINT deviceCreationFlags = 0;
#ifdef DEBUG
            deviceCreationFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

            HRESULT hr = D3D11CreateDevice(
                nullptr, // adapter
                D3D_DRIVER_TYPE_HARDWARE,
                nullptr, // software rasterizer (unused)
                deviceCreationFlags,
                nullptr, // feature levels
                0, // no feature levels
                D3D11_SDK_VERSION,
                &device,
                nullptr,
                &context);

            if (FAILED(hr))
            {
                Log(Log::Level::ERR) << "Failed to create the Direct3D 11 device";
                return false;
            }

            if (device->GetFeatureLevel() < D3D_FEATURE_LEVEL_10_0)
            {
                npotTexturesSupported = false;
            }

            IDXGIDevice* dxgiDevice;
            IDXGIFactory* factory;

            device->QueryInterface(IID_IDXGIDevice, (void**)&dxgiDevice);
            dxgiDevice->GetParent(IID_IDXGIAdapter, (void**)&adapter);
            hr = adapter->GetParent(IID_IDXGIFactory, (void**)&factory);
            if (FAILED(hr))
            {
                Log(Log::Level::ERR) << "Failed to get the DXGI factory";
                return false;
            }

            DXGI_ADAPTER_DESC adapterDesc;
            hr = adapter->GetDesc(&adapterDesc);
            if (FAILED(hr))
            {
                Log(Log::Level::ERR) << "Failed to get the DXGI adapter description";
            }
            else
            {
                char deviceName[256];
                WideCharToMultiByte(CP_UTF8, 0, adapterDesc.Description, -1, deviceName, sizeof(deviceName), nullptr, nullptr);
                Log(Log::Level::INFO) << "Using " << deviceName << " for rendering";
            }

            std::shared_ptr<WindowWin> windowWin = std::static_pointer_cast<WindowWin>(window);

            DXGI_SWAP_CHAIN_DESC swapChainDesc;
            memset(&swapChainDesc, 0, sizeof(swapChainDesc));

            width = static_cast<UINT>(size.width);
            height = static_cast<UINT>(size.height);

            UINT qualityLevels;

            if (FAILED(device->CheckMultisampleQualityLevels(DXGI_FORMAT_R8G8B8A8_UNORM, sampleCount, &qualityLevels)))
            {
                Log(Log::Level::WARN) << "Failed to check Direct3D 11 multisample quality levels";
            }

            if (qualityLevels == 0)
            {
                Log(Log::Level::WARN) << "Direct3D 11 device does not support chosen sample count, multisampling disabled";
                sampleCount = 1;
            }

            swapChainDesc.BufferDesc.Width = width;
            swapChainDesc.BufferDesc.Height = height;
            swapChainDesc.BufferDesc.RefreshRate.Numerator = 0;
            swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
            swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
            swapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_PROGRESSIVE;
            swapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_STRETCHED;
            swapChainDesc.SampleDesc.Count = sampleCount;
            swapChainDesc.SampleDesc.Quality = 0;
            swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
            swapChainDesc.BufferCount = 1;
            swapChainDesc.OutputWindow = windowWin->getNativeWindow();
            swapChainDesc.Windowed = TRUE;
            swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
            swapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

            swapInterval = verticalSync ? 1 : 0;

            hr = factory->CreateSwapChain(device, &swapChainDesc, &swapChain);
            if (FAILED(hr))
            {
                Log(Log::Level::ERR) << "Failed to create the Direct3D 11 swap chain";
                return false;
            }

            factory->MakeWindowAssociation(windowWin->getNativeWindow(), DXGI_MWA_NO_ALT_ENTER);

            factory->Release();
            dxgiDevice->Release();

            // Backbuffer
            hr = swapChain->GetBuffer(0, IID_ID3D11Texture2D, reinterpret_cast<void**>(&backBuffer));
            if (FAILED(hr))
            {
                Log(Log::Level::ERR) << "Failed to retrieve Direct3D 11 backbuffer";
                return false;
            }

            hr = device->CreateRenderTargetView(backBuffer, nullptr, &renderTargetView);
            if (FAILED(hr))
            {
                Log(Log::Level::ERR) << "Failed to create Direct3D 11 render target view";
                return false;
            }

            // Sampler state
            D3D11_SAMPLER_DESC samplerStateDesc;
            switch (textureFilter)
            {
                case TextureFilter::NONE:
                    samplerStateDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
                    break;
                case TextureFilter::LINEAR:
                    samplerStateDesc.Filter = D3D11_FILTER_MIN_MAG_POINT_MIP_LINEAR;
                    break;
                case TextureFilter::BILINEAR:
                    samplerStateDesc.Filter = D3D11_FILTER_MIN_MAG_LINEAR_MIP_POINT;
                    break;
                case TextureFilter::TRILINEAR:
                    samplerStateDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
                    break;
            }
            samplerStateDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
            samplerStateDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
            samplerStateDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
            samplerStateDesc.MipLODBias = 0.0f;
            samplerStateDesc.MaxAnisotropy = 1;
            samplerStateDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;

            samplerStateDesc.BorderColor[0] = 0.0f;
            samplerStateDesc.BorderColor[1] = 0.0f;
            samplerStateDesc.BorderColor[2] = 0.0f;
            samplerStateDesc.BorderColor[3] = 0.0f;

            samplerStateDesc.MinLOD = 0.0f;
            samplerStateDesc.MaxLOD = D3D11_FLOAT32_MAX;

            hr = device->CreateSamplerState(&samplerStateDesc, &samplerState);
            if (FAILED(hr))
            {
                Log(Log::Level::ERR) << "Failed to create Direct3D 11 sampler state";
                return false;
            }

            // Rasterizer state
            D3D11_RASTERIZER_DESC rasterStateDesc =
            {
                D3D11_FILL_SOLID,
                D3D11_CULL_NONE,
                FALSE, // front = ccw?
                0, 0, 0, // depth bias, clamp, slope scale
                TRUE, // depth clip
                FALSE, // scissor test
                (sampleCount > 1) ? TRUE : FALSE, // MSAA
                TRUE, // AA lines
            };

            hr = device->CreateRasterizerState(&rasterStateDesc, &rasterizerStates[0]);
            if (FAILED(hr))
            {
                Log(Log::Level::ERR) << "Failed to create Direct3D 11 rasterizer state";
                return false;
            }

            // wireframe
            rasterStateDesc.FillMode = D3D11_FILL_WIREFRAME;
            rasterStateDesc.ScissorEnable = FALSE;

            hr = device->CreateRasterizerState(&rasterStateDesc, &rasterizerStates[1]);
            if (FAILED(hr))
            {
                Log(Log::Level::ERR) << "Failed to create Direct3D 11 rasterizer state";
                return false;
            }

            // scissor test
            rasterStateDesc.FillMode = D3D11_FILL_SOLID;
            rasterStateDesc.ScissorEnable = TRUE;

            hr = device->CreateRasterizerState(&rasterStateDesc, &rasterizerStates[2]);
            if (FAILED(hr))
            {
                Log(Log::Level::ERR) << "Failed to create Direct3D 11 rasterizer state";
                return false;
            }

            // wireframe and scissor test
            rasterStateDesc.FillMode = D3D11_FILL_WIREFRAME;
            rasterStateDesc.ScissorEnable = TRUE;

            hr = device->CreateRasterizerState(&rasterStateDesc, &rasterizerStates[3]);
            if (FAILED(hr))
            {
                Log(Log::Level::ERR) << "Failed to create Direct3D 11 rasterizer state";
                return false;
            }

            // Depth/stencil state
            D3D11_DEPTH_STENCIL_DESC depthStencilStateDesc =
            {
                FALSE, // disable depth test
            };
            hr = device->CreateDepthStencilState(&depthStencilStateDesc, &depthStencilState);
            if (FAILED(hr))
            {
                Log(Log::Level::ERR) << "Failed to create Direct3D 11 depth stencil state";
                return false;
            }

            ShaderPtr textureShader = createShader();
            textureShader->initFromBuffers(std::vector<uint8_t>(std::begin(TEXTURE_PIXEL_SHADER_D3D11), std::end(TEXTURE_PIXEL_SHADER_D3D11)),
                                           std::vector<uint8_t>(std::begin(TEXTURE_VERTEX_SHADER_D3D11), std::end(TEXTURE_VERTEX_SHADER_D3D11)),
                                           VertexPCT::ATTRIBUTES,
                                           {{ "color", 4 * sizeof(float) }},
                                           {{ "modelViewProj", sizeof(Matrix4) }});

            sharedEngine->getCache()->setShader(SHADER_TEXTURE, textureShader);

            ShaderPtr colorShader = createShader();
            colorShader->initFromBuffers(std::vector<uint8_t>(std::begin(COLOR_PIXEL_SHADER_D3D11), std::end(COLOR_PIXEL_SHADER_D3D11)),
                                         std::vector<uint8_t>(std::begin(COLOR_VERTEX_SHADER_D3D11), std::end(COLOR_VERTEX_SHADER_D3D11)),
                                         VertexPC::ATTRIBUTES,
                                         {{ "color", 4 * sizeof(float) }},
                                         {{ "modelViewProj", sizeof(Matrix4) }});

            sharedEngine->getCache()->setShader(SHADER_COLOR, colorShader);

            BlendStatePtr noBlendState = createBlendState();

            noBlendState->init(false,
                               BlendState::BlendFactor::ONE, BlendState::BlendFactor::ZERO,
                               BlendState::BlendOperation::ADD,
                               BlendState::BlendFactor::ONE, BlendState::BlendFactor::ZERO,
                               BlendState::BlendOperation::ADD);

            sharedEngine->getCache()->setBlendState(BLEND_NO_BLEND, noBlendState);

            BlendStatePtr addBlendState = createBlendState();

            addBlendState->init(true,
                                BlendState::BlendFactor::ONE, BlendState::BlendFactor::ONE,
                                BlendState::BlendOperation::ADD,
                                BlendState::BlendFactor::ONE, BlendState::BlendFactor::ONE,
                                BlendState::BlendOperation::ADD);

            sharedEngine->getCache()->setBlendState(BLEND_ADD, addBlendState);

            BlendStatePtr multiplyBlendState = createBlendState();

            multiplyBlendState->init(true,
                                     BlendState::BlendFactor::DEST_COLOR, BlendState::BlendFactor::ZERO,
                                     BlendState::BlendOperation::ADD,
                                     BlendState::BlendFactor::ONE, BlendState::BlendFactor::ONE,
                                     BlendState::BlendOperation::ADD);

            sharedEngine->getCache()->setBlendState(BLEND_MULTIPLY, multiplyBlendState);

            BlendStatePtr alphaBlendState = createBlendState();

            alphaBlendState->init(true,
                                  BlendState::BlendFactor::SRC_ALPHA, BlendState::BlendFactor::INV_SRC_ALPHA,
                                  BlendState::BlendOperation::ADD,
                                  BlendState::BlendFactor::ONE, BlendState::BlendFactor::ONE,
                                  BlendState::BlendOperation::ADD);

            sharedEngine->getCache()->setBlendState(BLEND_ALPHA, alphaBlendState);

            TexturePtr whitePixelTexture = createTexture();
            whitePixelTexture->initFromBuffer( { 255, 255, 255, 255 }, Size2(1.0f, 1.0f), false, false);
            sharedEngine->getCache()->setTexture(TEXTURE_WHITE_PIXEL, whitePixelTexture);

            dirty = true;
            ready = true;

            return true;
        }

        bool RendererD3D11::update()
        {
            if (dirty)
            {
                bool newSizeDirty;

                {
                    std::lock_guard<std::mutex> lock(dataMutex);

                    newSizeDirty = sizeDirty;

                    frameBufferClearColor[0] = clearColor.getR();
                    frameBufferClearColor[1] = clearColor.getG();
                    frameBufferClearColor[2] = clearColor.getB();
                    frameBufferClearColor[3] = clearColor.getA();

                    sizeDirty = false;
                    dirty = false;
                }

                if (newSizeDirty)
                {
                    if (!resizeBackBuffer(0, 0))
                    {
                        return false;
                    }

                    Log(Log::Level::INFO) << "resized back buffer: " << width << ", " << height;

                    Renderer::setSize(Size2(static_cast<float>(width),
                                            static_cast<float>(height)));
                }
            }

            return true;
        }

        void RendererD3D11::setClearColor(Color color)
        {
            std::lock_guard<std::mutex> lock(dataMutex);

            Renderer::setClearColor(color);

            dirty = true;
        }

        void RendererD3D11::setSize(const Size2&)
        {
            std::lock_guard<std::mutex> lock(dataMutex);

            sizeDirty = true;
            dirty = true;
        }

        bool RendererD3D11::present()
        {
            if (!Renderer::present())
            {
                return false;
            }

            context->RSSetState(rasterizerStates[0]);

            if (!update())
            {
                return false;
            }

            std::vector<float> shaderData;

            D3D11_VIEWPORT viewport;

            if (drawQueue.empty())
            {
                frameBufferClearedFrame = currentFrame;

                if (clear)
                {
                    context->OMSetRenderTargets(1, &renderTargetView, nullptr);

                    viewport = {
                        0.0f, 0.0f,
                        static_cast<FLOAT>(width),
                        static_cast<FLOAT>(height),
                        0.0f, 1.0f
                    };

                    context->RSSetViewports(1, &viewport);

                    context->ClearRenderTargetView(renderTargetView, frameBufferClearColor);
                }
            }
            else for (const DrawCommand& drawCommand : drawQueue)
            {
                // render target
                ID3D11RenderTargetView* newRenderTargetView = nullptr;
                const float* newClearColor;
                bool clearBuffer = false;

                viewport = {
                    drawCommand.viewport.x,
                    drawCommand.viewport.y,
                    drawCommand.viewport.width,
                    drawCommand.viewport.height,
                    0.0f, 1.0f
                };

                if (drawCommand.renderTarget)
                {
                    std::shared_ptr<RenderTargetD3D11> renderTargetD3D11 = std::static_pointer_cast<RenderTargetD3D11>(drawCommand.renderTarget);

                    if (!renderTargetD3D11->getRenderTargetView())
                    {
                        continue;
                    }

                    std::shared_ptr<TextureD3D11> renderTargetTextureD3D11 = std::static_pointer_cast<TextureD3D11>(renderTargetD3D11->getTexture());
                    viewport.TopLeftY = renderTargetTextureD3D11->getSize().height - (viewport.TopLeftY + viewport.Height);

                    newRenderTargetView = renderTargetD3D11->getRenderTargetView();
                    newClearColor = renderTargetD3D11->getFrameBufferClearColor();

                    if (renderTargetD3D11->getFrameBufferClearedFrame() != currentFrame)
                    {
                        renderTargetD3D11->setFrameBufferClearedFrame(currentFrame);
                        clearBuffer = renderTargetD3D11->getClear();
                    }
                }
                else
                {
                    newRenderTargetView = renderTargetView;
                    newClearColor = frameBufferClearColor;

                    viewport.TopLeftY = height - (viewport.TopLeftY + viewport.Height);

                    if (frameBufferClearedFrame != currentFrame)
                    {
                        frameBufferClearedFrame = currentFrame;
                        clearBuffer = clear;
                    }
                }

                context->OMSetRenderTargets(1, &newRenderTargetView, nullptr);
                context->RSSetViewports(1, &viewport);

                if (clearBuffer)
                {
                    context->ClearRenderTargetView(newRenderTargetView, newClearColor);
                }

                uint32_t rasterizerStateIndex = 0;
                if (drawCommand.wireframe) rasterizerStateIndex |= 0x01;
                if (drawCommand.scissorTestEnabled) rasterizerStateIndex |= 0x02;

                context->RSSetState(rasterizerStates[rasterizerStateIndex]);

                // scissor test
                if (drawCommand.scissorTestEnabled)
                {
                    D3D11_RECT rects[1];
                    rects[0].left = static_cast<LONG>(drawCommand.scissorTest.x);
                    rects[0].right = static_cast<LONG>(drawCommand.scissorTest.x + drawCommand.scissorTest.width);
                    rects[0].bottom = static_cast<LONG>(drawCommand.scissorTest.y);
                    rects[0].top = static_cast<LONG>(drawCommand.scissorTest.y + drawCommand.scissorTest.height);

                    context->RSSetScissorRects(1, rects);
                }

                // shader
                std::shared_ptr<ShaderD3D11> shaderD3D11 = std::static_pointer_cast<ShaderD3D11>(drawCommand.shader);

                if (!shaderD3D11 || !shaderD3D11->getPixelShader() || !shaderD3D11->getVertexShader())
                {
                    // don't render if invalid shader
                    continue;
                }

                context->PSSetShader(shaderD3D11->getPixelShader(), nullptr, 0);
                context->VSSetShader(shaderD3D11->getVertexShader(), nullptr, 0);

                context->IASetInputLayout(shaderD3D11->getInputLayout());

                // pixel shader constants
                const std::vector<ShaderD3D11::Location>& pixelShaderConstantLocations = shaderD3D11->getPixelShaderConstantLocations();

                if (drawCommand.pixelShaderConstants.size() > pixelShaderConstantLocations.size())
                {
                    Log(Log::Level::ERR) << "Invalid pixel shader constant size";
                    return false;
                }

                shaderData.clear();

                for (size_t i = 0; i < drawCommand.pixelShaderConstants.size(); ++i)
                {
                    const ShaderD3D11::Location& pixelShaderConstantLocation = pixelShaderConstantLocations[i];
                    const std::vector<float>& pixelShaderConstant = drawCommand.pixelShaderConstants[i];

                    if (sizeof(float) * pixelShaderConstant.size() != pixelShaderConstantLocation.size)
                    {
                        Log(Log::Level::ERR) << "Invalid pixel shader constant size";
                        return false;
                    }

                    shaderData.insert(shaderData.end(), pixelShaderConstant.begin(), pixelShaderConstant.end());
                }

                shaderD3D11->uploadBuffer(shaderD3D11->getPixelShaderConstantBuffer(),
                                          shaderData.data(),
                                          static_cast<uint32_t>(sizeof(float) * shaderData.size()));

                ID3D11Buffer* pixelShaderConstantBuffers[1] = { shaderD3D11->getPixelShaderConstantBuffer() };
                context->PSSetConstantBuffers(0, 1, pixelShaderConstantBuffers);

                // vertex shader constants
                const std::vector<ShaderD3D11::Location>& vertexShaderConstantLocations = shaderD3D11->getVertexShaderConstantLocations();

                if (drawCommand.vertexShaderConstants.size() > vertexShaderConstantLocations.size())
                {
                    Log(Log::Level::ERR) << "Invalid vertex shader constant size";
                    return false;
                }

                shaderData.clear();

                for (size_t i = 0; i < drawCommand.vertexShaderConstants.size(); ++i)
                {
                    const ShaderD3D11::Location& vertexShaderConstantLocation = vertexShaderConstantLocations[i];
                    const std::vector<float>& vertexShaderConstant = drawCommand.vertexShaderConstants[i];

                    if (sizeof(float) * vertexShaderConstant.size() != vertexShaderConstantLocation.size)
                    {
                        Log(Log::Level::ERR) << "Invalid pixel shader constant size";
                        return false;
                    }

                    shaderData.insert(shaderData.end(), vertexShaderConstant.begin(), vertexShaderConstant.end());
                }

                shaderD3D11->uploadBuffer(shaderD3D11->getVertexShaderConstantBuffer(),
                                          shaderData.data(),
                                          static_cast<uint32_t>(sizeof(float) * shaderData.size()));

                ID3D11Buffer* vertexShaderConstantBuffers[1] = { shaderD3D11->getVertexShaderConstantBuffer() };
                context->VSSetConstantBuffers(0, 1, vertexShaderConstantBuffers);

                // blend state
                std::shared_ptr<BlendStateD3D11> blendStateD3D11 = std::static_pointer_cast<BlendStateD3D11>(drawCommand.blendState);

                if (!blendStateD3D11 || !blendStateD3D11->getBlendState())
                {
                    // don't render if invalid blend state
                    continue;
                }

                context->OMSetBlendState(blendStateD3D11->getBlendState(), NULL, 0xffffffff);

                bool texturesValid = true;

                // textures
                for (uint32_t layer = 0; layer < Texture::LAYERS; ++layer)
                {
                    std::shared_ptr<TextureD3D11> textureD3D11;

                    if (drawCommand.textures.size() > layer)
                    {
                        textureD3D11 = std::static_pointer_cast<TextureD3D11>(drawCommand.textures[layer]);
                    }

                    if (textureD3D11)
                    {
                        if (!textureD3D11->getResourceView())
                        {
                            texturesValid = false;
                            break;
                        }

                        resourceViews[layer] = textureD3D11->getResourceView();
                        samplerStates[layer] = samplerState;
                    }
                    else
                    {
                        resourceViews[layer] = nullptr;
                        samplerStates[layer] = nullptr;
                    }
                }

                if (!texturesValid)
                {
                    continue;
                }

                context->PSSetShaderResources(0, Texture::LAYERS, resourceViews);
                context->PSSetSamplers(0, Texture::LAYERS, samplerStates);

                // mesh buffer
                std::shared_ptr<MeshBufferD3D11> meshBufferD3D11 = std::static_pointer_cast<MeshBufferD3D11>(drawCommand.meshBuffer);

                if (!meshBufferD3D11)
                {
                    // don't render if invalid mesh buffer
                    continue;
                }

                // draw
                context->OMSetDepthStencilState(depthStencilState, 0);

                std::shared_ptr<IndexBufferD3D11> indexBufferD3D11 = std::static_pointer_cast<IndexBufferD3D11>(meshBufferD3D11->getIndexBuffer());
                std::shared_ptr<VertexBufferD3D11> vertexBufferD3D11 = std::static_pointer_cast<VertexBufferD3D11>(meshBufferD3D11->getVertexBuffer());

                if (!indexBufferD3D11 || !indexBufferD3D11->getBuffer() ||
                    !vertexBufferD3D11 || !vertexBufferD3D11->getBuffer())
                {
                    continue;
                }

                ID3D11Buffer* buffers[] = { vertexBufferD3D11->getBuffer() };
                UINT strides[] = { vertexBufferD3D11->getVertexSize() };
                UINT offsets[] = { 0 };
                context->IASetVertexBuffers(0, 1, buffers, strides, offsets);
                context->IASetIndexBuffer(indexBufferD3D11->getBuffer(), indexBufferD3D11->getFormat(), 0);

                D3D_PRIMITIVE_TOPOLOGY topology;

                switch (drawCommand.drawMode)
                {
                    case DrawMode::POINT_LIST: topology = D3D_PRIMITIVE_TOPOLOGY_POINTLIST; break;
                    case DrawMode::LINE_LIST: topology = D3D_PRIMITIVE_TOPOLOGY_LINELIST; break;
                    case DrawMode::LINE_STRIP: topology = D3D_PRIMITIVE_TOPOLOGY_LINESTRIP; break;
                    case DrawMode::TRIANGLE_LIST: topology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST; break;
                    case DrawMode::TRIANGLE_STRIP: topology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP; break;
                    default: Log(Log::Level::ERR) << "Invalid draw mode";  return false;
                }

                context->IASetPrimitiveTopology(topology);

                context->DrawIndexed(drawCommand.indexCount, drawCommand.startIndex, 0);
            }

            swapChain->Present(swapInterval, 0);

            if (!saveScreenshots())
            {
                return false;
            }

            return true;
        }

        IDXGIOutput* RendererD3D11::getOutput() const
        {
            std::shared_ptr<WindowWin> windowWin = std::static_pointer_cast<WindowWin>(window);

            HMONITOR monitor = windowWin->getMonitor();

            if (!monitor)
            {
                Log(Log::Level::ERR) << "Window is not on any monitor";
                return nullptr;
            }

            UINT i = 0;
            IDXGIOutput* output;
            DXGI_OUTPUT_DESC outputDesc;
            HRESULT hr;

            while (adapter->EnumOutputs(i, &output) != DXGI_ERROR_NOT_FOUND)
            {
                hr = output->GetDesc(&outputDesc);

                if (SUCCEEDED(hr) && outputDesc.Monitor == monitor)
                {
                    return output;
                }

                output->Release();

                ++i;
            }

            return nullptr;
        }

        std::vector<Size2> RendererD3D11::getSupportedResolutions() const
        {
            std::vector<Size2> result;

            IDXGIOutput* output = getOutput();

            if (!output)
            {
                return result;
            }

            UINT numModes = 0;
            DXGI_FORMAT format = DXGI_FORMAT_R8G8B8A8_UNORM;
            HRESULT hr = output->GetDisplayModeList(format, 0, &numModes, nullptr);
            if (FAILED(hr))
            {
                Log(Log::Level::ERR) << "Failed to get display mode list";
                output->Release();
                return result;
            }

            if (numModes > 0)
            {
                std::vector<DXGI_MODE_DESC> displayModes(numModes);
                output->GetDisplayModeList(format, 0, &numModes, displayModes.data());

                for (const DXGI_MODE_DESC& displayMode : displayModes)
                {
                    result.push_back(Size2(static_cast<float>(displayMode.Width),
                                           static_cast<float>(displayMode.Height)));
                }
            }

            output->Release();

            return result;
        }

        BlendStatePtr RendererD3D11::createBlendState()
        {
            std::shared_ptr<BlendStateD3D11> blendState = std::make_shared<BlendStateD3D11>();
            return blendState;
        }

        TexturePtr RendererD3D11::createTexture()
        {
            std::shared_ptr<TextureD3D11> texture = std::make_shared<TextureD3D11>();
            return texture;
        }

        RenderTargetPtr RendererD3D11::createRenderTarget()
        {
            std::shared_ptr<RenderTargetD3D11> renderTarget = std::make_shared<RenderTargetD3D11>();
            return renderTarget;
        }

        ShaderPtr RendererD3D11::createShader()
        {
            std::shared_ptr<ShaderD3D11> shader = std::make_shared<ShaderD3D11>();
            return shader;
        }

        MeshBufferPtr RendererD3D11::createMeshBuffer()
        {
            std::shared_ptr<MeshBufferD3D11> meshBuffer = std::make_shared<MeshBufferD3D11>();
            return meshBuffer;
        }

        IndexBufferPtr RendererD3D11::createIndexBuffer()
        {
            std::shared_ptr<IndexBufferD3D11> meshBuffer = std::make_shared<IndexBufferD3D11>();
            return meshBuffer;
        }

        VertexBufferPtr RendererD3D11::createVertexBuffer()
        {
            std::shared_ptr<VertexBufferD3D11> meshBuffer = std::make_shared<VertexBufferD3D11>();
            return meshBuffer;
        }

        bool RendererD3D11::saveScreenshots()
        {
            for (;;)
            {
                std::string filename;

                {
                    std::lock_guard<std::mutex> lock(screenshotMutex);

                    if (screenshotQueue.empty()) break;

                    filename = screenshotQueue.front();
                    screenshotQueue.pop();
                }

                ID3D11Texture2D* backBufferTexture;
                HRESULT hr = backBuffer->QueryInterface(IID_ID3D11Texture2D, reinterpret_cast<void**>(&backBufferTexture));

                if (FAILED(hr))
                {
                    Log(Log::Level::ERR) << "Failed to get Direct3D 11 back buffer texture";
                    return false;
                }

                D3D11_TEXTURE2D_DESC backBufferDesc;
                backBufferTexture->GetDesc(&backBufferDesc);

                D3D11_TEXTURE2D_DESC desc = backBufferDesc;
                desc.SampleDesc.Count = 1;
                desc.SampleDesc.Quality = 0;
                desc.Usage = D3D11_USAGE_STAGING;
                desc.BindFlags = 0;
                desc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
                desc.MiscFlags = 0;

                ID3D11Texture2D* texture;

                hr = device->CreateTexture2D(&desc, nullptr, &texture);

                if (FAILED(hr))
                {
                    Log(Log::Level::ERR) << "Failed to create Direct3D 11 texture";
                    return false;
                }

                if (backBufferDesc.SampleDesc.Count > 1)
                {
                    D3D11_TEXTURE2D_DESC tempDesc = backBufferDesc;
                    tempDesc.SampleDesc.Count = 1;
                    tempDesc.SampleDesc.Quality = 0;

                    ID3D11Texture2D* temp;
                    hr = device->CreateTexture2D(&tempDesc, nullptr, &temp);
                    if (FAILED(hr))
                    {
                        texture->Release();
                        Log(Log::Level::ERR) << "Failed to create Direct3D 11 texture";
                        return false;
                    }

                    for (UINT item = 0; item < backBufferDesc.ArraySize; ++item)
                    {
                        for (UINT level = 0; level < desc.MipLevels; ++level)
                        {
                            UINT index = D3D11CalcSubresource(level, item, backBufferDesc.MipLevels);
                            context->ResolveSubresource(temp, index, backBuffer, index, DXGI_FORMAT_R8G8B8A8_UNORM);
                        }
                    }

                    context->CopyResource(texture, temp);
                    temp->Release();
                }
                else
                {
                    context->CopyResource(texture, backBuffer);
                }

                D3D11_MAPPED_SUBRESOURCE mappedSubresource;
                hr = context->Map(texture, 0, D3D11_MAP_READ, 0, &mappedSubresource);

                if (FAILED(hr))
                {
                    texture->Release();
                    Log(Log::Level::ERR) << "Failed to map Direct3D 11 resource";
                    return false;
                }

                if (!stbi_write_png(filename.c_str(), desc.Width, desc.Height, 4, mappedSubresource.pData, static_cast<int>(mappedSubresource.RowPitch)))
                {
                    context->Unmap(texture, 0);
                    texture->Release();
                    Log(Log::Level::ERR) << "Failed to save screenshot to file";
                    return false;
                }

                context->Unmap(texture, 0);
                texture->Release();
            }

            return true;
        }

        bool RendererD3D11::resizeBackBuffer(UINT newWidth, UINT newHeight)
        {
            if (width != newWidth || newWidth == 0 ||
                height != newHeight || newHeight == 0)
            {
                if (renderTargetView)
                {
                    renderTargetView->Release();
                    renderTargetView = nullptr;
                }

                if (backBuffer)
                {
                    backBuffer->Release();
                    backBuffer = nullptr;
                }

                HRESULT hr = swapChain->ResizeBuffers(0, newWidth, newHeight, DXGI_FORMAT_UNKNOWN, DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH);
                if (FAILED(hr))
                {
                    Log(Log::Level::ERR) << "Failed to resize Direct3D 11 backbuffer";
                    return false;
                }

                hr = swapChain->GetBuffer(0, IID_ID3D11Texture2D, (void**)&backBuffer);
                if (FAILED(hr))
                {
                    Log(Log::Level::ERR) << "Failed to retrieve Direct3D 11 backbuffer";
                    return false;
                }

                hr = device->CreateRenderTargetView(backBuffer, nullptr, &renderTargetView);
                if (FAILED(hr))
                {
                    Log(Log::Level::ERR) << "Failed to create Direct3D 11 render target view";
                    return false;
                }

                D3D11_TEXTURE2D_DESC desc;
                backBuffer->GetDesc(&desc);

                width = desc.Width;
                height = desc.Height;
            }

            return true;
        }
    } // namespace graphics
} // namespace ouzel

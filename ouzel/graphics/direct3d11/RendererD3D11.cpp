// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "core/CompileConfig.h"

#if OUZEL_SUPPORTS_DIRECT3D11

#include "RendererD3D11.h"
#include "BlendStateResourceD3D11.h"
#include "TextureResourceD3D11.h"
#include "ShaderResourceD3D11.h"
#include "MeshBufferResourceD3D11.h"
#include "BufferResourceD3D11.h"
#include "TexturePSD3D11.h"
#include "TextureVSD3D11.h"
#include "ColorPSD3D11.h"
#include "ColorVSD3D11.h"
#include "core/Cache.h"
#include "core/Engine.h"
#include "utils/Log.h"
#include "core/windows/WindowWin.h"
#include "stb_image_write.h"

namespace ouzel
{
    namespace graphics
    {
        RendererD3D11::RendererD3D11():
            Renderer(Driver::DIRECT3D11)
        {
            apiMajorVersion = 11;
            apiMinorVersion = 0;

            std::fill(std::begin(rasterizerStates), std::end(rasterizerStates), nullptr);
            std::fill(std::begin(depthStencilStates), std::end(depthStencilStates), nullptr);
        }

        RendererD3D11::~RendererD3D11()
        {
            resourceDeleteSet.clear();
            resources.clear();

            for (uint32_t state = 0; state < 4; ++state)
            {
                if (depthStencilStates[state])
                {
                    depthStencilStates[state]->Release();
                }
            }

            if (depthStencilView)
            {
                depthStencilView->Release();
            }

            if (depthStencilTexture)
            {
                depthStencilTexture->Release();
            }

            for (uint32_t i = 0; i < 4; ++i)
            {
                if (rasterizerStates[i])
                {
                    rasterizerStates[i]->Release();
                }
            }

            for (const auto& samplerState : samplerStates)
            {
                samplerState.second->Release();
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

            if (context)
            {
                context->Release();
            }

            if (device)
            {
                device->Release();
            }
        }

        bool RendererD3D11::init(Window* newWindow,
                                 const Size2& newSize,
                                 uint32_t newSampleCount,
                                 Texture::Filter newTextureFilter,
                                 uint32_t newMaxAnisotropy,
                                 bool newVerticalSync,
                                 bool newDepth,
                                 bool newDebugRenderer)
        {
            if (!Renderer::init(newWindow,
                                newSize,
                                newSampleCount,
                                newTextureFilter,
                                newMaxAnisotropy,
                                newVerticalSync,
                                newDepth,
                                newDebugRenderer))
            {
                return false;
            }

            UINT deviceCreationFlags = 0;

            if (debugRenderer)
            {
                deviceCreationFlags |= D3D11_CREATE_DEVICE_DEBUG;
            }

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
                Log(Log::Level::ERR) << "Failed to create the Direct3D 11 device, error: " << hr;
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
                Log(Log::Level::ERR) << "Failed to get the DXGI factory, error: " << hr;
                return false;
            }

            DXGI_ADAPTER_DESC adapterDesc;
            hr = adapter->GetDesc(&adapterDesc);
            if (FAILED(hr))
            {
                Log(Log::Level::ERR) << "Failed to get the DXGI adapter description, error: " << hr;
            }
            else
            {
                char deviceName[256];
                WideCharToMultiByte(CP_UTF8, 0, adapterDesc.Description, -1, deviceName, sizeof(deviceName), nullptr, nullptr);
                Log(Log::Level::INFO) << "Using " << deviceName << " for rendering";
            }

            WindowWin* windowWin = static_cast<WindowWin*>(window);

            frameBufferWidth = static_cast<UINT>(newSize.v[0]);
            frameBufferHeight = static_cast<UINT>(newSize.v[1]);

            UINT qualityLevels;
            hr = device->CheckMultisampleQualityLevels(DXGI_FORMAT_R8G8B8A8_UNORM, sampleCount, &qualityLevels);
            if (FAILED(hr))
            {
                Log(Log::Level::WARN) << "Failed to check Direct3D 11 multisample quality levels, error: " << hr;
            }

            if (qualityLevels == 0)
            {
                Log(Log::Level::WARN) << "Direct3D 11 device does not support chosen sample count, multisampling disabled";
                sampleCount = 1;
            }

            DXGI_SWAP_CHAIN_DESC swapChainDesc;
            swapChainDesc.BufferDesc.Width = frameBufferWidth;
            swapChainDesc.BufferDesc.Height = frameBufferHeight;
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
                Log(Log::Level::ERR) << "Failed to create the Direct3D 11 swap chain, error: " << hr;
                return false;
            }

            factory->MakeWindowAssociation(windowWin->getNativeWindow(), DXGI_MWA_NO_ALT_ENTER);

            factory->Release();
            dxgiDevice->Release();

            // Backbuffer
            hr = swapChain->GetBuffer(0, IID_ID3D11Texture2D, reinterpret_cast<void**>(&backBuffer));
            if (FAILED(hr))
            {
                Log(Log::Level::ERR) << "Failed to retrieve Direct3D 11 backbuffer, error: " << hr;
                return false;
            }

            hr = device->CreateRenderTargetView(backBuffer, nullptr, &renderTargetView);
            if (FAILED(hr))
            {
                Log(Log::Level::ERR) << "Failed to create Direct3D 11 render target view, error: " << hr;
                return false;
            }

            // Rasterizer state
            D3D11_RASTERIZER_DESC rasterStateDesc;
            rasterStateDesc.FillMode = D3D11_FILL_SOLID;
            rasterStateDesc.CullMode = D3D11_CULL_NONE;
            rasterStateDesc.FrontCounterClockwise = FALSE;
            rasterStateDesc.DepthBias = 0;
            rasterStateDesc.DepthBiasClamp = 0;
            rasterStateDesc.SlopeScaledDepthBias = 0;
            rasterStateDesc.DepthClipEnable = TRUE;
            rasterStateDesc.ScissorEnable = FALSE;
            rasterStateDesc.MultisampleEnable = (sampleCount > 1) ? TRUE : FALSE;
            rasterStateDesc.AntialiasedLineEnable = TRUE;

            hr = device->CreateRasterizerState(&rasterStateDesc, &rasterizerStates[0]);
            if (FAILED(hr))
            {
                Log(Log::Level::ERR) << "Failed to create Direct3D 11 rasterizer state, error: " << hr;
                return false;
            }

            // wireframe
            rasterStateDesc.FillMode = D3D11_FILL_WIREFRAME;
            rasterStateDesc.ScissorEnable = FALSE;

            hr = device->CreateRasterizerState(&rasterStateDesc, &rasterizerStates[1]);
            if (FAILED(hr))
            {
                Log(Log::Level::ERR) << "Failed to create Direct3D 11 rasterizer state, error: " << hr;
                return false;
            }

            // scissor test
            rasterStateDesc.FillMode = D3D11_FILL_SOLID;
            rasterStateDesc.ScissorEnable = TRUE;

            hr = device->CreateRasterizerState(&rasterStateDesc, &rasterizerStates[2]);
            if (FAILED(hr))
            {
                Log(Log::Level::ERR) << "Failed to create Direct3D 11 rasterizer state, error: " << hr;
                return false;
            }

            // wireframe and scissor test
            rasterStateDesc.FillMode = D3D11_FILL_WIREFRAME;
            rasterStateDesc.ScissorEnable = TRUE;

            hr = device->CreateRasterizerState(&rasterStateDesc, &rasterizerStates[3]);
            if (FAILED(hr))
            {
                Log(Log::Level::ERR) << "Failed to create Direct3D 11 rasterizer state, error: " << hr;
                return false;
            }

            if (depth)
            {
                D3D11_TEXTURE2D_DESC depthStencilDesc;
                depthStencilDesc.Width = frameBufferWidth;
                depthStencilDesc.Height = frameBufferHeight;
                depthStencilDesc.MipLevels = 1;
                depthStencilDesc.ArraySize = 1;
                depthStencilDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
                depthStencilDesc.SampleDesc.Count = sampleCount;
                depthStencilDesc.SampleDesc.Quality = 0;
                depthStencilDesc.Usage = D3D11_USAGE_DEFAULT;
                depthStencilDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
                depthStencilDesc.CPUAccessFlags = 0;
                depthStencilDesc.MiscFlags = 0;
                hr = device->CreateTexture2D(&depthStencilDesc, nullptr, &depthStencilTexture);
                if (FAILED(hr))
                {
                    Log(Log::Level::ERR) << "Failed to create Direct3D 11 depth stencil texture, error: " << hr;
                    return false;
                }

                hr = device->CreateDepthStencilView(depthStencilTexture, nullptr, &depthStencilView);
                if (FAILED(hr))
                {
                    Log(Log::Level::ERR) << "Failed to create Direct3D 11 depth stencil view, error: " << hr;
                    return false;
                }

                for (uint32_t state = 0; state < 4; ++state)
                {
                    D3D11_DEPTH_STENCIL_DESC depthStencilStateDesc;
                    depthStencilStateDesc.DepthEnable = (state & 0x01) ? TRUE : FALSE;
                    depthStencilStateDesc.DepthWriteMask = (state & 0x02) ? D3D11_DEPTH_WRITE_MASK_ALL : D3D11_DEPTH_WRITE_MASK_ZERO;
                    depthStencilStateDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
                    depthStencilStateDesc.StencilEnable = FALSE;
                    depthStencilStateDesc.StencilReadMask = D3D11_DEFAULT_STENCIL_READ_MASK;
                    depthStencilStateDesc.StencilWriteMask = D3D11_DEFAULT_STENCIL_WRITE_MASK;
                    depthStencilStateDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
                    depthStencilStateDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
                    depthStencilStateDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
                    depthStencilStateDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
                    depthStencilStateDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
                    depthStencilStateDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
                    depthStencilStateDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
                    depthStencilStateDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;

                    hr = device->CreateDepthStencilState(&depthStencilStateDesc, &depthStencilStates[state]);
                    if (FAILED(hr))
                    {
                        Log(Log::Level::ERR) << "Failed to create Direct3D 11 depth stencil state, error: " << hr;
                        return false;
                    }
                }
            }

            std::shared_ptr<Shader> textureShader = std::make_shared<Shader>();
            textureShader->initFromBuffers(std::vector<uint8_t>(std::begin(TEXTURE_PIXEL_SHADER_D3D11), std::end(TEXTURE_PIXEL_SHADER_D3D11)),
                                           std::vector<uint8_t>(std::begin(TEXTURE_VERTEX_SHADER_D3D11), std::end(TEXTURE_VERTEX_SHADER_D3D11)),
                                           VertexPCT::ATTRIBUTES,
                                           {{"color", DataType::FLOAT_VECTOR4}},
                                           {{"modelViewProj", DataType::FLOAT_MATRIX4}});

            sharedEngine->getCache()->setShader(SHADER_TEXTURE, textureShader);

            std::shared_ptr<Shader> colorShader = std::make_shared<Shader>();
            colorShader->initFromBuffers(std::vector<uint8_t>(std::begin(COLOR_PIXEL_SHADER_D3D11), std::end(COLOR_PIXEL_SHADER_D3D11)),
                                         std::vector<uint8_t>(std::begin(COLOR_VERTEX_SHADER_D3D11), std::end(COLOR_VERTEX_SHADER_D3D11)),
                                         VertexPC::ATTRIBUTES,
                                         {{"color", DataType::FLOAT_VECTOR4}},
                                         {{"modelViewProj", DataType::FLOAT_MATRIX4}});

            sharedEngine->getCache()->setShader(SHADER_COLOR, colorShader);

            std::shared_ptr<BlendState> noBlendState = std::make_shared<BlendState>();

            noBlendState->init(false,
                               BlendState::BlendFactor::ONE, BlendState::BlendFactor::ZERO,
                               BlendState::BlendOperation::ADD,
                               BlendState::BlendFactor::ONE, BlendState::BlendFactor::ZERO,
                               BlendState::BlendOperation::ADD);

            sharedEngine->getCache()->setBlendState(BLEND_NO_BLEND, noBlendState);

            std::shared_ptr<BlendState> addBlendState = std::make_shared<BlendState>();

            addBlendState->init(true,
                                BlendState::BlendFactor::ONE, BlendState::BlendFactor::ONE,
                                BlendState::BlendOperation::ADD,
                                BlendState::BlendFactor::ONE, BlendState::BlendFactor::ONE,
                                BlendState::BlendOperation::ADD);

            sharedEngine->getCache()->setBlendState(BLEND_ADD, addBlendState);

            std::shared_ptr<BlendState> multiplyBlendState = std::make_shared<BlendState>();

            multiplyBlendState->init(true,
                                     BlendState::BlendFactor::DEST_COLOR, BlendState::BlendFactor::ZERO,
                                     BlendState::BlendOperation::ADD,
                                     BlendState::BlendFactor::ONE, BlendState::BlendFactor::ONE,
                                     BlendState::BlendOperation::ADD);

            sharedEngine->getCache()->setBlendState(BLEND_MULTIPLY, multiplyBlendState);

            std::shared_ptr<BlendState> alphaBlendState = std::make_shared<BlendState>();

            alphaBlendState->init(true,
                                  BlendState::BlendFactor::SRC_ALPHA, BlendState::BlendFactor::INV_SRC_ALPHA,
                                  BlendState::BlendOperation::ADD,
                                  BlendState::BlendFactor::ONE, BlendState::BlendFactor::ONE,
                                  BlendState::BlendOperation::ADD);

            sharedEngine->getCache()->setBlendState(BLEND_ALPHA, alphaBlendState);

            std::shared_ptr<Texture> whitePixelTexture = std::make_shared<Texture>();
            whitePixelTexture->initFromBuffer({255, 255, 255, 255}, Size2(1.0f, 1.0f), false, false);
            sharedEngine->getCache()->setTexture(TEXTURE_WHITE_PIXEL, whitePixelTexture);

            return true;
        }

        bool RendererD3D11::upload()
        {
            frameBufferClearColor[0] = clearColor.normR();
            frameBufferClearColor[1] = clearColor.normG();
            frameBufferClearColor[2] = clearColor.normB();
            frameBufferClearColor[3] = clearColor.normA();

            clearFrameBufferView = clearColorBuffer;
            clearDepthBufferView = clearDepthBuffer;

            if (static_cast<UINT>(size.v[0]) != frameBufferWidth ||
                static_cast<UINT>(size.v[1]) != frameBufferHeight)
            {
                if (!resizeBackBuffer(0, 0))
                {
                    return false;
                }

                size = Size2(static_cast<float>(frameBufferWidth),
                             static_cast<float>(frameBufferHeight));
            }

            dirty = false;

            return true;
        }

        bool RendererD3D11::draw(const std::vector<DrawCommand>& drawCommands)
        {
            ID3D11ShaderResourceView* resourceViews[Texture::LAYERS];
            ID3D11SamplerState* samplers[Texture::LAYERS];
            std::fill(std::begin(resourceViews), std::end(resourceViews), nullptr);
            std::fill(std::begin(samplers), std::end(samplers), nullptr);

            context->RSSetState(rasterizerStates[0]);

            std::vector<float> shaderData;

            D3D11_VIEWPORT viewport;
            viewport.MinDepth = 0.0f;
            viewport.MaxDepth = 1.0f;

            if (drawCommands.empty())
            {
                frameBufferClearedFrame = currentFrame;

                context->OMSetRenderTargets(1, &renderTargetView, depthStencilView);
                context->OMSetDepthStencilState(depthStencilStates[3], 0);

                viewport.TopLeftX = viewport.TopLeftY = 0.0f;
                viewport.Width = static_cast<FLOAT>(frameBufferWidth);
                viewport.Height = static_cast<FLOAT>(frameBufferHeight);
                context->RSSetViewports(1, &viewport);

                if (clearColorBuffer)
                {
                    context->ClearRenderTargetView(renderTargetView, frameBufferClearColor);
                }

                if (clearDepthBuffer)
                {
                    context->ClearDepthStencilView(depthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);
                }
            }
            else for (const DrawCommand& drawCommand : drawCommands)
            {
                // render target
                ID3D11RenderTargetView* newRenderTargetView = nullptr;
                ID3D11DepthStencilView* newDepthStencilView = nullptr;
                const float* newClearColor;
                bool newClearFrameBufferView = false;
                bool newClearDepthBufferView = false;

                UINT renderTargetWidth = 0;
                UINT renderTargetHeight = 0;

                if (drawCommand.renderTarget)
                {
                    TextureResourceD3D11* renderTargetD3D11 = static_cast<TextureResourceD3D11*>(drawCommand.renderTarget);

                    if (!renderTargetD3D11->getRenderTargetView())
                    {
                        continue;
                    }

                    TextureResourceD3D11* renderTargetTextureD3D11 = static_cast<TextureResourceD3D11*>(renderTargetD3D11);
                    renderTargetWidth = renderTargetTextureD3D11->getWidth();
                    renderTargetHeight = renderTargetTextureD3D11->getHeight();

                    newRenderTargetView = renderTargetD3D11->getRenderTargetView();
                    newDepthStencilView = renderTargetD3D11->getDepthStencilView();
                    newClearColor = renderTargetD3D11->getFrameBufferClearColor();

                    if (renderTargetD3D11->getFrameBufferClearedFrame() != currentFrame)
                    {
                        renderTargetD3D11->setFrameBufferClearedFrame(currentFrame);
                        newClearFrameBufferView = renderTargetD3D11->getClearFrameBufferView();
                        newClearDepthBufferView = renderTargetD3D11->getClearDepthBufferView();
                    }
                }
                else
                {
                    newRenderTargetView = renderTargetView;
                    newDepthStencilView = depthStencilView;
                    newClearColor = frameBufferClearColor;

                    renderTargetWidth = frameBufferWidth;
                    renderTargetHeight = frameBufferHeight;

                    if (frameBufferClearedFrame != currentFrame)
                    {
                        frameBufferClearedFrame = currentFrame;
                        newClearFrameBufferView = clearFrameBufferView;
                        newClearDepthBufferView = clearDepthBufferView;
                    }
                }

                context->OMSetRenderTargets(1, &newRenderTargetView, newDepthStencilView);

                viewport.TopLeftX = drawCommand.viewport.position.v[0];
                viewport.TopLeftY = static_cast<FLOAT>(renderTargetHeight) - (drawCommand.viewport.position.v[1] + drawCommand.viewport.size.v[1]);
                viewport.Width = drawCommand.viewport.size.v[0];
                viewport.Height = drawCommand.viewport.size.v[1];
                context->RSSetViewports(1, &viewport);

                if (newClearFrameBufferView)
                {
                    context->ClearRenderTargetView(newRenderTargetView, newClearColor);
                }

                if (newClearDepthBufferView)
                {
                    context->ClearDepthStencilView(newDepthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);
                }

                uint32_t rasterizerStateIndex = 0;
                if (drawCommand.wireframe) rasterizerStateIndex |= 0x01;

                // scissor test
                if (drawCommand.scissorTest)
                {
                    rasterizerStateIndex |= 0x02;

                    D3D11_RECT rect;
                    rect.left = static_cast<LONG>(drawCommand.scissorRectangle.position.v[0]);
                    rect.top = static_cast<LONG>(renderTargetHeight - (drawCommand.scissorRectangle.position.v[1] + drawCommand.scissorRectangle.size.v[1]));
                    rect.right = static_cast<LONG>(drawCommand.scissorRectangle.position.v[0] + drawCommand.scissorRectangle.size.v[0]);
                    rect.bottom = static_cast<LONG>(renderTargetHeight - drawCommand.scissorRectangle.position.v[1]);
                    context->RSSetScissorRects(1, &rect);
                }

                context->RSSetState(rasterizerStates[rasterizerStateIndex]);

                // shader
                ShaderResourceD3D11* shaderD3D11 = static_cast<ShaderResourceD3D11*>(drawCommand.shader);

                if (!shaderD3D11 || !shaderD3D11->getPixelShader() || !shaderD3D11->getVertexShader())
                {
                    // don't render if invalid shader
                    continue;
                }

                context->PSSetShader(shaderD3D11->getPixelShader(), nullptr, 0);
                context->VSSetShader(shaderD3D11->getVertexShader(), nullptr, 0);

                context->IASetInputLayout(shaderD3D11->getInputLayout());

                // pixel shader constants
                const std::vector<ShaderResourceD3D11::Location>& pixelShaderConstantLocations = shaderD3D11->getPixelShaderConstantLocations();

                if (drawCommand.pixelShaderConstants.size() > pixelShaderConstantLocations.size())
                {
                    Log(Log::Level::ERR) << "Invalid pixel shader constant size";
                    return false;
                }

                shaderData.clear();

                for (size_t i = 0; i < drawCommand.pixelShaderConstants.size(); ++i)
                {
                    const ShaderResourceD3D11::Location& pixelShaderConstantLocation = pixelShaderConstantLocations[i];
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

                ID3D11Buffer* pixelShaderConstantBuffers[1] = {shaderD3D11->getPixelShaderConstantBuffer()};
                context->PSSetConstantBuffers(0, 1, pixelShaderConstantBuffers);

                // vertex shader constants
                const std::vector<ShaderResourceD3D11::Location>& vertexShaderConstantLocations = shaderD3D11->getVertexShaderConstantLocations();

                if (drawCommand.vertexShaderConstants.size() > vertexShaderConstantLocations.size())
                {
                    Log(Log::Level::ERR) << "Invalid vertex shader constant size";
                    return false;
                }

                shaderData.clear();

                for (size_t i = 0; i < drawCommand.vertexShaderConstants.size(); ++i)
                {
                    const ShaderResourceD3D11::Location& vertexShaderConstantLocation = vertexShaderConstantLocations[i];
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

                ID3D11Buffer* vertexShaderConstantBuffers[1] = {shaderD3D11->getVertexShaderConstantBuffer()};
                context->VSSetConstantBuffers(0, 1, vertexShaderConstantBuffers);

                // blend state
                BlendStateResourceD3D11* blendStateD3D11 = static_cast<BlendStateResourceD3D11*>(drawCommand.blendState);

                if (!blendStateD3D11 || !blendStateD3D11->getBlendState())
                {
                    // don't render if invalid blend state
                    continue;
                }

                context->OMSetBlendState(blendStateD3D11->getBlendState(), NULL, 0xffffffff);

                // textures
                bool texturesValid = true;

                for (uint32_t layer = 0; layer < Texture::LAYERS; ++layer)
                {
                    TextureResourceD3D11* textureD3D11 = nullptr;

                    if (drawCommand.textures.size() > layer)
                    {
                        textureD3D11 = static_cast<TextureResourceD3D11*>(drawCommand.textures[layer]);
                    }

                    if (textureD3D11)
                    {
                        if (!textureD3D11->getResourceView() ||
                            !textureD3D11->getSamplerState())
                        {
                            texturesValid = false;
                            break;
                        }

                        resourceViews[layer] = textureD3D11->getResourceView();
                        samplers[layer] = textureD3D11->getSamplerState();
                    }
                    else
                    {
                        resourceViews[layer] = nullptr;
                        samplers[layer] = nullptr;
                    }
                }

                if (!texturesValid)
                {
                    continue;
                }

                context->PSSetShaderResources(0, Texture::LAYERS, resourceViews);
                context->PSSetSamplers(0, Texture::LAYERS, samplers);

                // depth-stencil state
                uint32_t depthStencilStateIndex = 0;
                if (drawCommand.depthTest) depthStencilStateIndex |= 0x01;
                if (drawCommand.depthWrite) depthStencilStateIndex |= 0x02;
                context->OMSetDepthStencilState(depthStencilStates[depthStencilStateIndex], 0);

                // draw// mesh buffer
                MeshBufferResourceD3D11* meshBufferD3D11 = static_cast<MeshBufferResourceD3D11*>(drawCommand.meshBuffer);
                BufferResourceD3D11* indexBufferD3D11 = meshBufferD3D11->getIndexBufferD3D11();
                BufferResourceD3D11* vertexBufferD3D11 = meshBufferD3D11->getVertexBufferD3D11();

                if (!meshBufferD3D11 ||
                    !indexBufferD3D11 ||
                    !indexBufferD3D11->getBuffer() ||
                    !vertexBufferD3D11 ||
                    !vertexBufferD3D11->getBuffer())
                {
                    continue;
                }

                ID3D11Buffer* buffers[] = {vertexBufferD3D11->getBuffer()};
                UINT strides[] = {meshBufferD3D11->getVertexSize()};
                UINT offsets[] = {0};
                context->IASetVertexBuffers(0, 1, buffers, strides, offsets);
                context->IASetIndexBuffer(indexBufferD3D11->getBuffer(), meshBufferD3D11->getIndexFormat(), 0);

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

            return true;
        }

        IDXGIOutput* RendererD3D11::getOutput() const
        {
            WindowWin* windowWin = static_cast<WindowWin*>(window);

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
                Log(Log::Level::ERR) << "Failed to get display mode list, error: " << hr;
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

        bool RendererD3D11::generateScreenshot(const std::string& filename)
        {
            ID3D11Texture2D* backBufferTexture;
            HRESULT hr = backBuffer->QueryInterface(IID_ID3D11Texture2D, reinterpret_cast<void**>(&backBufferTexture));
            if (FAILED(hr))
            {
                Log(Log::Level::ERR) << "Failed to get Direct3D 11 back buffer texture, error: " << hr;
                return false;
            }

            D3D11_TEXTURE2D_DESC backBufferDesc;
            backBufferTexture->GetDesc(&backBufferDesc);

            D3D11_TEXTURE2D_DESC textureDesc;
            textureDesc.Width = backBufferDesc.Width;
            textureDesc.Height = backBufferDesc.Height;
            textureDesc.MipLevels = 1;
            textureDesc.ArraySize = 1;
            textureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
            textureDesc.SampleDesc.Count = 1;
            textureDesc.SampleDesc.Quality = 0;
            textureDesc.Usage = D3D11_USAGE_STAGING;
            textureDesc.BindFlags = 0;
            textureDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
            textureDesc.MiscFlags = 0;

            ID3D11Texture2D* texture;

            hr = device->CreateTexture2D(&textureDesc, nullptr, &texture);

            if (FAILED(hr))
            {
                Log(Log::Level::ERR) << "Failed to create Direct3D 11 texture, error: " << hr;
                return false;
            }

            if (backBufferDesc.SampleDesc.Count > 1)
            {
                D3D11_TEXTURE2D_DESC resolveTextureDesc;
                resolveTextureDesc.Width = backBufferDesc.Width;
                resolveTextureDesc.Height = backBufferDesc.Height;
                resolveTextureDesc.MipLevels = 1;
                resolveTextureDesc.ArraySize = 1;
                resolveTextureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
                resolveTextureDesc.SampleDesc.Count = 1;
                resolveTextureDesc.SampleDesc.Quality = 0;
                resolveTextureDesc.Usage = D3D11_USAGE_DEFAULT;
                resolveTextureDesc.BindFlags = 0;
                resolveTextureDesc.CPUAccessFlags = 0;
                resolveTextureDesc.MiscFlags = 0;

                ID3D11Texture2D* resolveTexture;

                hr = device->CreateTexture2D(&resolveTextureDesc, nullptr, &resolveTexture);

                if (FAILED(hr))
                {
                    texture->Release();
                    Log(Log::Level::ERR) << "Failed to create Direct3D 11 texture, error: " << hr;
                    return false;
                }

                context->ResolveSubresource(resolveTexture, 0, backBuffer, 0, DXGI_FORMAT_R8G8B8A8_UNORM);
                context->CopyResource(texture, resolveTexture);
                resolveTexture->Release();
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
                Log(Log::Level::ERR) << "Failed to map Direct3D 11 resource, error: " << hr;
                return false;
            }

            if (!stbi_write_png(filename.c_str(), textureDesc.Width, textureDesc.Height, 4, mappedSubresource.pData, static_cast<int>(mappedSubresource.RowPitch)))
            {
                context->Unmap(texture, 0);
                texture->Release();
                Log(Log::Level::ERR) << "Failed to save screenshot to file";
                return false;
            }

            context->Unmap(texture, 0);
            texture->Release();

            return true;
        }

        BlendStateResource* RendererD3D11::createBlendState()
        {
            std::lock_guard<std::mutex> lock(resourceMutex);

            BlendStateResource* blendState = new BlendStateResourceD3D11();
            resources.push_back(std::unique_ptr<Resource>(blendState));
            return blendState;
        }

        TextureResource* RendererD3D11::createTexture()
        {
            std::lock_guard<std::mutex> lock(resourceMutex);

            TextureResource* texture = new TextureResourceD3D11();
            resources.push_back(std::unique_ptr<Resource>(texture));
            return texture;
        }

        ShaderResource* RendererD3D11::createShader()
        {
            std::lock_guard<std::mutex> lock(resourceMutex);

            ShaderResource* shader = new ShaderResourceD3D11();
            resources.push_back(std::unique_ptr<Resource>(shader));
            return shader;
        }

        MeshBufferResource* RendererD3D11::createMeshBuffer()
        {
            std::lock_guard<std::mutex> lock(resourceMutex);

            MeshBufferResource* meshBuffer = new MeshBufferResourceD3D11();
            resources.push_back(std::unique_ptr<Resource>(meshBuffer));
            return meshBuffer;
        }

        BufferResource* RendererD3D11::createBuffer()
        {
            std::lock_guard<std::mutex> lock(resourceMutex);

            BufferResource* buffer = new BufferResourceD3D11();
            resources.push_back(std::unique_ptr<Resource>(buffer));
            return buffer;
        }

        bool RendererD3D11::resizeBackBuffer(UINT newWidth, UINT newHeight)
        {
            if (frameBufferWidth != newWidth || newWidth == 0 ||
                frameBufferHeight != newHeight || newHeight == 0)
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
                    Log(Log::Level::ERR) << "Failed to resize Direct3D 11 backbuffer, error: " << hr;
                    return false;
                }

                hr = swapChain->GetBuffer(0, IID_ID3D11Texture2D, (void**)&backBuffer);
                if (FAILED(hr))
                {
                    Log(Log::Level::ERR) << "Failed to retrieve Direct3D 11 backbuffer, error: " << hr;
                    return false;
                }

                hr = device->CreateRenderTargetView(backBuffer, nullptr, &renderTargetView);
                if (FAILED(hr))
                {
                    Log(Log::Level::ERR) << "Failed to create Direct3D 11 render target view, error: " << hr;
                    return false;
                }

                D3D11_TEXTURE2D_DESC desc;
                backBuffer->GetDesc(&desc);

                frameBufferWidth = desc.Width;
                frameBufferHeight = desc.Height;

                if (depthStencilTexture)
                {
                    depthStencilTexture->Release();
                    depthStencilTexture = nullptr;
                }

                if (depthStencilView)
                {
                    depthStencilView->Release();
                    depthStencilView = nullptr;
                }

                if (depth)
                {
                    D3D11_TEXTURE2D_DESC depthStencilDesc;
                    depthStencilDesc.Width = frameBufferWidth;
                    depthStencilDesc.Height = frameBufferHeight;
                    depthStencilDesc.MipLevels = 1;
                    depthStencilDesc.ArraySize = 1;
                    depthStencilDesc.Format = DXGI_FORMAT_D32_FLOAT;
                    depthStencilDesc.SampleDesc.Count = sampleCount;
                    depthStencilDesc.SampleDesc.Quality = 0;
                    depthStencilDesc.Usage = D3D11_USAGE_DEFAULT;
                    depthStencilDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
                    depthStencilDesc.CPUAccessFlags = 0;
                    depthStencilDesc.MiscFlags = 0;
                    hr = device->CreateTexture2D(&depthStencilDesc, nullptr, &depthStencilTexture);
                    if (FAILED(hr))
                    {
                        Log(Log::Level::ERR) << "Failed to create Direct3D 11 depth stencil texture, error: " << hr;
                        return false;
                    }

                    hr = device->CreateDepthStencilView(depthStencilTexture, nullptr, &depthStencilView);
                    if (FAILED(hr))
                    {
                        Log(Log::Level::ERR) << "Failed to create Direct3D 11 depth stencil view, error: " << hr;
                        return false;
                    }
                }
            }

            return true;
        }

        ID3D11SamplerState* RendererD3D11::getSamplerState(const SamplerStateDesc& desc)
        {
            auto samplerStatesIterator = samplerStates.find(desc);

            if (samplerStatesIterator != samplerStates.end())
            {
                return samplerStatesIterator->second;
            }
            else
            {
                D3D11_SAMPLER_DESC samplerStateDesc;

                if (desc.maxAnisotropy > 1)
                {
                    samplerStateDesc.Filter = D3D11_FILTER_ANISOTROPIC;
                }
                else
                {
                    switch (desc.filter)
                    {
                        case Texture::Filter::DEFAULT:
                        case Texture::Filter::POINT:
                            samplerStateDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
                            break;
                        case Texture::Filter::LINEAR:
                            samplerStateDesc.Filter = D3D11_FILTER_MIN_MAG_POINT_MIP_LINEAR;
                            break;
                        case Texture::Filter::BILINEAR:
                            samplerStateDesc.Filter = D3D11_FILTER_MIN_MAG_LINEAR_MIP_POINT;
                            break;
                        case Texture::Filter::TRILINEAR:
                            samplerStateDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
                            break;
                    }
                }

                switch (desc.addressX)
                {
                    case Texture::Address::CLAMP:
                        samplerStateDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
                        break;
                    case Texture::Address::REPEAT:
                        samplerStateDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
                        break;
                    case Texture::Address::MIRROR_REPEAT:
                        samplerStateDesc.AddressU = D3D11_TEXTURE_ADDRESS_MIRROR;
                        break;
                }

                switch (desc.addressY)
                {
                    case Texture::Address::CLAMP:
                        samplerStateDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
                        break;
                    case Texture::Address::REPEAT:
                        samplerStateDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
                        break;
                    case Texture::Address::MIRROR_REPEAT:
                        samplerStateDesc.AddressV = D3D11_TEXTURE_ADDRESS_MIRROR;
                        break;
                }

                samplerStateDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
                samplerStateDesc.MipLODBias = 0.0f;
                samplerStateDesc.MaxAnisotropy = desc.maxAnisotropy;
                samplerStateDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;

                samplerStateDesc.BorderColor[0] = 0.0f;
                samplerStateDesc.BorderColor[1] = 0.0f;
                samplerStateDesc.BorderColor[2] = 0.0f;
                samplerStateDesc.BorderColor[3] = 0.0f;

                samplerStateDesc.MinLOD = 0.0f;
                samplerStateDesc.MaxLOD = D3D11_FLOAT32_MAX;

                ID3D11SamplerState* samplerState;

                HRESULT hr = device->CreateSamplerState(&samplerStateDesc, &samplerState);
                if (FAILED(hr))
                {
                    Log(Log::Level::ERR) << "Failed to create Direct3D 11 sampler state, error: " << hr;
                    return nullptr;
                }

                samplerStates[desc] = samplerState;

                return samplerState;
            }
        }
    } // namespace graphics
} // namespace ouzel

#endif

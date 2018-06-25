// Copyright (C) 2018 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "core/Setup.h"

#include <cassert>

#if OUZEL_COMPILE_DIRECT3D11

#include "RenderDeviceD3D11.hpp"
#include "BlendStateResourceD3D11.hpp"
#include "BufferResourceD3D11.hpp"
#include "RenderTargetResourceD3D11.hpp"
#include "ShaderResourceD3D11.hpp"
#include "TextureResourceD3D11.hpp"
#include "direct3d11/TexturePSD3D11.h"
#include "direct3d11/TextureVSD3D11.h"
#include "direct3d11/ColorPSD3D11.h"
#include "direct3d11/ColorVSD3D11.h"
#include "core/Engine.hpp"
#include "thread/Lock.hpp"
#include "utils/Errors.hpp"
#include "utils/Log.hpp"
#include "core/windows/WindowResourceWin.hpp"
#include "stb_image_write.h"

namespace ouzel
{
    namespace graphics
    {
        RenderDeviceD3D11::RenderDeviceD3D11():
            RenderDevice(Renderer::Driver::DIRECT3D11), running(false)
        {
            apiMajorVersion = 11;
            apiMinorVersion = 0;

            std::fill(std::begin(rasterizerStates), std::end(rasterizerStates), nullptr);
            std::fill(std::begin(depthStencilStates), std::end(depthStencilStates), nullptr);
        }

        RenderDeviceD3D11::~RenderDeviceD3D11()
        {
            running = false;
            flushCommands();

            if (renderThread.isJoinable()) renderThread.join();

            resourceDeleteSet.clear();
            resources.clear();

            for (ID3D11DepthStencilState* depthStencilState : depthStencilStates)
            {
                if (depthStencilState)
                    depthStencilState->Release();
            }

            if (depthStencilView)
                depthStencilView->Release();

            if (depthStencilTexture)
                depthStencilTexture->Release();

            for (ID3D11RasterizerState* rasterizerState : rasterizerStates)
            {
                if (rasterizerState)
                    rasterizerState->Release();
            }

            for (const auto& samplerState : samplerStates)
                samplerState.second->Release();

            if (renderTargetView)
                renderTargetView->Release();

            if (backBuffer)
                backBuffer->Release();

            if (swapChain)
                swapChain->Release();

            if (adapter)
                adapter->Release();

            if (context)
                context->Release();

            if (device)
                device->Release();
        }

        void RenderDeviceD3D11::init(Window* newWindow,
                                     const Size2& newSize,
                                     uint32_t newSampleCount,
                                     Texture::Filter newTextureFilter,
                                     uint32_t newMaxAnisotropy,
                                     bool newVerticalSync,
                                     bool newDepth,
                                     bool newDebugRenderer)
        {
            RenderDevice::init(newWindow,
                               newSize,
                               newSampleCount,
                               newTextureFilter,
                               newMaxAnisotropy,
                               newVerticalSync,
                               newDepth,
                               newDebugRenderer);

            UINT deviceCreationFlags = 0;

            if (debugRenderer)
                deviceCreationFlags |= D3D11_CREATE_DEVICE_DEBUG;

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
                throw SystemError("Failed to create the Direct3D 11 device, error: " + std::to_string(hr));

            if (device->GetFeatureLevel() < D3D_FEATURE_LEVEL_10_0)
                npotTexturesSupported = false;

            IDXGIDevice* dxgiDevice;
            IDXGIFactory* factory;

            device->QueryInterface(IID_IDXGIDevice, reinterpret_cast<void**>(&dxgiDevice));
            dxgiDevice->GetParent(IID_IDXGIAdapter, reinterpret_cast<void**>(&adapter));
            hr = adapter->GetParent(IID_IDXGIFactory, reinterpret_cast<void**>(&factory));
            if (FAILED(hr))
                throw SystemError("Failed to get the DXGI factory, error: " + std::to_string(hr));

            DXGI_ADAPTER_DESC adapterDesc;
            hr = adapter->GetDesc(&adapterDesc);
            if (FAILED(hr))
                throw SystemError("Failed to get the DXGI adapter description, error: " + std::to_string(hr));
            else
            {
                char deviceName[256];
                WideCharToMultiByte(CP_UTF8, 0, adapterDesc.Description, -1, deviceName, sizeof(deviceName), nullptr, nullptr);
                Log(Log::Level::INFO) << "Using " << deviceName << " for rendering";
            }

            WindowResourceWin* windowWin = static_cast<WindowResourceWin*>(window->getResource());

            frameBufferWidth = static_cast<UINT>(newSize.width);
            frameBufferHeight = static_cast<UINT>(newSize.height);

            UINT qualityLevels;
            UINT supportedSampleCount;
            for (supportedSampleCount = sampleCount; supportedSampleCount > 1; --supportedSampleCount)
            {
                hr = device->CheckMultisampleQualityLevels(DXGI_FORMAT_R8G8B8A8_UNORM, supportedSampleCount, &qualityLevels);
                if (FAILED(hr))
                    throw SystemError("Failed to check Direct3D 11 multisample quality levels, error: " + std::to_string(hr));
                else if (qualityLevels)
                    break;
            }

            if (supportedSampleCount != sampleCount)
            {
                sampleCount = supportedSampleCount;
                Log(Log::Level::WARN) << "Chosen sample count not supported, using: " << sampleCount;
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
            swapChainDesc.Windowed = (windowWin->isExclusiveFullscreen() && windowWin->isFullscreen()) ? FALSE : TRUE;
            swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
            swapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

            swapInterval = verticalSync ? 1 : 0;

            hr = factory->CreateSwapChain(device, &swapChainDesc, &swapChain);
            if (FAILED(hr))
                throw SystemError("Failed to create the Direct3D 11 swap chain, error: " + std::to_string(hr));

            factory->MakeWindowAssociation(windowWin->getNativeWindow(), DXGI_MWA_NO_ALT_ENTER);

            factory->Release();
            dxgiDevice->Release();

            // Backbuffer
            hr = swapChain->GetBuffer(0, IID_ID3D11Texture2D, reinterpret_cast<void**>(&backBuffer));
            if (FAILED(hr))
                throw SystemError("Failed to retrieve Direct3D 11 backbuffer, error: " + std::to_string(hr));

            hr = device->CreateRenderTargetView(backBuffer, nullptr, &renderTargetView);
            if (FAILED(hr))
                throw SystemError("Failed to create Direct3D 11 render target view, error: " + std::to_string(hr));

            // Rasterizer state
            D3D11_RASTERIZER_DESC rasterStateDesc;
            rasterStateDesc.FrontCounterClockwise = FALSE;
            rasterStateDesc.DepthBias = 0;
            rasterStateDesc.DepthBiasClamp = 0;
            rasterStateDesc.SlopeScaledDepthBias = 0;
            rasterStateDesc.DepthClipEnable = TRUE;
            rasterStateDesc.MultisampleEnable = (sampleCount > 1) ? TRUE : FALSE;
            rasterStateDesc.AntialiasedLineEnable = TRUE;

            uint32_t rasterStateIndex = 0;

            for (uint32_t fillMode = 0; fillMode < 2; ++fillMode)
            {
                for (uint32_t scissorEnable = 0; scissorEnable < 2; ++scissorEnable)
                {
                    for (uint32_t cullMode = 0; cullMode < 3; ++cullMode)
                    {
                        rasterStateDesc.FillMode = (fillMode == 0) ? D3D11_FILL_SOLID : D3D11_FILL_WIREFRAME;
                        rasterStateDesc.ScissorEnable = (scissorEnable == 0) ? FALSE : TRUE;
                        switch (cullMode)
                        {
                            case 0: rasterStateDesc.CullMode = D3D11_CULL_NONE; break;
                            case 1: rasterStateDesc.CullMode = D3D11_CULL_FRONT; break;
                            case 2: rasterStateDesc.CullMode = D3D11_CULL_BACK; break;
                        }

                        hr = device->CreateRasterizerState(&rasterStateDesc, &rasterizerStates[rasterStateIndex]);
                        if (FAILED(hr))
                            throw SystemError("Failed to create Direct3D 11 rasterizer state, error: " + std::to_string(hr));

                        ++rasterStateIndex;
                    }
                }
            }

            uint32_t depthStencilStateIndex = 0;

            for (uint32_t depthEnable = 0; depthEnable < 2; ++depthEnable)
            {
                for (uint32_t depthWriteMask = 0; depthWriteMask < 2; ++depthWriteMask)
                {
                    D3D11_DEPTH_STENCIL_DESC depthStencilStateDesc;
                    depthStencilStateDesc.DepthEnable = (depthEnable == 0) ? FALSE : TRUE;
                    depthStencilStateDesc.DepthWriteMask = (depthWriteMask == 0) ? D3D11_DEPTH_WRITE_MASK_ZERO : D3D11_DEPTH_WRITE_MASK_ALL;
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

                    hr = device->CreateDepthStencilState(&depthStencilStateDesc, &depthStencilStates[depthStencilStateIndex]);
                    if (FAILED(hr))
                        throw SystemError("Failed to create Direct3D 11 depth stencil state, error: " + std::to_string(hr));

                    ++depthStencilStateIndex;
                }
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
                    throw SystemError("Failed to create Direct3D 11 depth stencil texture, error: " + std::to_string(hr));

                hr = device->CreateDepthStencilView(depthStencilTexture, nullptr, &depthStencilView);
                if (FAILED(hr))
                    throw SystemError("Failed to create Direct3D 11 depth stencil view, error: " + std::to_string(hr));
            }

            std::shared_ptr<Shader> textureShader = std::make_shared<Shader>();
            textureShader->init(std::vector<uint8_t>(std::begin(TEXTURE_PIXEL_SHADER_D3D11), std::end(TEXTURE_PIXEL_SHADER_D3D11)),
                                std::vector<uint8_t>(std::begin(TEXTURE_VERTEX_SHADER_D3D11), std::end(TEXTURE_VERTEX_SHADER_D3D11)),
                                {Vertex::Attribute::Usage::POSITION, Vertex::Attribute::Usage::COLOR, Vertex::Attribute::Usage::TEXTURE_COORDINATES0},
                                {{"color", DataType::FLOAT_VECTOR4}},
                                {{"modelViewProj", DataType::FLOAT_MATRIX4}});

            engine->getCache()->setShader(SHADER_TEXTURE, textureShader);

            std::shared_ptr<Shader> colorShader = std::make_shared<Shader>();
            colorShader->init(std::vector<uint8_t>(std::begin(COLOR_PIXEL_SHADER_D3D11), std::end(COLOR_PIXEL_SHADER_D3D11)),
                              std::vector<uint8_t>(std::begin(COLOR_VERTEX_SHADER_D3D11), std::end(COLOR_VERTEX_SHADER_D3D11)),
                              {Vertex::Attribute::Usage::POSITION, Vertex::Attribute::Usage::COLOR},
                              {{"color", DataType::FLOAT_VECTOR4}},
                              {{"modelViewProj", DataType::FLOAT_MATRIX4}});

            engine->getCache()->setShader(SHADER_COLOR, colorShader);

            frameBufferClearColor[0] = clearColor.normR();
            frameBufferClearColor[1] = clearColor.normG();
            frameBufferClearColor[2] = clearColor.normB();
            frameBufferClearColor[3] = clearColor.normA();

            running = true;
            renderThread = Thread(std::bind(&RenderDeviceD3D11::main, this), "Render");
        }

        void RenderDeviceD3D11::setClearColor(Color color)
        {
            RenderDevice::setClearColor(color);

            frameBufferClearColor[0] = clearColor.normR();
            frameBufferClearColor[1] = clearColor.normG();
            frameBufferClearColor[2] = clearColor.normB();
            frameBufferClearColor[3] = clearColor.normA();
        }

        void RenderDeviceD3D11::setSize(const Size2& newSize)
        {
            RenderDevice::setSize(newSize);

            resizeBackBuffer(static_cast<UINT>(size.width),
                             static_cast<UINT>(size.height));
        }

        void RenderDeviceD3D11::setFullscreen(bool newFullscreen)
        {
            executeOnRenderThread([newFullscreen, this]() {
                if (FAILED(swapChain->SetFullscreenState(newFullscreen ? TRUE : FALSE, nullptr)))
                    throw SystemError("Failed to set fullscreen state");
            });
        }

        bool RenderDeviceD3D11::processCommands(CommandBuffer& commands)
        {
            std::vector<float> shaderData;

            uint32_t fillModeIndex = 0;
            uint32_t scissorEnableIndex = 0;
            uint32_t cullModeIndex = 0;
            ShaderResourceD3D11* currentShader = nullptr;

            while (Command* command = commands.front())
            {
                switch (command->type)
                {
                    case Command::Type::SET_RENDER_TARGET:
                    {
                        const SetRenderTargetCommand* setRenderTargetCommand = static_cast<const SetRenderTargetCommand*>(command);

                        ID3D11RenderTargetView* newRenderTargetView = nullptr;
                        ID3D11DepthStencilView* newDepthStencilView = nullptr;

                        if (setRenderTargetCommand->renderTarget)
                        {
                            TextureResourceD3D11* renderTargetD3D11 = static_cast<TextureResourceD3D11*>(setRenderTargetCommand->renderTarget);

                            if (!renderTargetD3D11->getRenderTargetView())
                                continue;

                            newRenderTargetView = renderTargetD3D11->getRenderTargetView();
                            newDepthStencilView = renderTargetD3D11->getDepthStencilView();
                        }
                        else
                        {
                            newRenderTargetView = renderTargetView;
                            newDepthStencilView = depthStencilView;
                        }

                        context->OMSetRenderTargets(1, &newRenderTargetView, newDepthStencilView);

                        break;
                    }

                    case Command::Type::CLEAR:
                    {
                        const ClearCommand* clearCommand = static_cast<const ClearCommand*>(command);

                        ID3D11RenderTargetView* newRenderTargetView = nullptr;
                        ID3D11DepthStencilView* newDepthStencilView = nullptr;
                        const FLOAT* newClearColor;
                        FLOAT newClearDepth;
                        bool newClearFrameBufferView = false;
                        bool newClearDepthBufferView = false;
                        UINT renderTargetWidth = 0;
                        UINT renderTargetHeight = 0;

                        if (clearCommand->renderTarget)
                        {
                            TextureResourceD3D11* renderTargetD3D11 = static_cast<TextureResourceD3D11*>(clearCommand->renderTarget);

                            if (!renderTargetD3D11->getRenderTargetView())
                                continue;

                            renderTargetWidth = renderTargetD3D11->getWidth();
                            renderTargetHeight = renderTargetD3D11->getHeight();
                            newRenderTargetView = renderTargetD3D11->getRenderTargetView();
                            newDepthStencilView = renderTargetD3D11->getDepthStencilView();
                            newClearColor = renderTargetD3D11->getFrameBufferClearColor();
                            newClearDepth = renderTargetD3D11->getClearDepth();
                            newClearFrameBufferView = renderTargetD3D11->getClearFrameBufferView();
                            newClearDepthBufferView = renderTargetD3D11->getClearDepthBufferView();
                        }
                        else
                        {
                            renderTargetWidth = frameBufferWidth;
                            renderTargetHeight = frameBufferHeight;
                            newRenderTargetView = renderTargetView;
                            newDepthStencilView = depthStencilView;
                            newClearColor = frameBufferClearColor;
                            newClearDepth = clearDepth;
                            newClearFrameBufferView = clearColorBuffer;
                            newClearDepthBufferView = clearDepthBuffer;
                        }

                        context->OMSetRenderTargets(1, &newRenderTargetView, newDepthStencilView);

                        D3D11_VIEWPORT viewport;
                        viewport.MinDepth = 0.0F;
                        viewport.MaxDepth = 1.0F;
                        viewport.TopLeftX = viewport.TopLeftY = 0.0f;
                        viewport.Width = static_cast<FLOAT>(renderTargetWidth);
                        viewport.Height = static_cast<FLOAT>(renderTargetHeight);
                        context->RSSetViewports(1, &viewport);

                        if (newClearFrameBufferView)
                            context->ClearRenderTargetView(newRenderTargetView, newClearColor);

                        if (newClearDepthBufferView)
                            context->ClearDepthStencilView(newDepthStencilView, D3D11_CLEAR_DEPTH, newClearDepth, 0);

                        break;
                    }

                    case Command::Type::SET_CULL_MODE:
                    {
                        const SetCullModeCommad* setCullModeCommad = static_cast<const SetCullModeCommad*>(command);

                        switch (setCullModeCommad->cullMode)
                        {
                            case Renderer::CullMode::NONE: cullModeIndex = 0; break;
                            case Renderer::CullMode::FRONT: cullModeIndex = 1; break;
                            case Renderer::CullMode::BACK: cullModeIndex = 2; break;
                            default: Log(Log::Level::ERR) << "Invalid cull mode"; return false;
                        }

                        uint32_t rasterizerStateIndex = fillModeIndex * 6 + scissorEnableIndex * 3 + cullModeIndex;
                        context->RSSetState(rasterizerStates[rasterizerStateIndex]);

                        break;
                    }

                    case Command::Type::SET_FILL_MODE:
                    {
                        const SetFillModeCommad* setFillModeCommad = static_cast<const SetFillModeCommad*>(command);

                        switch (setFillModeCommad->fillMode)
                        {
                            case Renderer::FillMode::SOLID: fillModeIndex = 0; break;
                            case Renderer::FillMode::WIREFRAME: fillModeIndex = 1; break;
                            default: Log(Log::Level::ERR) << "Invalid fill mode"; return false;
                        }

                        uint32_t rasterizerStateIndex = fillModeIndex * 6 + scissorEnableIndex * 3 + cullModeIndex;
                        context->RSSetState(rasterizerStates[rasterizerStateIndex]);

                        break;
                    }

                    case Command::Type::SET_SCISSOR_TEST:
                    {
                        const SetScissorTestCommand* setScissorTestCommand = static_cast<const SetScissorTestCommand*>(command);

                        if (setScissorTestCommand->enabled)
                        {
                            D3D11_RECT rect;
                            rect.left = static_cast<LONG>(setScissorTestCommand->rectangle.position.x);
                            rect.top = static_cast<LONG>(setScissorTestCommand->rectangle.position.y);
                            rect.right = static_cast<LONG>(setScissorTestCommand->rectangle.position.x + setScissorTestCommand->rectangle.size.width);
                            rect.bottom = static_cast<LONG>(setScissorTestCommand->rectangle.position.y + setScissorTestCommand->rectangle.size.height);
                            context->RSSetScissorRects(1, &rect);
                        }

                        scissorEnableIndex = (setScissorTestCommand->enabled) ? 1 : 0;

                        uint32_t rasterizerStateIndex = fillModeIndex * 6 + scissorEnableIndex * 3 + cullModeIndex;
                        context->RSSetState(rasterizerStates[rasterizerStateIndex]);

                        break;
                    }

                    case Command::Type::SET_VIEWPORT:
                    {
                        const SetViewportCommand* setViewportCommand = static_cast<const SetViewportCommand*>(command);

                        D3D11_VIEWPORT viewport;
                        viewport.MinDepth = 0.0F;
                        viewport.MaxDepth = 1.0F;
                        viewport.TopLeftX = setViewportCommand->viewport.position.x;
                        viewport.TopLeftY = setViewportCommand->viewport.position.y;
                        viewport.Width = setViewportCommand->viewport.size.width;
                        viewport.Height = setViewportCommand->viewport.size.height;
                        context->RSSetViewports(1, &viewport);

                        break;
                    }

                    case Command::Type::SET_DEPTH_STATE:
                    {
                        const SetDepthStateCommand* setDepthStateCommand = static_cast<const SetDepthStateCommand*>(command);

                        uint32_t depthTestIndex = setDepthStateCommand->depthTest ? 1 : 0;
                        uint32_t depthWriteIndex = setDepthStateCommand->depthWrite ? 1 : 0;
                        uint32_t depthStencilStateIndex = depthTestIndex * 2 + depthWriteIndex;

                        context->OMSetDepthStencilState(depthStencilStates[depthStencilStateIndex], 0);

                        break;
                    }

                    case Command::Type::SET_PIPELINE_STATE:
                    {
                        const SetPipelineStateCommand* setPipelineStateCommand = static_cast<const SetPipelineStateCommand*>(command);

                        BlendStateResourceD3D11* blendStateD3D11 = static_cast<BlendStateResourceD3D11*>(setPipelineStateCommand->blendState);
                        ShaderResourceD3D11* shaderD3D11 = static_cast<ShaderResourceD3D11*>(setPipelineStateCommand->shader);
                        currentShader = shaderD3D11;

                        if (blendStateD3D11)
                            context->OMSetBlendState(blendStateD3D11->getBlendState(), nullptr, 0xffffffff);
                        else
                            context->OMSetBlendState(nullptr, nullptr, 0xffffffff);

                        if (shaderD3D11)
                        {
                            assert(shaderD3D11->getFragmentShader());
                            assert(shaderD3D11->getVertexShader());
                            assert(shaderD3D11->getInputLayout());

                            context->PSSetShader(shaderD3D11->getFragmentShader(), nullptr, 0);
                            context->VSSetShader(shaderD3D11->getVertexShader(), nullptr, 0);
                            context->IASetInputLayout(shaderD3D11->getInputLayout());
                        }
                        else
                        {
                            context->PSSetShader(nullptr, nullptr, 0);
                            context->VSSetShader(nullptr, nullptr, 0);
                            context->IASetInputLayout(nullptr);
                        }

                        break;
                    }

                    case Command::Type::DRAW:
                    {
                        const DrawCommand* drawCommand = static_cast<const DrawCommand*>(command);

                        // draw mesh buffer
                        BufferResourceD3D11* indexBufferD3D11 = static_cast<BufferResourceD3D11*>(drawCommand->indexBuffer);
                        BufferResourceD3D11* vertexBufferD3D11 = static_cast<BufferResourceD3D11*>(drawCommand->vertexBuffer);

                        assert(indexBufferD3D11);
                        assert(indexBufferD3D11->getBuffer());
                        assert(vertexBufferD3D11);
                        assert(vertexBufferD3D11->getBuffer());

                        ID3D11Buffer* buffers[] = {vertexBufferD3D11->getBuffer()};
                        UINT strides[] = {sizeof(Vertex)};
                        UINT offsets[] = {0};
                        context->IASetVertexBuffers(0, 1, buffers, strides, offsets);

                        DXGI_FORMAT indexFormat;

                        switch (drawCommand->indexSize)
                        {
                            case 2: indexFormat = DXGI_FORMAT_R16_UINT; break;
                            case 4: indexFormat = DXGI_FORMAT_R32_UINT; break;
                            default:
                                indexFormat = DXGI_FORMAT_UNKNOWN;
                                Log(Log::Level::ERR) << "Invalid index size";
                                return false;
                        }

                        context->IASetIndexBuffer(indexBufferD3D11->getBuffer(), indexFormat, 0);

                        D3D_PRIMITIVE_TOPOLOGY topology;

                        switch (drawCommand->drawMode)
                        {
                            case Renderer::DrawMode::POINT_LIST: topology = D3D_PRIMITIVE_TOPOLOGY_POINTLIST; break;
                            case Renderer::DrawMode::LINE_LIST: topology = D3D_PRIMITIVE_TOPOLOGY_LINELIST; break;
                            case Renderer::DrawMode::LINE_STRIP: topology = D3D_PRIMITIVE_TOPOLOGY_LINESTRIP; break;
                            case Renderer::DrawMode::TRIANGLE_LIST: topology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST; break;
                            case Renderer::DrawMode::TRIANGLE_STRIP: topology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP; break;
                            default: Log(Log::Level::ERR) << "Invalid draw mode"; return false;
                        }

                        context->IASetPrimitiveTopology(topology);

                        assert(drawCommand->indexCount);
                        assert(indexBufferD3D11->getSize());
                        assert(vertexBufferD3D11->getSize());

                        context->DrawIndexed(drawCommand->indexCount, drawCommand->startIndex, 0);

                        break;
                    }

                    case Command::Type::PUSH_DEBUG_MARKER:
                    {
                        // const PushDebugMarkerCommand* pushDebugMarkerCommand = static_cast<const PushDebugMarkerCommand*>(command);
                        // D3D11 does not support debug markers
                        break;
                    }

                    case Command::Type::POP_DEBUG_MARKER:
                    {
                        // const PopDebugMarkerCommand* popDebugMarkerCommand = static_cast<const PopDebugMarkerCommand*>(command);
                        // D3D11 does not support debug markers
                        break;
                    }

                    case Command::Type::INIT_BLEND_STATE:
                    {
                        const InitBlendStateCommand* initBlendStateCommand = static_cast<const InitBlendStateCommand*>(command);

                        initBlendStateCommand->blendState->init(initBlendStateCommand->enableBlending,
                                                                initBlendStateCommand->colorBlendSource,
                                                                initBlendStateCommand->colorBlendDest,
                                                                initBlendStateCommand->colorOperation,
                                                                initBlendStateCommand->alphaBlendSource,
                                                                initBlendStateCommand->alphaBlendDest,
                                                                initBlendStateCommand->alphaOperation,
                                                                initBlendStateCommand->colorMask);
                        break;
                    }

                    case Command::Type::INIT_BUFFER:
                    {
                        const InitBufferCommand* initBufferCommand = static_cast<const InitBufferCommand*>(command);

                        initBufferCommand->buffer->init(initBufferCommand->usage,
                                                        initBufferCommand->flags,
                                                        initBufferCommand->data,
                                                        initBufferCommand->size);
                        break;
                    }

                    case Command::Type::SET_BUFFER_DATA:
                    {
                        const SetBufferDataCommand* setBufferDataCommand = static_cast<const SetBufferDataCommand*>(command);

                        setBufferDataCommand->buffer->setData(setBufferDataCommand->data);
                        break;
                    }

                    case Command::Type::INIT_SHADER:
                    {
                        const InitShaderCommand* initShaderCommand = static_cast<const InitShaderCommand*>(command);

                        initShaderCommand->shader->init(initShaderCommand->fragmentShader,
                                                        initShaderCommand->vertexShader,
                                                        initShaderCommand->vertexAttributes,
                                                        initShaderCommand->fragmentShaderConstantInfo,
                                                        initShaderCommand->vertexShaderConstantInfo,
                                                        initShaderCommand->fragmentShaderDataAlignment,
                                                        initShaderCommand->vertexShaderDataAlignment,
                                                        initShaderCommand->fragmentShaderFunction,
                                                        initShaderCommand->vertexShaderFunction);

                        break;
                    }

                    case Command::Type::SET_SHADER_CONSTANTS:
                    {
                        const SetShaderConstantsCommand* setShaderConstantsCommand = static_cast<const SetShaderConstantsCommand*>(command);

                        if (!currentShader)
                        {
                            Log(Log::Level::ERR) << "No shader set";
                            return false;
                        }

                        // pixel shader constants
                        const std::vector<ShaderResourceD3D11::Location>& fragmentShaderConstantLocations = currentShader->getFragmentShaderConstantLocations();

                        if (setShaderConstantsCommand->fragmentShaderConstants.size() > fragmentShaderConstantLocations.size())
                        {
                            Log(Log::Level::ERR) << "Invalid pixel shader constant size";
                            return false;
                        }

                        shaderData.clear();

                        for (size_t i = 0; i < setShaderConstantsCommand->fragmentShaderConstants.size(); ++i)
                        {
                            const ShaderResourceD3D11::Location& fragmentShaderConstantLocation = fragmentShaderConstantLocations[i];
                            const std::vector<float>& fragmentShaderConstant = setShaderConstantsCommand->fragmentShaderConstants[i];

                            if (sizeof(float) * fragmentShaderConstant.size() != fragmentShaderConstantLocation.size)
                            {
                                Log(Log::Level::ERR) << "Invalid pixel shader constant size";
                                return false;
                            }

                            shaderData.insert(shaderData.end(), fragmentShaderConstant.begin(), fragmentShaderConstant.end());
                        }

                        if (!uploadBuffer(currentShader->getFragmentShaderConstantBuffer(),
                                        shaderData.data(),
                                        static_cast<uint32_t>(sizeof(float) * shaderData.size())))
                            return false;

                        ID3D11Buffer* fragmentShaderConstantBuffers[1] = {currentShader->getFragmentShaderConstantBuffer()};
                        context->PSSetConstantBuffers(0, 1, fragmentShaderConstantBuffers);

                        // vertex shader constants
                        const std::vector<ShaderResourceD3D11::Location>& vertexShaderConstantLocations = currentShader->getVertexShaderConstantLocations();

                        if (setShaderConstantsCommand->vertexShaderConstants.size() > vertexShaderConstantLocations.size())
                        {
                            Log(Log::Level::ERR) << "Invalid vertex shader constant size";
                            return false;
                        }

                        shaderData.clear();

                        for (size_t i = 0; i < setShaderConstantsCommand->vertexShaderConstants.size(); ++i)
                        {
                            const ShaderResourceD3D11::Location& vertexShaderConstantLocation = vertexShaderConstantLocations[i];
                            const std::vector<float>& vertexShaderConstant = setShaderConstantsCommand->vertexShaderConstants[i];

                            if (sizeof(float) * vertexShaderConstant.size() != vertexShaderConstantLocation.size)
                            {
                                Log(Log::Level::ERR) << "Invalid pixel shader constant size";
                                return false;
                            }

                            shaderData.insert(shaderData.end(), vertexShaderConstant.begin(), vertexShaderConstant.end());
                        }

                        if (!uploadBuffer(currentShader->getVertexShaderConstantBuffer(),
                                          shaderData.data(),
                                          static_cast<uint32_t>(sizeof(float) * shaderData.size())))
                            return false;

                        ID3D11Buffer* vertexShaderConstantBuffers[1] = {currentShader->getVertexShaderConstantBuffer()};
                        context->VSSetConstantBuffers(0, 1, vertexShaderConstantBuffers);

                        break;
                    }

                    case Command::Type::SET_TEXTURES:
                    {
                        const SetTexturesCommand* setTexturesCommand = static_cast<const SetTexturesCommand*>(command);

                        ID3D11ShaderResourceView* resourceViews[Texture::LAYERS];
                        ID3D11SamplerState* samplers[Texture::LAYERS];

                        for (uint32_t layer = 0; layer < Texture::LAYERS; ++layer)
                        {
                            TextureResourceD3D11* textureD3D11 = static_cast<TextureResourceD3D11*>(setTexturesCommand->textures[layer]);

                            if (textureD3D11)
                            {
                                if (!textureD3D11->getResourceView() ||
                                    !textureD3D11->getSamplerState())
                                    return false;

                                resourceViews[layer] = textureD3D11->getResourceView();
                                samplers[layer] = textureD3D11->getSamplerState();
                            }
                            else
                            {
                                resourceViews[layer] = nullptr;
                                samplers[layer] = nullptr;
                            }
                        }

                        context->PSSetShaderResources(0, Texture::LAYERS, resourceViews);
                        context->PSSetSamplers(0, Texture::LAYERS, samplers);

                        break;
                    }

                    default: return false;
                }

                commands.pop();
            }

            swapChain->Present(swapInterval, 0);

            return true;
        }

        IDXGIOutput* RenderDeviceD3D11::getOutput() const
        {
            WindowResourceWin* windowWin = static_cast<WindowResourceWin*>(window->getResource());

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

            while ((hr = adapter->EnumOutputs(i, &output)) != DXGI_ERROR_NOT_FOUND)
            {
                if (SUCCEEDED(hr))
                {
                    hr = output->GetDesc(&outputDesc);

                    if (SUCCEEDED(hr) && outputDesc.Monitor == monitor)
                        return output;

                    output->Release();
                }

                ++i;
            }

            return nullptr;
        }

        std::vector<Size2> RenderDeviceD3D11::getSupportedResolutions() const
        {
            std::vector<Size2> result;

            IDXGIOutput* output = getOutput();

            if (!output) return result;

            UINT numModes = 0;
            DXGI_FORMAT format = DXGI_FORMAT_R8G8B8A8_UNORM;
            HRESULT hr = output->GetDisplayModeList(format, 0, &numModes, nullptr);
            if (FAILED(hr))
            {
                output->Release();
                throw SystemError("Failed to get display mode list, error: " + std::to_string(hr));
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

        void RenderDeviceD3D11::generateScreenshot(const std::string& filename)
        {
            ID3D11Texture2D* backBufferTexture;
            HRESULT hr = backBuffer->QueryInterface(IID_ID3D11Texture2D, reinterpret_cast<void**>(&backBufferTexture));
            if (FAILED(hr))
                throw SystemError("Failed to get Direct3D 11 back buffer texture, error: " + std::to_string(hr));

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
                throw SystemError("Failed to create Direct3D 11 texture, error: " + std::to_string(hr));

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
                    throw SystemError("Failed to create Direct3D 11 texture, error: " + std::to_string(hr));
                }

                context->ResolveSubresource(resolveTexture, 0, backBuffer, 0, DXGI_FORMAT_R8G8B8A8_UNORM);
                context->CopyResource(texture, resolveTexture);
                resolveTexture->Release();
            }
            else
                context->CopyResource(texture, backBuffer);

            D3D11_MAPPED_SUBRESOURCE mappedSubresource;
            hr = context->Map(texture, 0, D3D11_MAP_READ, 0, &mappedSubresource);
            if (FAILED(hr))
            {
                texture->Release();
                throw SystemError("Failed to map Direct3D 11 resource, error: " + std::to_string(hr));
            }

            if (!stbi_write_png(filename.c_str(), textureDesc.Width, textureDesc.Height, 4, mappedSubresource.pData, static_cast<int>(mappedSubresource.RowPitch)))
            {
                context->Unmap(texture, 0);
                texture->Release();
                throw FileError("Failed to save screenshot to file");
            }

            context->Unmap(texture, 0);
            texture->Release();
        }

        BlendStateResource* RenderDeviceD3D11::createBlendState()
        {
            Lock lock(resourceMutex);

            BlendStateResource* blendState = new BlendStateResourceD3D11(*this);
            resources.push_back(std::unique_ptr<RenderResource>(blendState));
            return blendState;
        }

        BufferResource* RenderDeviceD3D11::createBuffer()
        {
            Lock lock(resourceMutex);

            BufferResource* buffer = new BufferResourceD3D11(*this);
            resources.push_back(std::unique_ptr<RenderResource>(buffer));
            return buffer;
        }

        RenderTargetResource* RenderDeviceD3D11::createRenderTarget()
        {
            Lock lock(resourceMutex);

            RenderTargetResource* renderTarget = new RenderTargetResourceD3D11();
            resources.push_back(std::unique_ptr<RenderResource>(renderTarget));
            return renderTarget;
        }

        ShaderResource* RenderDeviceD3D11::createShader()
        {
            Lock lock(resourceMutex);

            ShaderResource* shader = new ShaderResourceD3D11(*this);
            resources.push_back(std::unique_ptr<RenderResource>(shader));
            return shader;
        }

        TextureResource* RenderDeviceD3D11::createTexture()
        {
            Lock lock(resourceMutex);

            TextureResource* texture = new TextureResourceD3D11(*this);
            resources.push_back(std::unique_ptr<RenderResource>(texture));
            return texture;
        }

        void RenderDeviceD3D11::resizeBackBuffer(UINT newWidth, UINT newHeight)
        {
            if (frameBufferWidth != newWidth || frameBufferHeight != newHeight)
            {
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
                    throw SystemError("Failed to resize Direct3D 11 backbuffer, error: " + std::to_string(hr));

                hr = swapChain->GetBuffer(0, IID_ID3D11Texture2D, reinterpret_cast<void**>(&backBuffer));
                if (FAILED(hr))
                    throw SystemError("Failed to retrieve Direct3D 11 backbuffer, error: " + std::to_string(hr));

                hr = device->CreateRenderTargetView(backBuffer, nullptr, &renderTargetView);
                if (FAILED(hr))
                    throw SystemError("Failed to create Direct3D 11 render target view, error: " + std::to_string(hr));

                D3D11_TEXTURE2D_DESC desc;
                backBuffer->GetDesc(&desc);

                if (depth)
                {
                    D3D11_TEXTURE2D_DESC depthStencilDesc;
                    depthStencilDesc.Width = desc.Width;
                    depthStencilDesc.Height = desc.Height;
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
                        throw SystemError("Failed to create Direct3D 11 depth stencil texture, error: " + std::to_string(hr));

                    hr = device->CreateDepthStencilView(depthStencilTexture, nullptr, &depthStencilView);
                    if (FAILED(hr))
                        throw SystemError("Failed to create Direct3D 11 depth stencil view, error: " + std::to_string(hr));
                }

                frameBufferWidth = desc.Width;
                frameBufferHeight = desc.Height;
            }
        }

        bool RenderDeviceD3D11::uploadBuffer(ID3D11Buffer* buffer, const void* data, uint32_t dataSize)
        {
            D3D11_MAPPED_SUBRESOURCE mappedSubresource;
            HRESULT hr = context->Map(buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedSubresource);
            if (FAILED(hr))
            {
                Log(Log::Level::ERR) << "Failed to lock Direct3D 11 buffer, error: " << hr;
                return false;
            }

            std::copy(static_cast<const uint8_t*>(data), static_cast<const uint8_t*>(data) + dataSize, static_cast<uint8_t*>(mappedSubresource.pData));

            context->Unmap(buffer, 0);

            return true;
        }

        ID3D11SamplerState* RenderDeviceD3D11::getSamplerState(const SamplerStateDesc& desc)
        {
            auto samplerStatesIterator = samplerStates.find(desc);

            if (samplerStatesIterator != samplerStates.end())
                return samplerStatesIterator->second;
            else
            {
                D3D11_SAMPLER_DESC samplerStateDesc;

                if (desc.maxAnisotropy > 1)
                    samplerStateDesc.Filter = D3D11_FILTER_ANISOTROPIC;
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
                samplerStateDesc.MipLODBias = 0.0F;
                samplerStateDesc.MaxAnisotropy = desc.maxAnisotropy;
                samplerStateDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;

                samplerStateDesc.BorderColor[0] = 0.0F;
                samplerStateDesc.BorderColor[1] = 0.0F;
                samplerStateDesc.BorderColor[2] = 0.0F;
                samplerStateDesc.BorderColor[3] = 0.0F;

                samplerStateDesc.MinLOD = 0.0F;
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

        void RenderDeviceD3D11::main()
        {
            while (running) process();
        }
    } // namespace graphics
} // namespace ouzel

#endif

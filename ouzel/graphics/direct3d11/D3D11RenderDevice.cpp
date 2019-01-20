// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#include "core/Setup.h"

#if OUZEL_COMPILE_DIRECT3D11

#include <cassert>
#include "D3D11RenderDevice.hpp"
#include "D3D11BlendState.hpp"
#include "D3D11Buffer.hpp"
#include "D3D11DepthStencilState.hpp"
#include "D3D11RenderTarget.hpp"
#include "D3D11Shader.hpp"
#include "D3D11Texture.hpp"
#include "core/Engine.hpp"
#include "core/Window.hpp"
#include "core/windows/NativeWindowWin.hpp"
#include "utils/Log.hpp"
#include "utils/Utils.hpp"
#include "stb_image_write.h"

namespace ouzel
{
    namespace graphics
    {
        const Direct3D11ErrorCategory direct3D11ErrorCategory {};

        static DXGI_FORMAT getIndexFormat(uint32_t indexSize)
        {
            switch (indexSize)
            {
                case 2: return DXGI_FORMAT_R16_UINT;
                case 4: return DXGI_FORMAT_R32_UINT;
                default: throw std::runtime_error("Invalid index size");
            }
        }

        static D3D_PRIMITIVE_TOPOLOGY getPrimitiveTopology(DrawMode drawMode)
        {
            switch (drawMode)
            {
                case DrawMode::POINT_LIST: return D3D_PRIMITIVE_TOPOLOGY_POINTLIST;
                case DrawMode::LINE_LIST: return D3D_PRIMITIVE_TOPOLOGY_LINELIST;
                case DrawMode::LINE_STRIP: return D3D_PRIMITIVE_TOPOLOGY_LINESTRIP;
                case DrawMode::TRIANGLE_LIST: return D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
                case DrawMode::TRIANGLE_STRIP: return D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP;
                default: throw std::runtime_error("Invalid draw mode");
            }
        }

        static D3D11_TEXTURE_ADDRESS_MODE getTextureAddressMode(Texture::Address address)
        {
            switch (address)
            {
                case Texture::Address::CLAMP:
                    return D3D11_TEXTURE_ADDRESS_CLAMP;
                case Texture::Address::REPEAT:
                    return D3D11_TEXTURE_ADDRESS_WRAP;
                case Texture::Address::MIRROR_REPEAT:
                    return D3D11_TEXTURE_ADDRESS_MIRROR;
                default:
                    throw std::runtime_error("Invalid address mode");
            }
        }

        D3D11RenderDevice::D3D11RenderDevice(const std::function<void(const Event&)>& initCallback):
            RenderDevice(Driver::DIRECT3D11, initCallback)
        {
            apiMajorVersion = 11;
            apiMinorVersion = 0;

            std::fill(std::begin(rasterizerStates), std::end(rasterizerStates), nullptr);
        }

        D3D11RenderDevice::~D3D11RenderDevice()
        {
            running = false;
            CommandBuffer commandBuffer;
            commandBuffer.pushCommand(std::unique_ptr<Command>(new PresentCommand()));
            submitCommandBuffer(std::move(commandBuffer));

            if (renderThread.joinable()) renderThread.join();

            resources.clear();

            if (defaultDepthStencilState)
                defaultDepthStencilState->Release();

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

        void D3D11RenderDevice::init(Window* newWindow,
                                     const Size2<uint32_t>& newSize,
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

            D3D_FEATURE_LEVEL featureLevel;
            HRESULT hr;
            if (FAILED(hr = D3D11CreateDevice(nullptr, // adapter
                                              D3D_DRIVER_TYPE_HARDWARE,
                                              nullptr, // software rasterizer (unused)
                                              deviceCreationFlags,
                                              nullptr, // feature levels
                                              0, // no feature levels
                                              D3D11_SDK_VERSION,
                                              &device,
                                              &featureLevel,
                                              &context)))
                throw std::system_error(hr, direct3D11ErrorCategory, "Failed to create the Direct3D 11 device");

            if (featureLevel < D3D_FEATURE_LEVEL_10_0)
                npotTexturesSupported = false;

            IDXGIDevice* dxgiDevice;
            IDXGIFactory* factory;

            device->QueryInterface(IID_IDXGIDevice, reinterpret_cast<void**>(&dxgiDevice));
            dxgiDevice->GetParent(IID_IDXGIAdapter, reinterpret_cast<void**>(&adapter));
            if (FAILED(hr = adapter->GetParent(IID_IDXGIFactory, reinterpret_cast<void**>(&factory))))
                throw std::system_error(hr, direct3D11ErrorCategory, "Failed to get the DXGI factory");

            DXGI_ADAPTER_DESC adapterDesc;
            if (FAILED(hr = adapter->GetDesc(&adapterDesc)))
                throw std::system_error(hr, direct3D11ErrorCategory, "Failed to get the DXGI adapter description");
            else
            {
                int bufferSize = WideCharToMultiByte(CP_UTF8, 0, adapterDesc.Description, -1, nullptr, 0, nullptr, nullptr);
                if (bufferSize != 0)
                {
                    std::vector<char> buffer(bufferSize);
                    if (WideCharToMultiByte(CP_UTF8, 0, adapterDesc.Description, -1, buffer.data(), bufferSize, nullptr, nullptr) != 0)
                        engine->log(Log::Level::INFO) << "Using " << buffer.data() << " for rendering";
                }
            }

            NativeWindowWin* windowWin = static_cast<NativeWindowWin*>(window->getNativeWindow());

            frameBufferWidth = static_cast<UINT>(newSize.v[0]);
            frameBufferHeight = static_cast<UINT>(newSize.v[1]);

            UINT qualityLevels;
            UINT supportedSampleCount;
            for (supportedSampleCount = sampleCount; supportedSampleCount > 1; --supportedSampleCount)
            {
                if (FAILED(hr = device->CheckMultisampleQualityLevels(DXGI_FORMAT_R8G8B8A8_UNORM, supportedSampleCount, &qualityLevels)))
                    throw std::system_error(hr, direct3D11ErrorCategory, "Failed to check Direct3D 11 multisample quality levels");
                else if (qualityLevels)
                    break;
            }

            if (supportedSampleCount != sampleCount)
            {
                sampleCount = supportedSampleCount;
                engine->log(Log::Level::WARN) << "Chosen sample count not supported, using: " << sampleCount;
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

            if (FAILED(hr = factory->CreateSwapChain(device, &swapChainDesc, &swapChain)))
                throw std::system_error(hr, direct3D11ErrorCategory, "Failed to create the Direct3D 11 swap chain");

            factory->MakeWindowAssociation(windowWin->getNativeWindow(), DXGI_MWA_NO_ALT_ENTER);

            factory->Release();
            dxgiDevice->Release();

            // Backbuffer
            if (FAILED(hr = swapChain->GetBuffer(0, IID_ID3D11Texture2D, reinterpret_cast<void**>(&backBuffer))))
                throw std::system_error(hr, direct3D11ErrorCategory, "Failed to retrieve Direct3D 11 backbuffer");

            if (FAILED(hr = device->CreateRenderTargetView(backBuffer, nullptr, &renderTargetView)))
                throw std::system_error(hr, direct3D11ErrorCategory, "Failed to create Direct3D 11 render target view");

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

                        if (FAILED(hr = device->CreateRasterizerState(&rasterStateDesc, &rasterizerStates[rasterStateIndex])))
                            throw std::system_error(hr, direct3D11ErrorCategory, "Failed to create Direct3D 11 rasterizer state");

                        ++rasterStateIndex;
                    }
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
                if (FAILED(hr = device->CreateTexture2D(&depthStencilDesc, nullptr, &depthStencilTexture)))
                    throw std::system_error(hr, direct3D11ErrorCategory, "Failed to create Direct3D 11 depth stencil texture");

                if (FAILED(hr = device->CreateDepthStencilView(depthStencilTexture, nullptr, &depthStencilView)))
                    throw std::system_error(hr, direct3D11ErrorCategory, "Failed to create Direct3D 11 depth stencil view");
            }

            D3D11_DEPTH_STENCIL_DESC depthStencilStateDesc;
            depthStencilStateDesc.DepthEnable = FALSE;
            depthStencilStateDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
            depthStencilStateDesc.DepthFunc = D3D11_COMPARISON_ALWAYS;
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

            if (FAILED(hr = device->CreateDepthStencilState(&depthStencilStateDesc, &defaultDepthStencilState)))
                throw std::system_error(hr, direct3D11ErrorCategory, "Failed to create Direct3D 11 depth stencil state");

            running = true;
            renderThread = std::thread(&D3D11RenderDevice::main, this);
        }

        void D3D11RenderDevice::setClearColorBuffer(bool clear)
        {
            clearColorBuffer = clear;
        }

        void D3D11RenderDevice::setClearDepthBuffer(bool clear)
        {
            clearDepthBuffer = clear;
        }

        void D3D11RenderDevice::setClearColor(Color newClearColor)
        {
            frameBufferClearColor[0] = newClearColor.normR();
            frameBufferClearColor[1] = newClearColor.normG();
            frameBufferClearColor[2] = newClearColor.normB();
            frameBufferClearColor[3] = newClearColor.normA();
        }

        void D3D11RenderDevice::setClearDepth(float newClearDepth)
        {
            clearDepth = newClearDepth;
        }

        void D3D11RenderDevice::setSize(const Size2<uint32_t>& newSize)
        {
            RenderDevice::setSize(newSize);

            resizeBackBuffer(static_cast<UINT>(size.v[0]),
                             static_cast<UINT>(size.v[1]));
        }

        void D3D11RenderDevice::setFullscreen(bool newFullscreen)
        {
            executeOnRenderThread([newFullscreen, this]() {
                HRESULT hr;
                if (FAILED(hr = swapChain->SetFullscreenState(newFullscreen ? TRUE : FALSE, nullptr)))
                    throw std::system_error(hr, direct3D11ErrorCategory, "Failed to set fullscreen state");
            });
        }

        void D3D11RenderDevice::process()
        {
            RenderDevice::process();
            executeAll();

            std::vector<float> shaderData;

            uint32_t fillModeIndex = 0;
            uint32_t scissorEnableIndex = 0;
            uint32_t cullModeIndex = 0;
            D3D11Texture* currentRenderTarget = nullptr;
            D3D11Shader* currentShader = nullptr;

            CommandBuffer commandBuffer;

            for (;;)
            {
                std::unique_lock<std::mutex> lock(commandQueueMutex);
                while (commandQueue.empty()) commandQueueCondition.wait(lock);
                commandBuffer = std::move(commandQueue.front());
                commandQueue.pop();
                lock.unlock();

                std::unique_ptr<Command> command;

                while (!commandBuffer.isEmpty())
                {
                    command = commandBuffer.popCommand();

                    switch (command->type)
                    {
                        case Command::Type::PRESENT:
                        {
                            if (currentRenderTarget)
                                currentRenderTarget->resolve();

                            swapChain->Present(swapInterval, 0);
                            break;
                        }

                        case Command::Type::DELETE_RESOURCE:
                        {
                            auto deleteResourceCommand = static_cast<const DeleteResourceCommand*>(command.get());
                            resources[deleteResourceCommand->resource - 1].reset();
                            break;
                        }

                        case Command::Type::SET_RENDER_TARGET_PARAMETERS:
                        {
                            auto setRenderTargetParametersCommand = static_cast<const SetRenderTargetParametersCommand*>(command.get());

                            if (setRenderTargetParametersCommand->renderTarget)
                            {
                                D3D11Texture* renderTarget = static_cast<D3D11Texture*>(resources[setRenderTargetParametersCommand->renderTarget - 1].get());
                                renderTarget->setClearColorBuffer(setRenderTargetParametersCommand->clearColorBuffer);
                                renderTarget->setClearDepthBuffer(setRenderTargetParametersCommand->clearDepthBuffer);
                                renderTarget->setClearColor(setRenderTargetParametersCommand->clearColor);
                                renderTarget->setClearDepth(setRenderTargetParametersCommand->clearDepth);
                            }
                            else
                            {
                                setClearColorBuffer(setRenderTargetParametersCommand->clearColorBuffer);
                                setClearDepthBuffer(setRenderTargetParametersCommand->clearDepthBuffer);
                                setClearColor(setRenderTargetParametersCommand->clearColor);
                                setClearDepth(setRenderTargetParametersCommand->clearDepth);
                            }

                            break;
                        }

                        case Command::ADD_RENDER_TARGET_COLOR_TEXTURE:
                        {
                            auto addRenderTargetColorTextureCommand = static_cast<const AddRenderTargetColorTextureCommand*>(command.get());
                            D3D11RenderTarget* renderTarget = static_cast<D3D11RenderTarget*>(resources[addRenderTargetColorTextureCommand->renderTarget - 1].get());
                            D3D11Texture* texture = addRenderTargetColorTextureCommand->texture ? static_cast<D3D11Texture*>(resources[addRenderTargetColorTextureCommand->texture - 1].get()) : nullptr;
                            renderTarget->addColorTexture(texture);
                            break;
                        }

                        case Command::REMOVE_RENDER_TARGET_COLOR_TEXTURE:
                        {
                            auto removeRenderTargetColorTextureCommand = static_cast<const RemoveRenderTargetColorTextureCommand*>(command.get());
                            D3D11RenderTarget* renderTarget = static_cast<D3D11RenderTarget*>(resources[removeRenderTargetColorTextureCommand->renderTarget - 1].get());
                            D3D11Texture* texture = removeRenderTargetColorTextureCommand->texture ? static_cast<D3D11Texture*>(resources[removeRenderTargetColorTextureCommand->texture - 1].get()) : nullptr;
                            renderTarget->removeColorTexture(texture);
                            break;
                        }

                        case Command::SET_RENDER_TARGET_DEPTH_TEXTURE:
                        {
                            auto setRenderTargetDepthTextureCommand = static_cast<const SetRenderTargetDepthTextureCommand*>(command.get());
                            D3D11RenderTarget* renderTarget = static_cast<D3D11RenderTarget*>(resources[setRenderTargetDepthTextureCommand->renderTarget - 1].get());
                            D3D11Texture* texture = setRenderTargetDepthTextureCommand->texture ? static_cast<D3D11Texture*>(resources[setRenderTargetDepthTextureCommand->texture - 1].get()) : nullptr;
                            renderTarget->setDepthTexture(texture);
                            break;
                        }

                        case Command::Type::SET_RENDER_TARGET:
                        {
                            auto setRenderTargetCommand = static_cast<const SetRenderTargetCommand*>(command.get());

                            D3D11Texture* newRenderTarget = nullptr;
                            if (setRenderTargetCommand->renderTarget)
                                newRenderTarget = static_cast<D3D11Texture*>(resources[setRenderTargetCommand->renderTarget - 1].get());

                            if (currentRenderTarget != newRenderTarget)
                            {
                                currentRenderTarget->resolve();

                                ID3D11RenderTargetView* newRenderTargetView = nullptr;
                                ID3D11DepthStencilView* newDepthStencilView = nullptr;
                                if (newRenderTarget)
                                {
                                    if (!newRenderTarget->getRenderTargetView()) break;
                                    newRenderTargetView = newRenderTarget->getRenderTargetView();
                                    newDepthStencilView = newRenderTarget->getDepthStencilView();
                                }
                                else
                                {
                                    newRenderTargetView = renderTargetView;
                                    newDepthStencilView = depthStencilView;
                                }

                                context->OMSetRenderTargets(1, &newRenderTargetView, newDepthStencilView);

                                currentRenderTarget = newRenderTarget;
                            }

                            break;
                        }

                        case Command::Type::CLEAR_RENDER_TARGET:
                        {
                            auto clearCommand = static_cast<const ClearRenderTargetCommand*>(command.get());

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
                                D3D11Texture* renderTarget = static_cast<D3D11Texture*>(resources[clearCommand->renderTarget - 1].get());

                                if (!renderTarget->getRenderTargetView()) break;

                                renderTargetWidth = renderTarget->getWidth();
                                renderTargetHeight = renderTarget->getHeight();
                                newRenderTargetView = renderTarget->getRenderTargetView();
                                newDepthStencilView = renderTarget->getDepthStencilView();
                                newClearColor = renderTarget->getFrameBufferClearColor();
                                newClearDepth = renderTarget->getClearDepth();
                                newClearFrameBufferView = renderTarget->getClearFrameBufferView();
                                newClearDepthBufferView = renderTarget->getClearDepthBufferView();
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

                        case Command::Type::BLIT:
                        {
                            auto blitCommand = static_cast<const BlitCommand*>(command.get());

                            D3D11Texture* sourceD3D11Texture = static_cast<D3D11Texture*>(resources[blitCommand->sourceTexture - 1].get());
                            D3D11Texture* destinationD3D11Texture = static_cast<D3D11Texture*>(resources[blitCommand->destinationTexture - 1].get());

                            D3D11_BOX box;
                            box.left = blitCommand->sourceX;
                            box.top = blitCommand->sourceY;
                            box.front = 0;
                            box.right = blitCommand->sourceX + blitCommand->sourceWidth;
                            box.bottom = blitCommand->sourceY + blitCommand->sourceHeight;
                            box.back = 0;

                            context->CopySubresourceRegion(destinationD3D11Texture->getTexture(),
                                                           blitCommand->destinationLevel,
                                                           blitCommand->destinationX,
                                                           blitCommand->destinationY,
                                                           0,
                                                           sourceD3D11Texture->getTexture(),
                                                           blitCommand->sourceLevel,
                                                           &box);
                            break;
                        }

                        case Command::Type::SET_CULL_MODE:
                        {
                            const SetCullModeCommad* setCullModeCommad = static_cast<const SetCullModeCommad*>(command.get());

                            switch (setCullModeCommad->cullMode)
                            {
                                case CullMode::NONE: cullModeIndex = 0; break;
                                case CullMode::FRONT: cullModeIndex = 1; break;
                                case CullMode::BACK: cullModeIndex = 2; break;
                                default: throw std::runtime_error("Invalid cull mode");
                            }

                            uint32_t rasterizerStateIndex = fillModeIndex * 6 + scissorEnableIndex * 3 + cullModeIndex;
                            context->RSSetState(rasterizerStates[rasterizerStateIndex]);

                            break;
                        }

                        case Command::Type::SET_FILL_MODE:
                        {
                            const SetFillModeCommad* setFillModeCommad = static_cast<const SetFillModeCommad*>(command.get());

                            switch (setFillModeCommad->fillMode)
                            {
                                case FillMode::SOLID: fillModeIndex = 0; break;
                                case FillMode::WIREFRAME: fillModeIndex = 1; break;
                                default: throw std::runtime_error("Invalid fill mode");
                            }

                            uint32_t rasterizerStateIndex = fillModeIndex * 6 + scissorEnableIndex * 3 + cullModeIndex;
                            context->RSSetState(rasterizerStates[rasterizerStateIndex]);

                            break;
                        }

                        case Command::Type::SET_SCISSOR_TEST:
                        {
                            auto setScissorTestCommand = static_cast<const SetScissorTestCommand*>(command.get());

                            if (setScissorTestCommand->enabled)
                            {
                                D3D11_RECT rect;
                                rect.left = static_cast<LONG>(setScissorTestCommand->rectangle.position.v[0]);
                                rect.top = static_cast<LONG>(setScissorTestCommand->rectangle.position.v[1]);
                                rect.right = static_cast<LONG>(setScissorTestCommand->rectangle.position.v[0] + setScissorTestCommand->rectangle.size.v[0]);
                                rect.bottom = static_cast<LONG>(setScissorTestCommand->rectangle.position.v[1] + setScissorTestCommand->rectangle.size.v[1]);
                                context->RSSetScissorRects(1, &rect);
                            }

                            scissorEnableIndex = (setScissorTestCommand->enabled) ? 1 : 0;

                            uint32_t rasterizerStateIndex = fillModeIndex * 6 + scissorEnableIndex * 3 + cullModeIndex;
                            context->RSSetState(rasterizerStates[rasterizerStateIndex]);

                            break;
                        }

                        case Command::Type::SET_VIEWPORT:
                        {
                            auto setViewportCommand = static_cast<const SetViewportCommand*>(command.get());

                            D3D11_VIEWPORT viewport;
                            viewport.MinDepth = 0.0F;
                            viewport.MaxDepth = 1.0F;
                            viewport.TopLeftX = setViewportCommand->viewport.position.v[0];
                            viewport.TopLeftY = setViewportCommand->viewport.position.v[1];
                            viewport.Width = setViewportCommand->viewport.size.v[0];
                            viewport.Height = setViewportCommand->viewport.size.v[1];
                            context->RSSetViewports(1, &viewport);

                            break;
                        }

                        case Command::Type::INIT_DEPTH_STENCIL_STATE:
                        {
                            auto initDepthStencilStateCommand = static_cast<const InitDepthStencilStateCommand*>(command.get());
                            std::unique_ptr<D3D11DepthStencilState> depthStencilState(new D3D11DepthStencilState(*this,
                                                                                                                 initDepthStencilStateCommand->depthTest,
                                                                                                                 initDepthStencilStateCommand->depthWrite,
                                                                                                                 initDepthStencilStateCommand->compareFunction));

                            if (initDepthStencilStateCommand->depthStencilState > resources.size())
                                resources.resize(initDepthStencilStateCommand->depthStencilState);
                            resources[initDepthStencilStateCommand->depthStencilState - 1] = std::move(depthStencilState);
                            break;
                        }

                        case Command::Type::SET_DEPTH_STENCIL_STATE:
                        {
                            auto setDepthStencilStateCommand = static_cast<const SetDepthStencilStateCommand*>(command.get());

                            if (setDepthStencilStateCommand->depthStencilState)
                            {
                                D3D11DepthStencilState* depthStencilState = static_cast<D3D11DepthStencilState*>(resources[setDepthStencilStateCommand->depthStencilState - 1].get());
                                context->OMSetDepthStencilState(depthStencilState->getDepthStencilState(), 0);
                            }
                            else
                                context->OMSetDepthStencilState(defaultDepthStencilState, 0);

                            break;
                        }

                        case Command::Type::SET_PIPELINE_STATE:
                        {
                            auto setPipelineStateCommand = static_cast<const SetPipelineStateCommand*>(command.get());

                            D3D11BlendState* blendState = static_cast<D3D11BlendState*>(resources[setPipelineStateCommand->blendState - 1].get());
                            D3D11Shader* shader = static_cast<D3D11Shader*>(resources[setPipelineStateCommand->shader - 1].get());
                            currentShader = shader;

                            if (blendState)
                                context->OMSetBlendState(blendState->getBlendState(), nullptr, 0xffffffff);
                            else
                                context->OMSetBlendState(nullptr, nullptr, 0xffffffff);

                            if (shader)
                            {
                                assert(shader->getFragmentShader());
                                assert(shader->getVertexShader());
                                assert(shader->getInputLayout());

                                context->PSSetShader(shader->getFragmentShader(), nullptr, 0);
                                context->VSSetShader(shader->getVertexShader(), nullptr, 0);
                                context->IASetInputLayout(shader->getInputLayout());
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
                            auto drawCommand = static_cast<const DrawCommand*>(command.get());

                            // draw mesh buffer
                            D3D11Buffer* indexBuffer = static_cast<D3D11Buffer*>(resources[drawCommand->indexBuffer - 1].get());
                            D3D11Buffer* vertexBuffer = static_cast<D3D11Buffer*>(resources[drawCommand->vertexBuffer - 1].get());

                            assert(indexBuffer);
                            assert(indexBuffer->getBuffer());
                            assert(vertexBuffer);
                            assert(vertexBuffer->getBuffer());

                            ID3D11Buffer* buffers[] = {vertexBuffer->getBuffer()};
                            UINT strides[] = {sizeof(Vertex)};
                            UINT offsets[] = {0};
                            context->IASetVertexBuffers(0, 1, buffers, strides, offsets);
                            context->IASetIndexBuffer(indexBuffer->getBuffer(),
                                                      getIndexFormat(drawCommand->indexSize), 0);
                            context->IASetPrimitiveTopology(getPrimitiveTopology(drawCommand->drawMode));

                            assert(drawCommand->indexCount);
                            assert(indexBuffer->getSize());
                            assert(vertexBuffer->getSize());

                            context->DrawIndexed(drawCommand->indexCount, drawCommand->startIndex, 0);

                            break;
                        }

                        case Command::Type::PUSH_DEBUG_MARKER:
                        {
                            // D3D11 does not support debug markers
                            break;
                        }

                        case Command::Type::POP_DEBUG_MARKER:
                        {
                            // D3D11 does not support debug markers
                            break;
                        }

                        case Command::Type::INIT_BLEND_STATE:
                        {
                            auto initBlendStateCommand = static_cast<const InitBlendStateCommand*>(command.get());

                            std::unique_ptr<D3D11BlendState> blendState(new D3D11BlendState(*this,
                                                                                            initBlendStateCommand->enableBlending,
                                                                                            initBlendStateCommand->colorBlendSource,
                                                                                            initBlendStateCommand->colorBlendDest,
                                                                                            initBlendStateCommand->colorOperation,
                                                                                            initBlendStateCommand->alphaBlendSource,
                                                                                            initBlendStateCommand->alphaBlendDest,
                                                                                            initBlendStateCommand->alphaOperation,
                                                                                            initBlendStateCommand->colorMask));

                            if (initBlendStateCommand->blendState > resources.size())
                                resources.resize(initBlendStateCommand->blendState);
                            resources[initBlendStateCommand->blendState - 1] = std::move(blendState);
                            break;
                        }

                        case Command::Type::INIT_BUFFER:
                        {
                            auto initBufferCommand = static_cast<const InitBufferCommand*>(command.get());

                            std::unique_ptr<D3D11Buffer> buffer(new D3D11Buffer(*this,
                                                                                initBufferCommand->usage,
                                                                                initBufferCommand->flags,
                                                                                initBufferCommand->data,
                                                                                initBufferCommand->size));

                            if (initBufferCommand->buffer > resources.size())
                                resources.resize(initBufferCommand->buffer);
                            resources[initBufferCommand->buffer - 1] = std::move(buffer);
                            break;
                        }

                        case Command::Type::SET_BUFFER_DATA:
                        {
                            auto setBufferDataCommand = static_cast<const SetBufferDataCommand*>(command.get());

                            D3D11Buffer* buffer = static_cast<D3D11Buffer*>(resources[setBufferDataCommand->buffer - 1].get());
                            buffer->setData(setBufferDataCommand->data);
                            break;
                        }

                        case Command::Type::INIT_SHADER:
                        {
                            auto initShaderCommand = static_cast<const InitShaderCommand*>(command.get());

                            std::unique_ptr<D3D11Shader> shader(new D3D11Shader(*this,
                                                                                initShaderCommand->fragmentShader,
                                                                                initShaderCommand->vertexShader,
                                                                                initShaderCommand->vertexAttributes,
                                                                                initShaderCommand->fragmentShaderConstantInfo,
                                                                                initShaderCommand->vertexShaderConstantInfo,
                                                                                initShaderCommand->fragmentShaderDataAlignment,
                                                                                initShaderCommand->vertexShaderDataAlignment,
                                                                                initShaderCommand->fragmentShaderFunction,
                                                                                initShaderCommand->vertexShaderFunction));

                            if (initShaderCommand->shader > resources.size())
                                resources.resize(initShaderCommand->shader);
                            resources[initShaderCommand->shader - 1] = std::move(shader);
                            break;
                        }

                        case Command::Type::SET_SHADER_CONSTANTS:
                        {
                            auto setShaderConstantsCommand = static_cast<const SetShaderConstantsCommand*>(command.get());

                            if (!currentShader)
                                throw std::runtime_error("No shader set");

                            // pixel shader constants
                            const std::vector<D3D11Shader::Location>& fragmentShaderConstantLocations = currentShader->getFragmentShaderConstantLocations();

                            if (setShaderConstantsCommand->fragmentShaderConstants.size() > fragmentShaderConstantLocations.size())
                                throw std::runtime_error("Invalid pixel shader constant size");

                            shaderData.clear();

                            for (size_t i = 0; i < setShaderConstantsCommand->fragmentShaderConstants.size(); ++i)
                            {
                                const D3D11Shader::Location& fragmentShaderConstantLocation = fragmentShaderConstantLocations[i];
                                const std::vector<float>& fragmentShaderConstant = setShaderConstantsCommand->fragmentShaderConstants[i];

                                if (sizeof(float) * fragmentShaderConstant.size() != fragmentShaderConstantLocation.size)
                                    throw std::runtime_error("Invalid pixel shader constant size");

                                shaderData.insert(shaderData.end(), fragmentShaderConstant.begin(), fragmentShaderConstant.end());
                            }

                            uploadBuffer(currentShader->getFragmentShaderConstantBuffer(),
                                         shaderData.data(),
                                         static_cast<uint32_t>(sizeof(float) * shaderData.size()));

                            ID3D11Buffer* fragmentShaderConstantBuffers[1] = {currentShader->getFragmentShaderConstantBuffer()};
                            context->PSSetConstantBuffers(0, 1, fragmentShaderConstantBuffers);

                            // vertex shader constants
                            const std::vector<D3D11Shader::Location>& vertexShaderConstantLocations = currentShader->getVertexShaderConstantLocations();

                            if (setShaderConstantsCommand->vertexShaderConstants.size() > vertexShaderConstantLocations.size())
                                throw std::runtime_error("Invalid vertex shader constant size");

                            shaderData.clear();

                            for (size_t i = 0; i < setShaderConstantsCommand->vertexShaderConstants.size(); ++i)
                            {
                                const D3D11Shader::Location& vertexShaderConstantLocation = vertexShaderConstantLocations[i];
                                const std::vector<float>& vertexShaderConstant = setShaderConstantsCommand->vertexShaderConstants[i];

                                if (sizeof(float) * vertexShaderConstant.size() != vertexShaderConstantLocation.size)
                                    throw std::runtime_error("Invalid pixel shader constant size");

                                shaderData.insert(shaderData.end(), vertexShaderConstant.begin(), vertexShaderConstant.end());
                            }

                            uploadBuffer(currentShader->getVertexShaderConstantBuffer(),
                                         shaderData.data(),
                                         static_cast<uint32_t>(sizeof(float) * shaderData.size()));

                            ID3D11Buffer* vertexShaderConstantBuffers[1] = {currentShader->getVertexShaderConstantBuffer()};
                            context->VSSetConstantBuffers(0, 1, vertexShaderConstantBuffers);

                            break;
                        }

                        case Command::Type::INIT_TEXTURE:
                        {
                            auto initTextureCommand = static_cast<const InitTextureCommand*>(command.get());

                            std::unique_ptr<D3D11Texture> texture(new D3D11Texture(*this,
                                                                                   initTextureCommand->levels,
                                                                                   initTextureCommand->flags,
                                                                                   initTextureCommand->sampleCount,
                                                                                   initTextureCommand->pixelFormat));

                            if (initTextureCommand->texture > resources.size())
                                resources.resize(initTextureCommand->texture);
                            resources[initTextureCommand->texture - 1] = std::move(texture);
                            break;
                        }

                        case Command::Type::SET_TEXTURE_DATA:
                        {
                            auto setTextureDataCommand = static_cast<const SetTextureDataCommand*>(command.get());

                            D3D11Texture* texture = static_cast<D3D11Texture*>(resources[setTextureDataCommand->texture - 1].get());
                            texture->setData(setTextureDataCommand->levels);

                            break;
                        }

                        case Command::Type::SET_TEXTURE_PARAMETERS:
                        {
                            auto setTextureParametersCommand = static_cast<const SetTextureParametersCommand*>(command.get());

                            D3D11Texture* texture = static_cast<D3D11Texture*>(resources[setTextureParametersCommand->texture - 1].get());
                            texture->setFilter(setTextureParametersCommand->filter);
                            texture->setAddressX(setTextureParametersCommand->addressX);
                            texture->setAddressY(setTextureParametersCommand->addressY);
                            texture->setMaxAnisotropy(setTextureParametersCommand->maxAnisotropy);

                            break;
                        }

                        case Command::Type::SET_TEXTURES:
                        {
                            auto setTexturesCommand = static_cast<const SetTexturesCommand*>(command.get());

                            ID3D11ShaderResourceView* resourceViews[Texture::LAYERS];
                            ID3D11SamplerState* samplers[Texture::LAYERS];

                            for (uint32_t layer = 0; layer < Texture::LAYERS; ++layer)
                            {
                                if (setTexturesCommand->textures[layer])
                                {
                                    D3D11Texture* texture = static_cast<D3D11Texture*>(resources[setTexturesCommand->textures[layer] - 1].get());
                                    resourceViews[layer] = texture->getResourceView();
                                    samplers[layer] = texture->getSamplerState();
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

                        default:
                            throw std::runtime_error("Invalid command");
                    }

                    if (command->type == Command::Type::PRESENT) return;
                }
            }
        }

        IDXGIOutput* D3D11RenderDevice::getOutput() const
        {
            NativeWindowWin* windowWin = static_cast<NativeWindowWin*>(window->getNativeWindow());

            HMONITOR monitor = windowWin->getMonitor();

            if (!monitor)
                throw std::runtime_error("Window is not on any monitor");

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

        std::vector<Size2<uint32_t>> D3D11RenderDevice::getSupportedResolutions() const
        {
            std::vector<Size2<uint32_t>> result;

            IDXGIOutput* output = getOutput();

            if (!output) return result;

            UINT numModes = 0;
            DXGI_FORMAT format = DXGI_FORMAT_R8G8B8A8_UNORM;
            HRESULT hr;
            if (FAILED(hr = output->GetDisplayModeList(format, 0, &numModes, nullptr)))
            {
                output->Release();
                throw std::system_error(hr, direct3D11ErrorCategory, "Failed to get display mode list");
            }

            if (numModes > 0)
            {
                std::vector<DXGI_MODE_DESC> displayModes(numModes);
                output->GetDisplayModeList(format, 0, &numModes, displayModes.data());

                for (const DXGI_MODE_DESC& displayMode : displayModes)
                    result.push_back(Size2<uint32_t>(static_cast<uint32_t>(displayMode.Width),
                                                     static_cast<uint32_t>(displayMode.Height)));
            }

            output->Release();

            return result;
        }

        void D3D11RenderDevice::generateScreenshot(const std::string& filename)
        {
            ID3D11Texture2D* backBufferTexture;
            HRESULT hr;
            if (FAILED(hr = backBuffer->QueryInterface(IID_ID3D11Texture2D, reinterpret_cast<void**>(&backBufferTexture))))
                throw std::system_error(hr, direct3D11ErrorCategory, "Failed to get Direct3D 11 back buffer texture");

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
            if (FAILED(hr = device->CreateTexture2D(&textureDesc, nullptr, &texture)))
                throw std::system_error(hr, direct3D11ErrorCategory, "Failed to create Direct3D 11 texture");

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
                if (FAILED(hr = device->CreateTexture2D(&resolveTextureDesc, nullptr, &resolveTexture)))
                {
                    texture->Release();
                    throw std::system_error(hr, direct3D11ErrorCategory, "Failed to create Direct3D 11 texture");
                }

                context->ResolveSubresource(resolveTexture, 0, backBuffer, 0, DXGI_FORMAT_R8G8B8A8_UNORM);
                context->CopyResource(texture, resolveTexture);
                resolveTexture->Release();
            }
            else
                context->CopyResource(texture, backBuffer);

            D3D11_MAPPED_SUBRESOURCE mappedSubresource;
            if (FAILED(hr = context->Map(texture, 0, D3D11_MAP_READ, 0, &mappedSubresource)))
            {
                texture->Release();
                throw std::system_error(hr, direct3D11ErrorCategory, "Failed to map Direct3D 11 resource");
            }

            if (!stbi_write_png(filename.c_str(), textureDesc.Width, textureDesc.Height, 4, mappedSubresource.pData, static_cast<int>(mappedSubresource.RowPitch)))
            {
                context->Unmap(texture, 0);
                texture->Release();
                throw std::runtime_error("Failed to save screenshot to file");
            }

            context->Unmap(texture, 0);
            texture->Release();
        }

        void D3D11RenderDevice::resizeBackBuffer(UINT newWidth, UINT newHeight)
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

                HRESULT hr;
                if (FAILED(hr = swapChain->ResizeBuffers(0, newWidth, newHeight, DXGI_FORMAT_UNKNOWN, DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH)))
                    throw std::system_error(hr, direct3D11ErrorCategory, "Failed to resize Direct3D 11 backbuffer");

                if (FAILED(hr = swapChain->GetBuffer(0, IID_ID3D11Texture2D, reinterpret_cast<void**>(&backBuffer))))
                    throw std::system_error(hr, direct3D11ErrorCategory, "Failed to retrieve Direct3D 11 backbuffer");

                if (FAILED(hr = device->CreateRenderTargetView(backBuffer, nullptr, &renderTargetView)))
                    throw std::system_error(hr, direct3D11ErrorCategory, "Failed to create Direct3D 11 render target view");

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
                    if (FAILED(hr = device->CreateTexture2D(&depthStencilDesc, nullptr, &depthStencilTexture)))
                        throw std::system_error(hr, direct3D11ErrorCategory, "Failed to create Direct3D 11 depth stencil texture");

                    if (FAILED(hr = device->CreateDepthStencilView(depthStencilTexture, nullptr, &depthStencilView)))
                        throw std::system_error(hr, direct3D11ErrorCategory, "Failed to create Direct3D 11 depth stencil view");
                }

                frameBufferWidth = desc.Width;
                frameBufferHeight = desc.Height;
            }
        }

        void D3D11RenderDevice::uploadBuffer(ID3D11Buffer* buffer, const void* data, uint32_t dataSize)
        {
            D3D11_MAPPED_SUBRESOURCE mappedSubresource;
            HRESULT hr;
            if (FAILED(hr = context->Map(buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedSubresource)))
                throw std::system_error(hr, direct3D11ErrorCategory, "Failed to lock Direct3D 11 buffer");

            std::copy(static_cast<const uint8_t*>(data), static_cast<const uint8_t*>(data) + dataSize, static_cast<uint8_t*>(mappedSubresource.pData));

            context->Unmap(buffer, 0);
        }

        ID3D11SamplerState* D3D11RenderDevice::getSamplerState(const SamplerStateDesc& desc)
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
                        default:
                            throw std::runtime_error("Invalid texture filter");
                    }
                }

                samplerStateDesc.AddressU = getTextureAddressMode(desc.addressX);
                samplerStateDesc.AddressV = getTextureAddressMode(desc.addressY);
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

                HRESULT hr;
                if (FAILED(hr = device->CreateSamplerState(&samplerStateDesc, &samplerState)))
                    throw std::system_error(hr, direct3D11ErrorCategory, "Failed to create Direct3D 11 sampler state");

                samplerStates[desc] = samplerState;

                return samplerState;
            }
        }

        void D3D11RenderDevice::main()
        {
            setCurrentThreadName("Render");

            while (running)
            {
                try
                {
                    process();
                }
                catch (const std::exception& e)
                {
                    engine->log(Log::Level::ERR) << e.what();
                }
            }
        }
    } // namespace graphics
} // namespace ouzel

#endif

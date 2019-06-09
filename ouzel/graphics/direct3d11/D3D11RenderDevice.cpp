// Copyright 2015-2019 Elviss Strazdins. All rights reserved.

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
        namespace d3d11
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

            static D3D11_TEXTURE_ADDRESS_MODE getTextureAddressMode(ouzel::graphics::Texture::Address address)
            {
                switch (address)
                {
                    case ouzel::graphics::Texture::Address::CLAMP_TO_EDGE:
                        return D3D11_TEXTURE_ADDRESS_CLAMP;
                    case ouzel::graphics::Texture::Address::CLAMP_TO_BORDER:
                        return D3D11_TEXTURE_ADDRESS_BORDER;
                    case ouzel::graphics::Texture::Address::REPEAT:
                        return D3D11_TEXTURE_ADDRESS_WRAP;
                    case ouzel::graphics::Texture::Address::MIRROR_REPEAT:
                        return D3D11_TEXTURE_ADDRESS_MIRROR;
                    default:
                        throw std::runtime_error("Invalid address mode");
                }
            }

            RenderDevice::RenderDevice(const std::function<void(const Event&)>& initCallback):
                ouzel::graphics::RenderDevice(Driver::DIRECT3D11, initCallback)
            {
                apiMajorVersion = 11;
                apiMinorVersion = 0;
            }

            RenderDevice::~RenderDevice()
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

            void RenderDevice::init(Window* newWindow,
                                    const Size2U& newSize,
                                    uint32_t newSampleCount,
                                    ouzel::graphics::Texture::Filter newTextureFilter,
                                    uint32_t newMaxAnisotropy,
                                    bool newSrgb,
                                    bool newVerticalSync,
                                    bool newDepth,
                                    bool newStencil,
                                    bool newDebugRenderer)
            {
                ouzel::graphics::RenderDevice::init(newWindow,
                                                    newSize,
                                                    newSampleCount,
                                                    newTextureFilter,
                                                    newMaxAnisotropy,
                                                    newSrgb,
                                                    newVerticalSync,
                                                    newDepth,
                                                    newStencil,
                                                    newDebugRenderer);

                anisotropicFilteringSupported = true;
                renderTargetsSupported = true;
                clampToBorderSupported = true;
                multisamplingSupported = true;

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

                if (featureLevel >= D3D_FEATURE_LEVEL_10_0)
                    npotTexturesSupported = true;

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
                    depthStencilDesc.Format = stencil ? DXGI_FORMAT_D24_UNORM_S8_UINT : DXGI_FORMAT_D32_FLOAT;
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
                renderThread = std::thread(&RenderDevice::main, this);
            }

            void RenderDevice::setFullscreen(bool newFullscreen)
            {
                executeOnRenderThread([newFullscreen, this]() {
                    HRESULT hr;
                    if (FAILED(hr = swapChain->SetFullscreenState(newFullscreen ? TRUE : FALSE, nullptr)))
                        throw std::system_error(hr, direct3D11ErrorCategory, "Failed to set fullscreen state");
                });
            }

            void RenderDevice::process()
            {
                ouzel::graphics::RenderDevice::process();
                executeAll();

                std::vector<float> shaderData;

                uint32_t fillModeIndex = 0;
                uint32_t scissorEnableIndex = 0;
                uint32_t cullModeIndex = 0;
                RenderTarget* currentRenderTarget = nullptr;
                Shader* currentShader = nullptr;

                std::vector<ID3D11ShaderResourceView*> currentResourceViews;
                std::vector<ID3D11SamplerState*> currentSamplerStates;

                CommandBuffer commandBuffer;
                std::unique_ptr<Command> command;

                for (;;)
                {
                    std::unique_lock<std::mutex> lock(commandQueueMutex);
                    while (commandQueue.empty()) commandQueueCondition.wait(lock);
                    commandBuffer = std::move(commandQueue.front());
                    commandQueue.pop();
                    lock.unlock();

                    while (!commandBuffer.isEmpty())
                    {
                        command = commandBuffer.popCommand();

                        switch (command->type)
                        {
                            case Command::Type::RESIZE:
                            {
                                auto resizeCommand = static_cast<const ResizeCommand*>(command.get());
                                resizeBackBuffer(static_cast<UINT>(resizeCommand->size.v[0]),
                                                 static_cast<UINT>(resizeCommand->size.v[1]));
                                break;
                            }

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

                            case Command::INIT_RENDER_TARGET:
                            {
                                auto initRenderTargetCommand = static_cast<const InitRenderTargetCommand*>(command.get());

                                std::set<Texture*> colorTextures;
                                for (uintptr_t colorTextureId : initRenderTargetCommand->colorTextures)
                                    colorTextures.insert(getResource<Texture>(colorTextureId));

                                std::unique_ptr<RenderTarget> renderTarget(new RenderTarget(*this,
                                                                                            colorTextures,
                                                                                            getResource<Texture>(initRenderTargetCommand->depthTexture)));

                                if (initRenderTargetCommand->renderTarget > resources.size())
                                    resources.resize(initRenderTargetCommand->renderTarget);
                                resources[initRenderTargetCommand->renderTarget - 1] = std::move(renderTarget);
                                break;
                            }

                            case Command::Type::SET_RENDER_TARGET:
                            {
                                auto setRenderTargetCommand = static_cast<const SetRenderTargetCommand*>(command.get());

                                if (currentRenderTarget)
                                    currentRenderTarget->resolve();

                                if (setRenderTargetCommand->renderTarget)
                                {
                                    currentRenderTarget = getResource<RenderTarget>(setRenderTargetCommand->renderTarget);
                                    context->OMSetRenderTargets(static_cast<UINT>(currentRenderTarget->getRenderTargetViews().size()),
                                                                currentRenderTarget->getRenderTargetViews().data(),
                                                                currentRenderTarget->getDepthStencilView());
                                }
                                else
                                {
                                    currentRenderTarget = nullptr;
                                    context->OMSetRenderTargets(1, &renderTargetView, depthStencilView);
                                }
                                break;
                            }

                            case Command::Type::CLEAR_RENDER_TARGET:
                            {
                                auto clearCommand = static_cast<const ClearRenderTargetCommand*>(command.get());

                                FLOAT frameBufferClearColor[4]{clearCommand->clearColor.normR(),
                                    clearCommand->clearColor.normG(),
                                    clearCommand->clearColor.normB(),
                                    clearCommand->clearColor.normA()};

                                if (currentRenderTarget)
                                {
                                    if (clearCommand->clearColorBuffer)
                                        for (ID3D11RenderTargetView* view : currentRenderTarget->getRenderTargetViews())
                                            context->ClearRenderTargetView(view, frameBufferClearColor);

                                    if (clearCommand->clearDepthBuffer || clearCommand->clearStencilBuffer)
                                        if (ID3D11DepthStencilView* view = currentRenderTarget->getDepthStencilView())
                                            context->ClearDepthStencilView(view,
                                                                           (clearCommand->clearDepthBuffer ? D3D11_CLEAR_DEPTH : 0) | (clearCommand->clearStencilBuffer ? D3D11_CLEAR_STENCIL : 0),
                                                                           clearCommand->clearDepth,
                                                                           static_cast<UINT8>(clearCommand->clearStencil));
                                }
                                else
                                {
                                    if (clearCommand->clearColorBuffer)
                                        context->ClearRenderTargetView(renderTargetView, frameBufferClearColor);

                                    if (clearCommand->clearDepthBuffer)
                                        context->ClearDepthStencilView(depthStencilView,
                                                                       (clearCommand->clearDepthBuffer ? D3D11_CLEAR_DEPTH : 0) | (clearCommand->clearStencilBuffer ? D3D11_CLEAR_STENCIL : 0),
                                                                       clearCommand->clearDepth,
                                                                       static_cast<UINT8>(clearCommand->clearStencil));
                                }

                                break;
                            }

                            case Command::Type::BLIT:
                            {
                                auto blitCommand = static_cast<const BlitCommand*>(command.get());

                                Texture* sourceTexture = getResource<Texture>(blitCommand->sourceTexture);
                                Texture* destinationTexture = getResource<Texture>(blitCommand->destinationTexture);

                                D3D11_BOX box;
                                box.left = blitCommand->sourceX;
                                box.top = blitCommand->sourceY;
                                box.front = 0;
                                box.right = blitCommand->sourceX + blitCommand->sourceWidth;
                                box.bottom = blitCommand->sourceY + blitCommand->sourceHeight;
                                box.back = 0;

                                context->CopySubresourceRegion(destinationTexture->getTexture(),
                                                               blitCommand->destinationLevel,
                                                               blitCommand->destinationX,
                                                               blitCommand->destinationY,
                                                               0,
                                                               sourceTexture->getTexture(),
                                                               blitCommand->sourceLevel,
                                                               &box);
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
                                std::unique_ptr<DepthStencilState> depthStencilState(new DepthStencilState(*this,
                                                                                                           initDepthStencilStateCommand->depthTest,
                                                                                                           initDepthStencilStateCommand->depthWrite,
                                                                                                           initDepthStencilStateCommand->compareFunction,
                                                                                                           initDepthStencilStateCommand->stencilEnabled,
                                                                                                           initDepthStencilStateCommand->stencilReadMask,
                                                                                                           initDepthStencilStateCommand->stencilWriteMask,
                                                                                                           initDepthStencilStateCommand->frontFaceStencil,
                                                                                                           initDepthStencilStateCommand->backFaceStencil));

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
                                    DepthStencilState* depthStencilState = getResource<DepthStencilState>(setDepthStencilStateCommand->depthStencilState);
                                    context->OMSetDepthStencilState(depthStencilState->getDepthStencilState(),
                                                                    setDepthStencilStateCommand->stencilReferenceValue);
                                }
                                else
                                    context->OMSetDepthStencilState(defaultDepthStencilState,
                                                                    setDepthStencilStateCommand->stencilReferenceValue);

                                break;
                            }

                            case Command::Type::SET_PIPELINE_STATE:
                            {
                                auto setPipelineStateCommand = static_cast<const SetPipelineStateCommand*>(command.get());

                                BlendState* blendState = getResource<BlendState>(setPipelineStateCommand->blendState);
                                Shader* shader = getResource<Shader>(setPipelineStateCommand->shader);
                                currentShader = shader;

                                if (blendState)
                                    context->OMSetBlendState(blendState->getBlendState(), nullptr, 0xFFFFFFFF);
                                else
                                    context->OMSetBlendState(nullptr, nullptr, 0xFFFFFFFF);

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

                                switch (setPipelineStateCommand->cullMode)
                                {
                                    case CullMode::NONE: cullModeIndex = 0; break;
                                    case CullMode::FRONT: cullModeIndex = 1; break;
                                    case CullMode::BACK: cullModeIndex = 2; break;
                                    default: throw std::runtime_error("Invalid cull mode");
                                }

                                switch (setPipelineStateCommand->fillMode)
                                {
                                    case FillMode::SOLID: fillModeIndex = 0; break;
                                    case FillMode::WIREFRAME: fillModeIndex = 1; break;
                                    default: throw std::runtime_error("Invalid fill mode");
                                }

                                uint32_t rasterizerStateIndex = fillModeIndex * 6 + scissorEnableIndex * 3 + cullModeIndex;
                                context->RSSetState(rasterizerStates[rasterizerStateIndex]);
                                break;
                            }

                            case Command::Type::DRAW:
                            {
                                auto drawCommand = static_cast<const DrawCommand*>(command.get());

                                // draw mesh buffer
                                Buffer* indexBuffer = getResource<Buffer>(drawCommand->indexBuffer);
                                Buffer* vertexBuffer = getResource<Buffer>(drawCommand->vertexBuffer);

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

                                std::unique_ptr<BlendState> blendState(new BlendState(*this,
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

                                std::unique_ptr<Buffer> buffer(new Buffer(*this,
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

                                Buffer* buffer = getResource<Buffer>(setBufferDataCommand->buffer);
                                buffer->setData(setBufferDataCommand->data);
                                break;
                            }

                            case Command::Type::INIT_SHADER:
                            {
                                auto initShaderCommand = static_cast<const InitShaderCommand*>(command.get());

                                std::unique_ptr<Shader> shader(new Shader(*this,
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
                                const std::vector<Shader::Location>& fragmentShaderConstantLocations = currentShader->getFragmentShaderConstantLocations();

                                if (setShaderConstantsCommand->fragmentShaderConstants.size() > fragmentShaderConstantLocations.size())
                                    throw std::runtime_error("Invalid pixel shader constant size");

                                shaderData.clear();

                                for (size_t i = 0; i < setShaderConstantsCommand->fragmentShaderConstants.size(); ++i)
                                {
                                    const Shader::Location& fragmentShaderConstantLocation = fragmentShaderConstantLocations[i];
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
                                const std::vector<Shader::Location>& vertexShaderConstantLocations = currentShader->getVertexShaderConstantLocations();

                                if (setShaderConstantsCommand->vertexShaderConstants.size() > vertexShaderConstantLocations.size())
                                    throw std::runtime_error("Invalid vertex shader constant size");

                                shaderData.clear();

                                for (size_t i = 0; i < setShaderConstantsCommand->vertexShaderConstants.size(); ++i)
                                {
                                    const Shader::Location& vertexShaderConstantLocation = vertexShaderConstantLocations[i];
                                    const std::vector<float>& vertexShaderConstant = setShaderConstantsCommand->vertexShaderConstants[i];

                                    if (sizeof(float) * vertexShaderConstant.size() != vertexShaderConstantLocation.size)
                                        throw std::runtime_error("Invalid vertex shader constant size");

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

                                std::unique_ptr<Texture> texture(new Texture(*this,
                                                                             initTextureCommand->levels,
                                                                             initTextureCommand->dimensions,
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

                                Texture* texture = getResource<Texture>(setTextureDataCommand->texture);
                                texture->setData(setTextureDataCommand->levels);

                                break;
                            }

                            case Command::Type::SET_TEXTURE_PARAMETERS:
                            {
                                auto setTextureParametersCommand = static_cast<const SetTextureParametersCommand*>(command.get());

                                Texture* texture = getResource<Texture>(setTextureParametersCommand->texture);
                                texture->setFilter(setTextureParametersCommand->filter);
                                texture->setAddressX(setTextureParametersCommand->addressX);
                                texture->setAddressY(setTextureParametersCommand->addressY);
                                texture->setAddressZ(setTextureParametersCommand->addressZ);
                                texture->setMaxAnisotropy(setTextureParametersCommand->maxAnisotropy);

                                break;
                            }

                            case Command::Type::SET_TEXTURES:
                            {
                                auto setTexturesCommand = static_cast<const SetTexturesCommand*>(command.get());

                                currentResourceViews.clear();
                                currentSamplerStates.clear();

                                for (uintptr_t resource : setTexturesCommand->textures)
                                {
                                    if (Texture* texture = getResource<Texture>(resource))
                                    {
                                        currentResourceViews.push_back(texture->getResourceView());
                                        currentSamplerStates.push_back(texture->getSamplerState());
                                    }
                                    else
                                    {
                                        currentResourceViews.push_back(nullptr);
                                        currentSamplerStates.push_back(nullptr);
                                    }
                                }

                                context->PSSetShaderResources(0, static_cast<UINT>(currentResourceViews.size()), currentResourceViews.data());
                                context->PSSetSamplers(0, static_cast<UINT>(currentSamplerStates.size()), currentSamplerStates.data());

                                break;
                            }

                            default:
                                throw std::runtime_error("Invalid command");
                        }

                        if (command->type == Command::Type::PRESENT) return;
                    }
                }
            }

            IDXGIOutput* RenderDevice::getOutput() const
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

            std::vector<Size2U> RenderDevice::getSupportedResolutions() const
            {
                std::vector<Size2U> result;

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
                        result.push_back(Size2U(static_cast<uint32_t>(displayMode.Width),
                                                static_cast<uint32_t>(displayMode.Height)));
                }

                output->Release();

                return result;
            }

            void RenderDevice::generateScreenshot(const std::string& filename)
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

            void RenderDevice::resizeBackBuffer(UINT newWidth, UINT newHeight)
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

            void RenderDevice::uploadBuffer(ID3D11Buffer* buffer, const void* data, uint32_t dataSize)
            {
                D3D11_MAPPED_SUBRESOURCE mappedSubresource;
                HRESULT hr;
                if (FAILED(hr = context->Map(buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedSubresource)))
                    throw std::system_error(hr, direct3D11ErrorCategory, "Failed to lock Direct3D 11 buffer");

                std::copy(static_cast<const uint8_t*>(data), static_cast<const uint8_t*>(data) + dataSize, static_cast<uint8_t*>(mappedSubresource.pData));

                context->Unmap(buffer, 0);
            }

            ID3D11SamplerState* RenderDevice::getSamplerState(const SamplerStateDesc& desc)
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
                            case ouzel::graphics::Texture::Filter::DEFAULT:
                            case ouzel::graphics::Texture::Filter::POINT:
                                samplerStateDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
                                break;
                            case ouzel::graphics::Texture::Filter::LINEAR:
                                samplerStateDesc.Filter = D3D11_FILTER_MIN_MAG_POINT_MIP_LINEAR;
                                break;
                            case ouzel::graphics::Texture::Filter::BILINEAR:
                                samplerStateDesc.Filter = D3D11_FILTER_MIN_MAG_LINEAR_MIP_POINT;
                                break;
                            case ouzel::graphics::Texture::Filter::TRILINEAR:
                                samplerStateDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
                                break;
                            default:
                                throw std::runtime_error("Invalid texture filter");
                        }
                    }

                    samplerStateDesc.AddressU = getTextureAddressMode(desc.addressX);
                    samplerStateDesc.AddressV = getTextureAddressMode(desc.addressY);
                    samplerStateDesc.AddressW = getTextureAddressMode(desc.addressZ);
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

            void RenderDevice::main()
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
        } // namespace d3d11
    } // namespace graphics
} // namespace ouzel

#endif

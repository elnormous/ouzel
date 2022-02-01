// Ouzel by Elviss Strazdins

#include "../../core/Setup.h"

#if OUZEL_COMPILE_DIRECT3D11

#include <cassert>
#include <cstring>
#include "D3D11RenderDevice.hpp"
#include "D3D11BlendState.hpp"
#include "D3D11Buffer.hpp"
#include "D3D11DepthStencilState.hpp"
#include "D3D11ErrorCategory.hpp"
#include "D3D11RenderTarget.hpp"
#include "D3D11Shader.hpp"
#include "D3D11Texture.hpp"
#include "../../core/Engine.hpp"
#include "../../core/Window.hpp"
#include "../../core/windows/NativeWindowWin.hpp"
#include "../../utils/Log.hpp"
#include "stb_image_write.h"

namespace ouzel::graphics::d3d11
{
    namespace
    {
        const ErrorCategory errorCategory{};

        constexpr DXGI_FORMAT getIndexFormat(std::uint32_t indexSize)
        {
            switch (indexSize)
            {
                case 2: return DXGI_FORMAT_R16_UINT;
                case 4: return DXGI_FORMAT_R32_UINT;
                default: throw std::runtime_error{"Invalid index size"};
            }
        }

        constexpr D3D_PRIMITIVE_TOPOLOGY getPrimitiveTopology(DrawMode drawMode)
        {
            switch (drawMode)
            {
                case DrawMode::pointList: return D3D_PRIMITIVE_TOPOLOGY_POINTLIST;
                case DrawMode::lineList: return D3D_PRIMITIVE_TOPOLOGY_LINELIST;
                case DrawMode::lineStrip: return D3D_PRIMITIVE_TOPOLOGY_LINESTRIP;
                case DrawMode::triangleList: return D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
                case DrawMode::triangleStrip: return D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP;
                default: throw std::runtime_error{"Invalid draw mode"};
            }
        }

        constexpr D3D11_TEXTURE_ADDRESS_MODE getTextureAddressMode(SamplerAddressMode address)
        {
            switch (address)
            {
                case SamplerAddressMode::clampToEdge:
                    return D3D11_TEXTURE_ADDRESS_CLAMP;
                case SamplerAddressMode::clampToBorder:
                    return D3D11_TEXTURE_ADDRESS_BORDER;
                case SamplerAddressMode::repeat:
                    return D3D11_TEXTURE_ADDRESS_WRAP;
                case SamplerAddressMode::mirrorRepeat:
                    return D3D11_TEXTURE_ADDRESS_MIRROR;
                default:
                    throw std::runtime_error{"Invalid address mode"};
            }
        }
    }

    const std::error_category& getErrorCategory() noexcept
    {
        return errorCategory;
    }

    RenderDevice::RenderDevice(const Settings& settings,
                               core::Window& initWindow):
        graphics::RenderDevice{Driver::direct3D11, settings, initWindow}
    {
        apiVersion = ApiVersion{11, 0};
    
        anisotropicFilteringSupported = true;
        renderTargetsSupported = true;
        clampToBorderSupported = true;
        multisamplingSupported = true;
        uintIndicesSupported = true;

        UINT deviceCreationFlags = 0U;

        if (debugRenderer)
            deviceCreationFlags |= D3D11_CREATE_DEVICE_DEBUG;

        ID3D11Device* newDevice;
        ID3D11DeviceContext* newContext;

        D3D_FEATURE_LEVEL featureLevel;
        if (const auto hr = D3D11CreateDevice(nullptr, // adapter
                                              D3D_DRIVER_TYPE_HARDWARE,
                                              nullptr, // software rasterizer (unused)
                                              deviceCreationFlags,
                                              nullptr, // feature levels
                                              0, // no feature levels
                                              D3D11_SDK_VERSION,
                                              &newDevice,
                                              &featureLevel,
                                              &newContext); FAILED(hr))
            throw std::system_error{hr, errorCategory, "Failed to create the Direct3D 11 device"};

        device = newDevice;
        context = newContext;

        if (featureLevel >= D3D_FEATURE_LEVEL_10_0)
            npotTexturesSupported = true;


        void* dxgiDevicePtr;
        device->QueryInterface(IID_IDXGIDevice, &dxgiDevicePtr);

        Pointer<IDXGIDevice> dxgiDevice = static_cast<IDXGIDevice*>(dxgiDevicePtr);

        void* newAdapter;
        if (const auto hr = dxgiDevice->GetParent(IID_IDXGIAdapter, &newAdapter); FAILED(hr))
            throw std::system_error{hr, errorCategory, "Failed to get the DXGI adapter"};

        adapter = static_cast<IDXGIAdapter*>(newAdapter);

        void* factoryPtr;
        if (const auto hr = adapter->GetParent(IID_IDXGIFactory, &factoryPtr); FAILED(hr))
            throw std::system_error{hr, errorCategory, "Failed to get the DXGI factory"};

        Pointer<IDXGIFactory> factory = static_cast<IDXGIFactory*>(factoryPtr);

        DXGI_ADAPTER_DESC adapterDesc;
        if (const auto hr = adapter->GetDesc(&adapterDesc); FAILED(hr))
            throw std::system_error{hr, errorCategory, "Failed to get the DXGI adapter description"};
        else
        {
            if (const auto bufferSize = WideCharToMultiByte(CP_UTF8, 0, adapterDesc.Description, -1, nullptr, 0, nullptr, nullptr); bufferSize != 0)
            {
                auto buffer = std::make_unique<char[]>(bufferSize);
                if (WideCharToMultiByte(CP_UTF8, 0, adapterDesc.Description, -1, buffer.get(), bufferSize, nullptr, nullptr) != 0)
                    log(Log::Level::info) << "Using " << buffer.get() << " for rendering";
            }
        }

        const auto& windowWin = window.getNativeWindow();

        frameBufferWidth = static_cast<UINT>(window.getResolution().v[0]);
        frameBufferHeight = static_cast<UINT>(window.getResolution().v[1]);

        UINT qualityLevels;
        UINT supportedSampleCount;
        for (supportedSampleCount = sampleCount; supportedSampleCount > 1; --supportedSampleCount)
        {
            if (const auto hr = device->CheckMultisampleQualityLevels(DXGI_FORMAT_R8G8B8A8_UNORM, supportedSampleCount, &qualityLevels); FAILED(hr))
                throw std::system_error{hr, errorCategory, "Failed to check Direct3D 11 multisample quality levels"};
            else if (qualityLevels)
                break;
        }

        if (supportedSampleCount != sampleCount)
        {
            sampleCount = supportedSampleCount;
            log(Log::Level::warning) << "Chosen sample count not supported, using: " << sampleCount;
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
        swapChainDesc.OutputWindow = windowWin.getNativeWindow();
        swapChainDesc.Windowed = (windowWin.isExclusiveFullscreen() && windowWin.isFullscreen()) ? FALSE : TRUE;
        swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
        swapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

        swapInterval = verticalSync ? 1 : 0;

        IDXGISwapChain* newSwapCahin;

        if (const auto hr = factory->CreateSwapChain(device.get(), &swapChainDesc, &newSwapCahin); FAILED(hr))
            throw std::system_error{hr, errorCategory, "Failed to create the Direct3D 11 swap chain"};

        swapChain = newSwapCahin;

        factory->MakeWindowAssociation(windowWin.getNativeWindow(), DXGI_MWA_NO_ALT_ENTER);

        // Backbuffer
        void* newBackBuffer;
        if (const auto hr = swapChain->GetBuffer(0, IID_ID3D11Texture2D, &newBackBuffer); FAILED(hr))
            throw std::system_error{hr, errorCategory, "Failed to retrieve Direct3D 11 backbuffer"};

        backBuffer = static_cast<ID3D11Texture2D*>(newBackBuffer);

        ID3D11RenderTargetView* newRenderTargetView;
        if (const auto hr = device->CreateRenderTargetView(backBuffer.get(), nullptr, &newRenderTargetView); FAILED(hr))
            throw std::system_error{hr, errorCategory, "Failed to create Direct3D 11 render target view"};

        renderTargetView = newRenderTargetView;

        // Rasterizer state
        D3D11_RASTERIZER_DESC rasterStateDesc;
        rasterStateDesc.FrontCounterClockwise = FALSE;
        rasterStateDesc.DepthBias = 0;
        rasterStateDesc.DepthBiasClamp = 0;
        rasterStateDesc.SlopeScaledDepthBias = 0;
        rasterStateDesc.DepthClipEnable = TRUE;
        rasterStateDesc.MultisampleEnable = (sampleCount > 1) ? TRUE : FALSE;
        rasterStateDesc.AntialiasedLineEnable = TRUE;

        std::size_t rasterStateIndex = 0;

        const std::array<D3D11_FILL_MODE, 2> fillModes = {D3D11_FILL_SOLID, D3D11_FILL_WIREFRAME};
        const std::array<D3D11_CULL_MODE, 3> cullModes = {D3D11_CULL_NONE, D3D11_CULL_FRONT, D3D11_CULL_BACK};

        for (std::size_t fillMode = 0; fillMode < fillModes.size(); ++fillMode)
            for (std::size_t cullMode = 0; cullMode < cullModes.size(); ++cullMode)
                for (std::size_t scissorEnable = 0; scissorEnable < 2U; ++scissorEnable)
                {
                    rasterStateDesc.FillMode = fillModes[fillMode];
                    rasterStateDesc.CullMode = cullModes[cullMode];
                    rasterStateDesc.ScissorEnable = (scissorEnable == 0) ? FALSE : TRUE;

                    ID3D11RasterizerState* newRasterizerState;
                    if (const auto hr = device->CreateRasterizerState(&rasterStateDesc, &newRasterizerState); FAILED(hr))
                        throw std::system_error{hr, errorCategory, "Failed to create Direct3D 11 rasterizer state"};

                    rasterizerStates[rasterStateIndex] = newRasterizerState;

                    ++rasterStateIndex;
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

            ID3D11Texture2D* newDepthStencilTexture;
            if (const auto hr = device->CreateTexture2D(&depthStencilDesc, nullptr, &newDepthStencilTexture); FAILED(hr))
                throw std::system_error{hr, errorCategory, "Failed to create Direct3D 11 depth stencil texture"};

            depthStencilTexture = newDepthStencilTexture;

            ID3D11DepthStencilView* newDepthStencilView;
            if (const auto hr = device->CreateDepthStencilView(depthStencilTexture.get(), nullptr, &newDepthStencilView); FAILED(hr))
                throw std::system_error{hr, errorCategory, "Failed to create Direct3D 11 depth stencil view"};

            depthStencilView = newDepthStencilView;
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

        ID3D11DepthStencilState* newDefaultDepthStencilState;
        if (const auto hr = device->CreateDepthStencilState(&depthStencilStateDesc, &newDefaultDepthStencilState); FAILED(hr))
            throw std::system_error{hr, errorCategory, "Failed to create Direct3D 11 depth stencil state"};

        defaultDepthStencilState = newDefaultDepthStencilState;
    }

    RenderDevice::~RenderDevice()
    {
        running = false;
        CommandBuffer commandBuffer;
        commandBuffer.pushCommand(std::make_unique<PresentCommand>());
        submitCommandBuffer(std::move(commandBuffer));

        if (renderThread.isJoinable()) renderThread.join();
    }

    void RenderDevice::setFullscreen(bool newFullscreen)
    {
        executeOnRenderThread([newFullscreen, this]() {
            if (const auto hr = swapChain->SetFullscreenState(newFullscreen ? TRUE : FALSE, nullptr); FAILED(hr))
                throw std::system_error{hr, errorCategory, "Failed to set fullscreen state"};
        });
    }

    void RenderDevice::process()
    {
        graphics::RenderDevice::process();
        executeAll();

        std::vector<float> shaderData;

        std::size_t fillModeIndex = 0U;
        std::size_t scissorEnableIndex = 0U;
        std::size_t cullModeIndex = 0U;
        RenderTarget* currentRenderTarget = nullptr;
        const Shader* currentShader = nullptr;

        std::vector<ID3D11ShaderResourceView*> currentResourceViews;
        std::vector<ID3D11SamplerState*> currentSamplerStates;

        CommandBuffer commandBuffer;
        std::unique_ptr<Command> command;

        for (;;)
        {
            std::unique_lock lock{commandQueueMutex};
            commandQueueCondition.wait(lock, [this]() noexcept { return !commandQueue.empty(); });
            commandBuffer = std::move(commandQueue.front());
            commandQueue.pop();
            lock.unlock();

            while (!commandBuffer.isEmpty())
            {
                command = commandBuffer.popCommand();

                switch (command->type)
                {
                    case Command::Type::resize:
                    {
                        const auto resizeCommand = static_cast<const ResizeCommand*>(command.get());
                        resizeBackBuffer(static_cast<UINT>(resizeCommand->size.v[0]),
                                         static_cast<UINT>(resizeCommand->size.v[1]));
                        break;
                    }

                    case Command::Type::present:
                    {
                        if (currentRenderTarget)
                            currentRenderTarget->resolve();

                        swapChain->Present(swapInterval, 0);
                        break;
                    }

                    case Command::Type::deleteResource:
                    {
                        const auto deleteResourceCommand = static_cast<const DeleteResourceCommand*>(command.get());
                        resources[deleteResourceCommand->resource - 1].reset();
                        break;
                    }

                    case Command::Type::initRenderTarget:
                    {
                        const auto initRenderTargetCommand = static_cast<const InitRenderTargetCommand*>(command.get());

                        std::set<Texture*> colorTextures;
                        for (const auto colorTextureId : initRenderTargetCommand->colorTextures)
                            colorTextures.insert(getResource<Texture>(colorTextureId));

                        auto renderTarget = std::make_unique<RenderTarget>(*this,
                                                                           colorTextures,
                                                                           getResource<Texture>(initRenderTargetCommand->depthTexture));

                        if (initRenderTargetCommand->renderTarget > resources.size())
                            resources.resize(initRenderTargetCommand->renderTarget);
                        resources[initRenderTargetCommand->renderTarget - 1] = std::move(renderTarget);
                        break;
                    }

                    case Command::Type::setRenderTarget:
                    {
                        const auto setRenderTargetCommand = static_cast<const SetRenderTargetCommand*>(command.get());

                        if (currentRenderTarget)
                            currentRenderTarget->resolve();

                        if (setRenderTargetCommand->renderTarget)
                        {
                            currentRenderTarget = getResource<RenderTarget>(setRenderTargetCommand->renderTarget);
                            assert(currentRenderTarget);
                            context->OMSetRenderTargets(static_cast<UINT>(currentRenderTarget->getRenderTargetViews().size()),
                                                        currentRenderTarget->getRenderTargetViews().data(),
                                                        currentRenderTarget->getDepthStencilView());
                        }
                        else
                        {
                            currentRenderTarget = nullptr;
                            ID3D11RenderTargetView* renderTargetViews[] = {renderTargetView.get()};
                            context->OMSetRenderTargets(1, renderTargetViews, depthStencilView.get());
                        }
                        break;
                    }

                    case Command::Type::clearRenderTarget:
                    {
                        const auto clearCommand = static_cast<const ClearRenderTargetCommand*>(command.get());

                        const std::array<FLOAT, 4> frameBufferClearColor{
                            clearCommand->clearColor.normR(),
                            clearCommand->clearColor.normG(),
                            clearCommand->clearColor.normB(),
                            clearCommand->clearColor.normA()
                        };

                        if (currentRenderTarget)
                        {
                            if (clearCommand->clearColorBuffer)
                                for (ID3D11RenderTargetView* view : currentRenderTarget->getRenderTargetViews())
                                    context->ClearRenderTargetView(view, frameBufferClearColor.data());

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
                                context->ClearRenderTargetView(renderTargetView.get(), frameBufferClearColor.data());

                            if (clearCommand->clearDepthBuffer)
                                context->ClearDepthStencilView(depthStencilView.get(),
                                                               (clearCommand->clearDepthBuffer ? D3D11_CLEAR_DEPTH : 0) | (clearCommand->clearStencilBuffer ? D3D11_CLEAR_STENCIL : 0),
                                                               clearCommand->clearDepth,
                                                               static_cast<UINT8>(clearCommand->clearStencil));
                        }

                        break;
                    }

                    case Command::Type::setScissorTest:
                    {
                        const auto setScissorTestCommand = static_cast<const SetScissorTestCommand*>(command.get());

                        if (setScissorTestCommand->enabled)
                        {
                            D3D11_RECT rect;
                            rect.left = static_cast<LONG>(setScissorTestCommand->rectangle.position.v[0]);
                            rect.top = static_cast<LONG>(setScissorTestCommand->rectangle.position.v[1]);
                            rect.right = static_cast<LONG>(setScissorTestCommand->rectangle.position.v[0] + setScissorTestCommand->rectangle.size.v[0]);
                            rect.bottom = static_cast<LONG>(setScissorTestCommand->rectangle.position.v[1] + setScissorTestCommand->rectangle.size.v[1]);
                            context->RSSetScissorRects(1, &rect);
                        }

                        scissorEnableIndex = (setScissorTestCommand->enabled) ? 1U : 0U;

                        const std::size_t rasterizerStateIndex = fillModeIndex * 2U * 3U + cullModeIndex * 2U + scissorEnableIndex;
                        context->RSSetState(rasterizerStates[rasterizerStateIndex].get());

                        break;
                    }

                    case Command::Type::setViewport:
                    {
                        const auto setViewportCommand = static_cast<const SetViewportCommand*>(command.get());

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

                    case Command::Type::initDepthStencilState:
                    {
                        const auto initDepthStencilStateCommand = static_cast<const InitDepthStencilStateCommand*>(command.get());
                        auto depthStencilState = std::make_unique<DepthStencilState>(*this,
                                                                                     initDepthStencilStateCommand->depthTest,
                                                                                     initDepthStencilStateCommand->depthWrite,
                                                                                     initDepthStencilStateCommand->compareFunction,
                                                                                     initDepthStencilStateCommand->stencilEnabled,
                                                                                     initDepthStencilStateCommand->stencilReadMask,
                                                                                     initDepthStencilStateCommand->stencilWriteMask,
                                                                                     initDepthStencilStateCommand->frontFaceStencilFailureOperation,
                                                                                     initDepthStencilStateCommand->frontFaceStencilDepthFailureOperation,
                                                                                     initDepthStencilStateCommand->frontFaceStencilPassOperation,
                                                                                     initDepthStencilStateCommand->frontFaceStencilCompareFunction,
                                                                                     initDepthStencilStateCommand->backFaceStencilFailureOperation,
                                                                                     initDepthStencilStateCommand->backFaceStencilDepthFailureOperation,
                                                                                     initDepthStencilStateCommand->backFaceStencilPassOperation,
                                                                                     initDepthStencilStateCommand->backFaceStencilCompareFunction);

                        if (initDepthStencilStateCommand->depthStencilState > resources.size())
                            resources.resize(initDepthStencilStateCommand->depthStencilState);
                        resources[initDepthStencilStateCommand->depthStencilState - 1] = std::move(depthStencilState);
                        break;
                    }

                    case Command::Type::setDepthStencilState:
                    {
                        const auto setDepthStencilStateCommand = static_cast<const SetDepthStencilStateCommand*>(command.get());

                        if (setDepthStencilStateCommand->depthStencilState)
                        {
                            const auto depthStencilState = getResource<DepthStencilState>(setDepthStencilStateCommand->depthStencilState);
                            context->OMSetDepthStencilState(depthStencilState->getDepthStencilState().get(),
                                                            setDepthStencilStateCommand->stencilReferenceValue);
                        }
                        else
                            context->OMSetDepthStencilState(defaultDepthStencilState.get(),
                                                            setDepthStencilStateCommand->stencilReferenceValue);

                        break;
                    }

                    case Command::Type::setPipelineState:
                    {
                        const auto setPipelineStateCommand = static_cast<const SetPipelineStateCommand*>(command.get());

                        const auto blendState = getResource<BlendState>(setPipelineStateCommand->blendState);
                        const auto shader = getResource<Shader>(setPipelineStateCommand->shader);
                        currentShader = shader;

                        if (blendState)
                            context->OMSetBlendState(blendState->getBlendState().get(), nullptr, 0xFFFFFFFFU);
                        else
                            context->OMSetBlendState(nullptr, nullptr, 0xFFFFFFFFU);

                        if (shader)
                        {
                            assert(shader->getFragmentShader());
                            assert(shader->getVertexShader());
                            assert(shader->getInputLayout());

                            context->PSSetShader(shader->getFragmentShader().get(), nullptr, 0);
                            context->VSSetShader(shader->getVertexShader().get(), nullptr, 0);
                            context->IASetInputLayout(shader->getInputLayout().get());
                        }
                        else
                        {
                            context->PSSetShader(nullptr, nullptr, 0);
                            context->VSSetShader(nullptr, nullptr, 0);
                            context->IASetInputLayout(nullptr);
                        }

                        switch (setPipelineStateCommand->cullMode)
                        {
                            case CullMode::none: cullModeIndex = 0U; break;
                            case CullMode::front: cullModeIndex = 1U; break;
                            case CullMode::back: cullModeIndex = 2U; break;
                            default: throw std::runtime_error{"Invalid cull mode"};
                        }

                        switch (setPipelineStateCommand->fillMode)
                        {
                            case FillMode::solid: fillModeIndex = 0U; break;
                            case FillMode::wireframe: fillModeIndex = 1U; break;
                            default: throw std::runtime_error{"Invalid fill mode"};
                        }

                        const std::size_t rasterizerStateIndex = fillModeIndex * 2U * 3U + cullModeIndex * 2U + scissorEnableIndex;
                        context->RSSetState(rasterizerStates[rasterizerStateIndex].get());
                        break;
                    }

                    case Command::Type::draw:
                    {
                        const auto drawCommand = static_cast<const DrawCommand*>(command.get());

                        // draw mesh buffer
                        const auto indexBuffer = getResource<Buffer>(drawCommand->indexBuffer);
                        const auto vertexBuffer = getResource<Buffer>(drawCommand->vertexBuffer);

                        assert(indexBuffer);
                        assert(indexBuffer->getBuffer());
                        assert(vertexBuffer);
                        assert(vertexBuffer->getBuffer());

                        ID3D11Buffer* buffers[] = {vertexBuffer->getBuffer().get()};
                        UINT strides[] = {sizeof(Vertex)};
                        UINT offsets[] = {0U};
                        context->IASetVertexBuffers(0, 1, buffers, strides, offsets);
                        context->IASetIndexBuffer(indexBuffer->getBuffer().get(),
                                                  getIndexFormat(drawCommand->indexSize), 0);
                        context->IASetPrimitiveTopology(getPrimitiveTopology(drawCommand->drawMode));

                        assert(drawCommand->indexCount);
                        assert(indexBuffer->getSize());
                        assert(vertexBuffer->getSize());

                        context->DrawIndexed(drawCommand->indexCount, drawCommand->startIndex, 0);

                        break;
                    }

                    case Command::Type::initBlendState:
                    {
                        const auto initBlendStateCommand = static_cast<const InitBlendStateCommand*>(command.get());

                        auto blendState = std::make_unique<BlendState>(*this,
                                                                       initBlendStateCommand->enableBlending,
                                                                       initBlendStateCommand->colorBlendSource,
                                                                       initBlendStateCommand->colorBlendDest,
                                                                       initBlendStateCommand->colorOperation,
                                                                       initBlendStateCommand->alphaBlendSource,
                                                                       initBlendStateCommand->alphaBlendDest,
                                                                       initBlendStateCommand->alphaOperation,
                                                                       initBlendStateCommand->colorMask);

                        if (initBlendStateCommand->blendState > resources.size())
                            resources.resize(initBlendStateCommand->blendState);
                        resources[initBlendStateCommand->blendState - 1] = std::move(blendState);
                        break;
                    }

                    case Command::Type::initBuffer:
                    {
                        const auto initBufferCommand = static_cast<const InitBufferCommand*>(command.get());

                        auto buffer = std::make_unique<Buffer>(*this,
                                                               initBufferCommand->bufferType,
                                                               initBufferCommand->flags,
                                                               initBufferCommand->data,
                                                               initBufferCommand->size);

                        if (initBufferCommand->buffer > resources.size())
                            resources.resize(initBufferCommand->buffer);
                        resources[initBufferCommand->buffer - 1] = std::move(buffer);
                        break;
                    }

                    case Command::Type::setBufferData:
                    {
                        const auto setBufferDataCommand = static_cast<const SetBufferDataCommand*>(command.get());

                        const auto buffer = getResource<Buffer>(setBufferDataCommand->buffer);
                        buffer->setData(setBufferDataCommand->data);
                        break;
                    }

                    case Command::Type::initShader:
                    {
                        const auto initShaderCommand = static_cast<const InitShaderCommand*>(command.get());

                        auto shader = std::make_unique<Shader>(*this,
                                                               initShaderCommand->fragmentShader,
                                                               initShaderCommand->vertexShader,
                                                               initShaderCommand->vertexAttributes,
                                                               initShaderCommand->fragmentShaderConstantInfo,
                                                               initShaderCommand->vertexShaderConstantInfo,
                                                               initShaderCommand->fragmentShaderFunction,
                                                               initShaderCommand->vertexShaderFunction);

                        if (initShaderCommand->shader > resources.size())
                            resources.resize(initShaderCommand->shader);
                        resources[initShaderCommand->shader - 1] = std::move(shader);
                        break;
                    }

                    case Command::Type::setShaderConstants:
                    {
                        const auto setShaderConstantsCommand = static_cast<const SetShaderConstantsCommand*>(command.get());

                        if (!currentShader)
                            throw std::runtime_error{"No shader set"};

                        // pixel shader constants
                        const auto& fragmentShaderConstantLocations = currentShader->getFragmentShaderConstantLocations();

                        if (setShaderConstantsCommand->fragmentShaderConstants.size() > fragmentShaderConstantLocations.size())
                            throw std::runtime_error{"Invalid pixel shader constant size"};

                        shaderData.clear();

                        for (std::size_t i = 0; i < setShaderConstantsCommand->fragmentShaderConstants.size(); ++i)
                        {
                            const auto& fragmentShaderConstantLocation = fragmentShaderConstantLocations[i];
                            const auto& fragmentShaderConstant = setShaderConstantsCommand->fragmentShaderConstants[i];

                            if (sizeof(float) * fragmentShaderConstant.size() != fragmentShaderConstantLocation.size)
                                throw std::runtime_error{"Invalid pixel shader constant size"};

                            shaderData.insert(shaderData.end(), fragmentShaderConstant.begin(), fragmentShaderConstant.end());
                        }

                        uploadBuffer(currentShader->getFragmentShaderConstantBuffer().get(),
                                     shaderData.data(),
                                     static_cast<std::uint32_t>(sizeof(float) * shaderData.size()));

                        ID3D11Buffer* fragmentShaderConstantBuffers[1] = {currentShader->getFragmentShaderConstantBuffer().get()};
                        context->PSSetConstantBuffers(0, 1, fragmentShaderConstantBuffers);

                        // vertex shader constants
                        const auto& vertexShaderConstantLocations = currentShader->getVertexShaderConstantLocations();

                        if (setShaderConstantsCommand->vertexShaderConstants.size() > vertexShaderConstantLocations.size())
                            throw std::runtime_error{"Invalid vertex shader constant size"};

                        shaderData.clear();

                        for (std::size_t i = 0; i < setShaderConstantsCommand->vertexShaderConstants.size(); ++i)
                        {
                            const auto& vertexShaderConstantLocation = vertexShaderConstantLocations[i];
                            const auto& vertexShaderConstant = setShaderConstantsCommand->vertexShaderConstants[i];

                            if (sizeof(float) * vertexShaderConstant.size() != vertexShaderConstantLocation.size)
                                throw std::runtime_error{"Invalid vertex shader constant size"};

                            shaderData.insert(shaderData.end(), vertexShaderConstant.begin(), vertexShaderConstant.end());
                        }

                        uploadBuffer(currentShader->getVertexShaderConstantBuffer().get(),
                                     shaderData.data(),
                                     static_cast<std::uint32_t>(sizeof(float) * shaderData.size()));

                        ID3D11Buffer* vertexShaderConstantBuffers[1] = {currentShader->getVertexShaderConstantBuffer().get()};
                        context->VSSetConstantBuffers(0, 1, vertexShaderConstantBuffers);

                        break;
                    }

                    case Command::Type::initTexture:
                    {
                        const auto initTextureCommand = static_cast<const InitTextureCommand*>(command.get());

                        auto texture = std::make_unique<Texture>(*this,
                                                                 initTextureCommand->levels,
                                                                 initTextureCommand->textureType,
                                                                 initTextureCommand->flags,
                                                                 initTextureCommand->sampleCount,
                                                                 initTextureCommand->pixelFormat,
                                                                 initTextureCommand->filter,
                                                                 initTextureCommand->maxAnisotropy);

                        if (initTextureCommand->texture > resources.size())
                            resources.resize(initTextureCommand->texture);
                        resources[initTextureCommand->texture - 1] = std::move(texture);
                        break;
                    }

                    case Command::Type::setTextureData:
                    {
                        const auto setTextureDataCommand = static_cast<const SetTextureDataCommand*>(command.get());

                        const auto texture = getResource<Texture>(setTextureDataCommand->texture);
                        texture->setData(setTextureDataCommand->levels);

                        break;
                    }

                    case Command::Type::setTextureParameters:
                    {
                        const auto setTextureParametersCommand = static_cast<const SetTextureParametersCommand*>(command.get());

                        const auto texture = getResource<Texture>(setTextureParametersCommand->texture);
                        texture->setFilter(setTextureParametersCommand->filter);
                        texture->setAddressX(setTextureParametersCommand->addressX);
                        texture->setAddressY(setTextureParametersCommand->addressY);
                        texture->setAddressZ(setTextureParametersCommand->addressZ);
                        texture->setMaxAnisotropy(setTextureParametersCommand->maxAnisotropy);

                        break;
                    }

                    case Command::Type::setTextures:
                    {
                        const auto setTexturesCommand = static_cast<const SetTexturesCommand*>(command.get());

                        currentResourceViews.clear();
                        currentSamplerStates.clear();

                        for (const auto textureId : setTexturesCommand->textures)
                            if (auto texture = getResource<Texture>(textureId))
                            {
                                currentResourceViews.push_back(texture->getResourceView().get());
                                currentSamplerStates.push_back(texture->getSamplerState());
                            }
                            else
                            {
                                currentResourceViews.push_back(nullptr);
                                currentSamplerStates.push_back(nullptr);
                            }

                        context->PSSetShaderResources(0, static_cast<UINT>(currentResourceViews.size()), currentResourceViews.data());
                        context->PSSetSamplers(0, static_cast<UINT>(currentSamplerStates.size()), currentSamplerStates.data());

                        break;
                    }

                    default:
                        throw std::runtime_error{"Invalid command"};
                }

                if (command->type == Command::Type::present) return;
            }
        }
    }

    Pointer<IDXGIOutput> RenderDevice::getOutput() const
    {
        const auto& windowWin = window.getNativeWindow();
        const auto monitor = windowWin.getMonitor();

        if (!monitor)
            throw std::runtime_error{"Window is not on any monitor"};

        HRESULT hr;
        IDXGIOutput* outputPointer;
        for (UINT i = 0U; (hr = adapter->EnumOutputs(i, &outputPointer)) != DXGI_ERROR_NOT_FOUND; ++i)
            if (SUCCEEDED(hr))
            {
                Pointer<IDXGIOutput> output = outputPointer;

                DXGI_OUTPUT_DESC outputDesc;
                if (SUCCEEDED(output->GetDesc(&outputDesc)) && outputDesc.Monitor == monitor)
                    return output;
            }

        return nullptr;
    }

    std::vector<math::Size<std::uint32_t, 2>> RenderDevice::getSupportedResolutions() const
    {
        std::vector<math::Size<std::uint32_t, 2>> result;

        Pointer<IDXGIOutput> output = getOutput();
        if (!output) return result;

        UINT numModes = 0U;
        DXGI_FORMAT format = DXGI_FORMAT_R8G8B8A8_UNORM;
        if (const auto hr = output->GetDisplayModeList(format, 0, &numModes, nullptr); FAILED(hr))
            throw std::system_error{hr, errorCategory, "Failed to get display mode list"};

        if (numModes > 0U)
        {
            std::vector<DXGI_MODE_DESC> displayModes(numModes);
            output->GetDisplayModeList(format, 0, &numModes, displayModes.data());

            for (const auto& displayMode : displayModes)
            {
                const math::Size<std::uint32_t, 2> resolution{
                    static_cast<std::uint32_t>(displayMode.Width),
                    static_cast<std::uint32_t>(displayMode.Height)
                };
            
                result.emplace_back(resolution);
            }
        }

        return result;
    }
    
    void RenderDevice::start()
    {
        running = true;
        renderThread = thread::Thread{&RenderDevice::renderMain, this};
    }

    namespace
    {
        class MappedSubresource final
        {
        public:
            MappedSubresource(ID3D11DeviceContext* context, ID3D11Resource* resource) noexcept:
                deviceContext{context}, mappedResource{resource}
            {
            }

            ~MappedSubresource()
            {
                deviceContext->Unmap(mappedResource, 0);
            }

        private:
            const ID3D11DeviceContext* deviceContext;
            const ID3D11Resource* mappedResource;
        };
    }
    

    void RenderDevice::generateScreenshot(const std::string& filename)
    {
        void* backBufferTexturePtr;
        if (const auto hr = backBuffer->QueryInterface(IID_ID3D11Texture2D, &backBufferTexturePtr); FAILED(hr))
            throw std::system_error{hr, errorCategory, "Failed to get Direct3D 11 back buffer texture"};

        Pointer<ID3D11Texture2D> backBufferTexture = static_cast<ID3D11Texture2D*>(backBufferTexturePtr);

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

        ID3D11Texture2D* texturePtr;
        if (const auto hr = device->CreateTexture2D(&textureDesc, nullptr, &texturePtr); FAILED(hr))
            throw std::system_error{hr, errorCategory, "Failed to create Direct3D 11 texture"};

        Pointer<ID3D11Texture2D> texture = texturePtr;

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

            ID3D11Texture2D* resolveTexturePtr;
            if (const auto hr = device->CreateTexture2D(&resolveTextureDesc, nullptr, &resolveTexturePtr); FAILED(hr))
                throw std::system_error{hr, errorCategory, "Failed to create Direct3D 11 texture"};

            Pointer<ID3D11Texture2D> resolveTexture = resolveTexturePtr;

            context->ResolveSubresource(resolveTexture.get(), 0, backBuffer.get(), 0, DXGI_FORMAT_R8G8B8A8_UNORM);
            context->CopyResource(texture.get(), resolveTexture.get());
        }
        else
            context->CopyResource(texture.get(), backBuffer.get());

        D3D11_MAPPED_SUBRESOURCE mappedSubresource;
        if (const auto hr = context->Map(texture.get(), 0, D3D11_MAP_READ, 0, &mappedSubresource); FAILED(hr))
            throw std::system_error{hr, errorCategory, "Failed to map Direct3D 11 resource"};

        MappedSubresource mappedResource{context.get(), texture.get()};

        if (!stbi_write_png(filename.c_str(),
                            textureDesc.Width, textureDesc.Height, 4,
                            mappedSubresource.pData,
                            static_cast<int>(mappedSubresource.RowPitch)))
        {
            throw std::runtime_error{"Failed to save screenshot to file"};
        }
    }

    void RenderDevice::resizeBackBuffer(UINT newWidth, UINT newHeight)
    {
        if (frameBufferWidth != newWidth || frameBufferHeight != newHeight)
        {
            backBuffer = nullptr;
            renderTargetView = nullptr;
            depthStencilTexture = nullptr;
            depthStencilView = nullptr;
            
            if (const auto hr = swapChain->ResizeBuffers(0, newWidth, newHeight, DXGI_FORMAT_UNKNOWN, DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH); FAILED(hr))
                throw std::system_error{hr, errorCategory, "Failed to resize Direct3D 11 backbuffer"};

            void* newBackBuffer;
            if (const auto hr = swapChain->GetBuffer(0, IID_ID3D11Texture2D, &newBackBuffer); FAILED(hr))
                throw std::system_error{hr, errorCategory, "Failed to retrieve Direct3D 11 backbuffer"};

            backBuffer = static_cast<ID3D11Texture2D*>(newBackBuffer);

            ID3D11RenderTargetView* newRenderTargetView;
            if (const auto hr = device->CreateRenderTargetView(backBuffer.get(), nullptr, &newRenderTargetView); FAILED(hr))
                throw std::system_error{hr, errorCategory, "Failed to create Direct3D 11 render target view"};

            renderTargetView = newRenderTargetView;

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

                ID3D11Texture2D* newDepthStencilTexture;
                if (const auto hr = device->CreateTexture2D(&depthStencilDesc, nullptr, &newDepthStencilTexture); FAILED(hr))
                    throw std::system_error{hr, errorCategory, "Failed to create Direct3D 11 depth stencil texture"};

                depthStencilTexture = newDepthStencilTexture;

                ID3D11DepthStencilView* newDepthStencilView;
                if (const auto hr = device->CreateDepthStencilView(depthStencilTexture.get(), nullptr, &newDepthStencilView); FAILED(hr))
                    throw std::system_error{hr, errorCategory, "Failed to create Direct3D 11 depth stencil view"};

                depthStencilView = newDepthStencilView;
            }

            frameBufferWidth = desc.Width;
            frameBufferHeight = desc.Height;
        }
    }

    void RenderDevice::uploadBuffer(ID3D11Buffer* buffer, const void* data, std::uint32_t dataSize)
    {
        D3D11_MAPPED_SUBRESOURCE mappedSubresource;
        if (const auto hr = context->Map(buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedSubresource); FAILED(hr))
            throw std::system_error{hr, errorCategory, "Failed to lock Direct3D 11 buffer"};

        std::memcpy(mappedSubresource.pData, data, dataSize);

        context->Unmap(buffer, 0);
    }

    ID3D11SamplerState* RenderDevice::getSamplerState(const SamplerStateDesc& desc)
    {
        if (const auto samplerStatesIterator = samplerStates.find(desc); samplerStatesIterator != samplerStates.end())
            return samplerStatesIterator->second.get();
        else
        {
            D3D11_SAMPLER_DESC samplerStateDesc;

            if (desc.maxAnisotropy > 1)
                samplerStateDesc.Filter = D3D11_FILTER_ANISOTROPIC;
            else
            {
                switch (desc.filter)
                {
                    case SamplerFilter::point:
                        samplerStateDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
                        break;
                    case SamplerFilter::linear:
                        samplerStateDesc.Filter = D3D11_FILTER_MIN_MAG_POINT_MIP_LINEAR;
                        break;
                    case SamplerFilter::bilinear:
                        samplerStateDesc.Filter = D3D11_FILTER_MIN_MAG_LINEAR_MIP_POINT;
                        break;
                    case SamplerFilter::trilinear:
                        samplerStateDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
                        break;
                    default:
                        throw std::runtime_error{"Invalid texture filter"};
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
            if (const auto hr = device->CreateSamplerState(&samplerStateDesc, &samplerState); FAILED(hr))
                throw std::system_error{hr, errorCategory, "Failed to create Direct3D 11 sampler state"};

            samplerStates[desc] = samplerState;

            return samplerState;
        }
    }

    void RenderDevice::renderMain()
    {
        while (running)
        {
            try
            {
                process();
            }
            catch (const std::exception& e)
            {
                log(Log::Level::error) << e.what();
            }
        }
    }
}

#endif

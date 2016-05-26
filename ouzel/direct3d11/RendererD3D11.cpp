// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "RendererD3D11.h"
#include "Engine.h"
#include "TextureD3D11.h"
#include "ShaderD3D11.h"
#include "MeshBufferD3D11.h"
#include "RenderTargetD3D11.h"
#include "Utils.h"
#include "TexturePSD3D11.h"
#include "TextureVSD3D11.h"
#include "ColorPSD3D11.h"
#include "ColorVSD3D11.h"
#include "Camera.h"
#include "Cache.h"
#include "BlendStateD3D11.h"
#include "win/WindowWin.h"
#include "stb_image_write.h"

using namespace ouzel;

namespace ouzel
{
    namespace graphics
    {
        RendererD3D11::RendererD3D11():
            Renderer(Driver::DIRECT3D11)
        {
            apiVersion = 11;
        }

        RendererD3D11::~RendererD3D11()
        {
            free();
        }

        void RendererD3D11::free()
        {
            Renderer::free();
            
            if (depthStencilState)
            {
                depthStencilState->Release();
                depthStencilState = nullptr;
            }

            if (rasterizerState)
            {
                rasterizerState->Release();
                rasterizerState = nullptr;
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

        bool RendererD3D11::init(const Size2& newSize,
                                 bool newFullscreen,
                                 uint32_t newSampleCount,
                                 TextureFiltering newTextureFiltering,
                                 float newTargetFPS,
                                 bool newVerticalSync)
        {
            if (!Renderer::init(newSize, newFullscreen, newSampleCount, newTextureFiltering, newTargetFPS, newVerticalSync))
            {
                return false;
            }

            free();

            UINT deviceCreationFlags = 0;
    #if D3D11_DEBUG
            deviceCreationFlags |= D3D11_CREATE_DEVICE_DEBUG;
    #endif

            HRESULT hr = D3D11CreateDevice(
                nullptr, // adapter
                D3D_DRIVER_TYPE_HARDWARE,
                nullptr, // software rasterizer (unused)
                deviceCreationFlags,
                nullptr, // feature levels
                0, // ^^
                D3D11_SDK_VERSION,
                &device,
                nullptr,
                &context);

            if (FAILED(hr))
            {
                log("Failed to create the D3D11 device");
                return false;
            }

            IDXGIDevice* dxgiDevice;
            IDXGIFactory* factory;

            device->QueryInterface(IID_IDXGIDevice, (void**)&dxgiDevice);
            dxgiDevice->GetParent(IID_IDXGIAdapter, (void**)&adapter);
            hr = adapter->GetParent(IID_IDXGIFactory, (void**)&factory);
            if (FAILED(hr))
            {
                log("Failed to get the DXGI factory");
                return false;
            }

            std::shared_ptr<WindowWin> windowWin = std::static_pointer_cast<WindowWin>(sharedEngine->getWindow());

            DXGI_SWAP_CHAIN_DESC swapChainDesc;
            memset(&swapChainDesc, 0, sizeof(swapChainDesc));

            swapChainDesc.BufferDesc.Width = static_cast<UINT>(size.width);
            swapChainDesc.BufferDesc.Height = static_cast<UINT>(size.height);
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
            swapChainDesc.Windowed = static_cast<BOOL>(!fullscreen);
            swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
            swapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
            
            swapInterval = verticalSync ? 1 : 0;

            hr = factory->CreateSwapChain(device, &swapChainDesc, &swapChain);
            if (FAILED(hr))
            {
                log("Failed to create the D3D11 swap chain");
                return false;
            }

            factory->MakeWindowAssociation(windowWin->getNativeWindow(), DXGI_MWA_NO_ALT_ENTER);

            factory->Release();
            dxgiDevice->Release();

            // Backbuffer
            hr = swapChain->GetBuffer(0, IID_ID3D11Texture2D, reinterpret_cast<void**>(&backBuffer));
            if (FAILED(hr))
            {
                log("Failed to retrieve D3D11 backbuffer");
                return false;
            }

            hr = device->CreateRenderTargetView(backBuffer, nullptr, &renderTargetView);
            if (FAILED(hr))
            {
                log("Failed to create D3D11 render target view");
                return false;
            }

            // Sampler state
            D3D11_SAMPLER_DESC samplerStateDesc;
            switch (textureFiltering)
            {
                case Renderer::TextureFiltering::NONE:
                    samplerStateDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
                    break;
                case Renderer::TextureFiltering::LINEAR:
                    samplerStateDesc.Filter = D3D11_FILTER_MIN_MAG_POINT_MIP_LINEAR;
                    break;
                case Renderer::TextureFiltering::BILINEAR:
                    samplerStateDesc.Filter = D3D11_FILTER_MIN_MAG_LINEAR_MIP_POINT;
                    break;
                case Renderer::TextureFiltering::TRILINEAR:
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
                log("Failed to create D3D11 sampler state");
                return false;
            }

            // Rasterizer state
            D3D11_RASTERIZER_DESC rasterStateDesc =
            {
                D3D11_FILL_SOLID,
                D3D11_CULL_NONE,
                FALSE, // front = ccw?
                0, 0, 0, // depth bias, clamp, slope scale
                FALSE, // depth clip
                FALSE, // scissor test
                (sampleCount > 1) ? TRUE : FALSE, // MSAA
                TRUE, // AA lines
            };

            hr = device->CreateRasterizerState(&rasterStateDesc, &rasterizerState);
            if (FAILED(hr))
            {
                log("Failed to create D3D11 rasterizer state");
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
                log("Failed to create D3D11 depth stencil state");
                return false;
            }

            ShaderPtr textureShader = loadShaderFromBuffers(TEXTURE_PIXEL_SHADER_D3D11, sizeof(TEXTURE_PIXEL_SHADER_D3D11),
                TEXTURE_VERTEX_SHADER_D3D11, sizeof(TEXTURE_VERTEX_SHADER_D3D11),
                VertexPCT::ATTRIBUTES);

            if (!textureShader)
            {
                return false;
            }
            
            textureShader->setVertexShaderConstantInfo({{"modelViewProj", sizeof(Matrix4)}});
            textureShader->setPixelShaderConstantInfo({{"color", 4 * sizeof(float)}}, 256);

            sharedEngine->getCache()->setShader(SHADER_TEXTURE, textureShader);

            ShaderPtr colorShader = loadShaderFromBuffers(COLOR_PIXEL_SHADER_D3D11, sizeof(COLOR_PIXEL_SHADER_D3D11),
                COLOR_VERTEX_SHADER_D3D11, sizeof(COLOR_VERTEX_SHADER_D3D11),
                VertexPC::ATTRIBUTES);

            if (!colorShader)
            {
                return false;
            }
            
            colorShader->setVertexShaderConstantInfo({{"modelViewProj", sizeof(Matrix4)}});
            colorShader->setPixelShaderConstantInfo({{"color", 4 * sizeof(float)}}, 256);

            sharedEngine->getCache()->setShader(SHADER_COLOR, colorShader);

            BlendStatePtr noBlendState = createBlendState(false,
                                                          BlendState::BlendFactor::ONE, BlendState::BlendFactor::ZERO,
                                                          BlendState::BlendOperation::ADD,
                                                          BlendState::BlendFactor::ONE, BlendState::BlendFactor::ZERO,
                                                          BlendState::BlendOperation::ADD);

            if (!noBlendState)
            {
                return false;
            }

            sharedEngine->getCache()->setBlendState(BLEND_NO_BLEND, noBlendState);

            BlendStatePtr addBlendState = createBlendState(true,
                                                           BlendState::BlendFactor::ONE, BlendState::BlendFactor::ONE,
                                                           BlendState::BlendOperation::ADD,
                                                           BlendState::BlendFactor::ONE, BlendState::BlendFactor::ONE,
                                                           BlendState::BlendOperation::ADD);

            if (!addBlendState)
            {
                return false;
            }

            sharedEngine->getCache()->setBlendState(BLEND_ADD, addBlendState);

            BlendStatePtr multiplyBlendState = createBlendState(true,
                                                                BlendState::BlendFactor::DEST_COLOR, BlendState::BlendFactor::ZERO,
                                                                BlendState::BlendOperation::ADD,
                                                                BlendState::BlendFactor::DEST_ALPHA, BlendState::BlendFactor::ZERO,
                                                                BlendState::BlendOperation::ADD);

            if (!multiplyBlendState)
            {
                return false;
            }

            sharedEngine->getCache()->setBlendState(BLEND_MULTIPLY, multiplyBlendState);

            BlendStatePtr alphaBlendState = createBlendState(true,
                                                             BlendState::BlendFactor::SRC_ALPHA, BlendState::BlendFactor::INV_SRC_ALPHA,
                                                             BlendState::BlendOperation::ADD,
                                                             BlendState::BlendFactor::ONE, BlendState::BlendFactor::ZERO,
                                                             BlendState::BlendOperation::ADD);

            if (!alphaBlendState)
            {
                return false;
            }

            sharedEngine->getCache()->setBlendState(BLEND_ALPHA, alphaBlendState);

            memset(&resourceViews, 0, sizeof(resourceViews));
            memset(&samplerStates, 0, sizeof(samplerStates));

            setSize(size);

            return true;
        }

        void RendererD3D11::clear()
        {
            Renderer::clear();

            clearedRenderTargetViews.clear();
        }

        void RendererD3D11::present()
        {
            Renderer::present();

            swapChain->Present(swapInterval, 0);
        }

        IDXGIOutput* RendererD3D11::getOutput() const
        {
            std::shared_ptr<WindowWin> windowWin = std::static_pointer_cast<WindowWin>(sharedEngine->getWindow());

            HMONITOR monitor = windowWin->getMonitor();

            if (!monitor)
            {
                log("Window is not on any monitor");
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
                log("Failed to get display mode list");
                output->Release();
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

        void RendererD3D11::setSize(const Size2& newSize)
        {
            Renderer::setSize(newSize);

            if (swapChain)
            {
                UINT width = static_cast<UINT>(size.width);
                UINT height = static_cast<UINT>(size.height);

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

                swapChain->ResizeBuffers(0, width, height, DXGI_FORMAT_UNKNOWN, DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH);

                HRESULT hr = swapChain->GetBuffer(0, IID_ID3D11Texture2D, (void**)&backBuffer);
                if (FAILED(hr))
                {
                    log("Failed to retrieve D3D11 backbuffer");
                    return;
                }

                hr = device->CreateRenderTargetView(backBuffer, nullptr, &renderTargetView);
                if (FAILED(hr))
                {
                    log("Failed to create D3D11 render target view");
                    return;
                }

                viewport = { 0, 0, size.width, size.height, 0.0f, 1.0f };
                
                if (!activeRenderTarget)
                {
                    context->OMSetRenderTargets(1, &renderTargetView, nullptr);
                    context->RSSetViewports(1, &viewport);
                }
            }
        }

        void RendererD3D11::setFullscreen(bool newFullscreen)
        {
            Renderer::setFullscreen(newFullscreen);

            BOOL isFullscreen;
            swapChain->GetFullscreenState(&isFullscreen, nullptr);

            if (isFullscreen != static_cast<BOOL>(fullscreen))
            {
                if (fullscreen)
                {
                    IDXGIOutput* output = getOutput();

                    if (!output)
                    {
                        return;
                    }

                    DXGI_OUTPUT_DESC desc;
                    HRESULT hr = output->GetDesc(&desc);
                    if (FAILED(hr))
                    {
                        output->Release();
                        return;
                    }

                    MONITORINFOEX info;
                    info.cbSize = sizeof(MONITORINFOEX);
                    GetMonitorInfo(desc.Monitor, &info);
                    DEVMODE devMode;
                    devMode.dmSize = sizeof(DEVMODE);
                    devMode.dmDriverExtra = 0;
                    EnumDisplaySettings(info.szDevice, ENUM_CURRENT_SETTINGS, &devMode);

                    DXGI_MODE_DESC current;
                    current.Width = devMode.dmPelsWidth;
                    current.Height = devMode.dmPelsHeight;
                    bool defaultRefreshRate = (devMode.dmDisplayFrequency == 0 || devMode.dmDisplayFrequency == 1);
                    current.RefreshRate.Numerator = defaultRefreshRate ? 0 : devMode.dmDisplayFrequency;
                    current.RefreshRate.Denominator = defaultRefreshRate ? 0 : 1;
                    current.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
                    current.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
                    current.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

                    DXGI_MODE_DESC closestDisplayMode;
                    hr = output->FindClosestMatchingMode(&current, &closestDisplayMode, nullptr);
                    if (FAILED(hr))
                    {
                        output->Release();
                        return;
                    }

                    setSize(Size2(static_cast<float>(closestDisplayMode.Width),
                                  static_cast<float>(closestDisplayMode.Height)));
                    swapChain->SetFullscreenState(TRUE, output);

                    output->Release();
                }
                else
                {
                    swapChain->SetFullscreenState(FALSE, nullptr);
                }
            }
        }

        BlendStatePtr RendererD3D11::createBlendState(bool enableBlending,
                                                      BlendState::BlendFactor colorBlendSource, BlendState::BlendFactor colorBlendDest,
                                                      BlendState::BlendOperation colorOperation,
                                                      BlendState::BlendFactor alphaBlendSource, BlendState::BlendFactor alphaBlendDest,
                                                      BlendState::BlendOperation alphaOperation)
        {
            std::shared_ptr<BlendStateD3D11> blendState(new BlendStateD3D11());
            blendState->init(enableBlending,
                             colorBlendSource, colorBlendDest,
                             colorOperation,
                             alphaBlendSource, alphaBlendDest,
                             alphaOperation);

            return blendState;
        }

        bool RendererD3D11::activateBlendState(BlendStatePtr blendState)
        {
            if (activeBlendState == blendState)
            {
                return true;
            }

            if (!Renderer::activateBlendState(blendState))
            {
                return false;
            }

            if (blendState)
            {
                std::shared_ptr<BlendStateD3D11> blendStateD3D11 = std::static_pointer_cast<BlendStateD3D11>(activeBlendState);

                context->OMSetBlendState(blendStateD3D11->getBlendState(), NULL, 0xffffffff);
            }
            else
            {
                context->OMSetBlendState(NULL, NULL, 0xffffffff);
            }

            return true;
        }

        TexturePtr RendererD3D11::createTexture(const Size2& size, bool dynamic, bool mipmaps)
        {
            std::shared_ptr<TextureD3D11> texture(new TextureD3D11());
            texture->init(size, dynamic, mipmaps);

            return texture;
        }

        TexturePtr RendererD3D11::loadTextureFromFile(const std::string& filename, bool dynamic, bool mipmaps)
        {
            std::shared_ptr<TextureD3D11> texture(new TextureD3D11());
            texture->initFromFile(filename, dynamic, mipmaps);

            return texture;
        }

        TexturePtr RendererD3D11::loadTextureFromData(const void* data, const Size2& size, bool dynamic, bool mipmaps)
        {
            std::shared_ptr<TextureD3D11> texture(new TextureD3D11());
            texture->initFromData(data, size, dynamic, mipmaps);

            return texture;
        }

        bool RendererD3D11::activateTexture(const TexturePtr& texture, uint32_t layer)
        {
            if (activeTextures[layer] == texture)
            {
                return true;
            }

            if (!Renderer::activateTexture(texture, layer))
            {
                return false;
            }

            if (activeTextures[layer])
            {
                std::shared_ptr<TextureD3D11> textureD3D11 = std::static_pointer_cast<TextureD3D11>(activeTextures[layer]);

                resourceViews[layer] = textureD3D11->getResourceView();
                samplerStates[layer] = samplerState;
            }
            else
            {
                resourceViews[layer] = nullptr;
                samplerStates[layer] = nullptr;
            }

            return true;
        }

        RenderTargetPtr RendererD3D11::createRenderTarget(const Size2& size, bool depthBuffer)
        {
            std::shared_ptr<RenderTargetD3D11> renderTarget(new RenderTargetD3D11());
            renderTarget->init(size, depthBuffer);

            return renderTarget;
        }

        bool RendererD3D11::activateRenderTarget(const RenderTargetPtr& renderTarget)
        {
            if (!Renderer::activateRenderTarget(renderTarget))
            {
                return false;
            }

            ID3D11RenderTargetView* newRenderTargetView = nullptr;
            Color newClearColor;
            D3D11_VIEWPORT newViewport;

            if (activeRenderTarget)
            {
                std::shared_ptr<RenderTargetD3D11> renderTargetD3D11 = std::static_pointer_cast<RenderTargetD3D11>(activeRenderTarget);

                newRenderTargetView = renderTargetD3D11->getRenderTargetView();
                newClearColor = renderTargetD3D11->getClearColor();
                newViewport = renderTargetD3D11->getViewport();
            }
            else
            {
                newRenderTargetView = renderTargetView;
                newClearColor = clearColor;
                newViewport = viewport;
            }

            context->OMSetRenderTargets(1, &newRenderTargetView, nullptr);
            context->RSSetViewports(1, &newViewport);

            if (clearedRenderTargetViews.find(newRenderTargetView) == clearedRenderTargetViews.end())
            {
                float color[4] = { newClearColor.getR(), newClearColor.getG(), newClearColor.getB(), newClearColor.getA() };
                context->ClearRenderTargetView(newRenderTargetView, color);

                clearedRenderTargetViews.insert(newRenderTargetView);
            }

            return true;
        }

        ShaderPtr RendererD3D11::loadShaderFromFiles(const std::string& pixelShader,
                                                     const std::string& vertexShader,
                                                     uint32_t vertexAttributes,
                                                     const std::string& pixelShaderFunction,
                                                     const std::string& vertexShaderFunction)
        {
            std::shared_ptr<ShaderD3D11> shader(new ShaderD3D11());
            shader->initFromFiles(pixelShader, vertexShader, vertexAttributes, pixelShaderFunction, vertexShaderFunction);

            return shader;
        }

        ShaderPtr RendererD3D11::loadShaderFromBuffers(const uint8_t* pixelShader,
                                                       uint32_t pixelShaderSize,
                                                       const uint8_t* vertexShader,
                                                       uint32_t vertexShaderSize,
                                                       uint32_t vertexAttributes,
                                                       const std::string& pixelShaderFunction,
                                                       const std::string& vertexShaderFunction)
        {
            std::shared_ptr<ShaderD3D11> shader(new ShaderD3D11());
            shader->initFromBuffers(pixelShader, pixelShaderSize, vertexShader, vertexShaderSize, vertexAttributes, pixelShaderFunction, vertexShaderFunction);

            return shader;
        }

        bool RendererD3D11::activateShader(const ShaderPtr& shader)
        {
            if (activeShader == shader)
            {
                return true;
            }

            if (!Renderer::activateShader(shader))
            {
                return false;
            }

            if (activeShader)
            {
                std::shared_ptr<ShaderD3D11> shaderD3D11 = std::static_pointer_cast<ShaderD3D11>(activeShader);

                ID3D11Buffer* pixelShaderConstantBuffers[1] = { shaderD3D11->getPixelShaderConstantBuffer() };
                context->PSSetConstantBuffers(0, 1, pixelShaderConstantBuffers);

                ID3D11Buffer* vertexShaderConstantBuffers[1] = { shaderD3D11->getVertexShaderConstantBuffer() };
                context->VSSetConstantBuffers(0, 1, vertexShaderConstantBuffers);

                context->PSSetShader(shaderD3D11->getPixelShader(), nullptr, 0);
                context->VSSetShader(shaderD3D11->getVertexShader(), nullptr, 0);

                context->IASetInputLayout(shaderD3D11->getInputLayout());
            }
            else
            {
                context->PSSetShader(nullptr, nullptr, 0);
                context->VSSetShader(nullptr, nullptr, 0);
            }

            return true;
        }

        MeshBufferPtr RendererD3D11::createMeshBuffer()
        {
            std::shared_ptr<MeshBufferD3D11> meshBuffer(new MeshBufferD3D11());
            meshBuffer->init();

            return meshBuffer;
        }

        MeshBufferPtr RendererD3D11::createMeshBufferFromData(const void* indices, uint32_t indexSize, uint32_t indexCount, bool dynamicIndexBuffer, const void* vertices, uint32_t vertexAttributes, uint32_t vertexCount, bool dynamicVertexBuffer)
        {
            std::shared_ptr<MeshBufferD3D11> meshBuffer(new MeshBufferD3D11());
            meshBuffer->initFromData(indices, indexSize, indexCount, dynamicIndexBuffer, vertices, vertexAttributes, vertexCount, dynamicVertexBuffer);

            return meshBuffer;
        }

        bool RendererD3D11::drawMeshBuffer(const MeshBufferPtr& meshBuffer, uint32_t indexCount, DrawMode drawMode, uint32_t startIndex)
        {
            if (!Renderer::drawMeshBuffer(meshBuffer))
            {
                return false;
            }

            if (!activeShader || !activeShader->isReady())
            {
                return false;
            }

            context->PSSetShaderResources(0, TEXTURE_LAYERS, resourceViews);
            context->PSSetSamplers(0, TEXTURE_LAYERS, samplerStates);

            std::shared_ptr<MeshBufferD3D11> meshBufferD3D11 = std::static_pointer_cast<MeshBufferD3D11>(meshBuffer);

            if (indexCount == 0)
            {
                indexCount = meshBufferD3D11->getIndexCount() - startIndex;
            }

            context->RSSetState(rasterizerState);
            context->OMSetDepthStencilState(depthStencilState, 0);

            ID3D11Buffer* buffers[] = { meshBufferD3D11->getVertexBuffer() };
            UINT stride = meshBufferD3D11->getVertexSize();
            UINT offset = 0;
            context->IASetVertexBuffers(0, 1, buffers, &stride, &offset);
            context->IASetIndexBuffer(meshBufferD3D11->getIndexBuffer(), meshBufferD3D11->getIndexFormat(), 0);

            D3D_PRIMITIVE_TOPOLOGY topology;

            switch (drawMode)
            {
                case DrawMode::POINT_LIST: topology = D3D_PRIMITIVE_TOPOLOGY_POINTLIST; break;
                case DrawMode::LINE_LIST: topology = D3D_PRIMITIVE_TOPOLOGY_LINELIST; break;
                case DrawMode::LINE_STRIP: topology = D3D_PRIMITIVE_TOPOLOGY_LINESTRIP; break;
                case DrawMode::TRIANGLE_LIST: topology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST; break;
                case DrawMode::TRIANGLE_STRIP: topology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP; break;
                default: return false;
            }

            context->IASetPrimitiveTopology(topology);

            context->DrawIndexed(indexCount, static_cast<UINT>(startIndex * meshBuffer->getIndexSize()), 0);

            return true;
        }

        bool RendererD3D11::saveScreenshot(const std::string& filename)
        {
            D3D11_TEXTURE2D_DESC desc;
            desc.Width = static_cast<UINT>(size.width);
            desc.Height = static_cast<UINT>(size.height);
            desc.MipLevels = 1;
            desc.ArraySize = 1;
            desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
            desc.SampleDesc.Count = 1;
            desc.SampleDesc.Quality = 0;
            desc.Usage = D3D11_USAGE_STAGING;
            desc.BindFlags = 0;
            desc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
            desc.MiscFlags = 0;

            ID3D11Texture2D* texture;

            HRESULT hr = device->CreateTexture2D(&desc, nullptr, &texture);

            if (FAILED(hr))
            {
                log("Failed to create D3D11 texture");
                return false;
            }

            context->CopyResource(texture, backBuffer);

            D3D11_MAPPED_SUBRESOURCE mappedSubresource;
            hr = context->Map(texture, 0, D3D11_MAP_READ, 0, &mappedSubresource);

            if (FAILED(hr))
            {
                texture->Release();
                log("Failed to map D3D11 resource");
                return false;
            }

            if (!stbi_write_png(filename.c_str(), desc.Width, desc.Height, 4, mappedSubresource.pData, static_cast<int>(mappedSubresource.RowPitch)))
            {
                context->Unmap(texture, 0);
                texture->Release();
                log("Failed to save screenshot to file");
                return false;
            }

            context->Unmap(texture, 0);

            texture->Release();

            return true;
        }
    } // namespace graphics
} // namespace ouzel

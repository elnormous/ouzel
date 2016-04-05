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
    namespace video
    {
        RendererD3D11::RendererD3D11():
            Renderer(Driver::DIRECT3D11)
        {

        }

        RendererD3D11::~RendererD3D11()
        {
            clean();
        }

        void RendererD3D11::clean()
        {
            if (_depthStencilState) _depthStencilState->Release();
            if (_rasterizerState) _rasterizerState->Release();
            if (_samplerState) _samplerState->Release();
            if (_rtView) _rtView->Release();
            if (_backBuffer) _backBuffer->Release();
            if (_swapChain) _swapChain->Release();
            if (_adapter) _adapter->Release();
        }

        bool RendererD3D11::init(const Size2& size, bool fullscreen)
        {
            if (!Renderer::init(size, fullscreen))
            {
                return false;
            }

            clean();

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
                &_device,
                nullptr,
                &_context
                );
            if (FAILED(hr))
            {
                log("Failed to create the D3D11 device");
                return false;
            }

            IDXGIDevice* dxgiDevice;
            IDXGIFactory* factory;

            _device->QueryInterface(IID_IDXGIDevice, (void**)&dxgiDevice);
            dxgiDevice->GetParent(IID_IDXGIAdapter, (void**)&_adapter);
            hr = _adapter->GetParent(IID_IDXGIFactory, (void**)&factory);
            if (FAILED(hr))
            {
                log("Failed to get the DXGI factory");
                return false;
            }

            std::shared_ptr<WindowWin> windowWin = std::static_pointer_cast<WindowWin>(sharedEngine->getWindow());

            DXGI_SWAP_CHAIN_DESC swapChainDesc;
            memset(&swapChainDesc, 0, sizeof(swapChainDesc));

            swapChainDesc.BufferDesc.Width = static_cast<UINT>(_size.width);
            swapChainDesc.BufferDesc.Height = static_cast<UINT>(_size.height);
            swapChainDesc.BufferDesc.RefreshRate.Numerator = _fullscreen ? 60 : 0; // TODO refresh rate?
            swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
            swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
            swapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_PROGRESSIVE;
            swapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_STRETCHED;
            swapChainDesc.SampleDesc.Count = 1; // TODO MSAA?
            swapChainDesc.SampleDesc.Quality = 0;
            swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
            swapChainDesc.BufferCount = 1;
            swapChainDesc.OutputWindow = windowWin->getNativeWindow();
            swapChainDesc.Windowed = static_cast<BOOL>(!_fullscreen);
            swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
            swapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

            hr = factory->CreateSwapChain(_device, &swapChainDesc, &_swapChain);
            if (FAILED(hr))
            {
                log("Failed to create the D3D11 swap chain");
                return false;
            }

            factory->MakeWindowAssociation(windowWin->getNativeWindow(), DXGI_MWA_NO_ALT_ENTER);

            factory->Release();
            dxgiDevice->Release();

            // Backbuffer
            hr = _swapChain->GetBuffer(0, IID_ID3D11Texture2D, reinterpret_cast<void**>(&_backBuffer));
            if (FAILED(hr) || !_backBuffer)
            {
                log("Failed to retrieve D3D11 backbuffer");
                return false;
            }

            hr = _device->CreateRenderTargetView(_backBuffer, nullptr, &_rtView);
            if (FAILED(hr) || !_rtView)
            {
                log("Failed to create D3D11 render target view");
                return false;
            }

            // Sampler state
            D3D11_SAMPLER_DESC samplerStateDesc;
            samplerStateDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
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

            hr = _device->CreateSamplerState(&samplerStateDesc, &_samplerState);
            if (FAILED(hr) || !_samplerState)
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
                FALSE, // TODO MSAA enable?
                TRUE, // AA lines
            };

            hr = _device->CreateRasterizerState(&rasterStateDesc, &_rasterizerState);
            if (FAILED(hr) || !_rasterizerState)
            {
                log("Failed to create D3D11 rasterizer state");
                return false;
            }

            // Depth/stencil state
            D3D11_DEPTH_STENCIL_DESC depthStencilStateDesc =
            {
                FALSE, // enable depth?
                       // ...
            };
            hr = _device->CreateDepthStencilState(&depthStencilStateDesc, &_depthStencilState);
            if (FAILED(hr) || !_depthStencilState)
            {
                log("Failed to create D3D11 depth stencil state");
                return false;
            }

            ShaderPtr textureShader = loadShaderFromBuffers(TEXTURE_PIXEL_SHADER_D3D11, sizeof(TEXTURE_PIXEL_SHADER_D3D11),
                TEXTURE_VERTEX_SHADER_D3D11, sizeof(TEXTURE_VERTEX_SHADER_D3D11),
                VertexPCT::ATTRIBUTES);

            if (textureShader)
            {
                sharedEngine->getCache()->setShader(SHADER_TEXTURE, textureShader);
            }

            ShaderPtr colorShader = loadShaderFromBuffers(COLOR_PIXEL_SHADER_D3D11, sizeof(COLOR_PIXEL_SHADER_D3D11),
                COLOR_VERTEX_SHADER_D3D11, sizeof(COLOR_VERTEX_SHADER_D3D11),
                VertexPC::ATTRIBUTES);

            if (colorShader)
            {
                sharedEngine->getCache()->setShader(SHADER_COLOR, colorShader);
            }

            D3D11_VIEWPORT viewport = { 0, 0, _size.width, _size.height, 0.0f, 1.0f };
            _context->RSSetViewports(1, &viewport);
            _context->OMSetRenderTargets(1, &_rtView, nullptr);

            memset(&_resourceViews, 0, sizeof(_resourceViews));
            memset(&_samplerStates, 0, sizeof(_samplerStates));

            setSize(_size);

            return true;
        }

        void RendererD3D11::clear()
        {
            Renderer::clear();

            float color[4] = { _clearColor.getR(), _clearColor.getG(), _clearColor.getB(), _clearColor.getA() };

            if (_activeRenderTarget)
            {
                std::shared_ptr<RenderTargetD3D11> renderTargetD3D11 = std::static_pointer_cast<RenderTargetD3D11>(_activeRenderTarget);

                _context->ClearRenderTargetView(renderTargetD3D11->getRenderTargetView(), color);
            }
            else
            {
                _context->ClearRenderTargetView(_rtView, color);
            }
        }

        void RendererD3D11::present()
        {
            Renderer::present();

            _swapChain->Present(1 /* TODO vsync off? */, 0);
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

            while (_adapter->EnumOutputs(i, &output) != DXGI_ERROR_NOT_FOUND)
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
                    result.push_back(Size2(displayMode.Width, displayMode.Height));
                }
            }

            output->Release();

            return result;
        }

        void RendererD3D11::setSize(const Size2& size)
        {
            Renderer::setSize(size);

            if (_swapChain)
            {
                UINT width = static_cast<UINT>(size.width);
                UINT height = static_cast<UINT>(size.height);

                if (_rtView)
                {
                    _rtView->Release();
                    _rtView = nullptr;
                }

                if (_backBuffer)
                {
                    _backBuffer->Release();
                    _backBuffer = nullptr;
                }

                _swapChain->ResizeBuffers(0, width, height, DXGI_FORMAT_UNKNOWN, DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH);

                HRESULT hr = _swapChain->GetBuffer(0, IID_ID3D11Texture2D, (void**)&_backBuffer);
                if (FAILED(hr) || !_backBuffer)
                {
                    log("Failed to retrieve D3D11 backbuffer");
                    return;
                }

                hr = _device->CreateRenderTargetView(_backBuffer, nullptr, &_rtView);
                if (FAILED(hr) || !_rtView)
                {
                    log("Failed to create D3D11 render target view");
                    return;
                }

                D3D11_VIEWPORT viewport = { 0, 0, size.width, size.height, 0.0f, 1.0f };
                _context->RSSetViewports(1, &viewport);
                _context->OMSetRenderTargets(1, &_rtView, nullptr);
            }
        }

        void RendererD3D11::setFullscreen(bool fullscreen)
        {
            Renderer::setFullscreen(fullscreen);

            BOOL isFullscreen;
            _swapChain->GetFullscreenState(&isFullscreen, nullptr);

            if (static_cast<bool>(isFullscreen) != _fullscreen)
            {
                if (_fullscreen)
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

                    setSize(Size2(closestDisplayMode.Width, closestDisplayMode.Height));
                    _swapChain->SetFullscreenState(TRUE, output);

                    output->Release();
                }
                else
                {
                    _swapChain->SetFullscreenState(FALSE, nullptr);
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

            if (!blendState->init(enableBlending,
                                  colorBlendSource, colorBlendDest,
                                  colorOperation,
                                  alphaBlendSource, alphaBlendDest,
                                  alphaOperation))
            {
                blendState.reset();
            }

            return blendState;
        }

        bool RendererD3D11::activateBlendState(BlendStatePtr blendState)
        {
            if (_activeBlendState == blendState)
            {
                return true;
            }
            
            if (!Renderer::activateBlendState(blendState))
            {
                return false;
            }

            if (blendState)
            {
                std::shared_ptr<BlendStateD3D11> blendStateD3D11 = std::static_pointer_cast<BlendStateD3D11>(_activeBlendState);

                _context->OMSetBlendState(blendStateD3D11->getBlendState(), NULL, 0xffffffff);
            }
            else
            {
                _context->OMSetBlendState(NULL, NULL, 0xffffffff);
            }

            return true;
        }

        TexturePtr RendererD3D11::createTexture(const Size2& size, bool dynamic, bool mipmaps)
        {
            std::shared_ptr<TextureD3D11> texture(new TextureD3D11());

            if (!texture->init(size, dynamic, mipmaps))
            {
                texture.reset();
            }

            return texture;
        }

        TexturePtr RendererD3D11::loadTextureFromFile(const std::string& filename, bool dynamic, bool mipmaps)
        {
            std::shared_ptr<TextureD3D11> texture(new TextureD3D11());

            if (!texture->initFromFile(filename, dynamic, mipmaps))
            {
                texture.reset();
            }

            return texture;
        }

        TexturePtr RendererD3D11::loadTextureFromData(const void* data, const Size2& size, bool dynamic, bool mipmaps)
        {
            std::shared_ptr<TextureD3D11> texture(new TextureD3D11());

            if (!texture->initFromData(data, size, dynamic, mipmaps))
            {
                texture.reset();
            }

            return texture;
        }

        bool RendererD3D11::activateTexture(const TexturePtr& texture, uint32_t layer)
        {
            if (_activeTextures[layer] == texture)
            {
                return true;
            }

            if (!Renderer::activateTexture(texture, layer))
            {
                return false;
            }

            if (_activeTextures[layer])
            {
                std::shared_ptr<TextureD3D11> textureD3D11 = std::static_pointer_cast<TextureD3D11>(_activeTextures[layer]);

                _resourceViews[layer] = textureD3D11->getResourceView();
                _samplerStates[layer] = _samplerState;
            }
            else
            {
                _resourceViews[layer] = nullptr;
                _samplerStates[layer] = nullptr;
            }

            return true;
        }

        RenderTargetPtr RendererD3D11::createRenderTarget(const Size2& size, bool depthBuffer)
        {
            std::shared_ptr<RenderTargetD3D11> renderTarget(new RenderTargetD3D11());

            if (!renderTarget->init(size, depthBuffer))
            {
                renderTarget.reset();
            }

            return renderTarget;
        }

        bool RendererD3D11::activateRenderTarget(const RenderTargetPtr& renderTarget)
        {
            if (_activeRenderTarget == renderTarget)
            {
                return true;
            }

            if (!Renderer::activateRenderTarget(renderTarget))
            {
                return false;
            }

            if (_activeRenderTarget)
            {
                std::shared_ptr<RenderTargetD3D11> renderTargetD3D11 = std::static_pointer_cast<RenderTargetD3D11>(_activeRenderTarget);

                ID3D11RenderTargetView* renderTargetView = renderTargetD3D11->getRenderTargetView();

                _context->OMSetRenderTargets(1, &renderTargetView, nullptr);
            }
            else
            {
                _context->OMSetRenderTargets(1, nullptr, nullptr);
            }

            return true;
        }

        ShaderPtr RendererD3D11::loadShaderFromFiles(const std::string& fragmentShader, const std::string& vertexShader, uint32_t vertexAttributes)
        {
            std::shared_ptr<ShaderD3D11> shader(new ShaderD3D11());

            if (!shader->initFromFiles(fragmentShader, vertexShader, vertexAttributes))
            {
                shader.reset();
            }

            return shader;
        }

        ShaderPtr RendererD3D11::loadShaderFromBuffers(const uint8_t* fragmentShader, uint32_t fragmentShaderSize, const uint8_t* vertexShader, uint32_t vertexShaderSize, uint32_t vertexAttributes)
        {
            std::shared_ptr<ShaderD3D11> shader(new ShaderD3D11());

            if (!shader->initFromBuffers(fragmentShader, fragmentShaderSize, vertexShader, vertexShaderSize, vertexAttributes))
            {
                shader.reset();
            }

            return shader;
        }

        bool RendererD3D11::activateShader(const ShaderPtr& shader)
        {
            if (_activeShader == shader)
            {
                return true;
            }

            if (!Renderer::activateShader(shader))
            {
                return false;
            }

            if (_activeShader)
            {
                std::shared_ptr<ShaderD3D11> shaderD3D11 = std::static_pointer_cast<ShaderD3D11>(_activeShader);

                ID3D11Buffer* pixelShaderConstantBuffers[1] = { shaderD3D11->getPixelShaderConstantBuffer() };
                _context->PSSetConstantBuffers(0, 1, pixelShaderConstantBuffers);

                ID3D11Buffer* vertexShaderConstantBuffers[1] = { shaderD3D11->getVertexShaderConstantBuffer() };
                _context->VSSetConstantBuffers(0, 1, vertexShaderConstantBuffers);

                _context->PSSetShader(shaderD3D11->getPixelShader(), nullptr, 0);
                _context->VSSetShader(shaderD3D11->getVertexShader(), nullptr, 0);

                _context->IASetInputLayout(shaderD3D11->getInputLayout());
            }
            else
            {
                _context->PSSetShader(nullptr, nullptr, 0);
                _context->VSSetShader(nullptr, nullptr, 0);
            }

            return true;
        }

        MeshBufferPtr RendererD3D11::createMeshBuffer(const void* indices, uint32_t indexSize, uint32_t indexCount, bool dynamicIndexBuffer, const void* vertices, uint32_t vertexSize, uint32_t vertexCount, bool dynamicVertexBuffer, uint32_t vertexAttributes)
        {
            std::shared_ptr<MeshBufferD3D11> meshBuffer(new MeshBufferD3D11());

            if (!meshBuffer->initFromData(indices, indexSize, indexCount, dynamicIndexBuffer, vertices, vertexSize, vertexCount, dynamicVertexBuffer, vertexAttributes))
            {
                meshBuffer.reset();
            }

            return meshBuffer;
        }

        bool RendererD3D11::drawMeshBuffer(const MeshBufferPtr& meshBuffer, uint32_t indexCount, DrawMode drawMode)
        {
            if (!Renderer::drawMeshBuffer(meshBuffer))
            {
                return false;
            }

            _context->PSSetShaderResources(0, TEXTURE_LAYERS, _resourceViews);
            _context->PSSetSamplers(0, TEXTURE_LAYERS, _samplerStates);

            std::shared_ptr<MeshBufferD3D11> meshBufferD3D11 = std::static_pointer_cast<MeshBufferD3D11>(meshBuffer);

            if (indexCount == 0)
            {
                indexCount = meshBufferD3D11->getIndexCount();
            }

            _context->RSSetState(_rasterizerState);
            _context->OMSetDepthStencilState(_depthStencilState, 0);

            ID3D11Buffer* buffers[] = { meshBufferD3D11->getVertexBuffer() };
            UINT stride = meshBufferD3D11->getVertexSize();
            UINT offset = 0;
            _context->IASetVertexBuffers(0, 1, buffers, &stride, &offset);
            _context->IASetIndexBuffer(meshBufferD3D11->getIndexBuffer(), meshBufferD3D11->getIndexFormat(), 0);

            D3D_PRIMITIVE_TOPOLOGY topology;

            switch (drawMode)
            {
                case DrawMode::POINT_LIST: topology = D3D_PRIMITIVE_TOPOLOGY_POINTLIST; break;
                case DrawMode::LINE_LIST: topology = D3D_PRIMITIVE_TOPOLOGY_LINELIST; break;
                case DrawMode::LINE_STRIP: topology = D3D_PRIMITIVE_TOPOLOGY_LINESTRIP; break;
                case DrawMode::TRIANGLE_LIST: topology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST; break;
                case DrawMode::TRIANGLE_STRIP: topology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP; break;
            }

            _context->IASetPrimitiveTopology(topology);

            _context->DrawIndexed(indexCount, 0, 0);

            return true;
        }

        bool RendererD3D11::saveScreenshot(const std::string& filename)
        {
            if (!Renderer::saveScreenshot(filename))
            {
                return false;
            }

            D3D11_TEXTURE2D_DESC desc;
            desc.Width = static_cast<UINT>(_size.width);
            desc.Height = static_cast<UINT>(_size.height);
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

            HRESULT hr = _device->CreateTexture2D(&desc, nullptr, &texture);

            if (FAILED(hr))
            {
                return false;
            }

            _context->CopyResource(texture, _backBuffer);

            D3D11_MAPPED_SUBRESOURCE mappedSubresource;
            hr = _context->Map(texture, 0, D3D11_MAP_READ, 0, &mappedSubresource);

            if (FAILED(hr))
            {
                texture->Release();
                return false;
            }

            uint32_t size = desc.Height * mappedSubresource.RowPitch;

            stbi_write_png(filename.c_str(), desc.Width, desc.Height, 4, mappedSubresource.pData, size);

            _context->Unmap(texture, 0);

            texture->Release();

            return true;
        }
    } // namespace video
} // namespace ouzel

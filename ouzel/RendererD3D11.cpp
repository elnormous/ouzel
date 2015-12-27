// Copyright (C) 2015 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "RendererD3D11.h"
#include "TextureD3D11.h"
#include "ShaderD3D11.h"
#include "MeshBufferD3D11.h"
#include "Utils.h"
#include "TexturePSD3D11.h"
#include "TextureVSD3D11.h"

static LRESULT CALLBACK windowProc(HWND window, UINT msg, WPARAM wParam, LPARAM lParam)
{
    ouzel::RendererD3D11* rendererD3D11 = reinterpret_cast<ouzel::RendererD3D11*>(GetWindowLongPtrW(window, GWLP_USERDATA));

    switch (msg)
    {
        case WM_SIZING:
        {
            RECT* rc = reinterpret_cast<LPRECT>(lParam);
            rendererD3D11->resize(ouzel::Size2(rc->right - rc->left, rc->bottom - rc->top));
            break;
        }
        case WM_DESTROY:
        {
            PostQuitMessage(0);
            return 0;
        }
    }

    return DefWindowProcW(window, msg, wParam, lParam);
}

namespace ouzel
{
    RendererD3D11::RendererD3D11(const Size2& size, bool resizable, bool fullscreen, Engine* engine):
        Renderer(size, resizable, fullscreen, engine, Driver::DIRECT3D11)
    {
        initWindow();
        initD3D11();
        recalculateProjection();
    }

    RendererD3D11::~RendererD3D11()
    {
        if (_depthStencilState) _depthStencilState->Release();
        if (_blendState) _blendState->Release();
        if (_rasterizerState) _rasterizerState->Release();
        if (_samplerState) _samplerState->Release();
        if (_rtView) _rtView->Release();
        if (_backBuffer) _backBuffer->Release();
        if (_swapChain) _swapChain->Release();
    }

    void RendererD3D11::initWindow()
    {
        HINSTANCE hInstance = GetModuleHandle(nullptr);

        WNDCLASSEXW wc;
        memset(&wc, 0, sizeof(wc));
        wc.cbSize = sizeof(wc);
        wc.style = CS_HREDRAW | CS_VREDRAW;
        wc.lpfnWndProc = windowProc;
        wc.hInstance = hInstance;
        wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
        wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
        wc.lpszClassName = L"OuzelWindow";
        RegisterClassExW(&wc);

        DWORD style = WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX;

        if (_resizable)
        {
            style |= WS_SIZEBOX;
        }

        int x = CW_USEDEFAULT;
        int y = CW_USEDEFAULT;
        if (_fullscreen)
        {
            style = WS_POPUP;
            x = 0;
            y = 0;
        }
        RECT windowRect = { 0, 0, (int)_size.width, (int)_size.height };
        AdjustWindowRect(&windowRect, style, FALSE);

        _window = CreateWindowExW(
            0,
            L"OuzelWindow",
            L"Ouzel",
            style,
            x,
            y,
            windowRect.right - windowRect.left,
            windowRect.bottom - windowRect.top,
            nullptr,
            nullptr,
            hInstance,
            nullptr);

        SetWindowLongPtrW(_window, GWLP_USERDATA, (LONG_PTR)this);
        ShowWindow(_window, SW_SHOW);
    }

    void RendererD3D11::initD3D11()
    {
        DXGI_SWAP_CHAIN_DESC swapChainDesc;
        memset(&swapChainDesc, 0, sizeof(swapChainDesc));

        swapChainDesc.BufferDesc.Width = (int)_size.width;
        swapChainDesc.BufferDesc.Height = (int)_size.height;
        swapChainDesc.BufferDesc.RefreshRate.Numerator = _fullscreen ? 60 : 0; // TODO refresh rate?
        swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
        swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        swapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_PROGRESSIVE;
        swapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_STRETCHED;
        swapChainDesc.SampleDesc.Count = 1; // TODO MSAA?
        swapChainDesc.SampleDesc.Quality = 0;
        swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
        swapChainDesc.BufferCount = 1;
        swapChainDesc.OutputWindow = _window;
        swapChainDesc.Windowed = _fullscreen == false;
        swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
        swapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

        UINT deviceCreationFlags = 0;
#if D3D11_DEBUG
        deviceCreationFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif
        HRESULT hr = D3D11CreateDeviceAndSwapChain(
            nullptr, // adapter
            D3D_DRIVER_TYPE_HARDWARE,
            nullptr, // software rasterizer (unused)
            deviceCreationFlags,
            nullptr, // feature levels
            0, // ^^
            D3D11_SDK_VERSION,
            &swapChainDesc,
            &_swapChain,
            &_device,
            nullptr,
            &_context
            );
        if (FAILED(hr))
        {
            log("Failed to create the D3D11 device");
            return;
        }

        // Backbuffer
        hr = _swapChain->GetBuffer(0, IID_ID3D11Texture2D, (void**)&_backBuffer);
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

        // Sampler state
        D3D11_SAMPLER_DESC samplerStateDesc =
        {
            D3D11_FILTER_MIN_MAG_MIP_LINEAR,
            D3D11_TEXTURE_ADDRESS_WRAP,
            D3D11_TEXTURE_ADDRESS_WRAP,
            D3D11_TEXTURE_ADDRESS_WRAP,
            0, 1, D3D11_COMPARISON_NEVER,{ 0,0,0,0 }, 0, D3D11_FLOAT32_MAX
        };
        
        hr = _device->CreateSamplerState(&samplerStateDesc, &_samplerState);
        if (FAILED(hr) || !_samplerState)
        {
            log("Failed to create D3D11 sampler state");
            return;
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
            return;
        }

        // Blending state
        D3D11_BLEND_DESC blendStateDesc = { FALSE, FALSE }; // alpha to coverage, independent blend
        D3D11_RENDER_TARGET_BLEND_DESC targetBlendDesc =
        {
            TRUE, // enable blending
            D3D11_BLEND_SRC_ALPHA, D3D11_BLEND_INV_SRC_ALPHA, D3D11_BLEND_OP_ADD, // color blend source/dest factors, op
            D3D11_BLEND_SRC_ALPHA, D3D11_BLEND_INV_SRC_ALPHA, D3D11_BLEND_OP_ADD, // alpha blend source/dest factors, op
            D3D11_COLOR_WRITE_ENABLE_ALL, // color write mask
        };
        blendStateDesc.RenderTarget[0] = targetBlendDesc;

        hr = _device->CreateBlendState(&blendStateDesc, &_blendState);
        if (FAILED(hr) || !_blendState)
        {
            log("Failed to create D3D11 blend state");
            return;
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
            return;
        }

        Shader* textureShader = loadShaderFromBuffers(TEXTURE_PIXEL_SHADER_D3D11, sizeof(TEXTURE_PIXEL_SHADER_D3D11),
                                                      TEXTURE_VERTEX_SHADER_D3D11, sizeof(TEXTURE_VERTEX_SHADER_D3D11));

        if (textureShader)
        {
            _shaders[SHADER_TEXTURE] = textureShader;
        }

        D3D11_VIEWPORT viewport = { 0, 0, _size.width, _size.height, 0.0f, 1.0f };
        _context->RSSetViewports(1, &viewport);
        _context->OMSetRenderTargets(1, &_rtView, nullptr);
    }

    void RendererD3D11::clear()
    {
        float color[4] = { _clearColor.getR(), _clearColor.getG(), _clearColor.getB(), _clearColor.getA() };
        _context->ClearRenderTargetView(_rtView, color);
    }

    void RendererD3D11::flush()
    {
        _swapChain->Present(1 /* TODO vsync off? */, 0);
    }

    void RendererD3D11::resize(const Size2& size)
    {
        UINT width = size.width;
        UINT height = size.height;

        DWORD style = WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX;
        int x = CW_USEDEFAULT;
        int y = CW_USEDEFAULT;
        UINT swpFlags = SWP_NOMOVE | SWP_NOZORDER;
        if (_fullscreen)
        {
            style = WS_POPUP;
            x = 0;
            y = 0;
            swpFlags &= ~SWP_NOMOVE;
        }
        RECT rect = { 0, 0, (int) width, (int) height };
        AdjustWindowRect(&rect, style, FALSE);
        SetWindowPos(_window, nullptr, 0, 0, width, height, swpFlags);

        _rtView->Release();
        _rtView = nullptr;

        _backBuffer->Release();
        _backBuffer = nullptr;

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

        Renderer::resize(size);
    }

    Texture* RendererD3D11::loadTextureFromFile(const std::string& filename)
    {
        TextureD3D11* texture = new TextureD3D11(this);

        if (!texture->initFromFile(filename))
        {
            delete texture;
            texture = nullptr;
        }

        return texture;
    }

    Shader* RendererD3D11::loadShaderFromFiles(const std::string& fragmentShader, const std::string& vertexShader)
    {
        ShaderD3D11* shader = new ShaderD3D11(this);

        if (!shader->initFromFiles(fragmentShader, vertexShader))
        {
            delete shader;
            shader = nullptr;
        }

        return shader;
    }

    Shader* RendererD3D11::loadShaderFromBuffers(const uint8_t* fragmentShader, uint32_t fragmentShaderSize, const uint8_t* vertexShader, uint32_t vertexShaderSize)
    {
        ShaderD3D11* shader = new ShaderD3D11(this);

        if (!shader->initFromBuffers(fragmentShader, fragmentShaderSize, vertexShader, vertexShaderSize))
        {
            delete shader;
            shader = nullptr;
        }

        return shader;
    }

    bool RendererD3D11::activateShader(Shader* shader)
    {
        if (!Renderer::activateShader(shader))
        {
            return false;
        }

        return true;
    }

    MeshBuffer* RendererD3D11::createMeshBuffer(const std::vector<uint16_t>& indices, const std::vector<Vertex>& vertices)
    {
        MeshBufferD3D11* meshBuffer = new MeshBufferD3D11(this);

        if (!meshBuffer->initFromData(indices, vertices))
        {
            delete meshBuffer;
            meshBuffer = nullptr;
        }

        return meshBuffer;
    }

    bool RendererD3D11::drawMeshBuffer(MeshBuffer* meshBuffer)
    {
        if (!Renderer::drawMeshBuffer(meshBuffer))
        {
            return false;
        }

        MeshBufferD3D11* meshBufferD3D11 = static_cast<MeshBufferD3D11*>(meshBuffer);
        ShaderD3D11* shaderD3D11 = static_cast<ShaderD3D11*>(_activeShader.item);

        if (shaderD3D11)
        {
            ID3D11Buffer* pixelShaderConstantBuffers[1] = { shaderD3D11->getPixelShaderConstantBuffer() };
            _context->PSSetConstantBuffers(0, 1, pixelShaderConstantBuffers);

            ID3D11Buffer* vertexShaderConstantBuffers[1] = { shaderD3D11->getVertexShaderConstantBuffer() };
            _context->VSSetConstantBuffers(0, 1, vertexShaderConstantBuffers);

            _context->PSSetShader(shaderD3D11->getPixelShader(), nullptr, 0);
            _context->VSSetShader(shaderD3D11->getVertexShader(), nullptr, 0);

            _context->RSSetState(_rasterizerState);
            _context->OMSetBlendState(_blendState, NULL, 0xffffffff);
            _context->OMSetDepthStencilState(_depthStencilState, 0);
        }

        ID3D11ShaderResourceView* resourceViews[TEXTURE_LAYERS];
        ID3D11SamplerState* samplerStates[TEXTURE_LAYERS];
        for (int i = 0; i < TEXTURE_LAYERS; ++i)
        {
            TextureD3D11* textureD3D11 = static_cast<TextureD3D11*>(_activeTextures[i].item);
            resourceViews[i] = textureD3D11 ? textureD3D11->getResourceView() : nullptr;
            samplerStates[i] = textureD3D11 ? _samplerState : nullptr;
        }
        _context->PSSetShaderResources(0, TEXTURE_LAYERS, resourceViews);
        _context->PSSetSamplers(0, TEXTURE_LAYERS, samplerStates);

        _context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
        _context->IASetInputLayout(shaderD3D11->getInputLayout());

        ID3D11Buffer* buffers[] = { meshBufferD3D11->getVertexBuffer() };
        UINT stride = sizeof(Vertex);
        UINT offset = 0;
        _context->IASetVertexBuffers(0, 1, buffers, &stride, &offset);
        _context->IASetIndexBuffer(meshBufferD3D11->getIndexBuffer(), DXGI_FORMAT_R16_UINT, 0);

        _context->DrawIndexed(meshBufferD3D11->getIndexCount(), 0, 0);

        return true;
    }
}

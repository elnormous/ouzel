// Copyright (C) 2015 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "RendererD3D11.h"
#include "TextureD3D11.h"
#include "ShaderD3D11.h"
#include "MeshBufferD3D11.h"
#include "Utils.h"

static LRESULT CALLBACK windowProc(HWND window, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg)
    {
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
    RendererD3D11::RendererD3D11(const Size2& size, bool fullscreen, Engine* engine):
        Renderer(size, fullscreen, engine, Driver::DIRECT3D11)
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
        HINSTANCE hInstance = GetModuleHandle(NULL);

        WNDCLASSEXW wc;
        memset(&wc, 0, sizeof(wc));
        wc.cbSize = sizeof(wc);
        wc.style = CS_HREDRAW | CS_VREDRAW;
        wc.lpfnWndProc = windowProc;
        wc.hInstance = hInstance;
        wc.hCursor = LoadCursor(NULL, IDC_ARROW);
        wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
        wc.lpszClassName = L"OuzelWindow";
        RegisterClassExW(&wc);

        DWORD style = WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX;
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
            NULL,
            L"OuzelWindow",
            L"Ouzel",
            style,
            x,
            y,
            windowRect.right - windowRect.left,
            windowRect.bottom - windowRect.top,
            NULL,
            NULL,
            hInstance,
            NULL);

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
            NULL, // adapter
            D3D_DRIVER_TYPE_HARDWARE,
            NULL, // software rasterizer (unused)
            deviceCreationFlags,
            NULL, // feature levels
            0, // ^^
            D3D11_SDK_VERSION,
            &swapChainDesc,
            &_swapChain,
            &_device,
            NULL,
            &_context
            );
        if (FAILED(hr))
        {
            log("Failed to create the D3D11 device");
            return;
        }

        // Backbuffer
        hr = _swapChain->GetBuffer(0, IID_ID3D11Texture2D, (void**)&_backBuffer);
        if (FAILED(hr) || _backBuffer == NULL)
        {
            log("Failed to retrieve D3D11 backbuffer");
            return;
        }

        hr = _device->CreateRenderTargetView(_backBuffer, NULL, &_rtView);
        if (FAILED(hr) || _rtView == NULL)
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

        Shader* textureShader = loadShaderFromFiles("ps_texture.cso", "vs_common.cso");
        assert(textureShader && "Failed to load texture shader");
        _shaders[SHADER_TEXTURE] = textureShader;

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

    Shader* RendererD3D11::loadShaderFromStrings(const std::string& fragmentShader, const std::string& vertexShader)
    {
        ShaderD3D11* shader = new ShaderD3D11(this);

        if (!shader->initFromStrings(fragmentShader, vertexShader))
        {
            delete shader;
            shader = nullptr;
        }

        return shader;
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
        return true;
    }
}

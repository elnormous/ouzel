// Copyright (C) 2015 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "RendererD3D11.h"
#include "Engine.h"
#include "TextureD3D11.h"
#include "ShaderD3D11.h"
#include "MeshBufferD3D11.h"
#include "Utils.h"
#include "TexturePSD3D11.h"
#include "TextureVSD3D11.h"
#include "Event.h"

using namespace ouzel;

KeyboardKey winKeyToEngineCode(WPARAM wParam, LPARAM lParam)
{
    switch (wParam)
    {
        case VK_CANCEL: return KeyboardKey::CANCEL;
        case VK_BACK: return KeyboardKey::BACKSPACE;
        case VK_TAB: return KeyboardKey::TAB;
        case VK_CLEAR: return KeyboardKey::CLEAR;
        case VK_RETURN: return KeyboardKey::RETURN;
        case VK_SHIFT: return KeyboardKey::SHIFT;
        case VK_CONTROL: return KeyboardKey::CONTROL;
        case VK_MENU: return KeyboardKey::MENU;
        case VK_PAUSE: return KeyboardKey::PAUSE;
        case VK_CAPITAL: return KeyboardKey::CAPITAL;

            // ... Japanese ...

        case VK_ESCAPE: return KeyboardKey::ESCAPE;

            // ... IME ...

        case VK_SPACE: return KeyboardKey::SPACE;
        case VK_PRIOR: return KeyboardKey::PRIOR;
        case VK_NEXT: return KeyboardKey::NEXT;
        case VK_END: return KeyboardKey::END;
        case VK_HOME: return KeyboardKey::HOME;
        case VK_LEFT: return KeyboardKey::LEFT;
        case VK_UP: return KeyboardKey::UP;
        case VK_RIGHT: return KeyboardKey::RIGHT;
        case VK_DOWN: return KeyboardKey::DOWN;

        case VK_SELECT: return KeyboardKey::SELECT;
        case VK_PRINT: return KeyboardKey::PRINT;
        case VK_EXECUTE: return KeyboardKey::EXECUT;

        case VK_SNAPSHOT: return KeyboardKey::SNAPSHOT;
        case VK_INSERT: return KeyboardKey::INSERT;
        case VK_DELETE: return KeyboardKey::DEL;
        case VK_HELP: return KeyboardKey::HELP;

        case '0': return KeyboardKey::KEY_0;
        case '1': return KeyboardKey::KEY_1;
        case '2': return KeyboardKey::KEY_2;
        case '3': return KeyboardKey::KEY_3;
        case '4': return KeyboardKey::KEY_4;
        case '5': return KeyboardKey::KEY_5;
        case '6': return KeyboardKey::KEY_6;
        case '7': return KeyboardKey::KEY_7;
        case '8': return KeyboardKey::KEY_8;
        case '9': return KeyboardKey::KEY_9;

        case 'A': return KeyboardKey::KEY_A;
        case 'B': return KeyboardKey::KEY_B;
        case 'C': return KeyboardKey::KEY_C;
        case 'D': return KeyboardKey::KEY_D;
        case 'E': return KeyboardKey::KEY_E;
        case 'F': return KeyboardKey::KEY_F;
        case 'G': return KeyboardKey::KEY_G;
        case 'H': return KeyboardKey::KEY_H;
        case 'I': return KeyboardKey::KEY_I;
        case 'J': return KeyboardKey::KEY_J;
        case 'K': return KeyboardKey::KEY_K;
        case 'L': return KeyboardKey::KEY_L;
        case 'M': return KeyboardKey::KEY_M;
        case 'N': return KeyboardKey::KEY_N;
        case 'O': return KeyboardKey::KEY_O;
        case 'P': return KeyboardKey::KEY_P;
        case 'Q': return KeyboardKey::KEY_Q;
        case 'R': return KeyboardKey::KEY_R;
        case 'S': return KeyboardKey::KEY_S;
        case 'T': return KeyboardKey::KEY_T;
        case 'U': return KeyboardKey::KEY_U;
        case 'V': return KeyboardKey::KEY_V;
        case 'W': return KeyboardKey::KEY_W;
        case 'X': return KeyboardKey::KEY_X;
        case 'Y': return KeyboardKey::KEY_Y;
        case 'Z': return KeyboardKey::KEY_Z;

        case VK_LWIN: return KeyboardKey::LWIN;
        case VK_RWIN: return KeyboardKey::RWIN;
        case VK_APPS: return KeyboardKey::MENU;
        case VK_SLEEP: return KeyboardKey::SLEEP;

        case VK_NUMPAD0: return KeyboardKey::NUMPAD0;
        case VK_NUMPAD1: return KeyboardKey::NUMPAD1;
        case VK_NUMPAD2: return KeyboardKey::NUMPAD2;
        case VK_NUMPAD3: return KeyboardKey::NUMPAD3;
        case VK_NUMPAD4: return KeyboardKey::NUMPAD4;
        case VK_NUMPAD5: return KeyboardKey::NUMPAD5;
        case VK_NUMPAD6: return KeyboardKey::NUMPAD6;
        case VK_NUMPAD7: return KeyboardKey::NUMPAD7;
        case VK_NUMPAD8: return KeyboardKey::NUMPAD8;
        case VK_NUMPAD9: return KeyboardKey::NUMPAD9;

        case VK_MULTIPLY: return KeyboardKey::MULTIPLY;
        case VK_ADD: return KeyboardKey::ADD;
        case VK_SEPARATOR: return KeyboardKey::SEPARATOR;
        case VK_SUBTRACT: return KeyboardKey::SUBTRACT;
        case VK_DECIMAL: return KeyboardKey::DECIMAL;
        case VK_DIVIDE: return KeyboardKey::DIVIDE;

        case VK_F1: return KeyboardKey::F1;
        case VK_F2: return KeyboardKey::F2;
        case VK_F3: return KeyboardKey::F3;
        case VK_F4: return KeyboardKey::F4;
        case VK_F5: return KeyboardKey::F5;
        case VK_F6: return KeyboardKey::F6;
        case VK_F7: return KeyboardKey::F7;
        case VK_F8: return KeyboardKey::F8;
        case VK_F9: return KeyboardKey::F9;
        case VK_F10: return KeyboardKey::F10;
        case VK_F11: return KeyboardKey::F11;
        case VK_F12: return KeyboardKey::F12;
        case VK_F13: return KeyboardKey::F13;
        case VK_F14: return KeyboardKey::F14;
        case VK_F15: return KeyboardKey::F15;
        case VK_F16: return KeyboardKey::F16;
        case VK_F17: return KeyboardKey::F17;
        case VK_F18: return KeyboardKey::F18;
        case VK_F19: return KeyboardKey::F19;
        case VK_F20: return KeyboardKey::F20;
        case VK_F21: return KeyboardKey::F21;
        case VK_F22: return KeyboardKey::F22;
        case VK_F23: return KeyboardKey::F23;
        case VK_F24: return KeyboardKey::F24;

        case VK_NUMLOCK: return KeyboardKey::NUMLOCK;
        case VK_SCROLL: return KeyboardKey::SCROLL;
        case VK_LSHIFT: return KeyboardKey::LSHIFT;
        case VK_RSHIFT: return KeyboardKey::RSHIFT;
        case VK_LCONTROL: return KeyboardKey::LCONTROL;
        case VK_RCONTROL: return KeyboardKey::RCONTROL;
        case VK_LMENU: return KeyboardKey::LMENU;
        case VK_RMENU: return KeyboardKey::RMENU;

        case VK_OEM_1: return KeyboardKey::OEM_1;
        case VK_OEM_PLUS: return KeyboardKey::PLUS;
        case VK_OEM_COMMA: return KeyboardKey::COMMA;
        case VK_OEM_MINUS: return KeyboardKey::MINUS;
        case VK_OEM_PERIOD: return KeyboardKey::PERIOD;
        case VK_OEM_2: return KeyboardKey::OEM_2;
        case VK_OEM_3: return KeyboardKey::OEM_3;
        case VK_OEM_4: return KeyboardKey::OEM_4;
        case VK_OEM_5: return KeyboardKey::OEM_5;
        case VK_OEM_6: return KeyboardKey::OEM_6;
        case VK_OEM_7: return KeyboardKey::OEM_7;
        case VK_OEM_8: return KeyboardKey::OEM_8;
        case VK_OEM_AX: return KeyboardKey::OEM_AX;
        case VK_OEM_102: return KeyboardKey::OEM_102;

        // ... misc keys ...
    }
    return KeyboardKey::NONE;
}

static void handleKeyEvent(RendererD3D11* rendererD3D11, UINT msg, WPARAM wParam, LPARAM lParam)
{
    bool isDown = msg == WM_KEYDOWN;
    KeyboardKey key = winKeyToEngineCode(wParam, lParam);

    Event event;
    event.type = isDown ? Event::Type::KEY_DOWN : Event::Type::KEY_UP;
    event.keyboardEvent.key = key;
    if (wParam & MK_SHIFT)
    {
        event.keyboardEvent.shiftDown = true;
    }
    if (wParam & MK_CONTROL)
    {
        event.keyboardEvent.controlDown = true;
    }
    rendererD3D11->getEngine()->handleEvent(event);
}

static void handleMouseMoveEvent(RendererD3D11* rendererD3D11, UINT msg, WPARAM wParam, LPARAM lParam)
{
    Vector2 pos(static_cast<float>(LOWORD(lParam)),
                rendererD3D11->getSize().height - static_cast<float>(HIWORD(lParam)));

    Event event;
    event.type = Event::Type::MOUSE_MOVE;
    event.mouseEvent.position = rendererD3D11->absoluteToWorldLocation(pos);
    rendererD3D11->getEngine()->handleEvent(event);
}

static void handleMouseButtonEvent(RendererD3D11* rendererD3D11, UINT msg, WPARAM wParam, LPARAM lParam)
{
    bool isDown = (msg == WM_LBUTTONDOWN || msg == WM_RBUTTONDOWN || msg == WM_MBUTTONDOWN);
    MouseButton button = MouseButton::NONE;

    if (msg == WM_LBUTTONDOWN || msg == WM_LBUTTONUP)
    {
        button = MouseButton::LEFT;
    }
    else if (msg == WM_RBUTTONDOWN || msg == WM_RBUTTONUP)
    {
        button = MouseButton::RIGHT;
    }
    else if (msg == WM_MBUTTONDOWN || msg == WM_MBUTTONUP)
    {
        button = MouseButton::MIDDLE;
    }

    Vector2 pos(static_cast<float>(LOWORD(lParam)),
                rendererD3D11->getSize().height - static_cast<float>(HIWORD(lParam)));

    Event event;
    event.type = isDown ? Event::Type::MOUSE_DOWN : Event::Type::MOUSE_UP;
    event.mouseEvent.button = button;
    event.mouseEvent.position = rendererD3D11->absoluteToWorldLocation(pos);
    rendererD3D11->getEngine()->handleEvent(event);
}

static void handleMouseWheelEvent(RendererD3D11* rendererD3D11, UINT msg, WPARAM wParam, LPARAM lParam)
{
    Vector2 pos(static_cast<float>(LOWORD(lParam)),
                rendererD3D11->getSize().height - static_cast<float>(HIWORD(lParam)));

    Event event;
    event.type = Event::Type::MOUSE_SCROLL;
    event.mouseEvent.scroll = Vector2(0.0f, static_cast<float>(HIWORD(wParam)) / static_cast<float>(WHEEL_DELTA));
    event.mouseEvent.position = rendererD3D11->absoluteToWorldLocation(pos);
    rendererD3D11->getEngine()->handleEvent(event);
}

static LRESULT CALLBACK windowProc(HWND window, UINT msg, WPARAM wParam, LPARAM lParam)
{
    RendererD3D11* rendererD3D11 = reinterpret_cast<RendererD3D11*>(GetWindowLongPtrW(window, GWLP_USERDATA));

    switch (msg)
    {
        case WM_KEYUP:
        case WM_KEYDOWN:
        {
            handleKeyEvent(rendererD3D11, msg, wParam, lParam);
            break;
        }
        case WM_LBUTTONDOWN:
        case WM_LBUTTONUP:
        case WM_RBUTTONDOWN:
        case WM_RBUTTONUP:
        case WM_MBUTTONDOWN:
        case WM_MBUTTONUP:
        {
            handleMouseButtonEvent(rendererD3D11, msg, wParam, lParam);
            break;
        }
        case WM_MOUSEMOVE:
        {
            handleMouseMoveEvent(rendererD3D11, msg, wParam, lParam);
            break;
        }
        case WM_MOUSEHWHEEL:
        {
            handleMouseWheelEvent(rendererD3D11, msg, wParam, lParam);
            break;
        }
        case WM_SIZE:
        {
            if (wParam == SIZE_MAXIMIZED || wParam == SIZE_RESTORED)
            {
                INT width = LOWORD(lParam);
                INT height = HIWORD(lParam);

                rendererD3D11->resize(Size2(width, height));
            }
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

        _windowStyle = WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX;

        if (_resizable)
        {
            _windowStyle |= WS_SIZEBOX | WS_MAXIMIZEBOX;
        }

        int x = CW_USEDEFAULT;
        int y = CW_USEDEFAULT;
        if (_fullscreen)
        {
            _windowStyle = WS_POPUP;
            x = 0;
            y = 0;
        }
        RECT windowRect = { 0, 0, (int)_size.width, (int)_size.height };
        AdjustWindowRect(&windowRect, _windowStyle, FALSE);

        _window = CreateWindowExW(
            0,
            L"OuzelWindow",
            L"Ouzel",
            _windowStyle,
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
        Renderer::resize(size);

        UINT width = size.width;
        UINT height = size.height;

        int x = CW_USEDEFAULT;
        int y = CW_USEDEFAULT;
        UINT swpFlags = SWP_NOMOVE | SWP_NOZORDER;
        if (_fullscreen)
        {
            x = 0;
            y = 0;
            swpFlags &= ~SWP_NOMOVE;
        }
        RECT rect = { 0, 0, static_cast<LONG>(width), static_cast<LONG>(height) };
        AdjustWindowRect(&rect, _windowStyle, FALSE);

        SetWindowPos(_window, nullptr, 0, 0, rect.right - rect.left, rect.bottom - rect.top, swpFlags);

        if (_rtView && _backBuffer)
        {
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
        }
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

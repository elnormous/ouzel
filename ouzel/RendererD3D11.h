// Copyright (C) 2015 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include <windows.h>
#include <d3d11.h>
#include "Renderer.h"

namespace ouzel
{
    class RendererD3D11: public Renderer
    {
    public:
        RendererD3D11(const Size2& size, bool fullscreen, Engine* engine);
        virtual ~RendererD3D11();

        bool initWindow();
        bool initD3D11();

        virtual void clear() override;
        virtual void flush() override;

        virtual Shader* loadShaderFromFiles(const std::string& fragmentShader, const std::string& vertexShader) override;
        virtual Shader* loadShaderFromStrings(const std::string& fragmentShader, const std::string& vertexShader) override;
        virtual Texture* loadTextureFromFile(const std::string& filename) override;

        virtual MeshBuffer* createMeshBuffer(const std::vector<uint16_t>& indices, const std::vector<Vertex>& vertices);
        virtual bool drawMeshBuffer(MeshBuffer* meshBuffer);

        ID3D11Device* getDevice() const { return _device; }
        ID3D11DeviceContext* getContext() const { return _context; }

    private:
        HWND _window;

        ID3D11Device* _device;
        ID3D11DeviceContext* _context;
        IDXGISwapChain* _swapChain;
        ID3D11Texture2D* _backBuffer;
        ID3D11RenderTargetView* _rtView;
        ID3D11SamplerState* _samplerState;
        ID3D11RasterizerState* _rasterizerState;
        ID3D11BlendState* _blendState;
        ID3D11DepthStencilState* _depthStencilState;
    };
}

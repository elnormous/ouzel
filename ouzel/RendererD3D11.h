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
        friend Engine;
    public:
        virtual ~RendererD3D11();

        void initWindow();
        void initD3D11();

        virtual void clear() override;
        virtual void flush() override;

        virtual void resize(const Size2& size) override;

        virtual Texture* loadTextureFromFile(const std::string& filename, bool dynamic) override;
        virtual Texture* loadTextureFromData(const void* data, const Size2& size, bool dynamic) override;

        virtual Shader* loadShaderFromFiles(const std::string& fragmentShader, const std::string& vertexShader, uint32_t vertexAttributes) override;
        virtual Shader* loadShaderFromBuffers(const uint8_t* fragmentShader, uint32_t fragmentShaderSize, const uint8_t* vertexShader, uint32_t vertexShaderSize, uint32_t vertexAttributes) override;

        virtual MeshBuffer* createMeshBuffer(const void* indices, uint32_t indexSize, uint32_t indexCount, bool dynamicIndexBuffer, const void* vertices, uint32_t vertexSize, uint32_t vertexCount, bool dynamicVertexBuffer, uint32_t vertexAttributes);
        virtual bool drawMeshBuffer(MeshBuffer* meshBuffer);

        virtual bool drawLine(const Vector2& start, const Vector2& finish, const Color& color, const Matrix4& transform = Matrix4()) override;
        virtual bool drawRectangle(const Rectangle& rectangle, const Color& color, const Matrix4& transform = Matrix4()) override;
        virtual bool drawQuad(const Rectangle& rectangle, const Color& color, const Matrix4& transform = Matrix4()) override;

        ID3D11Device* getDevice() const { return _device; }
        ID3D11DeviceContext* getContext() const { return _context; }

    protected:
        RendererD3D11(const Size2& size, bool resizable, bool fullscreen);

    private:
        HWND _window;
        DWORD _windowStyle;

        ID3D11Device* _device = nullptr;
        ID3D11DeviceContext* _context = nullptr;
        IDXGISwapChain* _swapChain = nullptr;
        ID3D11Texture2D* _backBuffer = nullptr;
        ID3D11RenderTargetView* _rtView = nullptr;
        ID3D11SamplerState* _samplerState = nullptr;
        ID3D11RasterizerState* _rasterizerState = nullptr;
        ID3D11BlendState* _blendState = nullptr;
        ID3D11DepthStencilState* _depthStencilState = nullptr;

        ID3D11ShaderResourceView* _resourceViews[TEXTURE_LAYERS];
        ID3D11SamplerState* _samplerStates[TEXTURE_LAYERS];
    };
}

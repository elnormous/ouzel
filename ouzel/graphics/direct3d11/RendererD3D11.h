// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include <atomic>
#define NOMINMAX
#include <set>
#include <windows.h>
#include <d3d11.h>
#include "graphics/Renderer.h"
#include "graphics/Texture.h"

namespace ouzel
{
    namespace graphics
    {
        class RendererD3D11: public Renderer
        {
            friend Engine;
        public:
            virtual ~RendererD3D11();
            virtual void free() override;

            virtual void setSize(const Size2& newSize) override;

            virtual bool present() override;

            virtual std::vector<Size2> getSupportedResolutions() const override;

            virtual BlendStatePtr createBlendState() override;
            virtual TexturePtr createTexture() override;
            virtual RenderTargetPtr createRenderTarget() override;
            virtual ShaderPtr createShader() override;
            virtual MeshBufferPtr createMeshBuffer() override;
            virtual IndexBufferPtr createIndexBuffer() override;
            virtual VertexBufferPtr createVertexBuffer() override;

            ID3D11Device* getDevice() const { return device; }
            ID3D11DeviceContext* getContext() const { return context; }

        protected:
            RendererD3D11();

            virtual bool init(Window* newWindow,
                              uint32_t newSampleCount,
                              TextureFilter newTextureFilter,
                              PixelFormat newBackBufferFormat,
                              bool newVerticalSync) override;
            bool update();
            bool resizeBackBuffer(UINT newWidth, UINT newHeight);

            bool saveScreenshots();

            virtual void setClearColor(Color color) override;

            IDXGIOutput* getOutput() const;

            ID3D11Device* device = nullptr;
            ID3D11DeviceContext* context = nullptr;
            IDXGISwapChain* swapChain = nullptr;
            IDXGIAdapter* adapter = nullptr;
            ID3D11Texture2D* backBuffer = nullptr;
            ID3D11RenderTargetView* renderTargetView = nullptr;
            std::set<ID3D11RenderTargetView*> clearedRenderTargetViews;
            ID3D11SamplerState* samplerState = nullptr;
            ID3D11RasterizerState* rasterizerStates[4];
            ID3D11DepthStencilState* depthStencilState = nullptr;

            UINT width = 0;
            UINT height = 0;

            ID3D11ShaderResourceView* resourceViews[Texture::LAYERS];
            ID3D11SamplerState* samplerStates[Texture::LAYERS];

            UINT swapInterval = 0;
            FLOAT frameBufferClearColor[4];

            bool sizeDirty = false;
            std::atomic<bool> dirty;
            std::mutex dataMutex;
        };
    } // namespace graphics
} // namespace ouzel

// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#define NOMINMAX
#include <set>
#include <windows.h>
#include <d3d11.h>
#include "graphics/Renderer.h"

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

            virtual void clear() override;
            virtual void present() override;

            virtual std::vector<Size2> getSupportedResolutions() const override;

            virtual void setSize(const Size2& newSize) override;

            virtual void setFullscreen(bool newFullscreen) override;

            virtual BlendStatePtr createBlendState() override;
            virtual bool activateBlendState(BlendStatePtr blendState) override;

            virtual TexturePtr createTexture() override;
            virtual bool activateTexture(const TexturePtr& texture, uint32_t layer) override;
            virtual RenderTargetPtr createRenderTarget() override;
            virtual bool activateRenderTarget(const RenderTargetPtr& renderTarget) override;

            virtual ShaderPtr createShader() override;
            virtual bool activateShader(const ShaderPtr& shader) override;

            virtual MeshBufferPtr createMeshBuffer();
            virtual bool drawMeshBuffer(const MeshBufferPtr& meshBuffer, uint32_t indexCount = 0, DrawMode drawMode = DrawMode::TRIANGLE_LIST, uint32_t startIndex = 0);

            virtual void activateScissorTest(const Rectangle& rectangle) override;

            virtual bool saveScreenshot(const std::string& filename) override;

            ID3D11Device* getDevice() const { return device; }
            ID3D11DeviceContext* getContext() const { return context; }

        protected:
            RendererD3D11();

            virtual bool init(const WindowPtr& window,
                              uint32_t newSampleCount,
                              TextureFiltering newTextureFiltering,
                              float newTargetFPS,
                              bool newVerticalSync) override;

            IDXGIOutput* getOutput() const;

        private:
            ID3D11Device* device = nullptr;
            ID3D11DeviceContext* context = nullptr;
            IDXGISwapChain* swapChain = nullptr;
            IDXGIAdapter* adapter = nullptr;
            ID3D11Texture2D* backBuffer = nullptr;
            ID3D11RenderTargetView* renderTargetView = nullptr;
            std::set<ID3D11RenderTargetView*> clearedRenderTargetViews;
            ID3D11SamplerState* samplerState = nullptr;
            ID3D11RasterizerState* rasterizerState = nullptr;
            ID3D11RasterizerState* scissorTestRasterizerState = nullptr;
            ID3D11DepthStencilState* depthStencilState = nullptr;

            D3D11_VIEWPORT viewport;

            ID3D11ShaderResourceView* resourceViews[TEXTURE_LAYERS];
            ID3D11SamplerState* samplerStates[TEXTURE_LAYERS];

            UINT swapInterval = 0;
        };
    } // namespace graphics
} // namespace ouzel

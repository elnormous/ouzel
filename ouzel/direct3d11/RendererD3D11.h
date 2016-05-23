// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#define NOMINMAX
#include <set>
#include <windows.h>
#include <d3d11.h>
#include "Renderer.h"

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

            virtual BlendStatePtr createBlendState(bool enableBlending,
                                                   BlendState::BlendFactor colorBlendSource, BlendState::BlendFactor colorBlendDest,
                                                   BlendState::BlendOperation colorOperation,
                                                   BlendState::BlendFactor alphaBlendSource, BlendState::BlendFactor alphaBlendDest,
                                                   BlendState::BlendOperation alphaOperation) override;
            virtual bool activateBlendState(BlendStatePtr blendState) override;

            virtual TexturePtr createTexture(const Size2& size, bool dynamic, bool mipmaps = true) override;
            virtual TexturePtr loadTextureFromFile(const std::string& filename, bool dynamic, bool mipmaps = true) override;
            virtual TexturePtr loadTextureFromData(const void* data, const Size2& size, bool dynamic, bool mipmaps = true) override;
            virtual bool activateTexture(const TexturePtr& texture, uint32_t layer) override;
            virtual RenderTargetPtr createRenderTarget(const Size2& size, bool depthBuffer) override;
            virtual bool activateRenderTarget(const RenderTargetPtr& renderTarget) override;

            virtual ShaderPtr loadShaderFromFiles(const std::string& pixelShader,
                                                  const std::string& vertexShader,
                                                  uint32_t vertexAttributes,
                                                  const std::string& pixelShaderFunction = "",
                                                  const std::string& vertexShaderFunction = "") override;
            virtual ShaderPtr loadShaderFromBuffers(const uint8_t* pixelShader,
                                                    uint32_t pixelShaderSize, const uint8_t* vertexShader,
                                                    uint32_t vertexShaderSize,
                                                    uint32_t vertexAttributes,
                                                    const std::string& pixelShaderFunction = "",
                                                    const std::string& vertexShaderFunction = "") override;
            virtual bool activateShader(const ShaderPtr& shader) override;

            virtual MeshBufferPtr createMeshBuffer();
            virtual MeshBufferPtr createMeshBufferFromData(const void* indices, uint32_t indexSize, uint32_t indexCount, bool dynamicIndexBuffer, const void* vertices, uint32_t vertexAttributes, uint32_t vertexCount, bool dynamicVertexBuffer);
            virtual bool drawMeshBuffer(const MeshBufferPtr& meshBuffer, uint32_t indexCount = 0, DrawMode drawMode = DrawMode::TRIANGLE_LIST, uint32_t startIndex = 0);

            virtual bool saveScreenshot(const std::string& filename) override;

            ID3D11Device* getDevice() const { return device; }
            ID3D11DeviceContext* getContext() const { return context; }

        protected:
            RendererD3D11();

            virtual bool init(const Size2& newSize, bool newFullscreen, uint32_t newSampleCount, bool newBilinearFiltering) override;

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
            ID3D11DepthStencilState* depthStencilState = nullptr;

            D3D11_VIEWPORT viewport;

            ID3D11ShaderResourceView* resourceViews[TEXTURE_LAYERS];
            ID3D11SamplerState* samplerStates[TEXTURE_LAYERS];
        };
    } // namespace graphics
} // namespace ouzel

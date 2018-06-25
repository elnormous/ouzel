// Copyright (C) 2018 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include "core/Setup.h"

#if OUZEL_COMPILE_DIRECT3D11

#include <atomic>
#include <map>
#include <d3d11.h>
#include "graphics/RenderDevice.hpp"
#include "graphics/Texture.hpp"
#include "thread/Thread.hpp"

namespace ouzel
{
    namespace graphics
    {
        class RenderDeviceD3D11: public RenderDevice
        {
            friend Renderer;
        public:
            virtual ~RenderDeviceD3D11();

            virtual void setClearColor(Color color) override;

            virtual std::vector<Size2> getSupportedResolutions() const override;

            ID3D11Device* getDevice() const { return device; }
            ID3D11DeviceContext* getContext() const { return context; }

            struct SamplerStateDesc
            {
                Texture::Filter filter;
                Texture::Address addressX;
                Texture::Address addressY;
                uint32_t maxAnisotropy;

                bool operator<(const SamplerStateDesc& other) const
                {
                    return std::tie(filter, addressX, addressY, maxAnisotropy) < std::tie(other.filter, other.addressX, other.addressY, other.maxAnisotropy);
                }
            };

            ID3D11SamplerState* getSamplerState(const SamplerStateDesc& desc);
            void setFullscreen(bool newFullscreen);

        protected:
            RenderDeviceD3D11();

            virtual void init(Window* newWindow,
                              const Size2& newSize,
                              uint32_t newSampleCount,
                              Texture::Filter newTextureFilter,
                              uint32_t newMaxAnisotropy,
                              bool newVerticalSync,
                              bool newDepth,
                              bool newDebugRenderer) override;

            virtual void setSize(const Size2& newSize);

            virtual bool processCommands(CommandBuffer& commands) override;
            void resizeBackBuffer(UINT newWidth, UINT newHeight);
            bool uploadBuffer(ID3D11Buffer* buffer, const void* data, uint32_t dataSize);
            virtual void generateScreenshot(const std::string& filename) override;
            void main();

            virtual BlendStateResource* createBlendState() override;
            virtual BufferResource* createBuffer() override;
            virtual RenderTargetResource* createRenderTarget() override;
            virtual ShaderResource* createShader() override;
            virtual TextureResource* createTexture() override;

            IDXGIOutput* getOutput() const;

            ID3D11Device* device = nullptr;
            ID3D11DeviceContext* context = nullptr;
            IDXGISwapChain* swapChain = nullptr;
            IDXGIAdapter* adapter = nullptr;
            ID3D11Texture2D* backBuffer = nullptr;
            ID3D11RenderTargetView* renderTargetView = nullptr;
            std::map<SamplerStateDesc, ID3D11SamplerState*> samplerStates;
            ID3D11RasterizerState* rasterizerStates[12];
            ID3D11Texture2D* depthStencilTexture = nullptr;
            ID3D11DepthStencilView* depthStencilView = nullptr;
            ID3D11DepthStencilState* depthStencilStates[4];

            UINT frameBufferWidth = 0;
            UINT frameBufferHeight = 0;

            UINT swapInterval = 0;
            FLOAT frameBufferClearColor[4];

            std::atomic<bool> running;
            Thread renderThread;
        };
    } // namespace graphics
} // namespace ouzel

#endif

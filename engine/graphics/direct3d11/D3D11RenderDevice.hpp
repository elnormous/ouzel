// Copyright 2015-2019 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_GRAPHICS_D3D11RENDERDEVICE_HPP
#define OUZEL_GRAPHICS_D3D11RENDERDEVICE_HPP

#include "core/Setup.h"

#if OUZEL_COMPILE_DIRECT3D11

#include <atomic>
#include <map>
#include <memory>
#include <system_error>
#include <thread>
#include <vector>

#pragma push_macro("WIN32_LEAN_AND_MEAN")
#pragma push_macro("NOMINMAX")
#ifndef WIN32_LEAN_AND_MEAN
#  define WIN32_LEAN_AND_MEAN
#endif
#ifndef NOMINMAX
#  define NOMINMAX
#endif
#include <d3d11.h>
#pragma pop_macro("WIN32_LEAN_AND_MEAN")
#pragma pop_macro("NOMINMAX")

#include "graphics/RenderDevice.hpp"
#include "graphics/direct3d11/D3D11Pointer.hpp"
#include "graphics/direct3d11/D3D11Texture.hpp"
#include "utils/Thread.hpp"

namespace ouzel
{
    namespace graphics
    {
        namespace d3d11
        {
            const std::error_category& getErrorCategory() noexcept;

            class RenderDevice final: public graphics::RenderDevice
            {
                friend Renderer;
            public:
                explicit RenderDevice(const std::function<void(const Event&)>& initCallback);
                ~RenderDevice() override;

                std::vector<Size2U> getSupportedResolutions() const final;

                inline auto& getDevice() const noexcept { return device; }
                inline auto& getContext() const noexcept { return context; }

                ID3D11SamplerState* getSamplerState(const SamplerStateDesc& desc);
                void setFullscreen(bool newFullscreen);

                template <class T>
                inline auto getResource(uintptr_t id) const
                {
                    return id ? static_cast<T*>(resources[id - 1].get()) : nullptr;
                }

            private:
                void init(Window* newWindow,
                          const Size2U& newSize,
                          uint32_t newSampleCount,
                          SamplerFilter newTextureFilter,
                          uint32_t newMaxAnisotropy,
                          bool newSrgb,
                          bool newVerticalSync,
                          bool newDepth,
                          bool newStencil,
                          bool newDebugRenderer) final;

                void process() final;
                void resizeBackBuffer(UINT newWidth, UINT newHeight);
                void uploadBuffer(ID3D11Buffer* buffer, const void* data, uint32_t dataSize);
                void generateScreenshot(const std::string& filename) final;
                void renderMain();

                IDXGIOutput* getOutput() const;

                Pointer<ID3D11Device> device;
                Pointer<ID3D11DeviceContext> context;
                Pointer<IDXGISwapChain> swapChain;
                Pointer<IDXGIAdapter> adapter;
                Pointer<ID3D11Texture2D> backBuffer;
                Pointer<ID3D11RenderTargetView> renderTargetView;
                std::map<SamplerStateDesc, Pointer<ID3D11SamplerState>> samplerStates;
                Pointer<ID3D11RasterizerState> rasterizerStates[12];
                Pointer<ID3D11Texture2D> depthStencilTexture;
                Pointer<ID3D11DepthStencilView> depthStencilView;
                Pointer<ID3D11DepthStencilState> defaultDepthStencilState;

                UINT frameBufferWidth = 0;
                UINT frameBufferHeight = 0;

                UINT swapInterval = 0;

                std::atomic_bool running{false};
                Thread renderThread;

                std::vector<std::unique_ptr<RenderResource>> resources;
            };
        } // namespace d3d11
    } // namespace graphics
} // namespace ouzel

#endif

#endif // OUZEL_GRAPHICS_D3D11RENDERDEVICE_HPP

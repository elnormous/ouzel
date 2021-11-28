// Ouzel by Elviss Strazdins

#ifndef OUZEL_GRAPHICS_D3D11RENDERDEVICE_HPP
#define OUZEL_GRAPHICS_D3D11RENDERDEVICE_HPP

#include "../../core/Setup.h"

#if OUZEL_COMPILE_DIRECT3D11

#include <array>
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

#include "../RenderDevice.hpp"
#include "D3D11Pointer.hpp"
#include "D3D11Texture.hpp"
#include "../../thread/Thread.hpp"

namespace ouzel::graphics::d3d11
{
    const std::error_category& getErrorCategory() noexcept;

    class RenderDevice final: public graphics::RenderDevice
    {
        friend Graphics;
    public:
        RenderDevice(const Settings& settings,
                     core::Window& initWindow,
                     const std::function<void(const Event&)>& initCallback);
        ~RenderDevice() override;

        std::vector<math::Size<std::uint32_t, 2>> getSupportedResolutions() const final;
        void start() final {}

        auto& getDevice() const noexcept { return device; }
        auto& getContext() const noexcept { return context; }

        ID3D11SamplerState* getSamplerState(const SamplerStateDesc& desc);
        void setFullscreen(bool newFullscreen);

        template <class T>
        auto getResource(std::size_t id) const
        {
            return id ? static_cast<T*>(resources[id - 1].get()) : nullptr;
        }

    private:
        void process() final;
        void resizeBackBuffer(UINT newWidth, UINT newHeight);
        void uploadBuffer(ID3D11Buffer* buffer, const void* data, std::uint32_t dataSize);
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
        std::array<Pointer<ID3D11RasterizerState>, 12> rasterizerStates;
        Pointer<ID3D11Texture2D> depthStencilTexture;
        Pointer<ID3D11DepthStencilView> depthStencilView;
        Pointer<ID3D11DepthStencilState> defaultDepthStencilState;

        UINT frameBufferWidth = 0;
        UINT frameBufferHeight = 0;

        UINT swapInterval = 0;

        std::atomic_bool running{false};
        thread::Thread renderThread;

        std::vector<std::unique_ptr<RenderResource>> resources;
    };
}
#endif

#endif // OUZEL_GRAPHICS_D3D11RENDERDEVICE_HPP

// Ouzel by Elviss Strazdins

#ifndef OUZEL_GRAPHICS_D3D11RENDERTARGET_HPP
#define OUZEL_GRAPHICS_D3D11RENDERTARGET_HPP

#include "../../core/Setup.h"

#if OUZEL_COMPILE_DIRECT3D11

#include <set>
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

#include "D3D11RenderResource.hpp"
#include "../../math/Color.hpp"

namespace ouzel::graphics::d3d11
{
    class RenderDevice;
    class Texture;

    class RenderTarget final: public RenderResource
    {
    public:
        RenderTarget(RenderDevice& initRenderDevice,
                     const std::set<Texture*>& initColorTextures,
                     Texture* initDepthTexture);

        void resolve();

        auto& getRenderTargetViews() const noexcept { return renderTargetViews; }
        auto getDepthStencilView() const noexcept { return depthStencilView; }

    private:
        std::set<Texture*> colorTextures;
        Texture* depthTexture = nullptr;

        std::vector<ID3D11RenderTargetView*> renderTargetViews;
        ID3D11DepthStencilView* depthStencilView = nullptr;
    };
}
#endif

#endif // OUZEL_GRAPHICS_D3D11RENDERTARGET_HPP

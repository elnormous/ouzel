// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#include <algorithm>
#include "TextureResource.hpp"
#include "Renderer.hpp"
#include "RenderDevice.hpp"
#include "core/Engine.hpp"
#include "math/MathUtils.hpp"
#include "utils/Errors.hpp"

namespace ouzel
{
    namespace graphics
    {
        TextureResource::TextureResource(RenderDevice& initRenderDevice):
            RenderResource(initRenderDevice)
        {
        }

        TextureResource::~TextureResource()
        {
        }
    } // namespace graphics
} // namespace ouzel

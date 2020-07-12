// Copyright 2015-2020 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_GRAPHICS_RENDERER_RENDERER_HPP
#define OUZEL_GRAPHICS_RENDERER_RENDERER_HPP

#include <cstddef>

namespace ouzel::graphics::renderer
{
    class Renderer final
    {
    public:
        Renderer() = default;

    private:
        std::size_t lastResourceId = 0;
        std::set<std::size_t> deletedResourceIds;
    };
}

#endif // OUZEL_GRAPHICS_RENDERER_RENDERER_HPP

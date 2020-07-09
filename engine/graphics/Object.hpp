// Copyright 2015-2020 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_GRAPHICS_OBJECT_HPP
#define OUZEL_GRAPHICS_OBJECT_HPP

#include <memory>
#include "Material.hpp"

namespace ouzel::graphics
{
    class Object final
    {
    public:
        Object() = default;

        Object(const Object&) = delete;
        Object& operator=(const Object&) = delete;

        Object(Object&&) = delete;
        Object& operator=(Object&&) = delete;

        const Material* material = nullptr;
    };
}

#endif // OUZEL_GRAPHICS_OBJECT_HPP

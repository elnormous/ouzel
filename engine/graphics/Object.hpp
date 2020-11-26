// Copyright 2015-2020 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_GRAPHICS_OBJECT_HPP
#define OUZEL_GRAPHICS_OBJECT_HPP

#include <memory>
#include "Buffer.hpp"
#include "Material.hpp"
#include "../math/Matrix.hpp"

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

        const Buffer* indexBuffer = nullptr;
        const Buffer* vertexBuffer = nullptr;
        const Material* material = nullptr;
        Matrix4F transform;
    };
}

#endif // OUZEL_GRAPHICS_OBJECT_HPP

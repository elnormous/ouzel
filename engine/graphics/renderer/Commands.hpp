// Copyright 2015-2020 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_GRAPHICS_RENDERER_COMMANDS_HPP
#define OUZEL_GRAPHICS_RENDERER_COMMANDS_HPP

namespace ouzel::graphics::renderer
{
    using ResourceId = std::size_t;

    struct Command final
    {
        enum class Type
        {
            initCamera,
            setCameraAttributes,
            initLight,
            setLightAttributes,
            initMaterial,
            setMaterialAttributes,
            initObject,
            setObjectAttributes,
            destroyResource
        };

        Type type;
        ResourceId resourceId;
    };
}

#endif // OUZEL_GRAPHICS_RENDERER_COMMANDS_HPP

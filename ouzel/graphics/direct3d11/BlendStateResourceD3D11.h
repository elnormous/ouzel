// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include "core/CompileConfig.h"

#if OUZEL_SUPPORTS_DIRECT3D11

#include <d3d11.h>
#include "graphics/BlendStateResource.h"

namespace ouzel
{
    namespace graphics
    {
        class BlendStateResourceD3D11: public BlendStateResource
        {
        public:
            BlendStateResourceD3D11();
            virtual ~BlendStateResourceD3D11();

            ID3D11BlendState* getBlendState() const { return blendState; }

        protected:
            virtual bool upload() override;

            ID3D11BlendState* blendState = nullptr;
        };
    } // namespace graphics
} // namespace ouzel

#endif

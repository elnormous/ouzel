// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#define NOMINMAX
#include <d3d11.h>
#include "graphics/BlendStateResource.h"

namespace ouzel
{
    namespace graphics
    {
        class BlendStateD3D11: public BlendStateResource
        {
        public:
            BlendStateD3D11();
            virtual ~BlendStateD3D11();
            virtual void free() override;

            ID3D11BlendState* getBlendState() const { return blendState; }

        protected:
            virtual bool upload() override;

            ID3D11BlendState* blendState = nullptr;
        };
    } // namespace graphics
} // namespace ouzel

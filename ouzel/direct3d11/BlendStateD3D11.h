// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#define NOMINMAX
#include <d3d11.h>
#include "BlendState.h"

namespace ouzel
{
    namespace graphics
    {
        class RendererD3D11;

        class BlendStateD3D11: public BlendState
        {
            friend RendererD3D11;
        public:
            virtual ~BlendStateD3D11();
            virtual void free() override;

            virtual bool init(bool enableBlending,
                              BlendFactor colorBlendSource, BlendFactor colorBlendDest,
                              BlendOperation colorOperation,
                              BlendFactor alphaBlendSource, BlendFactor alphaBlendDest,
                              BlendOperation alphaOperation) override;

            static D3D11_BLEND getBlendFactor(BlendFactor blendFactor);
            static D3D11_BLEND_OP getBlendOperation(BlendOperation blendOperation);

            ID3D11BlendState* getBlendState() const { return blendState; }

        protected:
            BlendStateD3D11();

            ID3D11BlendState* blendState = nullptr;
        };
    } // namespace graphics
} // namespace ouzel

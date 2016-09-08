// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include "graphics/RenderTarget.h"

namespace ouzel
{
    namespace graphics
    {
        class RendererEmpty;

        class RenderTargetEmpty: public RenderTarget
        {
            friend RendererEmpty;
        public:
            virtual ~RenderTargetEmpty();

            virtual bool init(const Size2& newSize, bool depthBuffer) override;

        protected:
            RenderTargetEmpty();
            virtual bool upload() override;
        };
    } // namespace graphics
} // namespace ouzel

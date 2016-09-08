// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include "graphics/RenderTarget.h"

namespace ouzel
{
    namespace graphics
    {
        class RendererVoid;

        class RenderTargetVoid: public RenderTarget
        {
            friend RendererVoid;
        public:
            virtual ~RenderTargetVoid();

            virtual bool init(const Size2& newSize, bool depthBuffer) override;

        protected:
            RenderTargetVoid();
            virtual bool upload() override;
        };
    } // namespace graphics
} // namespace ouzel

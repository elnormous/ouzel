// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include "CompileConfig.h"
#include "RenderTarget.h"
#include "Size2.h"

namespace ouzel
{
    namespace video
    {
        class RendererMetal;

        class RenderTargetMetal: public RenderTarget
        {
            friend RendererMetal;
        public:
            virtual ~RenderTargetMetal();

            virtual bool init(const Size2& size, bool depthBuffer) override;

        protected:
            RenderTargetMetal();
        };
    } // namespace video
} // namespace ouzel

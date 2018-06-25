// Copyright (C) 2018 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

namespace ouzel
{
    namespace graphics
    {
        class RenderResource
        {
        public:
            RenderResource() {}
            virtual ~RenderResource() {}

            RenderResource(const RenderResource&) = delete;
            RenderResource& operator=(const RenderResource&) = delete;

            RenderResource(RenderResource&&) = delete;
            RenderResource& operator=(RenderResource&&) = delete;

            virtual void reload() {}
        };
    } // namespace graphics
} // namespace ouzel

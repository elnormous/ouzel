// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "RenderTargetEmpty.h"
#include "TextureEmpty.h"

namespace ouzel
{
    namespace graphics
    {
        RenderTargetEmpty::RenderTargetEmpty()
        {
        }

        RenderTargetEmpty::~RenderTargetEmpty()
        {
        }

        bool RenderTargetEmpty::upload()
        {
            uploadData.dirty = false;
            
            return true;
        }
    } // namespace graphics
} // namespace ouzel

// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include <memory>
#include <atomic>
#include "utils/Types.h"
#include "utils/Noncopyable.h"
#include "graphics/Resource.h"
#include "graphics/Color.h"
#include "math/Size2.h"

namespace ouzel
{
    namespace graphics
    {
        class Renderer;
        class Texture;

        class RenderTarget: public Resource, public Noncopyable
        {
            friend Renderer;
        public:
            virtual ~RenderTarget();
            virtual void free() override;

            virtual bool init(const Size2& newSize, bool useDepthBuffer);

            void setClear(bool newClear) { clear = newClear; }
            bool getClear() const { return clear; }

            virtual void setClearColor(Color color) { clearColor = color; }
            virtual Color getClearColor() const { return clearColor; }

            void setFrameBufferClearedFrame(uint32_t clearedFrame) { frameBufferClearedFrame = clearedFrame; }
            uint32_t getFrameBufferClearedFrame() const { return frameBufferClearedFrame; }

            TexturePtr getTexture() const { return texture; }

            bool isReady() const { return ready; }

        protected:
            RenderTarget();
            virtual void update() override;

            Size2 size;
            Color clearColor;
            TexturePtr texture;
            uint32_t frameBufferClearedFrame = 0;

            std::atomic<bool> clear;
            bool depthBuffer = false;
            bool ready = false;
        };
    } // namespace graphics
} // namespace ouzel

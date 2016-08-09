// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include <mutex>
#include <atomic>
#define NOMINMAX
#include <d3d11.h>
#include "graphics/Texture.h"

namespace ouzel
{
    namespace graphics
    {
        class RendererD3D11;
        class RenderTargetD3D11;

        class TextureD3D11: public Texture
        {
            friend RendererD3D11;
            friend RenderTargetD3D11;
        public:
            virtual ~TextureD3D11();
            virtual void free() override;

            virtual bool init(const Size2& newSize, bool newDynamic, bool newMipmaps = true, bool newRenderTarget = false) override;
            virtual bool initFromBuffer(const std::vector<uint8_t>& newData, const Size2& newSize, bool newDynamic, bool newMipmaps = true) override;

            virtual bool upload(const std::vector<uint8_t>& newData, const Size2& newSize) override;

            ID3D11Texture2D* getTexture() const { return texture; }
            ID3D11ShaderResourceView* getResourceView() const { return resourceView; }

        protected:
            TextureD3D11();
            virtual bool update() override;

            virtual bool uploadData(const std::vector<uint8_t>& newData, const Size2& newSize) override;
            virtual bool uploadMipmap(uint32_t level, const Size2& mipMapSize, const std::vector<uint8_t>& newData) override;

            ID3D11Texture2D* texture = nullptr;
            ID3D11ShaderResourceView* resourceView = nullptr;

            UINT width = 0;
            UINT height = 0;

            struct Data
            {
                UINT width = 0;
                UINT height = 0;
                std::vector<uint8_t> data;
            };

            std::vector<Data> data;
            std::atomic<bool> dirty;
            std::mutex dataMutex;
        };
    } // namespace graphics
} // namespace ouzel

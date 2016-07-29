// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include <vector>
#include <string>
#include <unordered_map>
#include <memory>
#include "utils/Types.h"
#include "utils/Noncopyable.h"
#include "math/Rectangle.h"
#include "math/Matrix4.h"
#include "math/Size2.h"
#include "graphics/Color.h"
#include "math/AABB2.h"
#include "graphics/Vertex.h"
#include "graphics/Shader.h"
#include "graphics/BlendState.h"

namespace ouzel
{
    class Engine;

    namespace graphics
    {
        const std::string SHADER_TEXTURE = "shaderTexture";
        const std::string SHADER_COLOR = "shaderColor";

        const std::string BLEND_NO_BLEND = "blendNoBlend";
        const std::string BLEND_ADD = "blendAdd";
        const std::string BLEND_MULTIPLY = "blendMultiply";
        const std::string BLEND_ALPHA = "blendAlpha";

        class MeshBuffer;

        class Renderer: public Noncopyable
        {
            friend Engine;
            friend Window;
        public:
            static const uint32_t TEXTURE_LAYERS = 2;

            enum class Driver
            {
                DEFAULT,
                NONE,
                OPENGL,
                DIRECT3D11,
                METAL
            };

            enum class TextureFiltering
            {
                NONE,
                LINEAR,
                BILINEAR,
                TRILINEAR
            };

            enum class DrawMode
            {
                POINT_LIST,
                LINE_LIST,
                LINE_STRIP,
                TRIANGLE_LIST,
                TRIANGLE_STRIP
            };

            virtual ~Renderer() = 0;
            virtual void free();

            Driver getDriver() const { return driver; }

            virtual void setClearColor(Color color) { clearColor = color; }
            virtual Color getClearColor() const { return clearColor; }

            virtual void clear();
            virtual void present();
            virtual void flush();

            const Size2& getSize() const { return size; }
            uint32_t getSampleCount() const { return sampleCount; }
            TextureFiltering getTextureFiltering() const { return textureFiltering; }

            virtual std::vector<Size2> getSupportedResolutions() const = 0;

            virtual BlendStatePtr createBlendState();
            virtual bool activateBlendState(BlendStatePtr blendState);

            virtual TexturePtr createTexture();
            virtual bool activateTexture(const TexturePtr& texture, uint32_t layer);
            virtual TexturePtr getActiveTexture(uint32_t layer) const { return activeTextures[layer]; }
            virtual RenderTargetPtr createRenderTarget();
            virtual bool activateRenderTarget(const RenderTargetPtr& renderTarget);

            virtual ShaderPtr createShader();
            virtual bool activateShader(const ShaderPtr& shader);
            virtual ShaderPtr getActiveShader() const { return activeShader; }

            virtual MeshBufferPtr createMeshBuffer();
            virtual bool drawMeshBuffer(const MeshBufferPtr& meshBuffer, uint32_t indexCount = 0, DrawMode drawMode = DrawMode::TRIANGLE_LIST, uint32_t startIndex = 0);

            virtual void activateScissorTest(const Rectangle& rectangle);

            Vector2 viewToScreenLocation(const Vector2& position);
            Vector2 viewToScreenRelativeLocation(const Vector2& position);
            Vector2 screenToViewLocation(const Vector2& position);
            bool checkVisibility(const Matrix4& transform, const AABB2& boundingBox, const scene::CameraPtr& camera);

            virtual bool saveScreenshot(const std::string& filename) = 0;

            virtual uint32_t getDrawCallCount() const { return drawCallCount; }

            uint32_t getAPIVersion() const { return apiVersion; }
            void setAPIVersion(uint32_t version) { apiVersion = version; }

            bool isReady() const { return ready; }

        protected:
            Renderer(Driver pDriver = Driver::NONE);
            virtual bool init(const WindowPtr& window,
                              uint32_t newSampleCount,
                              TextureFiltering newTextureFiltering,
                              float newTargetFPS,
                              bool newVerticalSync);

            virtual void setSize(const Size2& newSize);
            virtual void setFullscreen(bool newFullscreen);

            Driver driver;
            Size2 size;
            bool fullscreen = false;
            uint32_t sampleCount = 1; // MSAA sample count
            TextureFiltering textureFiltering = TextureFiltering::NONE;
            float targetFPS = 0.0f;
            bool verticalSync = true;

            Color clearColor;

            BlendStatePtr activeBlendState;
            TexturePtr activeTextures[TEXTURE_LAYERS];
            ShaderPtr activeShader;
            RenderTargetPtr activeRenderTarget;

            uint32_t drawCallCount = 0;
            Rectangle scissorTest;

            uint32_t apiVersion = 0;

            bool ready = false;
        };
    } // namespace graphics
} // namespace ouzel

// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "Renderer.h"
#include "core/Engine.h"
#include "Texture.h"
#include "utils/Utils.h"
#include "Shader.h"
#include "scene/Camera.h"
#include "events/EventHandler.h"
#include "scene/SceneManager.h"
#include "MeshBuffer.h"
#include "events/EventDispatcher.h"
#include "RenderTarget.h"
#include "BlendState.h"
#include "core/Window.h"

namespace ouzel
{
    namespace graphics
    {
        Renderer::Renderer(Driver pDriver):
            driver(pDriver)
        {

        }

        Renderer::~Renderer()
        {

        }

        void Renderer::free()
        {
            ready = false;
        }

        bool Renderer::init(const WindowPtr& window,
                            uint32_t newSampleCount,
                            TextureFiltering newTextureFiltering,
                            float newTargetFPS,
                            bool newVerticalSync)
        {
            size = window->getSize();
            fullscreen = window->isFullscreen();
            sampleCount = newSampleCount;
            textureFiltering = newTextureFiltering;
            targetFPS = newTargetFPS;
            verticalSync = newVerticalSync;

            ready = true;

            return true;
        }

        void Renderer::clear()
        {
            drawCallCount = 0;
        }

        bool Renderer::present()
        {
            drawCallCount = drawQueue.size();

            return true;
        }

        void Renderer::flush()
        {
        }

        void Renderer::setSize(const Size2& newSize)
        {
            size = newSize;

            sharedEngine->getSceneManager()->recalculateProjection();
        }

        void Renderer::setFullscreen(bool newFullscreen)
        {
            fullscreen = newFullscreen;
        }

        BlendStatePtr Renderer::createBlendState()
        {
            BlendStatePtr blendState(new BlendState());
            return blendState;
        }

        TexturePtr Renderer::createTexture()
        {
            TexturePtr texture(new Texture());
            return texture;
        }

        RenderTargetPtr Renderer::createRenderTarget()
        {
            RenderTargetPtr renderTarget(new RenderTarget());
            return renderTarget;
        }

        ShaderPtr Renderer::createShader()
        {
            ShaderPtr shader(new Shader());
            return shader;
        }

        MeshBufferPtr Renderer::createMeshBuffer()
        {
            MeshBufferPtr meshBuffer(new MeshBuffer());
            return meshBuffer;
        }

        bool Renderer::addDrawCommand(const std::vector<TexturePtr>& textures,
                                      const ShaderPtr& shader,
                                      const std::vector<std::vector<float>>& pixelShaderConstants,
                                      const std::vector<std::vector<float>>& vertexShaderConstants,
                                      const BlendStatePtr& blendState,
                                      const MeshBufferPtr& meshBuffer,
                                      uint32_t indexCount,
                                      DrawMode drawMode,
                                      uint32_t startIndex,
                                      const RenderTargetPtr& renderTarget,
                                      bool scissorTestEnabled,
                                      const Rectangle& scissorTest)
        {
            tempDrawQueue.push({
                textures,
                shader,
                pixelShaderConstants,
                vertexShaderConstants,
                blendState,
                meshBuffer,
                indexCount,
                drawMode,
                startIndex,
                renderTarget,
                scissorTestEnabled,
                scissorTest
            });

            return true;
        }

        void Renderer::flushDrawCommands()
        {
            std::lock_guard<std::mutex> lock(drawQueueMutex);
            drawQueue = std::move(tempDrawQueue);
        }

        Vector2 Renderer::viewToScreenLocation(const Vector2& position)
        {
            float x = 2.0f * position.x / size.width - 1.0f;
            float y = 2.0f * (size.height - position.y) / size.height - 1.0f;

            return Vector2(x, y);
        }

        Vector2 Renderer::viewToScreenRelativeLocation(const Vector2& position)
        {
            float x = 2.0f * position.x / size.width;
            float y = -2.0f * position.y / size.height;

            return Vector2(x, y);
        }

        Vector2 Renderer::screenToViewLocation(const Vector2& position)
        {
            float x = (position.x + 1.0f) / 2.0f * size.width;
            float y = size.height - (position.y + 1.0f) / 2.0f * size.height;

            return Vector2(x, y);
        }

        bool Renderer::checkVisibility(const Matrix4& transform, const AABB2& boundingBox, const scene::CameraPtr& camera)
        {
            Rectangle visibleRect(0.0f, 0.0f, size.width, size.height);

            // transform center point to screen space
            Vector2 diff = boundingBox.max - boundingBox.min;

            Vector3 v3p(boundingBox.min.x + diff.x / 2.0f, boundingBox.min.y + diff.y / 2.0f, 0.0f);
            diff *= camera->getZoom();
            diff.x *= camera->getContentScale().x;
            diff.y *= camera->getContentScale().y;

            transform.transformPoint(v3p);

            Vector2 v2p = camera->projectPoint(v3p);

            Size2 halfSize(diff.x / 2.0f, diff.y / 2.0f);

            // convert content size to world coordinates
            Size2 halfWorldSize;

            halfWorldSize.width = std::max(fabsf(halfSize.width * transform.m[0] + halfSize.height * transform.m[4]), fabsf(halfSize.width * transform.m[0] - halfSize.height * transform.m[4]));
            halfWorldSize.height = std::max(fabsf(halfSize.width * transform.m[1] + halfSize.height * transform.m[5]), fabsf(halfSize.width * transform.m[1] - halfSize.height * transform.m[5]));

            // enlarge visible rect half size in screen coord
            visibleRect.x -= halfWorldSize.width;
            visibleRect.y -= halfWorldSize.height;
            visibleRect.width += halfWorldSize.width * 2.0f;
            visibleRect.height += halfWorldSize.height * 2.0f;

            return visibleRect.containsPoint(v2p);
        }
    } // namespace graphics
} // namespace ouzel

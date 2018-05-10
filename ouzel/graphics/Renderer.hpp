// Copyright (C) 2018 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include <cstdint>
#include <functional>
#include <memory>
#include <string>
#include <vector>
#include <queue>
#include <set>
#include <atomic>
#include "math/Rect.hpp"
#include "math/Matrix4.hpp"
#include "math/Size2.hpp"
#include "math/Color.hpp"
#include "graphics/Texture.hpp"

namespace ouzel
{
    class Engine;
    class Window;

    namespace graphics
    {
        const std::string SHADER_TEXTURE = "shaderTexture";
        const std::string SHADER_COLOR = "shaderColor";

        const std::string BLEND_NO_BLEND = "blendNoBlend";
        const std::string BLEND_ADD = "blendAdd";
        const std::string BLEND_MULTIPLY = "blendMultiply";
        const std::string BLEND_ALPHA = "blendAlpha";
        const std::string BLEND_SCREEN = "blendScreen";

        const std::string TEXTURE_WHITE_PIXEL = "textureWhitePixel";

        class RenderDevice;
        class BlendState;
        class MeshBuffer;
        class Shader;

        class Renderer final
        {
            friend Engine;
            friend Window;
        public:
            enum class Driver
            {
                DEFAULT,
                EMPTY,
                OPENGL,
                DIRECT3D11,
                METAL
            };

            enum class DrawMode
            {
                POINT_LIST,
                LINE_LIST,
                LINE_STRIP,
                TRIANGLE_LIST,
                TRIANGLE_STRIP
            };

            enum class CullMode
            {
                NONE,
                FRONT,
                BACK
            };

            enum class FillMode
            {
                SOLID,
                WIREFRAME
            };

            ~Renderer();

            Renderer(const Renderer&) = delete;
            Renderer& operator=(const Renderer&) = delete;

            Renderer(Renderer&&) = delete;
            Renderer& operator=(Renderer&&) = delete;

            static std::set<Driver> getAvailableRenderDrivers();

            inline RenderDevice* getDevice() const { return device.get(); }

            void executeOnRenderThread(const std::function<void(void)>& func);

            void setClearColorBuffer(bool clear);
            inline bool getClearColorBuffer() const { return clearColorBuffer; }

            void setClearDepthBuffer(bool clear);
            inline bool getClearDepthBuffer() const { return clearDepthBuffer; }

            void setClearColor(Color color);
            inline Color getClearColor() const { return clearColor; }

            void setClearDepth(float newClearDepth);
            inline float getClearDepth() const { return clearDepth; }

            inline const Size2& getSize() const { return size; }

            bool saveScreenshot(const std::string& filename);

            bool addSetRenderTargetCommand(const std::shared_ptr<Texture>& renderTarget);
            bool addClearCommand(const std::shared_ptr<Texture>& renderTarget);
            bool addSetCullModeCommad(Renderer::CullMode cullMode);
            bool addSetFillModeCommad(Renderer::FillMode fillMode);
            bool addSetScissorTestCommand(bool enabled, const Rect& rectangle);
            bool addSetViewportCommand(const Rect& viewport);
            bool addSetDepthStateCommand(bool depthTest, bool depthWrite);
            bool addDrawCommand(const std::vector<std::shared_ptr<Texture>>& textures,
                                const std::vector<std::vector<float>>& pixelShaderConstants,
                                const std::vector<std::vector<float>>& vertexShaderConstants,
                                const std::shared_ptr<MeshBuffer>& meshBuffer,
                                uint32_t indexCount,
                                DrawMode drawMode,
                                uint32_t startIndex);
            bool addPushDebugMarkerCommand(const std::string& name);
            bool addPopDebugMarkerCommand();
            bool addSetBlendStateCommand(const std::shared_ptr<BlendState>& blendState);
            bool addSetShaderCommand(const std::shared_ptr<Shader>& shader);

        protected:
            explicit Renderer(Driver driver);
            bool init(Window* newWindow,
                      const Size2& newSize,
                      uint32_t newSampleCount,
                      Texture::Filter newTextureFilter,
                      uint32_t newMaxAnisotropy,
                      bool newVerticalSync,
                      bool newDepth,
                      bool newDebugRenderer);

            void setSize(const Size2& newSize);

            std::unique_ptr<RenderDevice> device;

            Size2 size;
            Color clearColor;
            float clearDepth = 1.0;
            bool clearColorBuffer = true;
            bool clearDepthBuffer = false;
        };
    } // namespace graphics
} // namespace ouzel

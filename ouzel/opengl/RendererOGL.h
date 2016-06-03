// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include <set>
#include "core/CompileConfig.h"
#include "graphics/Renderer.h"

#if defined(OUZEL_PLATFORM_OSX)
    #include <OpenGL/gl3.h>
#elif defined(OUZEL_PLATFORM_IOS) || defined(OUZEL_PLATFORM_TVOS)
    #include <OpenGLES/ES2/gl.h>
    #include <OpenGLES/ES2/glext.h>
#elif defined(OUZEL_PLATFORM_ANDROID)
    #include <GLES2/gl2platform.h>
    #ifndef GL_GLEXT_PROTOTYPES
        #define GL_GLEXT_PROTOTYPES 1
    #endif
    #include <GLES2/gl2.h>
    #include <GLES2/gl2ext.h>
    #include <EGL/egl.h>
#elif defined(OUZEL_PLATFORM_LINUX)
    #ifndef GL_GLEXT_PROTOTYPES
        #define GL_GLEXT_PROTOTYPES 1
    #endif
    #include <GL/gl.h>
    #include <GL/glx.h>
    #include <GL/glext.h>
#elif defined(OUZEL_PLATFORM_RASPBERRY_PI)
    #ifndef GL_GLEXT_PROTOTYPES
        #define GL_GLEXT_PROTOTYPES 1
    #endif
    #include <GLES2/gl2.h>
    #include <GLES2/gl2ext.h>
    #include <EGL/egl.h>
#endif

#if defined(OUZEL_PLATFORM_ANDROID) || defined(OUZEL_PLATFORM_RASPBERRY_PI)
extern PFNGLGENVERTEXARRAYSOESPROC glGenVertexArraysOESEXT;
extern PFNGLBINDVERTEXARRAYOESPROC glBindVertexArrayOESEXT;
extern PFNGLDELETEVERTEXARRAYSOESPROC glDeleteVertexArraysOESEXT;
#endif

namespace ouzel
{
    class Engine;
    
    namespace graphics
    {
        class RendererOGL: public Renderer
        {
            friend Engine;
        public:
            virtual ~RendererOGL();

            static bool checkOpenGLErrors(bool logError = true);

            virtual void setClearColor(Color color) override;

            virtual void clear() override;
            virtual void present() override;
            virtual void flush() override;

            virtual std::vector<Size2> getSupportedResolutions() const override;

            virtual BlendStatePtr createBlendState(bool enableBlending,
                                                   BlendState::BlendFactor colorBlendSource, BlendState::BlendFactor colorBlendDest,
                                                   BlendState::BlendOperation colorOperation,
                                                   BlendState::BlendFactor alphaBlendSource, BlendState::BlendFactor alphaBlendDest,
                                                   BlendState::BlendOperation alphaOperation) override;
            virtual bool activateBlendState(BlendStatePtr blendState) override;

            virtual TexturePtr createTexture(const Size2& size, bool dynamic, bool mipmaps = true) override;
            virtual TexturePtr loadTextureFromFile(const std::string& filename, bool dynamic, bool mipmaps = true) override;
            virtual TexturePtr loadTextureFromData(const void* data, const Size2& size, bool dynamic, bool mipmaps = true) override;
            virtual bool activateTexture(const TexturePtr& texture, uint32_t layer) override;
            virtual RenderTargetPtr createRenderTarget(const Size2& size, bool depthBuffer) override;
            virtual bool activateRenderTarget(const RenderTargetPtr& renderTarget) override;

            virtual ShaderPtr loadShaderFromFiles(const std::string& pixelShader,
                                                  const std::string& vertexShader,
                                                  uint32_t vertexAttributes,
                                                  const std::string& pixelShaderFunction = "",
                                                  const std::string& vertexShaderFunction = "") override;
            virtual ShaderPtr loadShaderFromBuffers(const uint8_t* pixelShader,
                                                    uint32_t pixelShaderSize,
                                                    const uint8_t* vertexShader,
                                                    uint32_t vertexShaderSize,
                                                    uint32_t vertexAttributes,
                                                    const std::string& pixelShaderFunction = "",
                                                    const std::string& vertexShaderFunction = "") override;
            virtual bool activateShader(const ShaderPtr& shader) override;

            virtual MeshBufferPtr createMeshBuffer() override;
            virtual MeshBufferPtr createMeshBufferFromData(const void* indices, uint32_t indexSize, uint32_t indexCount, bool dynamicIndexBuffer, const void* vertices, uint32_t vertexAttributes, uint32_t vertexCount, bool dynamicVertexBuffer) override;
            virtual bool drawMeshBuffer(const MeshBufferPtr& meshBuffer, uint32_t indexCount = 0, DrawMode drawMode = DrawMode::TRIANGLE_LIST, uint32_t startIndex = 0) override;

            virtual bool saveScreenshot(const std::string& filename) override;

            static bool bindTexture(GLuint textureId, uint32_t layer);
            static bool bindProgram(GLuint programId);
            static bool bindFrameBuffer(GLuint frameBufferId);
            static bool bindElementArrayBuffer(GLuint elementArrayBufferId);
            static bool bindArrayBuffer(GLuint arrayBufferId);
            static bool bindVertexArray(GLuint vertexArrayId);

            static bool unbindTexture(GLuint textureId);
            static bool unbindProgram(GLuint programId);
            static bool unbindFrameBuffer(GLuint frameBufferId);
            static bool unbindElementArrayBuffer(GLuint elementArrayBufferId);
            static bool unbindArrayBuffer(GLuint arrayBufferId);
            static bool unbindVertexArray(GLuint vertexArrayId);

        protected:
            RendererOGL();

            virtual bool init(const WindowPtr& window,
                              uint32_t newSampleCount,
                              TextureFiltering newTextureFiltering,
                              float newTargetFPS,
                              bool newVerticalSync) override;

            virtual void setSize(const Size2& newSize) override;

            GLuint frameBufferId = 0;
            std::set<GLuint> clearedFrameBuffers;

            static GLuint currentTextureId[TEXTURE_LAYERS];
            static GLuint currentProgramId;
            static GLuint currentFrameBufferId;
            static GLuint currentElementArrayBufferId;
            static GLuint currentArrayBufferId;
            static GLuint currentVertexArrayId;

            Rectangle viewport;
        };
    } // namespace graphics
} // namespace ouzel

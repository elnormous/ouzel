// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include <set>
#include <queue>
#include <utility>
#include <mutex>
#include "core/CompileConfig.h"

#if OUZEL_PLATFORM_MACOS
    #include <OpenGL/gl3.h>
#elif OUZEL_PLATFORM_IOS || OUZEL_PLATFORM_TVOS
    #include <OpenGLES/ES2/gl.h>
    #include <OpenGLES/ES2/glext.h>
#elif OUZEL_PLATFORM_ANDROID
    #include <GLES2/gl2platform.h>
    #ifndef GL_GLEXT_PROTOTYPES
        #define GL_GLEXT_PROTOTYPES 1
    #endif
    #include <GLES2/gl2.h>
    #include <GLES2/gl2ext.h>
    #include <EGL/egl.h>
#elif OUZEL_PLATFORM_LINUX
    #ifndef GL_GLEXT_PROTOTYPES
        #define GL_GLEXT_PROTOTYPES 1
    #endif
    #include <GL/gl.h>
    #include <GL/glx.h>
    #include <GL/glext.h>
#elif OUZEL_PLATFORM_RASPBIAN
    #ifndef GL_GLEXT_PROTOTYPES
        #define GL_GLEXT_PROTOTYPES 1
    #endif
    #include <GLES2/gl2.h>
    #include <GLES2/gl2ext.h>
    #include <EGL/egl.h>
#endif

#if OUZEL_PLATFORM_ANDROID || OUZEL_PLATFORM_RASPBIAN
extern PFNGLGENVERTEXARRAYSOESPROC glGenVertexArraysOESEXT;
extern PFNGLBINDVERTEXARRAYOESPROC glBindVertexArrayOESEXT;
extern PFNGLDELETEVERTEXARRAYSOESPROC glDeleteVertexArraysOESEXT;
#endif

#include "graphics/Renderer.h"

namespace ouzel
{
    class Engine;
    
    namespace graphics
    {
        class RendererOGL: public Renderer
        {
            friend Engine;
        public:
            static bool checkOpenGLError(bool logError = true);
            
            virtual ~RendererOGL();

            virtual void setClearColor(Color color) override;

            virtual void clear() override;
            virtual bool present() override;
            virtual void flush() override;

            virtual std::vector<Size2> getSupportedResolutions() const override;

            virtual BlendStatePtr createBlendState() override;
            virtual TexturePtr createTexture() override;
            virtual RenderTargetPtr createRenderTarget() override;
            virtual ShaderPtr createShader() override;
            virtual MeshBufferPtr createMeshBuffer() override;

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

            enum class ResourceType
            {
                Buffer,
                VertexArray,
                RenderBuffer,
                FrameBuffer,
                Program,
                Shader,
                Texture
            };
            void deleteResource(GLuint resource, ResourceType resourceType);

        protected:
            RendererOGL();

            virtual bool init(const WindowPtr& window,
                              uint32_t newSampleCount,
                              TextureFiltering newTextureFiltering,
                              float newTargetFPS,
                              bool newVerticalSync) override;

            virtual void setSize(const Size2& newSize) override;

            GLuint frameBufferId = 0;

            static GLuint currentTextureId[TEXTURE_LAYERS];
            static GLuint currentProgramId;
            static GLuint currentFrameBufferId;
            static GLuint currentElementArrayBufferId;
            static GLuint currentArrayBufferId;
            static GLuint currentVertexArrayId;

            Rectangle viewport;

            std::queue<std::pair<GLuint, ResourceType>> deleteQueue;
            std::mutex deleteMutex;
        };
    } // namespace graphics
} // namespace ouzel

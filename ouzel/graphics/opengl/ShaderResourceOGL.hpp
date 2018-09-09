// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#pragma once

#include "core/Setup.h"

#if OUZEL_COMPILE_OPENGL

#if OUZEL_SUPPORTS_OPENGLES
#  define GL_GLEXT_PROTOTYPES 1
#  include "GLES/gl.h"
#  include "GLES2/gl2.h"
#  include "GLES2/gl2ext.h"
#  include "GLES3/gl3.h"
#else
#  define GL_GLEXT_PROTOTYPES 1
#  include "GL/glcorearb.h"
#  include "GL/glext.h"
#endif

#include "graphics/opengl/RenderResourceOGL.hpp"
#include "graphics/Shader.hpp"

namespace ouzel
{
    namespace graphics
    {
        class RenderDeviceOGL;

        class ShaderResourceOGL: public RenderResourceOGL
        {
        public:
            explicit ShaderResourceOGL(RenderDeviceOGL& renderDeviceOGL);
            virtual ~ShaderResourceOGL();

            void init(const std::vector<uint8_t>& newFragmentShader,
                      const std::vector<uint8_t>& newVertexShader,
                      const std::set<Vertex::Attribute::Usage>& newVertexAttributes,
                      const std::vector<Shader::ConstantInfo>& newFragmentShaderConstantInfo,
                      const std::vector<Shader::ConstantInfo>& newVertexShaderConstantInfo,
                      uint32_t newFragmentShaderDataAlignment = 0,
                      uint32_t newVertexShaderDataAlignment = 0,
                      const std::string& newFragmentShaderFunction = "",
                      const std::string& newVertexShaderFunction = "");

            virtual void reload() override;

            struct Location
            {
                GLint location;
                DataType dataType;
            };

            inline const std::set<Vertex::Attribute::Usage>& getVertexAttributes() const { return vertexAttributes; }

            inline uint32_t getFragmentShaderAlignment() const { return fragmentShaderAlignment; }
            inline uint32_t getVertexShaderAlignment() const { return vertexShaderAlignment; }

            inline const std::vector<Location>& getFragmentShaderConstantLocations() const { return fragmentShaderConstantLocations; }
            inline const std::vector<Location>& getVertexShaderConstantLocations() const { return vertexShaderConstantLocations; }

            inline GLuint getProgramId() const { return programId; }

        private:
            void compileShader();
            std::string getShaderMessage(GLuint shaderId);
            std::string getProgramMessage();

            std::set<Vertex::Attribute::Usage> vertexAttributes;

            std::vector<uint8_t> fragmentShaderData;
            std::vector<uint8_t> vertexShaderData;
            std::string fragmentShaderFunction;
            std::string vertexShaderFunction;

            std::vector<Shader::ConstantInfo> fragmentShaderConstantInfo;
            uint32_t fragmentShaderAlignment = 0;
            std::vector<Shader::ConstantInfo> vertexShaderConstantInfo;
            uint32_t vertexShaderAlignment = 0;

            GLuint fragmentShaderId = 0;
            GLuint vertexShaderId = 0;
            GLuint programId = 0;

            std::vector<Location> fragmentShaderConstantLocations;
            std::vector<Location> vertexShaderConstantLocations;
        };
    } // namespace graphics
} // namespace ouzel

#endif

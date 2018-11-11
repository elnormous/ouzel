// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_GRAPHICS_SHADERRESOURCEOGL_HPP
#define OUZEL_GRAPHICS_SHADERRESOURCEOGL_HPP

#include "core/Setup.h"

#if OUZEL_COMPILE_OPENGL

#if OUZEL_SUPPORTS_OPENGLES
#  include "GLES/gl.h"
#  include "GLES2/gl2.h"
#  include "GLES2/gl2ext.h"
#  include "GLES3/gl3.h"
#else
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

        class ShaderResourceOGL final: public RenderResourceOGL
        {
        public:
            ShaderResourceOGL(RenderDeviceOGL& renderDeviceOGL,
                              const std::vector<uint8_t>& newFragmentShader,
                              const std::vector<uint8_t>& newVertexShader,
                              const std::set<Vertex::Attribute::Usage>& newVertexAttributes,
                              const std::vector<Shader::ConstantInfo>& newFragmentShaderConstantInfo,
                              const std::vector<Shader::ConstantInfo>& newVertexShaderConstantInfo,
                              uint32_t newFragmentShaderDataAlignment,
                              uint32_t newVertexShaderDataAlignment,
                              const std::string& fragmentShaderFunction,
                              const std::string& vertexShaderFunction);
            ~ShaderResourceOGL();

            void reload() override;

            struct Location final
            {
                GLint location;
                DataType dataType;
            };

            inline const std::set<Vertex::Attribute::Usage>& getVertexAttributes() const { return vertexAttributes; }

            inline const std::vector<Location>& getFragmentShaderConstantLocations() const { return fragmentShaderConstantLocations; }
            inline const std::vector<Location>& getVertexShaderConstantLocations() const { return vertexShaderConstantLocations; }

            inline GLuint getProgramId() const { return programId; }

        private:
            void compileShader();
            std::string getShaderMessage(GLuint shaderId);
            std::string getProgramMessage();

            std::vector<uint8_t> fragmentShaderData;
            std::vector<uint8_t> vertexShaderData;

            std::set<Vertex::Attribute::Usage> vertexAttributes;

            std::vector<Shader::ConstantInfo> fragmentShaderConstantInfo;
            std::vector<Shader::ConstantInfo> vertexShaderConstantInfo;

            GLuint fragmentShaderId = 0;
            GLuint vertexShaderId = 0;
            GLuint programId = 0;

            std::vector<Location> fragmentShaderConstantLocations;
            std::vector<Location> vertexShaderConstantLocations;
        };
    } // namespace graphics
} // namespace ouzel

#endif

#endif // OUZEL_GRAPHICS_SHADERRESOURCEOGL_HPP

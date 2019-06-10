// Copyright 2015-2019 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_GRAPHICS_OGLSHADER_HPP
#define OUZEL_GRAPHICS_OGLSHADER_HPP

#include "core/Setup.h"

#if OUZEL_COMPILE_OPENGL

#include "graphics/opengl/OGL.h"

#if OUZEL_OPENGLES
#  include "GLES/gl.h"
#  include "GLES2/gl2.h"
#  include "GLES2/gl2ext.h"
#  include "GLES3/gl3.h"
#else
#  include "GL/glcorearb.h"
#  include "GL/glext.h"
#endif

#include "graphics/opengl/OGLRenderResource.hpp"
#include "graphics/Shader.hpp"

namespace ouzel
{
    namespace graphics
    {
        namespace opengl
        {
            class RenderDevice;

            class Shader final: public RenderResource
            {
            public:
                Shader(RenderDevice& renderDevice,
                       const std::vector<uint8_t>& newFragmentShader,
                       const std::vector<uint8_t>& newVertexShader,
                       const std::set<Vertex::Attribute::Usage>& newVertexAttributes,
                       const std::vector<graphics::Shader::ConstantInfo>& newFragmentShaderConstantInfo,
                       const std::vector<graphics::Shader::ConstantInfo>& newVertexShaderConstantInfo,
                       uint32_t newFragmentShaderDataAlignment,
                       uint32_t newVertexShaderDataAlignment,
                       const std::string& fragmentShaderFunction,
                       const std::string& vertexShaderFunction);
                ~Shader();

                void reload() final;

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
                std::string getShaderMessage(GLuint shaderId) const;
                std::string getProgramMessage() const;

                std::vector<uint8_t> fragmentShaderData;
                std::vector<uint8_t> vertexShaderData;

                std::set<Vertex::Attribute::Usage> vertexAttributes;

                std::vector<graphics::Shader::ConstantInfo> fragmentShaderConstantInfo;
                std::vector<graphics::Shader::ConstantInfo> vertexShaderConstantInfo;

                GLuint fragmentShaderId = 0;
                GLuint vertexShaderId = 0;
                GLuint programId = 0;

                std::vector<Location> fragmentShaderConstantLocations;
                std::vector<Location> vertexShaderConstantLocations;
            };
        } // namespace opengl
    } // namespace graphics
} // namespace ouzel

#endif

#endif // OUZEL_GRAPHICS_OGLSHADER_HPP

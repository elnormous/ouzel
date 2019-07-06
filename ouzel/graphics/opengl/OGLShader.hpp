// Copyright 2015-2019 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_GRAPHICS_OGLSHADER_HPP
#define OUZEL_GRAPHICS_OGLSHADER_HPP

#include <utility>

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
#include "graphics/DataType.hpp"
#include "graphics/Vertex.hpp"

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
                Shader(RenderDevice& initRenderDevice,
                       const std::vector<uint8_t>& initFragmentShader,
                       const std::vector<uint8_t>& initVertexShader,
                       const std::set<Vertex::Attribute::Usage>& initVertexAttributes,
                       const std::vector<std::pair<std::string, DataType>>& initFragmentShaderConstantInfo,
                       const std::vector<std::pair<std::string, DataType>>& initVertexShaderConstantInfo,
                       uint32_t initFragmentShaderDataAlignment,
                       uint32_t initVertexShaderDataAlignment,
                       const std::string& fragmentShaderFunction,
                       const std::string& vertexShaderFunction);
                ~Shader();

                void reload() final;

                struct Location final
                {
                    Location(GLint initLocation, DataType initDataType):
                        location(initLocation), dataType(initDataType)
                    {
                    }

                    GLint location;
                    DataType dataType;
                };

                inline const std::set<Vertex::Attribute::Usage>& getVertexAttributes() const { return vertexAttributes; }

                inline const std::vector<Location>& getFragmentShaderConstantLocations() const { return fragmentShaderConstantLocations; }
                inline const std::vector<Location>& getVertexShaderConstantLocations() const { return vertexShaderConstantLocations; }

                inline auto getProgramId() const { return programId; }

            private:
                void compileShader();
                std::string getShaderMessage(GLuint shaderId) const;
                std::string getProgramMessage() const;

                std::vector<uint8_t> fragmentShaderData;
                std::vector<uint8_t> vertexShaderData;

                std::set<Vertex::Attribute::Usage> vertexAttributes;

                std::vector<std::pair<std::string, DataType>> fragmentShaderConstantInfo;
                std::vector<std::pair<std::string, DataType>> vertexShaderConstantInfo;

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

// Ouzel by Elviss Strazdins

#ifndef OUZEL_GRAPHICS_OGLSHADER_HPP
#define OUZEL_GRAPHICS_OGLSHADER_HPP

#include <utility>

#include "../../core/Setup.h"

#if OUZEL_COMPILE_OPENGL

#include "OGL.h"

#if OUZEL_OPENGLES
#  include "GLES/gl.h"
#  include "GLES2/gl2.h"
#  include "GLES2/gl2ext.h"
#  include "GLES3/gl3.h"
#else
#  include "GL/glcorearb.h"
#  include "GL/glext.h"
#endif

#include "OGLRenderResource.hpp"
#include "../DataType.hpp"
#include "../Vertex.hpp"

namespace ouzel::graphics::opengl
{
    class RenderDevice;

    class Shader final: public RenderResource
    {
    public:
        Shader(RenderDevice& initRenderDevice,
               const std::vector<std::uint8_t>& initFragmentShader,
               const std::vector<std::uint8_t>& initVertexShader,
               const std::set<Vertex::Attribute::Usage>& initVertexAttributes,
               const std::vector<std::pair<std::string, DataType>>& initFragmentShaderConstantInfo,
               const std::vector<std::pair<std::string, DataType>>& initVertexShaderConstantInfo,
               const std::string& fragmentShaderFunction,
               const std::string& vertexShaderFunction);
        ~Shader() override;

        void reload() final;

        struct Location final
        {
            Location(GLint initLocation, DataType initDataType) noexcept:
                location(initLocation), dataType(initDataType)
            {
            }

            GLint location;
            DataType dataType;
        };

        auto& getVertexAttributes() const noexcept { return vertexAttributes; }

        auto& getFragmentShaderConstantLocations() const noexcept { return fragmentShaderConstantLocations; }
        auto& getVertexShaderConstantLocations() const noexcept { return vertexShaderConstantLocations; }

        auto getProgramId() const noexcept { return programId; }

    private:
        void compileShader();
        std::string getShaderMessage(GLuint shaderId) const;
        std::string getProgramMessage() const;

        std::vector<std::uint8_t> fragmentShaderData;
        std::vector<std::uint8_t> vertexShaderData;

        std::set<Vertex::Attribute::Usage> vertexAttributes;

        std::vector<std::pair<std::string, DataType>> fragmentShaderConstantInfo;
        std::vector<std::pair<std::string, DataType>> vertexShaderConstantInfo;

        GLuint fragmentShaderId = 0;
        GLuint vertexShaderId = 0;
        GLuint programId = 0;

        std::vector<Location> fragmentShaderConstantLocations;
        std::vector<Location> vertexShaderConstantLocations;
    };
}
#endif

#endif // OUZEL_GRAPHICS_OGLSHADER_HPP

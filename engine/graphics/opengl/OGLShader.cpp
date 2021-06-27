// Ouzel by Elviss Strazdins

#include "../../core/Setup.h"

#if OUZEL_COMPILE_OPENGL

#include "OGLShader.hpp"
#include "OGLError.hpp"
#include "OGLRenderDevice.hpp"

namespace ouzel::graphics::opengl
{
    Shader::Shader(RenderDevice& initRenderDevice,
                   const std::vector<std::uint8_t>& initFragmentShader,
                   const std::vector<std::uint8_t>& initVertexShader,
                   const std::set<Vertex::Attribute::Semantic>& initVertexAttributes,
                   const std::vector<std::pair<std::string, DataType>>& initFragmentShaderConstantInfo,
                   const std::vector<std::pair<std::string, DataType>>& initVertexShaderConstantInfo,
                   const std::string&,
                   const std::string&):
        RenderResource{initRenderDevice},
        fragmentShaderData{initFragmentShader},
        vertexShaderData{initVertexShader},
        vertexAttributes{initVertexAttributes},
        fragmentShaderConstantInfo{initFragmentShaderConstantInfo},
        vertexShaderConstantInfo{initVertexShaderConstantInfo}
    {
        compileShader();
    }

    Shader::~Shader()
    {
        if (programId) renderDevice.deleteProgram(programId);
        if (vertexShaderId) renderDevice.glDeleteShaderProc(vertexShaderId);
        if (fragmentShaderId) renderDevice.glDeleteShaderProc(fragmentShaderId);
    }

    void Shader::reload()
    {
        fragmentShaderId = 0;
        vertexShaderId = 0;
        programId = 0;

        compileShader();
    }

    std::string Shader::getShaderMessage(GLuint shaderId) const
    {
        GLint logLength = 0;
        renderDevice.glGetShaderivProc(shaderId, GL_INFO_LOG_LENGTH, &logLength);

        if (logLength > 0)
        {
            std::vector<char> logMessage(static_cast<std::size_t>(logLength));
            renderDevice.glGetShaderInfoLogProc(shaderId, logLength, nullptr, logMessage.data());

            return std::string(logMessage.data());
        }

        return std::string();
    }

    std::string Shader::getProgramMessage() const
    {
        GLint logLength = 0;
        renderDevice.glGetProgramivProc(programId, GL_INFO_LOG_LENGTH, &logLength);

        if (logLength > 0)
        {
            std::vector<char> logMessage(static_cast<std::size_t>(logLength));
            renderDevice.glGetProgramInfoLogProc(programId, logLength, nullptr, logMessage.data());

            return std::string("Shader linking error: ") + logMessage.data();
        }

        return std::string();
    }

    namespace
    {
        std::string semanticToString(Vertex::Attribute::Semantic semantic)
        {
            switch (semantic.usage)
            {
                case Vertex::Attribute::Usage::binormal: return "binormal" + std::to_string(semantic.index);
                case Vertex::Attribute::Usage::blendIndices: return "blendIndices" + std::to_string(semantic.index);
                case Vertex::Attribute::Usage::blendWeight: return "blendWeight" + std::to_string(semantic.index);
                case Vertex::Attribute::Usage::color: return "color" + std::to_string(semantic.index);
                case Vertex::Attribute::Usage::normal: return "normal" + std::to_string(semantic.index);
                case Vertex::Attribute::Usage::position: return "position" + std::to_string(semantic.index);
                case Vertex::Attribute::Usage::positionTransformed: return "positionT";
                case Vertex::Attribute::Usage::pointSize: return "pointSize" + std::to_string(semantic.index);
                case Vertex::Attribute::Usage::tangent: return "tangent" + std::to_string(semantic.index);
                case Vertex::Attribute::Usage::textureCoordinates: return "texCoord" + std::to_string(semantic.index);
                default:
                    throw Error("Invalid vertex attribute usage");
            }
        }
    }

    void Shader::compileShader()
    {
        fragmentShaderId = renderDevice.glCreateShaderProc(GL_FRAGMENT_SHADER);

        const auto fragmentShaderBuffer = reinterpret_cast<const GLchar*>(fragmentShaderData.data());
        const auto fragmentShaderSize = static_cast<GLint>(fragmentShaderData.size());

        renderDevice.glShaderSourceProc(fragmentShaderId, 1, &fragmentShaderBuffer, &fragmentShaderSize);
        renderDevice.glCompileShaderProc(fragmentShaderId);

        GLint status;
        renderDevice.glGetShaderivProc(fragmentShaderId, GL_COMPILE_STATUS, &status);
        if (status == GL_FALSE)
            throw Error("Failed to compile pixel shader, error: " + getShaderMessage(fragmentShaderId));

        if (const auto error = renderDevice.glGetErrorProc(); error != GL_NO_ERROR)
            throw std::system_error(makeErrorCode(error), "Failed to get shader compile status");

        vertexShaderId = renderDevice.glCreateShaderProc(GL_VERTEX_SHADER);

        const auto vertexShaderBuffer = reinterpret_cast<const GLchar*>(vertexShaderData.data());
        const auto vertexShaderSize = static_cast<GLint>(vertexShaderData.size());

        renderDevice.glShaderSourceProc(vertexShaderId, 1, &vertexShaderBuffer, &vertexShaderSize);
        renderDevice.glCompileShaderProc(vertexShaderId);

        renderDevice.glGetShaderivProc(vertexShaderId, GL_COMPILE_STATUS, &status);
        if (status == GL_FALSE)
            throw Error("Failed to compile vertex shader, error: " + getShaderMessage(vertexShaderId));

        programId = renderDevice.glCreateProgramProc();

        renderDevice.glAttachShaderProc(programId, vertexShaderId);
        renderDevice.glAttachShaderProc(programId, fragmentShaderId);

        GLuint index = 0;

        for (const auto& vertexAttribute : RenderDevice::vertexAttributes)
            if (vertexAttributes.find(vertexAttribute.semantic) != vertexAttributes.end())
            {
                const auto usage = semanticToString(vertexAttribute.semantic);
                renderDevice.glBindAttribLocationProc(programId, index, usage.c_str());
                ++index;
            }

        renderDevice.glLinkProgramProc(programId);

        renderDevice.glGetProgramivProc(programId, GL_LINK_STATUS, &status);
        if (status == GL_FALSE)
            throw Error("Failed to link shader" + getProgramMessage());

        if (const auto error = renderDevice.glGetErrorProc(); error != GL_NO_ERROR)
            throw std::system_error(makeErrorCode(error), "Failed to get shader link status");

        renderDevice.glDetachShaderProc(programId, vertexShaderId);
        renderDevice.glDeleteShaderProc(vertexShaderId);
        vertexShaderId = 0;

        renderDevice.glDetachShaderProc(programId, fragmentShaderId);
        renderDevice.glDeleteShaderProc(fragmentShaderId);
        fragmentShaderId = 0;

        if (const auto error = renderDevice.glGetErrorProc(); error != GL_NO_ERROR)
            throw std::system_error(makeErrorCode(error), "Failed to detach shader");

        renderDevice.useProgram(programId);

        const auto texture0Location = renderDevice.glGetUniformLocationProc(programId, "texture0");
        if (texture0Location != -1) renderDevice.glUniform1iProc(texture0Location, 0);

        const auto texture1Location = renderDevice.glGetUniformLocationProc(programId, "texture1");
        if (texture1Location != -1) renderDevice.glUniform1iProc(texture1Location, 1);

        if (const auto error = renderDevice.glGetErrorProc(); error != GL_NO_ERROR)
            throw std::system_error(makeErrorCode(error), "Failed to get uniform location");

        if (!fragmentShaderConstantInfo.empty())
        {
            fragmentShaderConstantLocations.clear();
            fragmentShaderConstantLocations.reserve(fragmentShaderConstantInfo.size());

            for (const auto& [name, dataType] : fragmentShaderConstantInfo)
            {
                const auto location = renderDevice.glGetUniformLocationProc(programId, name.c_str());

                if (const auto error = renderDevice.glGetErrorProc(); error != GL_NO_ERROR)
                    throw std::system_error(makeErrorCode(error), "Failed to get OpenGL uniform location");

                if (location == -1)
                    throw Error("Failed to get OpenGL uniform location");

                fragmentShaderConstantLocations.emplace_back(location, dataType);
            }
        }

        if (!vertexShaderConstantInfo.empty())
        {
            vertexShaderConstantLocations.clear();
            vertexShaderConstantLocations.reserve(vertexShaderConstantInfo.size());

            for (const auto& [name, dataType] : vertexShaderConstantInfo)
            {
                const auto location = renderDevice.glGetUniformLocationProc(programId, name.c_str());

                if (const auto error = renderDevice.glGetErrorProc(); error != GL_NO_ERROR)
                    throw std::system_error(makeErrorCode(error), "Failed to get OpenGL uniform location");

                if (location == -1)
                    throw Error("Failed to get OpenGL uniform location");

                vertexShaderConstantLocations.emplace_back(location, dataType);
            }
        }
    }
}

#endif

// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include <string>
#include <vector>
#include <cstdint>
#include "Noncopyable.h"
#include "Vector3.h"
#include "Vector4.h"
#include "Matrix4.h"

namespace ouzel
{
    namespace video
    {
        class Renderer;

        class Shader: public Noncopyable
        {
            friend Renderer;
        public:
            virtual ~Shader();

            virtual bool initFromFiles(const std::string& fragmentShader, const std::string& vertexShader, uint32_t vertexAttributes);
            virtual bool initFromBuffers(const uint8_t* fragmentShader, uint32_t fragmentShaderSize, const uint8_t* vertexShader, uint32_t vertexShaderSize, uint32_t vertexAttributes);

            uint32_t getVertexAttributes() const { return _vertexAttributes; }

            virtual uint32_t getPixelShaderConstantId(const std::string& name);
            virtual bool setPixelShaderConstant(uint32_t index, const std::vector<Vector3>& vectors);
            virtual bool setPixelShaderConstant(uint32_t index, const std::vector<Vector4>& vectors);
            virtual bool setPixelShaderConstant(uint32_t index, const std::vector<Matrix4>& matrices);

            virtual uint32_t getVertexShaderConstantId(const std::string& name);
            virtual bool setVertexShaderConstant(uint32_t index, const std::vector<Vector3>& vectors);
            virtual bool setVertexShaderConstant(uint32_t index, const std::vector<Vector4>& vectors);
            virtual bool setVertexShaderConstant(uint32_t index, const std::vector<Matrix4>& matrices);

        protected:
            Shader();

            std::string _fragmentShaderFilename;
            std::string _vertexShaderFilename;

            uint32_t _vertexAttributes;
        };
    } // namespace video
} // namespace ouzel

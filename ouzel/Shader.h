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
            struct ConstantInfo
            {
                std::string name;
                uint32_t size;
            };

            virtual ~Shader();

            virtual bool initFromFiles(const std::string& pixelShader,
                                       const std::string& vertexShader,
                                       uint32_t vertexAttributes,
                                       const std::string& pixelShaderFunction = "",
                                       const std::string& vertexShaderFunction = "");
            virtual bool initFromBuffers(const uint8_t* pixelShader,
                                         uint32_t pixelShaderSize,
                                         const uint8_t* vertexShader,
                                         uint32_t vertexShaderSize,
                                         uint32_t vertexAttributes,
                                         const std::string& pixelShaderFunction = "",
                                         const std::string& vertexShaderFunction = "");

            uint32_t getVertexAttributes() const { return _vertexAttributes; }

            virtual bool setPixelShaderConstantInfo(const std::vector<ConstantInfo>& constantInfo);
            virtual bool setVertexShaderConstantInfo(const std::vector<ConstantInfo>& constantInfo);
            
            virtual bool setPixelShaderConstant(uint32_t index, const std::vector<Vector3>& vectors);
            virtual bool setPixelShaderConstant(uint32_t index, const std::vector<Vector4>& vectors);
            virtual bool setPixelShaderConstant(uint32_t index, const std::vector<Matrix4>& matrices);

            virtual bool setVertexShaderConstant(uint32_t index, const std::vector<Vector3>& vectors);
            virtual bool setVertexShaderConstant(uint32_t index, const std::vector<Vector4>& vectors);
            virtual bool setVertexShaderConstant(uint32_t index, const std::vector<Matrix4>& matrices);

        protected:
            Shader();

            std::string _pixelShaderFilename;
            std::string _vertexShaderFilename;

            uint32_t _vertexAttributes;

            std::vector<ConstantInfo> _pixelShaderConstantInfo;
            std::vector<ConstantInfo> _vertexShaderConstantInfo;
        };
    } // namespace video
} // namespace ouzel

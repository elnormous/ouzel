// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include "CompileConfig.h"
#include "Shader.h"

#ifdef __OBJC__
#import <Metal/Metal.h>
typedef id<MTLFunction> MTLFunctionPtr;
#else
#include <objc/objc.h>
typedef id MTLFunctionPtr;
#endif

namespace ouzel
{
    namespace video
    {
        class RendererMetal;

        class ShaderMetal: public Shader
        {
            friend RendererMetal;
        public:
            virtual ~ShaderMetal();

            virtual bool initFromBuffers(const uint8_t* fragmentShader, uint32_t fragmentShaderSize, const uint8_t* vertexShader, uint32_t vertexShaderSize, uint32_t vertexAttributes) override;

            virtual uint32_t getPixelShaderConstantId(const std::string& name) override;
            virtual bool setPixelShaderConstant(uint32_t index, const std::vector<Vector3>& vectors) override;
            virtual bool setPixelShaderConstant(uint32_t index, const std::vector<Vector4>& vectors) override;
            virtual bool setPixelShaderConstant(uint32_t index, const std::vector<Matrix4>& matrices) override;

            virtual uint32_t getVertexShaderConstantId(const std::string& name) override;
            virtual bool setVertexShaderConstant(uint32_t index, const std::vector<Vector3>& vectors) override;
            virtual bool setVertexShaderConstant(uint32_t index, const std::vector<Vector4>& vectors) override;
            virtual bool setVertexShaderConstant(uint32_t index, const std::vector<Matrix4>& matrices) override;

        protected:
            ShaderMetal();

            void destroy();

            MTLFunctionPtr _vertexShader = Nil;
            MTLFunctionPtr _fragmentShader = Nil;
        };
    } // namespace video
} // namespace ouzel

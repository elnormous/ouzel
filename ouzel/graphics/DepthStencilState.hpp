// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_DEPTHSTENCILSTATE_H
#define OUZEL_DEPTHSTENCILSTATE_H

#include <cstdint>

namespace ouzel
{
    namespace graphics
    {
        class Renderer;

        class DepthStencilState final
        {
        public:
            enum class CompareFunction
            {
                NEVER,
                LESS,
                EQUAL,
                LESS_EQUAL,
                GREATER,
                NOT_EQUAL,
                GREATER_EQUAL,
                ALWAYS,
            };

            explicit DepthStencilState(Renderer& initRenderer);
            DepthStencilState(Renderer& initRenderer,
                              bool initDepthTest,
                              bool initDepthWrite,
                              CompareFunction initCompareFunction);
            ~DepthStencilState();

            DepthStencilState(const DepthStencilState&) = delete;
            DepthStencilState& operator=(const DepthStencilState&) = delete;

            DepthStencilState(DepthStencilState&&) = delete;
            DepthStencilState& operator=(DepthStencilState&&) = delete;

            void init(bool newDepthTest,
                      bool newDepthWrite,
                      CompareFunction newCompareFunction);

            inline uintptr_t getResource() const { return resource; }

        private:
            Renderer& renderer;
            uintptr_t resource = 0;

            bool depthTest = false;
            bool depthWrite = false;
            CompareFunction compareFunction;
        };
    } // namespace graphics
} // namespace ouzel

#endif // OUZEL_DEPTHSTENCILSTATE_H

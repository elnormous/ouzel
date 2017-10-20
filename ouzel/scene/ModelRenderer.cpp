// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "ModelRenderer.hpp"
#include "core/Engine.hpp"

namespace ouzel
{
    namespace scene
    {
        ModelRenderer::ModelRenderer():
            Component(TYPE)
        {
        }

        ModelRenderer::ModelRenderer(const ModelData& modelData):
            Component(TYPE)
        {
            init(modelData);
        }

        ModelRenderer::ModelRenderer(const std::string& filename, bool mipmaps):
            Component(TYPE)
        {
            init(filename, mipmaps);
        }

        bool ModelRenderer::init(const ModelData& modelData)
        {
            return true;
        }

        bool ModelRenderer::init(const std::string& filename, bool mipmaps)
        {
            //const ModelData& modelData = sharedEngine->getCache()->
            return true;
        }
    } // namespace scene
} // namespace ouzel

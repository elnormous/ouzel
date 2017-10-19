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

        ModelRenderer::ModelRenderer(const ModelDefinition& modelDefinition):
            Component(TYPE)
        {
            init(modelDefinition);
        }

        ModelRenderer::ModelRenderer(const std::string& filename, bool mipmaps):
            Component(TYPE)
        {
            init(filename, mipmaps);
        }

        bool ModelRenderer::init(const ModelDefinition& modelDefinition)
        {
            return true;
        }

        bool ModelRenderer::init(const std::string& filename, bool mipmaps)
        {
            //const ModelDefinition& modelDefinition = sharedEngine->getCache()->
            return true;
        }
    } // namespace scene
} // namespace ouzel

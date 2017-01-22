// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "MeshBufferMetal.h"
#include "core/Engine.h"
#include "RendererMetal.h"
#include "IndexBufferMetal.h"
#include "VertexBufferMetal.h"
#include "utils/Log.h"

namespace ouzel
{
    namespace graphics
    {
        MeshBufferMetal::MeshBufferMetal()
        {
        }

        MeshBufferMetal::~MeshBufferMetal()
        {
        }

        bool MeshBufferMetal::upload()
        {
            if (!MeshBufferResource::upload())
            {
                return false;
            }

            switch (uploadData.indexSize)
            {
                case 2:
                    indexType = MTLIndexTypeUInt16;
                    bytesPerIndex = 2;
                    break;
                case 4:
                    indexType = MTLIndexTypeUInt32;
                    bytesPerIndex = 4;
                    break;
                default:
                    bytesPerIndex = 0;
                    Log(Log::Level::ERR) << "Invalid index size";
                    return false;
            }

            return true;
        }
    } // namespace graphics
} // namespace ouzel

// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "core/CompileConfig.h"

#if OUZEL_SUPPORTS_METAL

#include "MeshBufferResourceMetal.h"
#include "RendererMetal.h"
#include "BufferResourceMetal.h"
#include "core/Engine.h"
#include "utils/Log.h"

namespace ouzel
{
    namespace graphics
    {
        MeshBufferResourceMetal::MeshBufferResourceMetal()
        {
        }

        MeshBufferResourceMetal::~MeshBufferResourceMetal()
        {
        }

        bool MeshBufferResourceMetal::upload()
        {
            std::lock_guard<std::mutex> lock(uploadMutex);

            if (dirty)
            {
                switch (indexSize)
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

                indexBufferMetal = static_cast<BufferResourceMetal*>(indexBuffer);
                vertexBufferMetal = static_cast<BufferResourceMetal*>(vertexBuffer);

                dirty = 0;
            }

            return true;
        }
    } // namespace graphics
} // namespace ouzel

#endif

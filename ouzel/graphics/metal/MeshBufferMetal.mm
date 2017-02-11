// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "MeshBufferMetal.h"
#include "core/Engine.h"
#include "RendererMetal.h"
#include "BufferMetal.h"
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

                indexBufferMetal = static_cast<BufferMetal*>(indexBuffer);
                vertexBufferMetal = static_cast<BufferMetal*>(vertexBuffer);

                dirty = 0;
            }

            return true;
        }
    } // namespace graphics
} // namespace ouzel

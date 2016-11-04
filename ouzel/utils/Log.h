// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include <sstream>
#include "math/Vector2.h"
#include "math/Vector3.h"
#include "math/Vector4.h"
#include "math/Size2.h"
#include "math/Size3.h"

namespace ouzel
{
    class Log
    {
    public:
        enum class Level
        {
            OFF,
            ERR,
            WARN,
            INFO,
            ALL
        };

        static Level threshold;

        Log()
        {
        }

        Log(Level aLevel): level(aLevel)
        {
        }

        ~Log();

        template<typename T> Log& operator<<(T val)
        {
            s << val;

            return *this;
        }

        Log& operator<<(const Vector2& val)
        {
            s << val.x << "," << val.y;

            return *this;
        }

        Log& operator<<(const Vector3& val)
        {
            s << val.x << "," << val.y << "," << val.z;

            return *this;
        }

        Log& operator<<(const Vector4& val)
        {
            s << val.x << "," << val.y << "," << val.z << "," << val.w;

            return *this;
        }

        Log& operator<<(const Size2& val)
        {
            s << val.width << "," << val.height;

            return *this;
        }

        Log& operator<<(const Size3& val)
        {
            s << val.width << "," << val.height << "," << val.depth;

            return *this;
        }

    private:
        Level level = Level::INFO;
        std::stringstream s;
    };
}

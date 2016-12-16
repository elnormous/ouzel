// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include <string>
#include "utils/Utils.h"
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

        Log(const Log& other)
        {
            level = other.level;
            s = other.s;
        }

        Log(Log&& other)
        {
            level = other.level;
            s = std::move(other.s);
        }

        Log& operator=(const Log& other)
        {
            flush();
            level = other.level;
            s = other.s;

            return *this;
        }

        Log& operator=(Log&& other)
        {
            flush();
            level = other.level;
            s = std::move(other.s);
            
            return *this;
        }

        ~Log()
        {
            flush();
        }

        template<typename T> Log& operator<<(T val)
        {
            if (level <= threshold)
            {
                s += toString(val);
            }

            return *this;
        }

        Log& operator<<(const std::string& val)
        {
            if (level <= threshold)
            {
                s += val;
            }

            return *this;
        }

        Log& operator<<(const char* val)
        {
            if (level <= threshold)
            {
                s += val;
            }

            return *this;
        }

        Log& operator<<(char* val)
        {
            if (level <= threshold)
            {
                s += val;
            }

            return *this;
        }

        Log& operator<<(const Vector2& val)
        {
            if (level <= threshold)
            {
                s += toString(val.x()) + "," + toString(val.y());
            }

            return *this;
        }

        Log& operator<<(const Vector3& val)
        {
            if (level <= threshold)
            {
                s += toString(val.x) + "," + toString(val.y) + "," +
                    toString(val.z);
            }

            return *this;
        }

        Log& operator<<(const Vector4& val)
        {
            if (level <= threshold)
            {
                s += toString(val.x) + "," + toString(val.y) + "," +
                    toString(val.z) + "," + toString(val.w);
            }

            return *this;
        }

        Log& operator<<(const Size2& val)
        {
            if (level <= threshold)
            {
                s += toString(val.width) + "," + toString(val.height);
            }

            return *this;
        }

        Log& operator<<(const Size3& val)
        {
            if (level <= threshold)
            {
                s += toString(val.width) + "," + toString(val.height) + "," +
                    toString(val.depth);
            }

            return *this;
        }

    private:
        void flush();

        Level level = Level::INFO;
        std::string s;
    };
}

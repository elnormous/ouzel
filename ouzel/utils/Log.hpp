// Copyright (C) 2018 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include <string>
#include "math/Matrix4.hpp"
#include "math/Quaternion.hpp"
#include "math/Size2.hpp"
#include "math/Size3.hpp"
#include "math/Vector2.hpp"
#include "math/Vector3.hpp"
#include "math/Vector4.hpp"

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

        explicit Log(Level initLevel = Level::INFO): level(initLevel)
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
            other.level = Level::INFO;
            s = std::move(other.s);
        }

        Log& operator=(const Log& other)
        {
            level = other.level;
            s = other.s;

            return *this;
        }

        Log& operator=(Log&& other)
        {
            level = other.level;
            other.level = Level::INFO;
            s = std::move(other.s);

            return *this;
        }

        ~Log();

        template<typename T> Log& operator<<(T val)
        {
            if (level <= threshold)
            {
                s += std::to_string(val);
            }

            return *this;
        }

        Log& operator<<(const std::string& val)
        {
            if (level <= threshold)
                s += val;

            return *this;
        }

        Log& operator<<(const char* val)
        {
            if (level <= threshold)
                s += val;

            return *this;
        }

        Log& operator<<(char* val)
        {
            if (level <= threshold)
                s += val;

            return *this;
        }

        Log& operator<<(const Matrix4& val)
        {
            s += std::to_string(val.m[0]) + "," + std::to_string(val.m[1]) + "," + std::to_string(val.m[2]) + "," + std::to_string(val.m[3]) + "\n" +
                std::to_string(val.m[4]) + "," + std::to_string(val.m[5]) + "," + std::to_string(val.m[6]) + "," + std::to_string(val.m[7]) + "\n" +
                std::to_string(val.m[8]) + "," + std::to_string(val.m[9]) + "," + std::to_string(val.m[10]) + "," + std::to_string(val.m[11]) + "\n" +
                std::to_string(val.m[12]) + "," + std::to_string(val.m[13]) + "," + std::to_string(val.m[14]) + "," + std::to_string(val.m[15]);

            return *this;
        }

        Log& operator<<(const Quaternion& val)
        {
            if (level <= threshold)
            {
                s += std::to_string(val.x) + "," + std::to_string(val.y) + "," +
                    std::to_string(val.z) + "," + std::to_string(val.w);
            }

            return *this;
        }

        Log& operator<<(const Size2& val)
        {
            if (level <= threshold)
            {
                s += std::to_string(val.width) + "," + std::to_string(val.height);
            }

            return *this;
        }

        Log& operator<<(const Size3& val)
        {
            if (level <= threshold)
            {
                s += std::to_string(val.width) + "," + std::to_string(val.height) + "," +
                    std::to_string(val.depth);
            }

            return *this;
        }

        Log& operator<<(const Vector2& val)
        {
            if (level <= threshold)
            {
                s += std::to_string(val.x) + "," + std::to_string(val.y);
            }

            return *this;
        }

        Log& operator<<(const Vector3& val)
        {
            if (level <= threshold)
            {
                s += std::to_string(val.x) + "," + std::to_string(val.y) + "," +
                    std::to_string(val.z);
            }

            return *this;
        }

        Log& operator<<(const Vector4& val)
        {
            if (level <= threshold)
            {
                s += std::to_string(val.x) + "," + std::to_string(val.y) + "," +
                    std::to_string(val.z) + "," + std::to_string(val.w);
            }

            return *this;
        }

    private:
        Level level = Level::INFO;
        std::string s;
    };
}

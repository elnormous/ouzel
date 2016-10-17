// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include <sstream>

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

        template <typename T> Log& operator << (T val)
        {
            s << val;

            return *this;
        }

    private:
        Level level = Level::INFO;
        std::stringstream s;
    };
}

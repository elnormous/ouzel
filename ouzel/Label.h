// Copyright (C) 2015 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include "Node.h"

namespace ouzel
{
    class Label: Node
    {
    public:
        Label(std::string const& font, std::string const& text);
        virtual ~Label();
    };
}

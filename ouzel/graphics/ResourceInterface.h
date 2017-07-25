//
//  ResourceInterface.h
//  ouzel
//
//  Created by Elviss Strazdins on 26/07/2017.
//  Copyright (c) 2017 Elviss Strazdins. All rights reserved.
//

#pragma once

#include "utils/Noncopyable.h"

namespace ouzel
{
    namespace graphics
    {
        class ResourceInterface: public Noncopyable
        {
        public:
            ResourceInterface() {}
            virtual ~ResourceInterface() {}
        };
    } // namespace graphics
} // namespace ouzel

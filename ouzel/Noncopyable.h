//
//  Noncopyable.h
//  Ouzel
//
//  Created by Elviss Strazdins on 10/07/15.
//  Copyright (c) 2015 Elviss. All rights reserved.
//

#pragma once

namespace ouzel
{
    class Noncopyable
    {
    protected:
        Noncopyable() = default;
        ~Noncopyable() = default;
        
        Noncopyable(const Noncopyable&) = delete;
        Noncopyable& operator=(const Noncopyable&) = delete;
        
        Noncopyable(const Noncopyable&&) = delete;
        Noncopyable& operator=(const Noncopyable&&) = delete;
    };
}

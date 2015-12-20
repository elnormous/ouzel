// Copyright (C) 2015 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include <cstdint>
#include <cassert>

namespace ouzel
{
    class ReferenceCounted
    {
    public:
        ReferenceCounted()
        {
            //LeakHunter::addObject(this);
        }

        virtual ~ReferenceCounted()
        {
            //LeakHunter::removeObject(this);
        }

        void retain() const { ++_referenceCounter; }

        bool release() const
        {
            assert(_referenceCounter > 0 && "Reference count must be positive");

            if (--_referenceCounter <= 0)
            {
                delete this;
                return true;
            }

            return false;
        }

        int32_t getReferenceCount() const
        {
            return _referenceCounter;
        }

        const char* getDebugName() const
        {
            return _debugName;
        }

    protected:
        void setDebugName(const char* newName)
        {
            _debugName = newName;
        }

    private:
        const char* _debugName = nullptr;
        mutable int32_t _referenceCounter = 1;
    };
}

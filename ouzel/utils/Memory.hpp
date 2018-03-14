// Copyright (C) 2018 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

namespace ouzel
{
    template <class T> class UniquePtr final
    {
    public:
        typedef T ElementType;

        UniquePtr() = default;
        explicit UniquePtr(ElementType* p):
            ptr(p)
        {

        }

        ~UniquePtr()
        {
            if (ptr) delete ptr;
        }

        UniquePtr(const UniquePtr&) = delete;
        UniquePtr& operator=(const UniquePtr&) = delete;

        UniquePtr(UniquePtr&& other):
            ptr(other.release())
        {
        }

        template <class U>
        UniquePtr(UniquePtr<U>&& other):
            ptr(other.release())
        {
        }

        UniquePtr& operator=(UniquePtr&& other)
        {
            if (ptr) delete ptr;
            ptr = other.release();
            return *this;
        }

        template <class U>
        UniquePtr& operator=(UniquePtr<U>&& other)
        {
            if (ptr) delete ptr;
            ptr = other.release();
            return *this;
        }

        inline void reset(ElementType* p = nullptr)
        {
            if (ptr) delete ptr;
            ptr = p;
        }

        inline ElementType* release()
        {
            ElementType* result = ptr;
            ptr = nullptr;
            return result;
        }

        inline ElementType* get() const
        {
            return ptr;
        }

        inline ElementType& operator*() const
        {
            return *ptr;
        }

        inline operator bool()
        {
            return ptr != nullptr;
        }
        
        inline bool operator!()
        {
            return ptr == nullptr;
        }

        inline ElementType* operator->() const
        {
            return ptr;
        }

    private:
        ElementType* ptr = nullptr;
    };
}

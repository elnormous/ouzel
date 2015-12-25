// Copyright (C) 2015 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

namespace ouzel
{
    template<class T>
    struct AutoPtr
    {
        typedef T subtype;
        T* item;
        
        AutoPtr() : item(nullptr) { }
        AutoPtr(T* v) : item(v) { if (item) item->retain(); }
        AutoPtr(const AutoPtr& other): item(other.item) { if (item) item->retain(); }
        ~AutoPtr() { if (item) { item->release(); item = nullptr; } }
        AutoPtr& operator = (const AutoPtr& other) { if (item) item->release(); item = other.item; if (item) item->retain(); return *this; }
        
        AutoPtr(AutoPtr&& other): item(other.item) { other.item = nullptr; }
        AutoPtr& operator = (AutoPtr&& other) { if (this != &other) { if (item) item->release(); item = other.item; other.item = nullptr; } return *this; }
        
        T* operator -> () const { return item; }
        T& operator * () const { return *item; }
        operator T* () const { return item; }
    };
}

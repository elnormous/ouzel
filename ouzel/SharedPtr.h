// Copyright (C) 2015 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

namespace ouzel
{
    template<class Type>
    struct SharedPtr
    {
        typedef Type subtype;
        
        SharedPtr() { }
        SharedPtr(Type* v): _obj(v) { if (_obj) _obj->retain(); }
        SharedPtr(const SharedPtr& other): _obj(other._obj) { if (_obj) _obj->retain(); }
        SharedPtr(SharedPtr&& other): _obj(other._obj) { other._obj = nullptr; }
        
        ~SharedPtr() { if (_obj) { _obj->release(); _obj = nullptr; } }
        
        SharedPtr& operator = (const SharedPtr& other) { if (_obj) _obj->release(); _obj = other._obj; if (_obj) _obj->retain(); return *this; }
        SharedPtr& operator = (SharedPtr&& other) { if (this != &other) { if (_obj) _obj->release(); _obj = other._obj; other._obj = nullptr; } return *this; }
        
        Type* operator -> () const { return _obj; }
        Type& operator * () const { return *_obj; }
        operator Type* () const { return _obj; }
        
        Type* obj() { return _obj; }
        
    protected:
        Type* _obj = nullptr;
    };
}

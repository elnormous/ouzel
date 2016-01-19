// Copyright (C) 2015 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include "Types.h"

namespace ouzel
{
    class Animator
    {
    public:
        Animator(float length);
        
        virtual void update(float delta);
        
        virtual void start(NodePtr const& node);
        virtual void reset();
        bool isRunning() const { return _running; }
        bool isDone() const { return _done; }
        
        float getLength() const { return _length; }
        float getCurrentTime() const { return _currentTime; }
        void setCurrentTime(float currentTime) { _currentTime = currentTime; }
        
        float getProgress() const { return _progress; }
        virtual void setProgress(float progress) { _progress = progress; }
        
    protected:
        float _length = 0.0f;
        float _currentTime = 0.0f;
        float _progress = 0.0f;
        bool _done = false;
        bool _running = false;
        
        NodeWeakPtr _node;
    };
}

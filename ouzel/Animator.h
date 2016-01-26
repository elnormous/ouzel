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
        
        virtual void start(const NodePtr& node);
        
        virtual void resume();
        virtual void stop(bool resetAnimation = false);
        virtual void reset();
        
        bool isRunning() const { return _running; }
        bool isDone() const { return _done; }
        
        float getLength() const { return _length; }
        float getCurrentTime() const { return _currentTime; }
        
        float getProgress() const { return _progress; }
        virtual void setProgress(float progress);
        
    protected:
        float _length = 0.0f;
        float _currentTime = 0.0f;
        float _progress = 0.0f;
        bool _done = false;
        bool _running = false;
        
        NodeWeakPtr _node;
    };
}

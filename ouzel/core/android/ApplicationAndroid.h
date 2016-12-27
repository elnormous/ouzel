// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include <jni.h>
#include <android/asset_manager.h>
#include <android/asset_manager_jni.h>
#include "core/Application.h"

namespace ouzel
{
    class ApplicationAndroid: public Application
    {
    public:
        ApplicationAndroid(jobject aMainActivity, AAssetManager* aAssetManager);

        virtual int run() override;

        AAssetManager* getAssetManager() const { return assetManager; }

    private:
        jobject mainActivity;
        AAssetManager* assetManager;
    };
}

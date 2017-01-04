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
        ApplicationAndroid(JNIEnv* aJNIEnv, jobject aMainActivity, jobject aAssetManager);

        virtual int run() override;

        bool step();

        JNIEnv* getJNIEnv() const { return jniEnv; }
        jobject getMainActivity() const { return mainActivity; }
        AAssetManager* getAssetManager() const { return assetManager; }

    private:
        JNIEnv* jniEnv;
        jobject mainActivity;
        AAssetManager* assetManager;
    };
}

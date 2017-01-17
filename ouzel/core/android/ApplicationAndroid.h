// Copyright (C) 2017 Elviss Strazdins
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
        ApplicationAndroid(JNIEnv* jniEnv, jobject aMainActivity, jobject aAssetManager);
        virtual ~ApplicationAndroid();

        virtual int run() override;

        virtual bool openURL(const std::string& url) override;

        bool step();

        JavaVM* getJavaVM() const { return javaVM; }
        jobject getMainActivity() const { return mainActivity; }
        jmethodID getCreateSurfaceMethod() const { return createSurfaceMethod; }
        jmethodID getOpenURLMethod() const { return openURLMethod; }
        AAssetManager* getAssetManager() const { return assetManager; }

    private:
        JavaVM* javaVM;
        jobject mainActivity;
        jmethodID createSurfaceMethod;
        jmethodID openURLMethod;
        AAssetManager* assetManager;
    };
}

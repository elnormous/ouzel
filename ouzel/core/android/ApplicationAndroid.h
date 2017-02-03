// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include <thread>
#include <jni.h>
#include <android/asset_manager.h>
#include <android/asset_manager_jni.h>
#include "core/Application.h"

namespace ouzel
{
    class ApplicationAndroid: public Application
    {
    public:
        ApplicationAndroid(JavaVM* aJavaVM);
        virtual ~ApplicationAndroid();

        void setMainActivity(jobject aMainActivity);
        void setAssetManager(jobject aAssetManager);
        void setSurface(jobject aSurface);

        virtual int run() override;

        virtual bool openURL(const std::string& url) override;

        JavaVM* getJavaVM() const { return javaVM; }
        jobject getMainActivity() const { return mainActivity; }
        jobject getSurface() const { return surface; }
        AAssetManager* getAssetManager() const { return assetManager; }
        jmethodID getOpenURLMethod() const { return openURLMethod; }

    private:
        void update();

        JavaVM* javaVM;
        jobject mainActivity;
        jobject surface;
        AAssetManager* assetManager = nullptr;
        jmethodID openURLMethod;

        std::thread updateThread;
    };
}

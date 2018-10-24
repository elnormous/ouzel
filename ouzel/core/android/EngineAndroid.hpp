// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#pragma once

#include <jni.h>
#include <android/asset_manager.h>
#include <android/asset_manager_jni.h>
#include <android/looper.h>
#include "core/Engine.hpp"

namespace ouzel
{
    class EngineAndroid: public Engine
    {
    public:
        static const jint ORIENTATION_PORTRAIT = 0x00000001;
        static const jint ORIENTATION_LANDSCAPE = 0x00000002;

        EngineAndroid(JavaVM* initJavaVM);
        virtual ~EngineAndroid();

        void onCreate(jobject initMainActivity);
        void onSurfaceCreated(jobject newSurface);
        void onSurfaceDestroyed();
        void onConfigurationChanged(jobject newConfig);

        void run();

        virtual void executeOnMainThread(const std::function<void(void)>& func) override;
        virtual void openURL(const std::string& url) override;

        virtual void setScreenSaverEnabled(bool newScreenSaverEnabled) override;

        JavaVM* getJavaVM() const { return javaVM; }
        jobject getMainActivity() const { return mainActivity; }
        jobject getSurface() const { return surface; }
        AAssetManager* getAssetManager() const { return assetManager; }

        const std::string& getFilesDirectory() const { return filesDirectory; }
        const std::string& getCacheDirectory() const { return cacheDirectory; }

        void executeAll();

    private:
        virtual void main() override;

        JavaVM* javaVM = nullptr;
        jclass uriClass = nullptr;
        jmethodID parseMethod = nullptr;
        jclass intentClass = nullptr;
        jmethodID intentConstructor = nullptr;
        jobject mainActivity = nullptr;
        jmethodID startActivityMethod = nullptr;
        jclass configurationClass = nullptr;
        jfieldID orientationField = nullptr;
        jint orientation = 0;
        jobject surface = nullptr;
        AAssetManager* assetManager = nullptr;
        jobject androidWindow = nullptr;
        jmethodID addFlagsMethod = nullptr;
        jmethodID clearFlagsMethod = nullptr;
        ALooper* looper = nullptr;
        int looperPipe[2];

        std::string filesDirectory;
        std::string cacheDirectory;

        std::queue<std::function<void(void)>> executeQueue;
        std::mutex executeMutex;
    };
}

// Copyright 2015-2021 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_CORE_ENGINEANDROID_HPP
#define OUZEL_CORE_ENGINEANDROID_HPP

#include <array>
#include <system_error>
#include <jni.h>
#include <android/asset_manager.h>
#include <android/asset_manager_jni.h>
#include <android/looper.h>
#include "../Engine.hpp"

namespace ouzel::core::android
{
    const std::error_category& getErrorCategory() noexcept;

    class Engine final: public core::Engine
    {
    public:
        Engine(JavaVM* initJavaVm);
        ~Engine() override;

        void onCreate(jobject initMainActivity);
        void onSurfaceCreated(jobject newSurface);
        void onSurfaceDestroyed();
        void onConfigurationChanged(jobject newConfig);

        void run();

        void openUrl(const std::string& url) final;

        void setScreenSaverEnabled(bool newScreenSaverEnabled) final;

        auto getJavaVm() const noexcept { return javaVm; }
        auto getMainActivity() const noexcept { return mainActivity; }
        auto getSurface() const noexcept { return surface; }
        auto getAssetManager() const noexcept { return assetManager; }

        auto& getFilesDirectory() const noexcept { return filesDirectory; }
        auto& getCacheDirectory() const noexcept { return cacheDirectory; }

        void executeAll();

    private:
        void engineMain() final;
        void runOnMainThread(const std::function<void()>& func) final;

        JavaVM* javaVm = nullptr;
        jclass uriClass = nullptr;
        jmethodID parseMethod = nullptr;
        jclass intentClass = nullptr;
        jmethodID intentConstructor = nullptr;
        jobject mainActivity = nullptr;
        jmethodID startActivityMethod = nullptr;
        jfieldID orientationField = nullptr;
        jint orientation = 0;
        jobject surface = nullptr;
        AAssetManager* assetManager = nullptr;
        jobject androidWindow = nullptr;
        jmethodID addFlagsMethod = nullptr;
        jmethodID clearFlagsMethod = nullptr;
        ALooper* looper = nullptr;
        std::array<int, 2> looperPipe{-1, -1};

        std::string filesDirectory;
        std::string cacheDirectory;

        std::queue<std::function<void()>> executeQueue;
        std::mutex executeMutex;
    };
}

#endif // OUZEL_CORE_ENGINEANDROID_HPP

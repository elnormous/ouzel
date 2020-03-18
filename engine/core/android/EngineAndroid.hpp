// Copyright 2015-2020 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_CORE_ENGINEANDROID_HPP
#define OUZEL_CORE_ENGINEANDROID_HPP

#include <system_error>
#include <jni.h>
#include <android/asset_manager.h>
#include <android/asset_manager_jni.h>
#include <android/looper.h>
#include "core/Engine.hpp"

namespace ouzel
{
    const std::error_category& getErrorCategory() noexcept;

    class EngineAndroid final: public Engine
    {
    public:
        static constexpr jint ORIENTATION_PORTRAIT = 0x00000001;
        static constexpr jint ORIENTATION_LANDSCAPE = 0x00000002;

        EngineAndroid(JavaVM* initJavaVm);
        ~EngineAndroid() override;

        void onCreate(jobject initMainActivity);
        void onSurfaceCreated(jobject newSurface);
        void onSurfaceDestroyed();
        void onConfigurationChanged(jobject newConfig);

        void run();

        void openUrl(const std::string& url) final;

        void setScreenSaverEnabled(bool newScreenSaverEnabled) final;

        inline auto getJavaVm() const noexcept { return javaVm; }
        inline auto getMainActivity() const noexcept { return mainActivity; }
        inline auto getSurface() const noexcept { return surface; }
        inline auto getAssetManager() const noexcept { return assetManager; }

        inline auto& getFilesDirectory() const noexcept { return filesDirectory; }
        inline auto& getCacheDirectory() const noexcept { return cacheDirectory; }

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
        int looperPipe[2] = {-1, -1};

        std::string filesDirectory;
        std::string cacheDirectory;

        std::queue<std::function<void()>> executeQueue;
        std::mutex executeMutex;
    };
}

#endif // OUZEL_CORE_ENGINEANDROID_HPP

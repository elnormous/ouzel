// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

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
    class JNIErrorCategory: public std::error_category
    {
    public:
        const char* name() const noexcept override
        {
            return "JNI";
        }

        std::string message(int condition) const override
        {
            switch (condition)
            {
                case JNI_ERR: return "JNI_ERR";
                case JNI_EDETACHED: return "JNI_EDETACHED";
                case JNI_EVERSION: return "JNI_EVERSION";
                default: return "Unknown error (" + std::to_string(condition) + ")";
            }
        }
    };

    extern const JNIErrorCategory jniErrorCategory;

    class EngineAndroid final: public Engine
    {
    public:
        static constexpr jint ORIENTATION_PORTRAIT = 0x00000001;
        static constexpr jint ORIENTATION_LANDSCAPE = 0x00000002;

        EngineAndroid(JavaVM* initJavaVM);
        ~EngineAndroid();

        void onCreate(jobject initMainActivity);
        void onSurfaceCreated(jobject newSurface);
        void onSurfaceDestroyed();
        void onConfigurationChanged(jobject newConfig);

        void run();

        void executeOnMainThread(const std::function<void()>& func) override;
        void openURL(const std::string& url) override;

        void setScreenSaverEnabled(bool newScreenSaverEnabled) override;

        JavaVM* getJavaVM() const { return javaVM; }
        jobject getMainActivity() const { return mainActivity; }
        jobject getSurface() const { return surface; }
        AAssetManager* getAssetManager() const { return assetManager; }

        const std::string& getFilesDirectory() const { return filesDirectory; }
        const std::string& getCacheDirectory() const { return cacheDirectory; }

        void executeAll();

    private:
        void main() override;

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
        int looperPipe[2] = {-1, -1};

        std::string filesDirectory;
        std::string cacheDirectory;

        std::queue<std::function<void()>> executeQueue;
        std::mutex executeMutex;
    };
}

#endif // OUZEL_CORE_ENGINEANDROID_HPP

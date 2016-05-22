LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := samples
LOCAL_CFLAGS := -Wall -Wextra
LOCAL_CPPFLAGS += -std=c++11
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../../../ouzel

LOCAL_PCH := Prefix.h

LOCAL_SRC_FILES := $(LOCAL_PATH)/jni.cpp \
    $(LOCAL_PATH)/../../Application.cpp \
    $(LOCAL_PATH)/../../main.cpp

LOCAL_STATIC_LIBRARIES := ouzel
LOCAL_LDLIBS := -lGLESv1_CM -lGLESv2 -lEGL -llog -landroid

include $(BUILD_SHARED_LIBRARY)

include $(LOCAL_PATH)/../../../build/jni/Android.mk
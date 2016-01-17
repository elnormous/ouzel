LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE    := sprite
LOCAL_CFLAGS    := -Wall -Wextra
LOCAL_CPPFLAGS += -std=c++11
LOCAL_C_INCLUDES += ../../../ouzel

LOCAL_PCH := Prefix.h

LOCAL_SRC_FILES := jni.cpp \
    ../../Application.cpp \
    ../../main.cpp

LOCAL_STATIC_LIBRARIES := ouzel
LOCAL_LDLIBS := -lGLESv1_CM -lGLESv2 -lEGL -llog

include $(BUILD_SHARED_LIBRARY)

include ../../../build/android/jni/Android.mk
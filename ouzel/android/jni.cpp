// Copyright (C) 2015 Elviss Strazdins
// This file is part of the Ouzel engine.

#include <jni.h>

JNIEXPORT void JNICALL Java_lv_elviss_ouzel_OuzelLibJNIWrapper_on_1surface_1created(JNIEnv* env, jclass cls)
{
	//on_surface_created();
}

JNIEXPORT void JNICALL Java_lv_elviss_ouzel_OuzelLibJNIWrapper_on_1surface_1changed(JNIEnv* env, jclass cls, jint width, jint height)
{
	//on_surface_changed();
}

JNIEXPORT void JNICALL Java_lv_elviss_ouzel_OuzelLibJNIWrapper_on_1draw_1frame(JNIEnv* env, jclass cls)
{
	//on_draw_frame();
}

// Alternative method: use normal method names and register them in a JNI_OnLoad function:
/*
static JNINativeMethod methodTable[] = {
  {"on_surface_created", "()V", (void *) jni_on_surface_created},
  {"on_surface_changed", "(II)V", (void *) jni_on_surface_changed},
  {"on_draw_frame", "()V", (void *) jni_on_draw_frame},
};


jint JNI_OnLoad(JavaVM* vm, void* reserved)
{
    JNIEnv* env;
    if ((*vm)->GetEnv(vm, (void**)&env, JNI_VERSION_1_6) != JNI_OK)
    {
		return -1;
	}

    (*env)->RegisterNatives(env,
    						(*env)->FindClass(env, "lv/elviss/ouzel/GameLibJNIWrapper"),
    						methodTable,
    						sizeof(methodTable) / sizeof(methodTable[0]));

    return JNI_VERSION_1_6;
}
*/

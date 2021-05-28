#include "ble_interface.h"
#include <android/log.h>

void OnBLEMessageReseived(
    JNIEnv* env,
    jobject jobj,
    jstring message)
{
    const char* t_msg = env->GetStringUTFChars(message, NULL);
    __android_log_print(android_LogPriority::ANDROID_LOG_INFO, "BLE_MSG ", "%s", t_msg);
}

static JNINativeMethod ble_methods[] = {
   {"OnBLEMessageReseived", "(Ljava/lang/String;)V", (void*)OnBLEMessageReseived}
};

void InitBleInterface(JNIEnv* env)
{
    jclass cls = env->FindClass("org/luxoft/sdl_core/JavaToNativeBleAdapter");
    auto globalClass = reinterpret_cast<jclass>(env->NewGlobalRef(cls));

    int len = sizeof(ble_methods) / sizeof(ble_methods[0]);

    env->RegisterNatives(globalClass, ble_methods, len);
}
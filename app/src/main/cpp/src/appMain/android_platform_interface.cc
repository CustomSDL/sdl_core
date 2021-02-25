#include <jni.h>

#include "utils/logger.h"
#include "config_profile/profile.h"
#include "appMain/life_cycle_impl.h"

void StartSDL(JNIEnv* env, jobject);
void StopSDL(JNIEnv* env, jobject);

static JNINativeMethod s_methods[] = {
   {"StartSDL", "()V", (void*)StartSDL},
   {"StopSDL", "()V", (void*)StopSDL}
};

jint JNI_OnLoad(JavaVM* vm, void*) {
   SDL_LOG_INFO("Hello SDL");
	
   JNIEnv *env = NULL;
   vm->GetEnv((void**)&env, JNI_VERSION_1_6);

   jclass cls = env->FindClass("org/luxoft/sdl_core/MainActivity");
   jclass globalClass = reinterpret_cast<jclass>(env->NewGlobalRef(cls));

   int len = sizeof(s_methods) / sizeof(s_methods[0]);

   env->RegisterNatives(globalClass, s_methods, len);
   return JNI_VERSION_1_6;
}

void StartSDL(JNIEnv* env, jobject)
{
    SDL_LOG_INFO("Start SDL");

	profile::Profile profile_instance;
	std::unique_ptr<main_namespace::LifeCycle> life_cycle(
      new main_namespace::LifeCycleImpl(profile_instance));

    //StartComponents();
    //LInitMessageSystem();

    SDL_LOG_INFO("SDL is OK");
}
void StopSDL(JNIEnv*, jobject) {
}
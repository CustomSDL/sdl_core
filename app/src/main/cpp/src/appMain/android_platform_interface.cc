#include <jni.h>

#include "config_profile/profile.h"
#include "appMain/life_cycle_impl.h"

#ifdef ENABLE_LOG
#include "utils/logger/androidlogger.h"
#include "utils/logger/logger_impl.h"
#endif  // ENABLE_LOG

SDL_CREATE_LOCAL_LOG_VARIABLE("Main")

void StartSDL(JNIEnv* env, jobject);
void StopSDL(JNIEnv* env, jobject);

static JNINativeMethod s_methods[] = {
   {"StartSDL", "()V", (void*)StartSDL},
   {"StopSDL", "()V", (void*)StopSDL}
};

jint JNI_OnLoad(JavaVM* vm, void*) {
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
#ifdef ENABLE_LOG
    auto logger_impl =
            std::unique_ptr<logger::LoggerImpl>(new logger::LoggerImpl());
    logger::Logger::instance(logger_impl.get());
#endif  // ENABLE_LOG

	profile::Profile profile_instance;
	std::unique_ptr<main_namespace::LifeCycle> life_cycle(
      new main_namespace::LifeCycleImpl(profile_instance));

    profile_instance.set_config_file_name("/sdcard/SDL/smartDeviceLink.ini");

#ifdef ENABLE_LOG
  if (profile_instance.logs_enabled()) {
    // Logger initialization
    // Redefine for each paticular logger implementation
    auto logger = std::unique_ptr<logger::AndroidLogger>(
        new logger::AndroidLogger());
    logger_impl->Init(std::move(logger));
  }
#endif

    SDL_LOG_INFO("Application started!");
    SDL_LOG_INFO("SDL version: " << profile_instance.sdl_version());

    // Check if no error values were read from config file
    if (profile_instance.ErrorOccured()) {
        SDL_LOG_FATAL(profile_instance.ErrorDescription());
        SDL_DEINIT_LOGGER();
        exit(EXIT_FAILURE);
    }

    if (!life_cycle->StartComponents()) {
        SDL_LOG_FATAL("Failed to start components");
        life_cycle->StopComponents();
        SDL_DEINIT_LOGGER();
        return;
    }

    SDL_LOG_INFO("Components Started");

    if (!life_cycle->InitMessageSystem()) {
        SDL_LOG_FATAL("Failed to init message system");
        life_cycle->StopComponents();
        SDL_DEINIT_LOGGER();
        return;
    }

    life_cycle->Run();
    SDL_LOG_INFO("Stop SDL due to caught signal");

    life_cycle->StopComponents();
    SDL_LOG_INFO("Application has been stopped successfully");

    SDL_DEINIT_LOGGER();

    SDL_LOG_TRACE("StartSDL: exit");
}
void StopSDL(JNIEnv*, jobject) {
    SDL_LOG_INFO("Stop from main activity requested");
    // TODO: Implement sending of SIGTERM to lifecycle thread
}
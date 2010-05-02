#ifndef RONNIE_COMMON
#define RONNIE_COMMON

#if defined _WIN32 || defined __CYGWIN__
  #ifdef BUILDING_DLL
    #define RONNIE_API __declspec(dllexport)
  #else
    #define RONNIE_API __declspec(dllimport)
  #endif
#else
  #if __GNUC__ >= 4
    #define RONNIE_API __attribute__ ((visibility("default")))
  #else
    #define RONNIE_API
  #endif
#endif

#endif // RONNIE_COMMON

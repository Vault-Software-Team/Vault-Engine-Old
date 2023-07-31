#pragma once
#include <dllapi.hpp>
#ifdef _WIN32
#ifdef BUILD_DLL
#define DLL_API
#else
#define DLL_API __declspec(dllexport)
#endif
#else
#define DLL_API
#endif
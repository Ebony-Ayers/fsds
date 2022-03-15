#pragma once

#if defined(__clang__)
	#define FSDS_COMPILER_CLANG
#elif defined(__GNUC__) || defined(__GNUG__)
	#define FSDS_COMPILER_GCC
#elif defined(_MSC_VER)
	#define FSDS_COMPILER_MSVC
#else
	#define FSDS_COMPILER_UNKNOWN
#endif

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__) || defined(WIN64) || defined(_WIN64) || defined(__WIN64__)
	#define FSDS_PLATFORM_WINDOWS
#elif defined(__linux__)
	#define FSDS_PLATFORM_LINUX
#elif defined(__APPLE__)
	#define FSDS_PLATFORM_APPLE
#else
	#define FSDS_PLATFORM_UNKNOWN
#endif
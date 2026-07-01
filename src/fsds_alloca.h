#ifndef FSDS_ALLOCA_HEADER_GUARD
#define FSDS_ALLOCA_HEADER_GUARD

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__) || defined(WIN64) || defined(_WIN64) || defined(__WIN64__)
	#include <malloc.h>
	#define alloca _alloca
elif defined(__FREEBSD__) || defined(__OpenBSD__) || defined(__NetBSD__) || defined(__DragonFly__)
	#include <stdlib.h>
#else
	#include <alloca.h>
#endif

#endif //FSDS_ALLOCA_HEADER_GUARD
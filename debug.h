#ifndef DEBUG_HGUARD_kgbw9712
#define DEBUG_HGUARD_kgbw9712
// debug.h
// Created by 080004721, 2011.

#define DEBUG 0
#define debug_printf(fmt, ...)                                     \
	do {                                                           \
		if (DEBUG)                                                 \
			fprintf(stderr, "Debug %4lu: " fmt "\n", debug_line++, \
					##__VA_ARGS__);                                \
	} while (0)

unsigned long debug_line = 0l;

#endif

/*
 * standard.h : This header contains standard defines for program operation
 */

#ifndef STANDARD_INCLUDE

#include "adsinc.h"

#ifndef BOOLEAN_MAX

typedef enum {
	Fal,
	Tru,
	BOOLEAN_MAX
} BOOLEAN;

#endif /* True */

#ifndef DIRECTION_MAX

typedef enum {
	Left,
	Right,
	DIRECTION_MAX
} DIRECTION;

#endif /* Left */

#define STANDARD_INCLUDE

#endif 	/* STANDARD_INCLUDE */


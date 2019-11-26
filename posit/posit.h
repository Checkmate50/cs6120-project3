#ifndef POSIT_TYPE

#include <stdbool.h>
#include <stdint.h>

#define signP32UI( a ) ((bool) ((uint32_t) (a)>>31))
#define signregP32UI( a ) ((bool) (((uint32_t) (a)>>30) & 0x1))
#define packToP32UI(regime, expA, fracA) ( (uint32_t) regime + (uint32_t) expA + ((uint32_t)(fracA)) )

typedef struct { uint32_t v; bool exact; } posit32_t;
union ui32_p32 { uint32_t ui; posit32_t p; };
union ui64_double { uint64_t ui; double d; };

#endif
#define POSIT_TYPE

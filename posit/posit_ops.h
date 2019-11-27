#include "posit.h"

posit32_t p32_add( posit32_t a, posit32_t b );
posit32_t p32_sub( posit32_t a, posit32_t b );
posit32_t p32_mul( posit32_t a, posit32_t b );
posit32_t p32_div( posit32_t a, posit32_t b );
/* posit32_t p32_sqrt( posit32_t a ); */

bool p32_eq( posit32_t a, posit32_t b );
bool p32_le( posit32_t a, posit32_t b );
bool p32_lt( posit32_t a, posit32_t b );

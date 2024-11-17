#ifndef TGN_CMN_H
#define TGN_CMN_H

#include <stddef.h>
#include <inttypes.h>

#include "fs.h"

typedef const char* str_t;


void debug_err(str_t format, ...);
void debug_wrn(str_t format, ...);
void debug_log(str_t format, ...);

void assert(bool statement, str_t msg, ...);


// add '|' behind to extend to multiple flags
// (1 << 0) example on how to set a flag

#define flag_set(flags, FLAG) flags |= FLAG 
#define flag_rem(flags, FLAG) flags &= ~FLAG
#define flag_toggle(flags, FLAG) flags ^= FLAG
#define flag_active(flags, FLAG) flags & FLAG
#define flag_not_active(flags, FLAG) !(flags & FLAG)
#define flag_any(flags, FLAG1, FLAG2) flags & (FLAG1 | FLAG2)

#endif // !TGN_CMN_H

#ifndef TGN_CMN_H
#define TGN_CMN_H

#include <stddef.h>
#include <inttypes.h>

#include "fs.h"

typedef const char* str_t;


void __debug_err(const char* file, int line, str_t format, ...);
void __debug_wrn(const char* file, int line, str_t format, ...);
void __debug_log(const char* file, int line, str_t format, ...);

#define debug_err(fmt, ...) __debug_err( __FILE__, __LINE__,fmt, ##__VA_ARGS__)
#define debug_wrn(fmt, ...) __debug_wrn( __FILE__, __LINE__,fmt, ##__VA_ARGS__)
#define debug_log(fmt, ...) __debug_log( __FILE__, __LINE__,fmt, ##__VA_ARGS__)

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

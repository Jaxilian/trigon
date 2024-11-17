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


#endif // !TGN_CMN_H

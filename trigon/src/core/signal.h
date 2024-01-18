#ifndef TRIGON_SIGNAL_H
#define TRIGON_SIGNAL_H
#include <inttypes.h>
typedef void (*signal_cb)();

typedef enum {
	ON_START_SIGNAL,
	ON_UPDATE_SIGNAL,
	ON_DRAW_SIGNAL,
	ON_QUIT_SIGNAL,
	ON_WINDOW_RESIZED_SIGNAL,
	SIGNAL_TYPE_COUNT
}SIGNAL_TYPE;

void signal_init();
void signal_clear();

uint32_t	signal_connect( SIGNAL_TYPE type, signal_cb cb);
void		signal_disconnect(SIGNAL_TYPE type, uint32_t id);
void		signal_fire(SIGNAL_TYPE type);

#endif
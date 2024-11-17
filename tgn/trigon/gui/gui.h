#ifndef TGN_GUI_H
#define TGN_GUI_H

#include "trigon/math/math.h"

typedef struct {
	char	name[16];
	vec2	pos;
	vec2	anchor;
	vec2	scale;
	vec4	colro;
	float	rotation;
} gui_canvas_t;

void gui_canvas_new(gui_canvas_t* out);
void gui_canvas_del(gui_canvas_t* in);

#endif // !TGN_GUI_H

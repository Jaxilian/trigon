#ifndef GUI_H
#define GUI_H
#include <stdbool.h>
#include <vkl/vkl.h>
#include <cglm/cglm.h>

typedef struct {
    vec2 pos;
    vec2 uv;
} gui_vertex_t;

typedef struct {
    mat4 matrix;
    vec3 colors;
} gui_quad_t;


bool gui_new();
void gui_pipeline_del();
void gui_draw();
void gui_rebuild();
void gui_add(gui_quad_t* data);
void gui_submit();
bool gui_draw_quad();

#endif
#ifndef TGSRV_H
#define TGSRV_H

void srv_renderer_start();
void srv_renderer_stop();

void srv_renderer_event_poll();
void srv_renderer_frame_begin();
void srv_renderer_frame_end();

#endif // !TGSRV_H

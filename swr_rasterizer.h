#ifndef _SWR_RASTERIZER_H_
#define _SWR_RASTERIZER_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "swr_sdl_context.h"
/*
Rasterizer 2D coordinate system:
Origin: at top-left coorner of the screen
X: increases from left to right
Y: increases from top to bottom
*/

/* set the context (struct for screen surface pixels, width, height etc) */
void rasterizer_set_swr_sdl_context(swr_sdl_context* ptr_to_swr_sdl_context);

/* set current drawing color */
void rasterizer_set_cur_color(int r, int g, int b, int a);

/* set current clear color */
void rasterizer_set_clear_color(int r, int g, int b, int a);

/* enable or disable alpha blending */
void enable_alpha_blending(int flag);

/* put pixel */
void rasterizer_put_pixel(int x, int y);
void rasterizer_put_pixel2(int x, int y, swr_color *pc);

/* draw line */
void rasterizer_draw_line_dda(int x0, int y0, int x1, int y1);

/* draw line */
void rasterizer_draw_line_bres(int x0, int y0, int x1, int y1);

/* draw rectangle */
void rasterizer_draw_rect(int left, int top, int bottom, int right);

/* fill rectangle */
void rasterizer_fill_rect(int x0, int y0, int x1, int y1);

/* copy pixels to screen */
void rasterizer_copy_pixels(int dst_x_start, int dst_y_start, int src_width, int src_height, unsigned char* src_pixels);

/* clear screen */
void rasterizer_clear();

#ifdef __cplusplus
}
#endif

#endif


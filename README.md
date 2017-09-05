# GraphicsLabFramework
A cross platform framework for implementing and  
studying basic algorithms in computer graphics.  

File Info:  
GraphicsLabFramework.zip (size: 730KB)  

Useful for:  
- Writing lab programs in a first course for computer graphics  (pixel, line, polygons, circle, filling, clipping, transforms etc.)
- Writing raytracers  
- Writing programs for advanced curves  
- Writing software rasterizers   
- Writing image processing routines
- and so on.  
  
Building:  
You need to make a project using the files in the repository  
and resolve dependencies with SDL2 Library (available at https://www.libsdl.org)  
in order to run the program.  
  
Core:  
The core of this program is a function which calls  
a display callback (which may modify a the framebuffer pixel array)   
and renders the framebuffer pixel array to a screen aligned quad.  
  
Facilities:  
Access to Display Context and Frame buffer  
Callback function support for Display and Input  
Drawing Lines and Rectangles  
Reading and Writing PPM files   
Writing the framebuffer to PPM file    
Generating Procedural Image (Checker and Gradient)  
Copying pixels to framebuffer with alpha blending  
  
How to use the framework:  
Example code: Drawing a line  
```C
#include <stdio.h>
#include <math.h>
#include "swr_sdl_window.h"
#include "swr_rasterizer.h"

swr_sdl_context* ctx;

int main(int argc, char **argv)
{
	swr_sdl_create_context(640, 480);
	ctx = swr_sdl_get_context();

	rasterizer_set_swr_sdl_context(ctx);
	rasterizer_set_cur_color(255, 255, 255, ctx->opaque_alpha_value);
	rasterizer_set_clear_color(0, 0, 128, ctx->transparent_alpha_value);
	
	rasterizer_clear();
	rasterizer_draw_line_bres(0, 0, 300, 400);
	swr_sdl_render_screen_texture();
	swr_sdl_wait_for_events();

	swr_sdl_destroy_context();
	return 0;
}
```
Example code: Animating the rotation of a line     
```C
#include <stdio.h>
#include <math.h>
#include "swr_sdl_window.h"
#include "swr_rasterizer.h"

swr_sdl_context* ctx;

void rotate_line()
{
	int xc = 640 / 2;
	int yc = 480 / 2;
	double x, y;
	static double theta = 0.0;
	if (theta <= 2 * M_PI) {
		x = xc + 50.0 * cos(theta);
		y = yc + 50.0 * sin(theta);
		rasterizer_draw_line_bres(xc, yc, (int)x, (int)y);
		theta += M_PI / 180;
	} else {
		theta = 0;
	}
}

void display(swr_sdl_context *ctx)
{	
	rasterizer_clear();
	rotate_line();
}

int main(int argc, char **argv)
{
	swr_sdl_create_context(640, 480);
	swr_sdl_set_display_handler(display);
	ctx = swr_sdl_get_context();

	rasterizer_set_swr_sdl_context(ctx);
	rasterizer_set_cur_color(255, 255, 255, ctx->opaque_alpha_value);
	rasterizer_set_clear_color(0, 0, 128, ctx->transparent_alpha_value);
	
	swr_sdl_main_loop();

	swr_sdl_destroy_context();
	return 0;
}
```
Example Code: Inverting an Image  
```C
#include <stdio.h>
#include <math.h>
#include "swr_sdl_window.h"
#include "swr_rasterizer.h"
#include "swr_image.h"

swr_sdl_context* ctx;
unsigned char *realimage;
int realimagewidth;
int realimageheight;

void invert_image(unsigned char *img, int width, int height)
{
	unsigned char* loc = img;
	int n = width * height;
	int i;
	int r, g, b;
	for (i = 0; i < n; ++i)
	{
		*loc = 255 - *loc++;
		*loc = 255 - *loc++;
		*loc = 255 - *loc++;
		++loc;
	}
}

int main(int argc, char **argv)
{
	swr_sdl_create_context(640, 480);
	ctx = swr_sdl_get_context();

	rasterizer_set_swr_sdl_context(ctx);
	rasterizer_set_cur_color(255, 255, 255, ctx->opaque_alpha_value);
	rasterizer_set_clear_color(0, 0, 128, ctx->transparent_alpha_value);

	realimage = read_ppm_raw("Lenaclor.ppm", LE, &realimagewidth, &realimageheight);
	

	rasterizer_clear();
	
	rasterizer_copy_pixels(0, 0, realimagewidth, realimageheight, realimage);
	swr_sdl_render_screen_texture();
	swr_sdl_wait_for_events();
	
	invert_image(realimage, realimagewidth, realimageheight);

	rasterizer_copy_pixels(0, 0, realimagewidth, realimageheight, realimage);
	swr_sdl_render_screen_texture();
	swr_sdl_wait_for_events();

	/*
	Capture screen:
	Save framebuffer to a datetime stamped PPM image file
	*/
	write_ppm_raw(NULL, ctx->screen_texture_pixels,
		ctx->screen_texture_pixels_wide,
		ctx->screen_texture_pixels_high);


	swr_sdl_destroy_context();
	return 0;
}
```

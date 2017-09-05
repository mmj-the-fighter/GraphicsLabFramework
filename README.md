# GraphicsLabFramework
A cross platform framework for implementing and 
studying basic algorithms in computer graphics.  
  
Uses:  
- for writing lab programs in a first course for computer graphics  
- for writing raytracers.  
- for writing programs for curves  
- for writing software rasterizers   
- and so on.  
  
Building:  
You need to make a project using this files and resolve dependencies  
with SDL2 Library (available at https://www.libsdl.org)  
in order to run this.  

Example Usage:
  
Core:  
The core of this program is a function which calls  
a display callback (which may modify a pixel array)   
and renders the pixels to a screen aligned quad.  
  
Facilities:  
Access to Frame buffer  
Display Callback  
Input Callback  
Line  
Rectangle  
PPM Image  
Procedural Image (Checker and Gradient)  
Copying pixels to framebuffer with alpha blending  


How to use the framework:  
Example code   
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
	static double i = 0.0;
	if (i <= 2 * M_PI) {
		x = xc + 50.0 * cos(i);
		y = yc + 50.0 * sin(i);
		rasterizer_draw_line_bres(xc, yc, (int)x, (int)y);
		i += M_PI / 180;
	} else {
		i = 0;
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

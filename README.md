# GraphicsLabFramework
A cross platform framework for implementing and  
studying basic algorithms in computer graphics.
It uses SDL2 library.
  
License:  
MIT License  
  
Useful for:  
- Writing lab programs in a first course for computer graphics  
  (pixels, lines, polygons, circles,  ellipses, filling, clipping, transforms etc.)  
- Writing raytracers  
- Writing programs for advanced curves  
- Writing software rasterizers   
- Writing image processing routines  
- and so on.  
  
Building:  
You need to make a project using the files in the repository  
and resolve dependencies with SDL2 Library (available at https://www.libsdl.org)  
in order to run the program.  
  
Note:  
Images format used for this project is PPM.  
You may need programs like GIMP, IrfanView or Photoshop to work with PPM files.  
Upate(4-Feb-2024): Support for loading png files added using the stb_image library  
  
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
  
*How to use the framework:* 

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

void rotating_line(swr_sdl_context *ctx)
{
	char buf[256];
	int xc = 640 / 2;
	int yc = 480 / 2;
	double x, y;
	double omega = 6 * (M_PI / 180);
	static double theta = 0.0;
	if (theta <= 2 * M_PI) {
		x = xc + 50.0 * cos(theta);
		y = yc + 50.0 * sin(theta);
		rasterizer_draw_line_bres(xc, yc, x, y);
		//rasterizer_draw_line_dda(xc, yc, (int)x, (int)y);
		theta += omega * ctx->lastFrameTime;
	}
	else {
		theta = 0;
	}
	sprintf(buf, "rotation angle %f", (theta*180.0 / M_PI));
	rasterizer_draw_text(font, 100, 200, buf);
}

void display(swr_sdl_context *ctx)
{	
	rasterizer_clear();
	rotate_line(ctx);
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

	destroy_image(realimage);
	swr_sdl_destroy_context();
	return 0;
}
```
Example Code: Blurring an Image  
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

/*applying a box filter of size 3x3*/
void blur_image(unsigned char *img, int width, int height)
{
	int n = width * height;
	int i, j;
	int r, g, b;
	int x, y;
	float v = 1.0 / 9.0;
	float kernel[3][3] =
	{
		{ v, v, v },
		{ v, v, v },
		{ v, v, v }
	};
	unsigned char* resimage = (unsigned char *)malloc(width * height * 4 * sizeof(unsigned char));
	memcpy(resimage, img, width*height * 4);
	
	for (y = 1; y < height - 1; ++y) {
		for (x = 1; x < width - 1; ++x) {
			float bs = 0.0;
			float gs = 0.0;
			float rs = 0.0;
			for (i = -1; i <= 1; ++i) {
				for (j = -1; j <= 1; ++j){
					float weight = (float)kernel[i + 1][j + 1];
					unsigned char* buffer = img + width * 4 * (y + j) + (x + i) * 4;
					bs += weight * *buffer;
					gs += weight * *(buffer + 1);
					rs += weight * *(buffer + 2);
				}
			}
			unsigned char* outbuffer = resimage + width * 4 * y + x * 4;
			*outbuffer = bs;
			*(outbuffer + 1) = gs;
			*(outbuffer + 2) = rs;
			*(outbuffer + 3) = 255;
		}
	}
	memcpy(img, resimage, width*height * 4);
	free(resimage);
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

	blur_image(realimage, realimagewidth, realimageheight);

	rasterizer_copy_pixels(0, 0, realimagewidth, realimageheight, realimage);
	swr_sdl_render_screen_texture();
	swr_sdl_wait_for_events();

	/*
	Capture screen:
	Save framebuffer to a datetime stamped PPM image file
	*/
	/*	
	write_ppm_raw(NULL, ctx->screen_texture_pixels,
		ctx->screen_texture_pixels_wide,
		ctx->screen_texture_pixels_high);
	*/
	destroy_image(realimage);
	swr_sdl_destroy_context();
	return 0;
}
```
Example Code: Thresholding an Image  
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

void threshold_image(unsigned char *img, int width, int height)
{
	unsigned char* buf = img;
	int n = width * height;
	int i;
	int r, g, b;
	float sum=0;
	long count=0;
	float avg_intensity;

	//find threshold
	for (i = 0; i < n; ++i)
	{
		b = *buf;
		g = *(buf + 1);
		r = *(buf + 2);
		float intensity = (float)((b + g + r)) / 3.0f;
		sum += intensity;
		++count;
		buf += 4;
	}
	avg_intensity = sum / (float)count;

	//make the image into a binary image based on the threshold
	buf = img;
	for (i = 0; i < n; ++i)
	{
		b = *buf;
		g = *(buf + 1);
		r = *(buf + 2);
		float intensity = (float)((b + g + r)) / 3.0f;

		if (intensity < avg_intensity)
		{
			*buf++ = 0;
			*buf++ = 0;
			*buf++ = 0;
			*buf++ = 255;
		}
		else
		{
			*buf++ = 255;
			*buf++ = 255;
			*buf++ = 255;
			*buf++ = 255;
		}
	}
}



int main(int argc, char **argv)
{
	realimage = read_ppm_raw("Lenaclor.ppm", LE, &realimagewidth, &realimageheight);

	swr_sdl_create_context(realimagewidth, realimageheight);
	ctx = swr_sdl_get_context();

	rasterizer_set_swr_sdl_context(ctx);
	rasterizer_set_cur_color(255, 255, 255, ctx->opaque_alpha_value);
	rasterizer_set_clear_color(0, 0, 128, ctx->transparent_alpha_value);

	


	rasterizer_clear();

	rasterizer_copy_pixels(0, 0, realimagewidth, realimageheight, realimage);
	swr_sdl_render_screen_texture();
	swr_sdl_wait_for_events();

	threshold_image(realimage, realimagewidth, realimageheight);

	rasterizer_copy_pixels(0, 0, realimagewidth, realimageheight, realimage);
	swr_sdl_render_screen_texture();
	swr_sdl_wait_for_events();

	/*
	Capture screen:
	Save framebuffer to a datetime stamped PPM image file
	*/
	/*	
	write_ppm_raw(NULL, ctx->screen_texture_pixels,
		ctx->screen_texture_pixels_wide,
		ctx->screen_texture_pixels_high);
		*/

	destroy_image(realimage);
	swr_sdl_destroy_context();
	return 0;
}
```
Example Code: Detecting edges of an image using Sobel filter  
```C
#include <stdio.h>
#include <string.h>
#include <math.h>
#include "swr_sdl_window.h"
#include "swr_rasterizer.h"
#include "swr_image.h"

swr_sdl_context* ctx;
unsigned char *realimage;
int realimagewidth;
int realimageheight;


void sobel_edge_detect(unsigned char *img, int width, int height)
{
	int n = width * height;
	int i, j;
	int r, g, b;
	int x, y;

	float sobel_x[3][3] =
	{
		{ -1, 0, 1 },
		{ -2, 0, 2 },
		{ -1, 0, 1 }
	};

	float sobel_y[3][3] =
	{
		{ -1, -2, -1 },
		{ 0, 0, 0 },
		{ 1, 2, 1 }
	};


	unsigned char* resimage = (unsigned char *)malloc(width * height * 4 * sizeof(unsigned char));
	memcpy(resimage, img, width*height * 4);

	for (y = 1; y < height - 1; ++y) {
		for (x = 1; x < width - 1; ++x) {
			
			float sx = 0.0f;
			float sy = 0.0f;
			for (i = -1; i <= 1; ++i) {
				for (j = -1; j <= 1; ++j){
					unsigned char* buffer = img + width * 4 * (y + j) + (x + i) * 4;
					b = *buffer;
					g = *(buffer + 1);
					r = *(buffer + 2);
					float luminance = 0.3f * r + 0.59f * g + 0.11f * b;
					sx += sobel_x[i + 1][j + 1] * luminance;
				}
			}
			for (i = -1; i <= 1; ++i) {
				for (j = -1; j <= 1; ++j){
					unsigned char* buffer = img + width * 4 * (y + j) + (x + i) * 4;
					b = *buffer;
					g = *(buffer + 1);
					r = *(buffer + 2);
					float luminance = 0.3f * r + 0.59f * g + 0.11f * b;
					sy += sobel_y[i + 1][j + 1] * luminance;
				}
			}
			int s = (int) (sqrt(sx*sx+sy*sy));
			if (s < 0) {
				s = 0;
			}
			else if (s > 255) {
				s = 255;
			}
			unsigned char* outbuffer = resimage + width * 4 * y + x * 4;
			*outbuffer = s;
			*(outbuffer + 1) = s;
			*(outbuffer + 2) = s;
			*(outbuffer + 3) = 255;
		}
	}
	memcpy(img, resimage, width*height * 4);
	free(resimage);
}

int main(int argc, char **argv)
{
	realimage = read_ppm_raw("Lenaclor.ppm", LE, &realimagewidth, &realimageheight);
	
	swr_sdl_create_context(realimagewidth, realimageheight);
	ctx = swr_sdl_get_context();
	rasterizer_set_swr_sdl_context(ctx);

	rasterizer_copy_pixels(0, 0, realimagewidth, realimageheight, realimage);
	swr_sdl_render_screen_texture();
	swr_sdl_wait_for_events();
	
	sobel_edge_detect(realimage, realimagewidth, realimageheight);
	rasterizer_copy_pixels(0, 0, realimagewidth, realimageheight, realimage);
	swr_sdl_render_screen_texture();
	swr_sdl_wait_for_events();
	
	destroy_image(realimage);
	swr_sdl_destroy_context();
	return 0;
}
```

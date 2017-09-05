/*
Dependencies: SDL2 Library
https://www.libsdl.org/
*/

#include <stdio.h>
#include <math.h>
#include "swr_sdl_window.h"
#include "swr_rasterizer.h"
#include "swr_pixel.h"
#include "swr_image.h"

swr_sdl_context *ctx;
unsigned char *checkerimage;
unsigned char *gradientimage;
unsigned char *realimage;
int realimagewidth;
int realimageheight;

void rotating_line()
{
	int xc = 640 / 2;
	int yc = 480 / 2;
	double x, y;
	static double i = 0.0;
	if (i <= 2 * M_PI)
	{
		x = xc + 50.0 * cos(i);
		y = yc + 50.0 * sin(i);
		/* rasterizer_draw_line_bres(xc, yc, x, y); */
		rasterizer_draw_line_dda(xc, yc, (int)x, (int)y);
		i += M_PI / 180;
	}
	else{
		i = 0;
	}
}

void test_point_line()
{
	int xc = 640 / 2;
	int yc = 480 / 2;
	rasterizer_draw_line_bres(xc, yc, xc, yc);
}

void test_line()
{
	int xc = 640 / 2;
	int yc = 480 / 2;
	double x, y;
	double i = 0.0;
	while (i <= 2*M_PI)
	{
		x = xc + 50.0 * cos(i);
		y = yc + 50.0 * sin(i);
		rasterizer_draw_line_bres(xc, yc, x, y);
		i += M_PI/8;
	}
}

void moving_gradient(swr_sdl_context *ctx)
{
	static int x = 0;
	rasterizer_copy_pixels(x, 0, 256, 256, gradientimage);
	if (++x > ctx->screen_texture_pixels_wide)
		x = 0;
}

void moving_checker(swr_sdl_context *ctx)
{
	static int x = 50;
	rasterizer_copy_pixels(x, 600-256, 256, 256, checkerimage);
	if (++x > ctx->screen_texture_pixels_wide)
		x = 0;
}

void moving_lena(swr_sdl_context *ctx)
{
	static int x = 0;
	rasterizer_copy_pixels(x, 0, realimagewidth, realimageheight, realimage);
	if (++x > ctx->screen_texture_pixels_wide)
		x = 0;
}

void unit_tests(swr_sdl_context *ctx)
{
	rasterizer_clear();	
	test_point_line();
	test_line();
	moving_lena(ctx);
	moving_checker(ctx);
	moving_gradient(ctx);
	rasterizer_draw_rect(10, 10, 640 - 10, 480 - 10);
	rotating_line();
}

void capture_screen()
{
	write_ppm_raw(NULL, ctx->screen_texture_pixels,
		ctx->screen_texture_pixels_wide,
		ctx->screen_texture_pixels_high);
}

void display(swr_sdl_context *ctx)
{	
	unit_tests(ctx);	
}

int input(SDL_Event* e)
{
	if (e->type == SDL_KEYDOWN)
	{
		switch (e->key.keysym.scancode)
		{
		case SDL_SCANCODE_ESCAPE:
			return 1;
		case SDL_SCANCODE_F12:
			capture_screen();
			break;
		}
	}
	else
	{
		if (e->type == SDL_MOUSEBUTTONDOWN)
		{
			printf("%d %d %d down\n", e->button.button,e->button.x,e->button.y);
		}
	}
	return 0;
}

int main(int argc, char **argv)
{
	checkerimage = create_checker_image(256, 256, 32, 32, 
		255, 255, 255, 0, 
		0, 0, 0, 255);

	gradientimage = create_gradient_image(0, 0, 0, 255, 255, 255, 255, 0, 256, 256);

	/* 
	image source: https://homepages.cae.wisc.edu/~ece533/images/Lenaclor.ppm 
	*/
	realimage = read_ppm_raw("Lenaclor.ppm",LE, &realimagewidth, &realimageheight);
	
	swr_sdl_create_context(800, 600);
	swr_sdl_set_input_handler(input);
	swr_sdl_set_display_handler(display);
	ctx = swr_sdl_get_context();

	rasterizer_set_swr_sdl_context(ctx);
	rasterizer_set_cur_color(255, 255, 255, ctx->opaque_alpha_value);
	rasterizer_set_clear_color(0, 0, 128, ctx->transparent_alpha_value);
	rasterizer_clear();

	/*
	test_point_line();
	swr_sdl_render_screen_texture();
	swr_sdl_wait_for_events();
	*/

	/*
	test_line();
	swr_sdl_render_screen_texture();
	swr_sdl_wait_for_events();
	*/

	enable_alpha_blending(1);
	swr_sdl_main_loop();
	
	destroy_image(realimage);
	destroy_image(gradientimage);
	destroy_image(checkerimage);

	swr_sdl_destroy_context();
	return 0;
}
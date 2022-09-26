#include <stdio.h>
#include <string.h>
#include <math.h>
#include "swr_sdl_window.h"
#include "swr_rasterizer.h"
#include "swr_image.h"
#include "swr_rfont.h"

swr_sdl_context* ctx;
unsigned char *realimage;
unsigned char *checkerimage;
unsigned char *gradientimage;
int realimagewidth;
int realimageheight;
swr_rfont* font;
swr_color textcolor = { 0, 0, 255, 255 };

void rotating_line()
{
	char buf[256];
	int xc = 640 / 2;
	int yc = 480 / 2;
	double x, y;
	static double i = 0.0;
	if (i <= 2 * M_PI) {
		x = xc + 50.0 * cos(i);
		y = yc + 50.0 * sin(i);
		/* rasterizer_draw_line_bres(xc, yc, x, y); */
		rasterizer_draw_line_dda(xc, yc, (int)x, (int)y);
		i += M_PI / 180;
	}
	else {
		i = 0;
	}
	sprintf(buf, "rotation angle %f", (i*180.0/M_PI));
	rasterizer_draw_text(font, 100, 200, buf);
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
	while (i <= 2 * M_PI)
	{
		x = xc + 50.0 * cos(i);
		y = yc + 50.0 * sin(i);
		rasterizer_draw_line_bres(xc, yc, x, y);
		i += M_PI / 8;
	}
}

void moving_gradient(swr_sdl_context *ctx)
{
	static int x = 0;
	rasterizer_copy_pixels(x, 0, 256, 256, gradientimage);
	if (++x > ctx->screen_texture_pixels_wide) {
		x = 0;
	}
}

void moving_checker(swr_sdl_context *ctx)
{
	static int x = 0;
	//rasterizer_copy_pixels(x, 600 - 256, 256, 256, checkerimage);
	rasterizer_copy_pixels_subimage(x, 600 - 256, 100, 100, 100, 100, 256, 256, checkerimage);
	if (++x > ctx->screen_texture_pixels_wide) {
		x = 0;
	}
}

void moving_lena(swr_sdl_context *ctx)
{
	char buf[256];
	static int x = 0;
	rasterizer_copy_pixels(x, 0, realimagewidth, realimageheight, realimage);
	//rasterizer_copy_pixels_subimage(x, 0, 100, 100, 256, 256, realimagewidth, realimageheight, realimage);
	if (++x > ctx->screen_texture_pixels_wide) {
		x = 0;
	}
	sprintf(buf, "Lena pos %d", x);
//	rasterizer_draw_text(font, 100, 80, buf);
	rasterizer_draw_text_with_color(font, &textcolor, 100, 80, buf);
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
	rasterizer_draw_text(font, 100, 20, "Unit Tests");
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
	if (e->type == SDL_KEYDOWN) {
		switch (e->key.keysym.scancode)
		{
		case SDL_SCANCODE_ESCAPE:
			return 1;
		case SDL_SCANCODE_F12:
			capture_screen();
			break;
		}
	}
	else {
		if (e->type == SDL_MOUSEBUTTONDOWN) {
			printf("%d %d %d down\n", e->button.button, e->button.x, e->button.y);
		}
	}
	return 0;
}


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

	for (x = 1; x < width - 1; ++x) {
		for (y = 1; y < height - 1; ++y) {

			float sx = 0.0f;
			float sy = 0.0f;
			for (i = -1; i <= 1; ++i) {
				for (j = -1; j <= 1; ++j) {
					unsigned char* buffer = img + width * 4 * (y + j) + (x + i) * 4;
					b = *buffer;
					g = *(buffer + 1);
					r = *(buffer + 2);
					float luminance = 0.3f * r + 0.59f * g + 0.11f * b;
					sx += sobel_x[i + 1][j + 1] * luminance;
				}
			}
			for (i = -1; i <= 1; ++i) {
				for (j = -1; j <= 1; ++j) {
					unsigned char* buffer = img + width * 4 * (y + j) + (x + i) * 4;
					b = *buffer;
					g = *(buffer + 1);
					r = *(buffer + 2);
					float luminance = 0.3f * r + 0.59f * g + 0.11f * b;
					sy += sobel_y[i + 1][j + 1] * luminance;
				}
			}
			int s = (int)(sqrt(sx*sx + sy*sy));
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
	checkerimage = create_checker_image(256, 256, 32, 32,
		255, 255, 255, 0,
		0, 0, 0, 255);

	gradientimage = create_gradient_image(0, 0, 0, 255, 255, 255, 255, 0, 256, 256);

	font = create_font("glyphs.ppm", "glyphs.csv");
	

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

	rasterizer_clear();
	
	
	rasterizer_copy_pixels(0, 0, realimagewidth, realimageheight, realimage);
	rasterizer_draw_text(font, 100, 20, "Base Image");
	swr_sdl_render_screen_texture();
	swr_sdl_wait_for_events();

	sobel_edge_detect(realimage, realimagewidth, realimageheight);
	rasterizer_copy_pixels(0, 0, realimagewidth, realimageheight, realimage);
	rasterizer_draw_text(font, 100, 20, "After applying Sobel Filter");
	swr_sdl_render_screen_texture();
	swr_sdl_wait_for_events();
	
	destroy_image(gradientimage);
	destroy_image(checkerimage);
	destroy_image(realimage);
	destroy_font(font);
	swr_sdl_destroy_context();
	return 0;
}

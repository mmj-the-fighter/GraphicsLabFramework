#include <stdio.h>
#include <string.h>
#include <math.h>
#include "swr_sdl_window.h"
#include "swr_rasterizer.h"
#include "swr_image.h"
#include "swr_rfont.h"

swr_sdl_context* ctx;
unsigned char *realimage;
unsigned char *realimage_clone;
unsigned char *checkerimage;
unsigned char *gradientimage;


unsigned char *birdimage;
int birdimagewidth, birdimageheight;

int realimagewidth;
int realimageheight;
swr_rfont* font;
swr_color textcolor = { 255, 255, 255, 255 };

float velx = 100.0f;

int dx = 0, dy = 0;

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
	static float x = -256;
	rasterizer_copy_pixels(x, 0, 256, 256, gradientimage);
	x += velx * ctx->lastFrameTime;
	if (x > ctx->screen_texture_pixels_wide) {
		x = -256;
	}
}

void moving_checker(swr_sdl_context *ctx)
{
	static float x = -256;
	rasterizer_copy_pixels(x, 600 - 256, 256, 256, checkerimage);
	x += velx * ctx->lastFrameTime;
	if (x > ctx->screen_texture_pixels_wide) {
		x = -256;
	}
}

void moving_lena(swr_sdl_context *ctx)
{
	char buf[256];
	static float x = -256;
	rasterizer_copy_pixels(x, 0, realimagewidth, realimageheight, realimage);
	//rasterizer_copy_pixels_subimage(x, 0, 100, 100, 256, 256, realimagewidth, realimageheight, realimage);
	x += velx * ctx->lastFrameTime;
	if (x > ctx->screen_texture_pixels_wide) {
		x = -256;
	}
	sprintf(buf, "Lena pos %d", x);
//	rasterizer_draw_text(font, 100, 80, buf);
	rasterizer_draw_text_with_color(font, &textcolor, 100+dx, 80+dy, buf);
}

void movable_text(swr_sdl_context *ctx)
{
	rasterizer_draw_text(font, 100+dx, 20+dy, "Unit Tests");
}

void movable_bird(swr_sdl_context *ctx)
{
	swr_color green = {0,255,0,255};
	rasterizer_copy_pixels_chromakey(dx, dy, birdimagewidth, birdimageheight, &green, birdimage);
}

void movable_portionofimage(swr_sdl_context *ctx)
{
	rasterizer_copy_pixels_subimage(200+dx, 200+dy, 238, 248, 53, 33, realimagewidth, realimageheight, realimage);
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
	rotating_line(ctx);
	movable_text(ctx);
	movable_bird(ctx);
	movable_portionofimage(ctx);
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
		case SDL_SCANCODE_LEFT:
			--dx;
			break;
		case SDL_SCANCODE_RIGHT:
			++dx;
			break;
		case SDL_SCANCODE_UP:
			--dy;
			break;
		case SDL_SCANCODE_DOWN:
			++dy;
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

//Thanks to https://www.geeksforgeeks.org/histogram-equalisation-in-c-image-processing/
void histogram_equalize(unsigned char *img, int width, int height) 
{
	int histogramRed[256];
	int histogramGreen[256];
	int histogramBlue[256];
	int newRed[256];
	int newGreen[256];
	int newBlue[256];
	int i = 0;
	int x, y;
	int r, g, b;
	float total_num_of_pixels = (float)(width * height);
	int redSum = 0, blueSum = 0, greenSum = 0;

	//init
	for (i = 0; i < 256; ++i) {
		histogramRed[i] = histogramGreen[i] = histogramBlue[i] = 0;
		newRed[i] = newGreen[i] = newBlue[i] = 0;
	}

	//calculate histograms
	for (y = 0; y < height; ++y) {
		for (x = 0; x < width; ++x) {
			unsigned char* buffer = img + (width * 4 * y) + (x * 4);
			b = *buffer;
			g = *(buffer + 1);
			r = *(buffer + 2);
			histogramRed[r]++;
			histogramGreen[g]++; 
			histogramBlue[b]++;
		}
	}

	//calculate new colors
	redSum = 0;
	blueSum = 0;
	greenSum = 0;
	for (i = 0; i < 256; i++) {
		redSum += histogramRed[i];
		newRed[i] = round((((float)redSum) * 255.0f) / total_num_of_pixels);
		greenSum += histogramGreen[i];
		newGreen[i] = round((((float)greenSum) * 255.0f) / total_num_of_pixels);
		blueSum += histogramBlue[i];
		newBlue[i] = round((((float)blueSum) * 255.0f) / total_num_of_pixels);
	}

	//equalize
	for (y = 0; y < height; ++y) {
		for (x = 0; x < width; ++x) {
			unsigned char* buffer = img + (width * 4 * y) + (x * 4);
			b = *buffer;
			g = *(buffer + 1);
			r = *(buffer + 2);
			*buffer = newBlue[b];
			*(buffer + 1) = newGreen[g];
			*(buffer + 2) = newRed[r];
		}
	}

}

/*apply box filter of size NxN */
void blur_image_boxfilter(unsigned char *img, int kernelSize, int width, int height)
{
	int i, j;
	int x, y;
	int nk = kernelSize*kernelSize;
	int halfKernelSize = kernelSize / 2;
	float oneOverNk = 1.0f / (float)nk;
	float* kernel = (float*)malloc(sizeof(float)*nk);
	for (i = 0; i < nk; i++){
		kernel[i] = oneOverNk;
	}
	unsigned char* resimage = (unsigned char *)malloc(width * height * 4 * sizeof(unsigned char));

	for (y = 0; y < height; ++y) {
		for (x = 0; x < width; ++x) {
			float bs = 0.0;
			float gs = 0.0;
			float rs = 0.0;
			//if (x <= halfKernelSize || y <= halfKernelSize || x + halfKernelSize >= width || y + halfKernelSize >= height)
			//continue;
			for (i = -halfKernelSize; i <= halfKernelSize; ++i) {
				for (j = -halfKernelSize; j <= halfKernelSize; ++j) {
					unsigned char* buffer = NULL;
					float weight = kernel[(i + halfKernelSize)*kernelSize + (j + halfKernelSize)];
					if (x <= halfKernelSize || y <= halfKernelSize || x + halfKernelSize >= width || y + halfKernelSize >= height){
						bs += 0;
						gs += 0;
						rs += 0;
					}
					else {
						buffer = img + width * 4 * (y + j) + (x + i) * 4;
						bs += weight * *buffer;
						gs += weight * *(buffer + 1);
						rs += weight * *(buffer + 2);
					}
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
	free(kernel);
}

int clamp(int value, int min, int max) {
	if (value <= min) {
		return min;
	}
	else if (value >= max) {
		return max;
	}
	else{
		return value;
	}
}
/*fast*/
void blur_image_boxfilter_separable_convolution(unsigned char *src_img, int kernel_size, int width, int height)
{
	int i, j;
	int x, y;
	const int channels = 4;
	int half_kernel_size = kernel_size / 2;

	//allocate kernels
	float* horiz_kernel = (float*) malloc(sizeof(float) * kernel_size);
	float* vert_kernel = (float*) malloc(sizeof(float) * kernel_size);

	//init horizontal and vertical kernels (for box filtering)
	float filter_value = 1.0f / (float)kernel_size;
	for (i = 0; i < kernel_size; i++) {
		horiz_kernel[i] = filter_value;
		vert_kernel[i] = filter_value;
	}

	//allocate temporary buffers
	unsigned char* fully_filtered_img =
		(unsigned char *)malloc(width * height * channels * sizeof(unsigned char));
	unsigned char* horiz_filtered_img =
		(unsigned char *)malloc(width * height * channels * sizeof(unsigned char));


	//apply filter horizontally
	for (y = 0; y < height; ++y) {
		for (x = 0; x < width; ++x) {
			float bs = 0.0;
			float gs = 0.0;
			float rs = 0.0;
			for (i = -half_kernel_size; i <= half_kernel_size; ++i) {
				int sample_location_x = clamp(x + i, 0, width - 1); //boundary value padding
				unsigned char* buffer = src_img +
					(width * channels * y) +
					(sample_location_x * channels);
				float weight = horiz_kernel[i + half_kernel_size];
				bs += weight * *buffer;
				gs += weight * *(buffer + 1);
				rs += weight * *(buffer + 2);
			}
			unsigned char* outbuffer =
				horiz_filtered_img +
				(width * channels * y) +
				(x * channels);
			*outbuffer = bs;
			*(outbuffer + 1) = gs;
			*(outbuffer + 2) = rs;
			*(outbuffer + 3) = 255; //alpha
		}
	}

	//apply filter vertically
	for (y = 0; y < height; ++y) {
		for (x = 0; x < width; ++x) {
			float bs = 0.0;
			float gs = 0.0;
			float rs = 0.0;
			for (i = -half_kernel_size; i <= half_kernel_size; ++i) {
				int sample_location_y = clamp(y + i, 0, height - 1); //boundary value padding
				unsigned char* buffer =
					horiz_filtered_img +
					(width * channels * sample_location_y) +
					(x * channels);
				float weight = vert_kernel[i + half_kernel_size];
				bs += weight * *buffer;
				gs += weight * *(buffer + 1);
				rs += weight * *(buffer + 2);
			}

			unsigned char* outbuffer =
				fully_filtered_img +
				(width * channels * y) +
				(x * channels);

			*outbuffer = bs;
			*(outbuffer + 1) = gs;
			*(outbuffer + 2) = rs;
			*(outbuffer + 3) = 255; //alpha
		}
	}

	//change the source image to filtered
	memcpy(src_img, fully_filtered_img, width*height * channels);

	//free temporary buffers
	free(horiz_kernel);
	free(vert_kernel);
	free(horiz_filtered_img);
	free(fully_filtered_img);
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

	for (y = 1; y < height - 1; ++y) {
		for (x = 1; x < width - 1; ++x) {

			float sx = 0.0f;
			float sy = 0.0f;
			float w = 0.0f;
			for (i = -1; i <= 1; ++i) {
				for (j = -1; j <= 1; ++j) {
					unsigned char* buffer = img + width * 4 * (y + j) + (x + i) * 4;
					b = *buffer;
					g = *(buffer + 1);
					r = *(buffer + 2);
					float luminance = 0.3f * r + 0.59f * g + 0.11f * b;
					w = sobel_x[i + 1][j + 1];
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
	realimage_clone = clone_image(realimage, realimagewidth, realimageheight, 4);

	birdimage = read_ppm_raw("bird.ppm", LE, &birdimagewidth, &birdimageheight);
	
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
	
	//Sobel filter
	rasterizer_copy_pixels(0, 0, realimagewidth, realimageheight, realimage);
	rasterizer_draw_text(font, 100, 20, "Base Image");
	swr_sdl_render_screen_texture();
	swr_sdl_wait_for_events();
	
	sobel_edge_detect(realimage, realimagewidth, realimageheight);
	rasterizer_copy_pixels(0, 0, realimagewidth, realimageheight, realimage);
	rasterizer_draw_text(font, 100, 20, "After applying Sobel Filter");
	swr_sdl_render_screen_texture();
	swr_sdl_wait_for_events();
	
	//box filter 
	rasterizer_clear();
	rasterizer_copy_pixels(0, 0, realimagewidth, realimageheight, realimage_clone);
	rasterizer_draw_text(font, 100, 20, "Before applying box Filter");
	swr_sdl_render_screen_texture();
	swr_sdl_wait_for_events();

	rasterizer_clear();
	blur_image_boxfilter(realimage_clone, 15, realimagewidth, realimageheight);
	rasterizer_copy_pixels(0, 0, realimagewidth, realimageheight, realimage_clone);
	rasterizer_draw_text(font, 100, 20, "After applying box Filter of kernelsize 15");
	swr_sdl_render_screen_texture();
	swr_sdl_wait_for_events();
	
	
	destroy_image(gradientimage);
	destroy_image(checkerimage);
	destroy_image(realimage);
	destroy_font(font);
	destroy_image(realimage_clone);
	destroy_image(birdimage);

	swr_sdl_destroy_context();
	return 0;
}

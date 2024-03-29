#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <ctype.h>
#include "swr_pixel.h"
#include "swr_image.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

unsigned char * read_png_using_stb(const char *filename, int *textureWidth, int *textureHeight)
{
	int w, h, bpp;
	int num_of_pixels;
	int i;
	unsigned char *srcloc, *dstloc;
	unsigned char *image;
	/* stbi_info(filename, &textureWidth, &textureHeight, &bytesPerPixel); */
	unsigned char* imgdata_stb = stbi_load(filename, &w, &h, &bpp, STBI_rgb_alpha);
	if (imgdata_stb == NULL) {
		*textureWidth = 0;
		*textureHeight = 0;
		printf("Loading png file %s failed.", filename);
		return NULL;
	}

	num_of_pixels = w * h;
	image = (unsigned char *)malloc(num_of_pixels * 4 * sizeof(unsigned char));
	dstloc = image;
	srcloc = imgdata_stb;

	unsigned char rgba[4];
	for (i = 0; i < num_of_pixels; ++i) {
		rgba[0] = *srcloc++;
		rgba[1] = *srcloc++;
		rgba[2] = *srcloc++;
		rgba[3] = *srcloc++;
		*dstloc++ = rgba[2];
		*dstloc++ = rgba[1];
		*dstloc++ = rgba[0];
		*dstloc++ = rgba[3];
	}
	*textureWidth = w;
	*textureHeight = h;
	stbi_image_free(imgdata_stb);
	return image;
}


void write_bmp(const char* filename, unsigned char* pixels, int width, int height)
{
	unsigned char *loc;
	char fn[256];
	FILE *fp;
	int i, num_of_pixels;
	time_t tm;
	const int bmp_file_size = width * height * 3 + 54;
	unsigned char* bmp_data;
/*
Why height is given negative in bmp_header ?
https://stackoverflow.com/questions/26144955/after-writing-bmp-file-image-is-flipped-upside-down
*/
	const unsigned char bmp_header[54] = {
		0x42, 0x4D, // BM
		bmp_file_size & 0xFF, bmp_file_size >> 8 & 0xFF, bmp_file_size >> 16 & 0xFF, bmp_file_size >> 24 & 0xFF, // File size
		0x00, 0x00, 0x00, 0x00, // Reserved
		0x36, 0x00, 0x00, 0x00, // Offset to pixel data
		0x28, 0x00, 0x00, 0x00, // Info header size
		width & 0xFF, width >> 8 & 0xFF, width >> 16 & 0xFF, width >> 24 & 0xFF, // Image width
		-height & 0xFF, -height >> 8 & 0xFF, -height >> 16 & 0xFF, -height >> 24 & 0xFF, // Image height, negative since this bitmap is a top-down DIB with the origin at the upper left corner.
		0x01, 0x00, // Number of color planes
		0x18, 0x00, // Bits per pixel
		0x00, 0x00, 0x00, 0x00, // Compression method
		0x00, 0x00, 0x00, 0x00, // Image size
		0x13, 0x0B, 0x00, 0x00, // Horizontal resolution (2835 pixels/meter)
		0x13, 0x0B, 0x00, 0x00, // Vertical resolution (2835 pixels/meter)
		0x00, 0x00, 0x00, 0x00, // Number of colors in the palette
		0x00, 0x00, 0x00, 0x00 // Number of important colors
	};

	if (filename != NULL) {
		strcpy(fn, filename);
	}
	else
	{
		/* if filename is not given, Make file name from time stamp*/
		tm = time(NULL);
		strftime(fn, sizeof(fn), "IMG_%Y_%m_%d_%H_%M_%S.bmp", localtime(&tm));
	}
	fp = fopen(fn, "wb");

	if (fp == NULL) {
		return;
	}


	/* Dump r,g,b values */
	bmp_data = (unsigned char*)malloc(width*height * 3);
	loc = pixels;
	num_of_pixels = width * height;

	int j = 0;
	for (i = 0; i < num_of_pixels; ++i)
	{
		/*
		rgb[0] = *loc++;
		rgb[1] = *loc++;
		rgb[2] = *loc++;
		loc++;
		*/
		bmp_data[j++] = *loc++;
		bmp_data[j++] = *loc++;
		bmp_data[j++] = *loc++;
		loc++;
	}
	fwrite(bmp_header, sizeof(bmp_header), 1, fp);
	fwrite(bmp_data, width * height * 3, 1, fp);
	fclose(fp);
}

void write_bmp2(const char* filename, unsigned char* pixels, int width, int height)
{
	unsigned char *loc;
	char fn[256];
	FILE *fp;
	int i, num_of_pixels;
	time_t tm;
	const int bmp_file_size = width * height * 3 + 54;
	unsigned char* bmp_data;
	const unsigned char bmp_header[54] = {
		0x42, 0x4D, // BM
		bmp_file_size & 0xFF, bmp_file_size >> 8 & 0xFF, bmp_file_size >> 16 & 0xFF, bmp_file_size >> 24 & 0xFF, // File size
		0x00, 0x00, 0x00, 0x00, // Reserved
		0x36, 0x00, 0x00, 0x00, // Offset to pixel data
		0x28, 0x00, 0x00, 0x00, // Info header size
		width & 0xFF, width >> 8 & 0xFF, width >> 16 & 0xFF, width >> 24 & 0xFF, // Image width
		height & 0xFF, height >> 8 & 0xFF, height >> 16 & 0xFF, height >> 24 & 0xFF, // Image height
		0x01, 0x00, // Number of color planes
		0x18, 0x00, // Bits per pixel
		0x00, 0x00, 0x00, 0x00, // Compression method
		0x00, 0x00, 0x00, 0x00, // Image size
		0x13, 0x0B, 0x00, 0x00, // Horizontal resolution (2835 pixels/meter)
		0x13, 0x0B, 0x00, 0x00, // Vertical resolution (2835 pixels/meter)
		0x00, 0x00, 0x00, 0x00, // Number of colors in the palette
		0x00, 0x00, 0x00, 0x00 // Number of important colors
	};
	int j;
	int pitch;
	int x, y;

	if (filename != NULL) {
		strcpy(fn, filename);
	}
	else
	{
		/* if filename is not given, Make file name from time stamp*/
		tm = time(NULL);
		strftime(fn, sizeof(fn), "IMG_%Y_%m_%d_%H_%M_%S.bmp", localtime(&tm));
	}
	fp = fopen(fn, "wb");

	if (fp == NULL) {
		return;
	}


	/* Dump r,g,b values from bottom row to top row*/
	bmp_data = (unsigned char*)malloc(width*height * 3);
	j = 0;
	pitch = width * 4;
	for (y = height - 1; y >= 0; --y)
	{
		for (x = 0; x < width; ++x)
		{
			loc = pixels + pitch * y + x * 4;
			bmp_data[j++] = *loc++;
			bmp_data[j++] = *loc++;
			bmp_data[j++] = *loc++;
		}
	}
			
	fwrite(bmp_header, sizeof(bmp_header), 1, fp);
	fwrite(bmp_data, width * height * 3, 1, fp);
	fclose(fp);
}



void write_ppm_raw(const char* filename, unsigned char* pixels, int width, int height)
{
	unsigned char rgb[3];
	unsigned char *loc;
	char fn[256];
	FILE *fp;
	int i, num_of_pixels;
	time_t tm;
	
	if (filename != NULL) {
		strcpy(fn, filename);
	}
	else
	{
		/* if filename is not given, Make file name from time stamp*/
		tm = time(NULL);
		strftime(fn, sizeof(fn), "IMG_%Y_%m_%d_%H_%M_%S.ppm", localtime(&tm));
	}
	fp = fopen(fn, "wb");

	if (fp == NULL) {
		return;
	}

	/* Write ppm header */
	fprintf(fp, "P6\n%d %d\n255\n",
		width,
		height);

	/* Dump r,g,b values */
	loc = pixels;
	num_of_pixels = width * height;
	for (i = 0; i < num_of_pixels; ++i)
	{
		rgb[2] = *loc++;
		rgb[1] = *loc++;
		rgb[0] = *loc++;
		loc++;
		fwrite(rgb, sizeof(unsigned char), sizeof(rgb), fp);
	}
	fclose(fp);
}

void write_ppm_ascii(const char* filename, unsigned char* pixels, int width, int height)
{
	unsigned char rgb[3];
	unsigned char *loc;
	char fn[256];
	FILE *fp;
	int i, num_of_pixels;
	time_t tm;

	if (filename != NULL) {
		strcpy(fn, filename);
	}
	else
	{
		/* if filename is not given, Make file name from time stamp*/
		tm = time(NULL);
		strftime(fn, sizeof(fn), "SCRN_%Y_%m_%d_%H_%M_%S.ppm", localtime(&tm));
	}
	fp = fopen(fn, "w");
	if (fp == NULL) {
		return;
	}

	/* Write ppm header */
	fprintf(fp, "P3\n%d %d\n255\n",
		width,
		height);

	/* Dump r,g,b values */
	loc = pixels;
	num_of_pixels = width * height;
	for (i = 0; i < num_of_pixels; ++i)
	{
		rgb[2] = *loc++;
		rgb[1] = *loc++;
		rgb[0] = *loc++;
		loc++;
		fprintf(fp, "\n%d\n%d\n%d", rgb[0], rgb[1], rgb[2]);
	}
	fclose(fp);
}

unsigned char * read_ppm_raw(const char *filename, enum ByteOrder byteOrder, int *textureWidth, int *textureHeight)
{
	FILE *fp;
	char c;
	int width = 0, height = 0, color_depth = 0;
	unsigned char rgb[3];
	char buffer[3];
	unsigned char *image = 0;
	unsigned char* loc;
	int num_pixels;
	int i;

	fp = fopen(filename, "rb");
	if (fp == NULL) {
		printf("Cannot open file %s", filename);
		return NULL;
	}

	/* read first three characters */
	i = 0;
	while (i < 3) {
		c = fgetc(fp);
		if (c == EOF) {
			printf("\n%s : Unexpected end of file", filename);
			fclose(fp);
			return NULL;
		}
		buffer[i++] = c;
	}

	/* check for P6 */
	if (buffer[0] != 'P' || buffer[1] != '6' || !isspace(buffer[2])) {
		printf("\n%s : P6(raw ppm signature) not found", filename);
		fclose(fp);
		return NULL;
	}

	/* check for comment */
	c = fgetc(fp);
	if (c == '#') {
		while ((c = fgetc(fp)) != '\n') {
			if (c == EOF) {
				printf("\n%s : Unexpected end of file", filename);
				fclose(fp);
				return NULL;
			}
		}
	}
	else if (c == EOF) {
		printf("\n%s : Unexpected end of file", filename);
		fclose(fp);
		return NULL;
	}
	else {
		ungetc(c, fp);
	}

	/* read width */
	fscanf(fp, "%d", &width);

	/* read height */
	fscanf(fp, "%d", &height);

	/* read color depth */
	fscanf(fp, "%d", &color_depth);

	c = fgetc(fp);
	if (c != '\n') {
		return NULL;
	}

	/* read colors */
	num_pixels = width * height;
	image = (unsigned char *)malloc(num_pixels * 4 * sizeof(unsigned char));
	loc = image;
	
	for (i = 0; i < num_pixels; ++i) {
		fread(rgb, sizeof(unsigned char), 3, fp);
		*loc++ = rgb[2];
		*loc++ = rgb[1];
		*loc++ = rgb[0];
		*loc++ = 255;
	}
	*textureWidth = width;
	*textureHeight = height;
	return image;
}

unsigned char * read_ppm_ascii(const char *filename, int *textureWidth, int *textureHeight)
{
	FILE *fp;
	char c;
	int width = 0, height = 0, color_depth = 0;
	int r = 0, g = 0, b = 0;
	char buffer[3];
	unsigned char *image = 0;
	unsigned char* loc;
	int num_pixels;
	int i, rv;
	int comment_is_there = 0;

	fp = fopen(filename, "r");
	if (fp == NULL) {
		printf("Cannot open file %s", filename);
		return NULL;
	}

	/* read first three characters */
	i = 0;
	while (i < 3) {
		c = fgetc(fp);
		if (c == EOF) {
			printf("\n%s : Unexpected end of file", filename);
			fclose(fp);
			return NULL;
		}
		buffer[i++] = c;
	}

	/* check for P3 */
	if (buffer[0] != 'P' || buffer[1] != '3' || !isspace(buffer[2])) {
		printf("\n%s : P3(ascii ppm signature) not found", filename);
		fclose(fp);
		return NULL;
	}

	/* check for comment */
	c = fgetc(fp);
	if (c == '#') {
		while ((c = fgetc(fp)) != '\n') {
			if (c == EOF) {
				printf("\n%s : Unexpected end of file", filename);
				fclose(fp);
				return NULL;
			}
		}
	}
	else if (c == EOF) {
		printf("\n%s : Unexpected end of file", filename);
		fclose(fp);
		return NULL;
	}
	else {
		ungetc(c, fp);
	}

	/* read width */
	fscanf(fp, "%d", &width);

	/* read height */
	fscanf(fp, "%d", &height);

	/* read color depth */
	fscanf(fp, "%d", &color_depth);

	/* read colors */
	num_pixels = width * height;
	image = (unsigned char *)malloc(num_pixels * 4 * sizeof(unsigned char));
	loc = image;
	for (i = 0; i < num_pixels; ++i) {
		rv = fscanf(fp, "%d%d%d", &r, &g, &b);
		*loc++ = b;
		*loc++ = g;
		*loc++ = r;
		*loc++ = 255;
		if (rv == EOF) {
			break;
		}
	}
	*textureWidth = width;
	*textureHeight = height;
	return image;
}

#define NUM_OF_CHANNELS 4
unsigned char * create_gradient_image(int ar, int ag, int ab, int aa, int br, int bg, int bb, int ba, int width, int height)
{
	int i;
	double t, delta;
	int pitch = width * NUM_OF_CHANNELS;
	unsigned char* loc;
	unsigned char* pixels = (unsigned char*)malloc(pitch * height);

	if (pixels == NULL) {
		return NULL;
	}

	/* Fill first row with gradient */
	loc = pixels;
	for (i = 0, t = 0.0, delta = 1.0 / width;
		i < width;
		++i, t += delta)
	{
		*loc++ = (unsigned char)(ab + (bb - ab) * t);
		*loc++ = (unsigned char)(ag + (bg - ag) * t);
		*loc++ = (unsigned char)(ar + (br - ar) * t);
		*loc++ = (unsigned char)(aa + (ba - aa) * t);
	}

	/* Copy first row over all rows */
	loc = pixels + pitch;
	for (i = 1; i < height; ++i)
	{
		memcpy(loc, pixels, pitch);
		loc += pitch;
	}
	return pixels;
}

unsigned char* create_checker_image(int width, int height, int tileWidth, int tileHeight, int ar, int ag, int ab, int aa, int br, int bg, int bb, int ba)
{
	int x, y;
	int pitch = width * NUM_OF_CHANNELS;
	unsigned char* loc;
	size_t image_size = pitch * height;
	unsigned char* pixels = (unsigned char*)malloc(image_size);
	int even_row;
	int even_col;
	if (pixels == NULL) {
		return NULL;
	}

	for (y = 0; y < height; ++y)
	{
		for (x = 0; x < width; ++x)
		{
			even_col = ((x / tileWidth) % 2 == 0) ? 1 : 0;
			even_row = ((y / tileHeight) % 2 == 0) ? 1 : 0;
			loc = pixels + (pitch * y) + (x * 4);
			if ((even_row && even_col) || (!even_row && !even_col)) {
				*loc++ = ab;
				*loc++ = ag;
				*loc++ = ar;
				*loc = aa;
			}
			else {
				*loc++ = bb;
				*loc++ = bg;
				*loc++ = br;
				*loc = ba;
			}
		}
	}
	return pixels;
}
#undef NUM_OF_CHANNELS

unsigned char* clone_image(unsigned char* image, int width, int height, int number_of_channels)
{
	size_t image_size = width * height * number_of_channels * sizeof(unsigned char);
	unsigned char* clone = (unsigned char*)malloc(image_size);
	memcpy(clone, image, image_size);
	return clone;
}

void destroy_image(unsigned char *pimg)
{
	if (pimg != NULL) {
		free(pimg);
	}
}

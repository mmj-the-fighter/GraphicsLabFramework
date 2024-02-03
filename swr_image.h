#ifndef _SWR_IMAGE_H_
#define _SWR_IMAGE_H_

#ifdef __cplusplus
extern "C" {
#endif

	enum ByteOrder { LE, BE };

	/* load png using stb lib */
	unsigned char * read_png_using_stb(const char *filename, int *textureWidth, int *textureHeight);

	/* save image in bmp format*/
	void write_bmp(const char* filename, unsigned char* pixels, int width, int height);

	/* save image in bmp format without storing negative height as bmp header*/
	void write_bmp2(const char* filename, unsigned char* pixels, int width, int height);

	/* save image in raw ppm format */
	void write_ppm_raw(const char* filename, unsigned char* pixels, int width, int height);

	/* save image in ascii ppm format */
	void write_ppm_ascii(const char* filename, unsigned char* pixels, int width, int height);
	
	/* load image of raw ppm format */
	unsigned char * read_ppm_raw(const char *filename, enum ByteOrder byteOrder, int *textureWidth, int *textureHeight);

	/* load image of ascii ppm format */
	unsigned char * read_ppm_ascii(const char *filename, int *textureWidth, int *textureHeight);

	/* make a gradient image */
	unsigned char * create_gradient_image(int ar, int ag, int ab, int aa, int br, int bg, int bb, int ba, int width, int height);

	/* make a checker board pattern */
	unsigned char* create_checker_image(int width, int height, int tileWidth, int tileHeight, int ar, int ag, int ab, int aa, int br, int bg, int bb, int ba);
	
	/*clone an image*/
	unsigned char* clone_image(unsigned char* image, int width, int height, int number_of_channels);

	/* destroy image pointer safely */
	void destroy_image(unsigned char *pimg);

#ifdef __cplusplus
}
#endif

#endif


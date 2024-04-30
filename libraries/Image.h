#ifndef __IMAGE_H
#define __IMAGE_H

typedef struct img Image;

typedef enum {
	GREY_SCALE,
	ALL_COLORS
} Color_Image;

Image* Image_new(const char* path);

#endif
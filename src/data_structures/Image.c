#include "Image.h"

#include <SDL.h>
#include <SDL_image.h>
#include <stdio.h>
#include <stdlib.h>
#include "VectorPointers.h"
#include "StringVec.h"
#include "error_management.h"

struct img
{
    char* path;
    Color_Image image_type;
    VectorPointers* pixels;
};

Image* Image_new(const char* path)
{
    // Inicializar SDL
    String* errStr = String_new_container(10);

    ASSERT(SDL_Init(SDL_INIT_VIDEO) == 0, IMAGE_SDL_ERROR_INIT);
    ASSERT((IMG_Init(IMG_INIT_JPG) & IMG_INIT_JPG) == IMG_INIT_JPG, IMAGE_SDL_IMAGE_ERROR_INIT);

    Image* newImg = (Image*)malloc(sizeof(Image));
    ASSERT(newImg, MEMORY_NOT_ASSIGNED_EXCEPTION);

    // Cargar imagen
    SDL_Surface* image = IMG_Load(path);
    ASSERT(image, IMAGE_FAILED_LOAD_IMAGE);

    SDL_Surface* converted_img = SDL_ConvertSurfaceFormat(image, SDL_PIXELFORMAT_RGBA32, 0);
    if (converted_img == NULL) {
        printf("Error al convertir la imagen: %s\n", SDL_GetError());
        SDL_FreeSurface(image);
        SDL_Quit();
        exit(1);
    }

    newImg->pixels = VectorPointers_new_init_size(NULL, image->h * image->w);
    ASSERT(newImg->pixels, MEMORY_NOT_ASSIGNED_EXCEPTION);

    // Acceder a los píxeles
    Uint32* pixels = (Uint32*)converted_img->pixels;
    int x, y;
    for (y = 0; y < converted_img->h; y++) {
        for (x = 0; x < converted_img->w; x++) {
            Uint8 r, g, b;
            Uint32 pixel = pixels[y * converted_img->w + x];
            SDL_GetRGB(pixel, converted_img->format, &r, &g, &b);
            printf("Pixel en (%d, %d): R=%d, G=%d, B=%d\n", x, y, r, g, b);
        }
    }

    // Liberar memoria y cerrar SDL
    SDL_FreeSurface(image);
    SDL_FreeSurface(converted_img);
    IMG_Quit();
    SDL_Quit();

    return newImg;
}

// Función para cargar una imagen y acceder a sus píxeles
void cargar_imagen_y_acceder_pixeles(const char* ruta) {
    // Inicializar SDL
    if (SDL_Init(SDL_INIT_VIDEO) != 0)
    {
        printf("Error initializing SDL: %s\n", SDL_GetError());
        fgetc(stdin);
        return;
    }

    if ((IMG_Init(IMG_INIT_JPG) & IMG_INIT_JPG) != IMG_INIT_JPG)
    {
        printf("Error initializing SDL_image: %s\n", IMG_GetError());
        fgetc(stdin);
        SDL_Quit();
        return;
    }

    // Cargar imagen
    SDL_Surface* image = IMG_Load(ruta);
    if (image == NULL) {
        printf("Error loading image: %s\n", SDL_GetError());
        fgetc(stdin);
        SDL_Quit();
        return;
    }

    // Acceder a los píxeles
    int x, y;
    Uint8* pixel;
    for (y = 0; y < image->h; y++) {
        for (x = 0; x < image->w; x++) {
            pixel = (Uint8*)image->pixels + y * image->pitch + x * image->format->BytesPerPixel;
            printf("Pixel en (%d, %d): R=%d, G=%d, B=%d\n", x, y, pixel[2], pixel[1], pixel[0]);
        }
    }

    // Liberar memoria y cerrar SDL
    SDL_FreeSurface(image);
    IMG_Quit();
    SDL_Quit();
}
#ifndef __GRAPHICS_H
#define __GRAPHICS_H
#include "SDL.h"
#include "SDL_ttf.h"
#include "VectorPointers.h"
#include "StringVec.h"
#include "Vector.h"

typedef struct
{
	short x;
	short y;
} CoordsWindow;

typedef struct
{
	String* text;
	CoordsWindow coords;
} Text;

struct UIState
{
	int mousex;
	int mousey;
	int mousedown;

	int hotitem;
	int activeitem;

	int kbditem;
	int keyentered;
	int keymod;
	int keychar;

	int lastwidget;
};

struct Window
{
	SDL_Window* window;
	SDL_Renderer* renderer;
	TTF_Font* font;
	CoordsWindow current_cursor_pos;
	VectorPointers* words_to_print;
	SDL_Surface* gScreen;
	SDL_Surface* gFont;
	struct UIState uistate;
};

extern struct Window* wnd_hnd;

struct inputParameters
{
	double* learningRate;
	double* minimumCostToReach;
};

struct Window* GUI_InitWindow(int width, int height);

/// <summary>
/// Agrega una palabra nueva a la colecci�n de textos. El cursor se mover� 20 p�xeles a la derecha para dar un espacio para una palabra nueva.
/// </summary>
/// <param name="text"></param>
void GUI_AddWord(String* text);

/// <summary>
/// Agrega una palabra nueva a la colecci�n de textos. El cursor se mover� 30 p�xeles hacia abajo para realizar lo equivalente a un salto de l�nea.
/// </summary>
/// <param name="text"></param>
void GUI_AddWordNewLine(String* text);

void GUI_AddNewLine();

void destroyWindow();

/// <summary>
/// Ejecuta el bucle de renderizado.
/// </summary>
/// <param name="labelsForInputs">Cadenas de caracteres representativas de cada dato a ingresar</param>
/// <param name="inputs">Contenedor del mismo tama�o que labelForInputs en el que se guardar�n los datos ingresados</param>
void GUIloop(VectorPointers* labelsForInputs, Vector* inputs);

#endif
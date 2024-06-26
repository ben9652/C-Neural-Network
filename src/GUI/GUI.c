#include "GUI.h"
#include <stdio.h>
#include <string.h>
#include "error_management.h"
#include "Vector.h"
#include "SharedVariables.h"
#include "ThreadsManagement.h"

VectorPointers* v;

struct Window* wnd_hnd;
char* text;

#define START_POS_X 20
#define START_POS_Y 25
#define INPUT_STR_SIZE 20

Text* Text_new(String* text, CoordsWindow coords);
void Text_delete(Text* self);

unsigned char receiveInput(String* inputString, Vector* inputs);

struct Window* GUI_InitWindow(int width, int height)
{
    SDL_Window* window = NULL;
    SDL_Renderer* renderer = NULL;

    ASSERT(TTF_Init() != -1, 0);

    TTF_Font* font = TTF_OpenFont("arial.ttf", 24);
    ASSERT_STR(font != NULL, TTF_GetError());

    // Inicializar SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        String error = String_create_emplace("SDL couldn't be initialized! SDL Error: ");
        String_add(&error, SDL_GetError());
        ASSERT_STR(0, error.Buffer);
    }

    // Crear ventana
    window = SDL_CreateWindow("Neural network training", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, width, height, SDL_WINDOW_SHOWN);
    if (window == NULL) {
        printf("No se pudo crear la ventana! SDL_Error: %s\n", SDL_GetError());
        ASSERT(0, 0);
    }

    // Crear lienzo
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (renderer == NULL) {
        printf("No se pudo crear el lienzo! SDL_Error: %s\n", SDL_GetError());
        ASSERT(0, 0);
    }
    
    struct Window* window_handler = (struct Window*)malloc(sizeof(struct Window));
    ASSERT(window_handler, MEMORY_NOT_ASSIGNED_EXCEPTION);
    window_handler->window = window;
    window_handler->renderer = renderer;
    window_handler->current_cursor_pos = (CoordsWindow){ START_POS_X , START_POS_Y };
    window_handler->words_to_print = VectorPointers_new(Text_delete);
    window_handler->font = font;

    return window_handler;
}

void renderText(SDL_Renderer* renderer, TTF_Font* font, const char* text, int x, int y)
{
    if(strlen(text))
    {
        SDL_Color color = { 255, 255, 255 }; // Color blanco
        SDL_Surface* surface = TTF_RenderText_Solid(font, text, color);
        ASSERT_STR(surface, TTF_GetError());

        SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
        ASSERT_STR(texture, TTF_GetError());

        SDL_Rect rect = { x, y, surface->w, surface->h };
        SDL_RenderCopy(renderer, texture, NULL, &rect);
        SDL_FreeSurface(surface);
        SDL_DestroyTexture(texture);
    }
}

void destroyWindow()
{
    free(wnd_hnd);
    wnd_hnd = NULL;
}

void GUIloop(VectorPointers* labelsForInputs, Vector* inputs)
{
    size_t sizeInput = labelsForInputs ? labelsForInputs->size : 0;

    SDL_Event e;
    int quit = 0;
    int escaped = 0;

    unsigned char inputsAssigned = 0;
    
    // Contendr� los n�meros ingresados pero en cadenas de caracteres
    VectorPointers strInputsContainer = VectorPointers_create_init_size(String_delete, sizeInput);
    for (size_t i = 0; i < sizeInput; i++)
        VectorPointers_add(&strInputsContainer, String_new(INPUT_STR_SIZE));

    if(sizeInput != 0)
    {
        String* firstLabel = VectorPointers_get(labelsForInputs, 0);
        GUI_AddWord(firstLabel);

        String* firstNumber = VectorPointers_get(&strInputsContainer, 0);
        GUI_AddWordNewLine(firstNumber);
    }

    size_t index = 0;
    String* inputString;
    while (!quit)
    {
        if(index < sizeInput)
            inputString = VectorPointers_get(&strInputsContainer, index);
        while (SDL_PollEvent(&e) != 0)
        {
            switch (e.type)
            {
            case SDL_KEYDOWN:
                int key_code = e.key.keysym.sym;

                if (key_code == SDLK_KP_9) key_code = '9';
                if (key_code == SDLK_KP_8) key_code = '8';
                if (key_code == SDLK_KP_7) key_code = '7';
                if (key_code == SDLK_KP_6) key_code = '6';
                if (key_code == SDLK_KP_5) key_code = '5';
                if (key_code == SDLK_KP_4) key_code = '4';
                if (key_code == SDLK_KP_3) key_code = '3';
                if (key_code == SDLK_KP_2) key_code = '2';
                if (key_code == SDLK_KP_1) key_code = '1';
                if (key_code == SDLK_KP_0) key_code = '0';
                if (key_code == SDLK_KP_PERIOD) key_code = '.';
                if (key_code == SDLK_KP_ENTER) key_code = '\r';

                if (index < sizeInput)
                {
                    if ((key_code >= '0' && key_code <= '9' || key_code == '.') && inputString->size < INPUT_STR_SIZE)
                        String_add_char(inputString, (char)key_code);
                }
                
                switch (key_code)
                {
                case '\r':
                    if (neural_network_learned)
                        quit = 1;
                    if(index < sizeInput)
                    {
                        if (receiveInput(inputString, inputs))
                        {
                            if (++index < sizeInput)
                            {
                                String* label_input = VectorPointers_get(labelsForInputs, index);
                                GUI_AddWord(label_input);
                                String* str_input = VectorPointers_get(&strInputsContainer, index);
                                GUI_AddWordNewLine(str_input);
                                inputString = str_input;
                            }
                            else
                            {
                                // Si ya se terminaron de asignar las entradas se libera al sem�foro para que el testeo de la red neuronal prosiga
                                if (!inputsAssigned)
                                    sem_post(&NN_Thread_Management);
                                inputsAssigned = 1;
                            }
                        }
                    }
                    break;
                case SDLK_BACKSPACE:
                    if (index < sizeInput)
                        String_backspace(inputString);
                    break;
                case SDLK_ESCAPE:
                    quit = 1;
                    escaped = 1;
                    break;
                }
                break;
            case SDL_QUIT:
                quit = 1;
                break;
            }
        }

        SDL_Delay(10);

        SDL_SetRenderDrawColor(wnd_hnd->renderer, 0, 0, 0, 255);
        SDL_RenderClear(wnd_hnd->renderer);

        Text* txt;
        for (size_t i = 0; i < wnd_hnd->words_to_print->size; i++)
        {
            txt = (Text*)VectorPointers_get(wnd_hnd->words_to_print, i);
            renderText(wnd_hnd->renderer, wnd_hnd->font, txt->text->Buffer, txt->coords.x, txt->coords.y);
        }

        SDL_RenderPresent(wnd_hnd->renderer);
    }

    if(escaped)
        VectorPointers_delete_stacked(&strInputsContainer);
    if(neural_network_learned || escaped)
        VectorPointers_delete(wnd_hnd->words_to_print);
    SDL_DestroyRenderer(wnd_hnd->renderer);
    SDL_DestroyWindow(wnd_hnd->window);
    TTF_CloseFont(wnd_hnd->font);
    TTF_Quit();
    SDL_Quit();
    destroyWindow(wnd_hnd);
}

void GUI_AddWord(String* text)
{
    CoordsWindow coords = wnd_hnd->current_cursor_pos;
    
    int width = 0;
    int height = 0;
    int res = TTF_SizeText(wnd_hnd->font, text->Buffer, &width, &height);
    ASSERT_STR(res == 0, TTF_GetError());

    wnd_hnd->current_cursor_pos.x += width + 7;

    Text* newText = Text_new(text, coords);
    VectorPointers_add(wnd_hnd->words_to_print, newText);
}

void GUI_AddWordNewLine(String* text)
{
    CoordsWindow coords = wnd_hnd->current_cursor_pos;

    wnd_hnd->current_cursor_pos.x = START_POS_X;
    wnd_hnd->current_cursor_pos.y += 30;

    Text* newText = Text_new(text, coords);
    VectorPointers_add(wnd_hnd->words_to_print, newText);
}

void GUI_AddNewLine()
{
    wnd_hnd->current_cursor_pos.x = START_POS_X;
    wnd_hnd->current_cursor_pos.y += 30;
}

/*
int NumberField(int id, int x, int y, char* buffer)
{
    int len = (int)strlen(buffer);
    int changed = 0;

    DrawString(buffer, x, y);

    switch (wnd_hnd->uistate.keyentered)
    {
    case SDLK_BACKSPACE:
        if (len > 0)
        {
            len--;
            buffer[len] = 0;
            changed = 1;
        }
        break;
    }
    if ((wnd_hnd->uistate.keychar >= '0' && wnd_hnd->uistate.keychar < '9' || wnd_hnd->uistate.keychar == '.') && len < 20)
    {
        buffer[len] = wnd_hnd->uistate.keychar;
        len++;
        buffer[len] = 0;
        changed = 1;
    }

    return changed;
}
*/
Text* Text_new(String* text, CoordsWindow coords)
{
    Text* newText = (Text*)malloc(sizeof(Text));
    ASSERT(newText, MEMORY_NOT_ASSIGNED_EXCEPTION);

    newText->text = text;
    newText->coords = coords;

    return newText;
}

void Text_delete(Text* self)
{
    String_delete(self->text);
    free(self);
}

unsigned char receiveInput(String* inputString, Vector* inputs)
{
    char* ptr;
    double numInput = strtod(inputString->Buffer, &ptr);
    if (ptr != inputString->Buffer && *ptr == '\0')
    {
        printf("%.17lf\n", numInput);
        Vector_add(inputs, numInput);
        return 1;
    }
    else
    {
        String_clear(inputString);
        return 0;
    }
}

#include "ConsolePositions.h"
#include "error_management.h"
#include <stdlib.h>

struct cursor_position
{
    HANDLE handler;
    COORD coordinates;
};

CursorPosition* CursorPosition_new()
{
    CursorPosition* newCoords = (CursorPosition*)malloc(sizeof(CursorPosition));
    ASSERT(newCoords, MEMORY_NOT_ASSIGNED_EXCEPTION);
    newCoords->handler = GetStdHandle(STD_OUTPUT_HANDLE);

    return newCoords;
}

void CursorPosition_delete(CursorPosition* self)
{
    free(self);
}

void UpdateCursorPosition(CursorPosition* self)
{
    CONSOLE_SCREEN_BUFFER_INFO csfi;
    GetConsoleScreenBufferInfo(self->handler, &csfi);
    self->coordinates = csfi.dwCursorPosition;
}

COORD GetCursorPosition(CursorPosition* self)
{
    return self->coordinates;
}

void PositionCursor(CursorPosition* self, COORD coords)
{
    SetConsoleCursorPosition(self->handler, coords);
}

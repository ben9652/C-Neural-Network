#include <Windows.h>

typedef struct cursor_position CursorPosition;

struct cursor_position* CursorPosition_new();
void CursorPosition_delete(CursorPosition* self);

/// <summary>
/// Guarda en la coordenada la posición actual del cursor
/// </summary>
/// <param name="self"></param>
void UpdateCursorPosition(CursorPosition* self);

/// <summary>
/// Obtiene la coordenada guardada
/// </summary>
/// <param name="self"></param>
/// <returns></returns>
COORD GetCursorPosition(CursorPosition* self);

/// <summary>
/// Posiciona al cursor en una coordenada arbitraria
/// </summary>
/// <param name="self"></param>
/// <param name="coords"></param>
void PositionCursor(CursorPosition* self, COORD coords);

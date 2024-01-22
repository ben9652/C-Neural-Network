#ifndef __CAPA_OCULTA_
#define __CAPA_OCULTA_

struct n_layer;

typedef struct in_layer {
	struct n_layer base;
} *HiddenLayer;

#include "NeuronsLayer.h"

#endif
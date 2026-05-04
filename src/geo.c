#include <stdio.h>
#include <stdlib.h>
#include "geo.h"
#include "banco.h"

void geo_insertQuadra(char *cep, double x, double y, double w, double h) {       
    banco_addQuadra(cep, x, y, w, h);
}

void geo_setQuadraStyle(char *fill, char *stroke, char *sw) {
    banco_setQuadraStyle(fill, stroke, sw);
}
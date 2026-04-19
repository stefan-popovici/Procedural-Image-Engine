//POPOVICI STEFAN 314CB
#ifndef TURTLE_H
#define TURTLE_H

#include "ppm.h"

void draw_line(ppmFile *img, int x0, int y0, int x1, int y1, Pixel col);

void turtle_movements(ppmFile *img, const char *comenzi, double x, double y,
					double unghi, double pas, double delta, Pixel col);

#endif
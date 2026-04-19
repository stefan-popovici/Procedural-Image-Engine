//POPOVICI STEFAN 314CB
#ifndef PPM_H
#define PPM_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
	unsigned char r;
	unsigned char g;
	unsigned char b;
} Pixel;

typedef struct {
	int w, h;
	Pixel **imagine;
	/// semnatura si max value sunt garantate, nu mai ocup memorie cu ele
} ppmFile;

ppmFile *alocare_ppm(int w, int h);
void free_ppm(ppmFile *img);
ppmFile *load_ppm(const char *path);
int save_ppm(const char *path, ppmFile *img);
ppmFile *deep_copy_ppm(ppmFile *src);
void errors_check(ppmFile *poza);

#endif
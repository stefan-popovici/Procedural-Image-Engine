//POPOVICI STEFAN 314CB
#ifndef FONT_H
#define FONT_H

#include "ppm.h"

typedef struct {
	int char_code;
	int dwx, dwy;
	int bbw, bbh;
	int bbxoff, bbyoff;
	int **pixelmap;
} Glyph;

typedef struct {
	char *nume_font;
	char *nume_fisier;
	Glyph *chars[256];
	// char e maxim un byte, deci putem alcatui maxim 2^8 = 256 de caractere
	// este de asemenea convenabil, pentru ca avem acces la desenul fiecarei
	//litere in O(1) la char[char_code]
} Font;

Font *load_font(const char *path);
void free_font(Font *f);
int extract_int(char **ptr);
void type(ppmFile *img, Font *f, const char *text, int x, int y, Pixel col);
Font *deep_copy_font(Font *src);

#endif
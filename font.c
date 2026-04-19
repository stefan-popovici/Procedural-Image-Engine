//POPOVICI STEFAN 314CB
#include <stdlib.h>
#include <string.h>
#include "font.h"
#include "lsystem.h"
void free_font(Font *f)
{
	if (!f)
		return;
	for (int i = 0; i < 256; i++) {
		if (f->chars[i]) {
			if (f->chars[i]->pixelmap) {
				for (int j = 0; j < f->chars[i]->bbh; j++)
					free(f->chars[i]->pixelmap[j]);
			}
			free(f->chars[i]->pixelmap);
		}
		free(f->chars[i]);
	}
	if (f->nume_font)
		free(f->nume_font);
	if (f->nume_fisier)
		free(f->nume_fisier);
	free(f);
}

int conversie(char c)
{
	/// in fisier avem datele in hexa, aici le fac conversia
	if (c >= '0' && c <= '9')
		return c - '0';
	if (c >= 'A' && c <= 'F')
		return c - 'A' + 10;
	if (c >= 'a' && c <= 'f')
		return c - 'a' + 10;
	return 0;
}

int extract_int(char **ptr)
{
	char *p = *ptr;
	while (*p && !((*p >= '0' && *p <= '9') || *p == '-'))
		p++;
	int valoare = atoi(p);
	while (*p && ((*p >= '0' && *p <= '9') || *p == '-'))
		p++;
	*ptr = p;
	return valoare;
}

void hexa_to_binary(Glyph *g, int rand, const char *line)
{
	int col = 0;

	for (int i = 0; line[i] != '\n' && line[i] != 'r' && line[i] != '\0'; i++) {
		int val = conversie(line[i]);
		for (int bit = 3; bit >= 0; bit--) {
			if (col < g->bbw) {
				g->pixelmap[rand][col] = (val >> bit) & 1;
				col++;
				///luam bit cu bit din int si ii punem in pixelmap
			}
		}
	}
}

char *read_line_file(FILE *f)
{
	int cap = 256;
	int poz = 0;
	char *line = (char *)malloc(cap * sizeof(char));
	if (!line)
		return NULL;
	int c;
	while ((c = fgetc(f)) != '\n' && c != EOF) {
		line[poz++] = (char)c;
		if (poz >= cap) {
			cap *= 2;
			char *aux = (char *)realloc(line, cap * sizeof(char));
			if (!aux) {
				free(line);
				return NULL;
			}
			line = aux;
		}
	}
	if (c == EOF && poz == 0) {
		free(line);
		return NULL;
	}
	line[poz] = '\0';
	return line;
}

Font *load_font(const char *path)
{
	FILE *f = fopen(path, "r");
	if (!f)
		return NULL;

	Font *font = (Font *)calloc(1, sizeof(Font));
	if (!font) {
		fclose(f);
		return NULL;
	}

	font->nume_fisier = my_strdup(path);

	char *line = NULL;
	Glyph *curr_glyph = NULL; // caracterul curenta

	while ((line = read_line_file(f))) {
		/// vedem daca comanda ne intereseaza. daca nu, o ignoram
		/// daca da, ii extragem datele pentru ca stim la fiecare comanda
		/// exact ce vrem sa scoatem de la ea
		char *p = line;
		if (strncmp(p, "STARTCHAR", 9) == 0) {
			curr_glyph = (Glyph *)calloc(1, sizeof(Glyph));
		} else if (strncmp(p, "ENCODING", 8) == 0) {
			p += 8; // sarim peste "ENCODING"
			curr_glyph->char_code = extract_int(&p);
		} else if (strncmp(p, "DWIDTH", 6) == 0) {
			p += 6;
			curr_glyph->dwx = extract_int(&p);
			curr_glyph->dwy = extract_int(&p);
		} else if (strncmp(p, "BBX", 3) == 0) {
			p += 3;
			curr_glyph->bbw = extract_int(&p);
			curr_glyph->bbh = extract_int(&p);
			curr_glyph->bbxoff = extract_int(&p);
			curr_glyph->bbyoff = extract_int(&p);
		} else if (strncmp(p, "BITMAP", 6) == 0) {
			curr_glyph->pixelmap = (int **)calloc
					(curr_glyph->bbh, sizeof(int *));

			for (int i = 0; i < curr_glyph->bbh; i++) {
				curr_glyph->pixelmap[i] = (int *)
					calloc(curr_glyph->bbw, sizeof(int));
				char *bitmap_line = read_line_file(f);
				hexa_to_binary(curr_glyph, i, bitmap_line);
				free(bitmap_line);
			}
		} else if (strncmp(p, "ENDCHAR", 7) == 0) {
			if (curr_glyph->char_code >= 0 && curr_glyph->char_code < 256) {
				font->chars[curr_glyph->char_code] = curr_glyph;
			} else {
				//todo: free glyph
				for (int j = 0; j < curr_glyph->bbh; j++)
					free(curr_glyph->pixelmap[j]);
				free(curr_glyph->pixelmap);
				free(curr_glyph);
			}
			curr_glyph = NULL; // ne pregatim pt urmatorul STARTCHAR
		} else if (strncmp(p, "FONT ", 5) == 0) {
			font->nume_font = my_strdup(p + 5);
		}
		free(line);
	}
	fclose(f);
	return font;
}

void type(ppmFile *img, Font *f, const char *text, int x, int y, Pixel col)
{
	if (!img || !f || !text)
		return;

	int cur_x = x;
	int cur_y = y;

	for (int k = 0; text[k] != 0; k++) {
		unsigned char c = (unsigned char)text[k];
		//litera curenta din text se afla la f->chars[c]
		Glyph *g = f->chars[c];
		// desenam glyph-ul la pozitia (cur_x, cur_y)

		if (g) {
			for (int i = 0; i < g->bbh; i++) {
				for (int j = 0; j < g->bbw; j++) {
					// o luam pixel cu pixel: daca avem 1 in matrice inseamna
					// ca acolo trebuie sa punem culoare
					if (g->pixelmap[i][j] == 1) {
						int px = cur_x + g->bbxoff + j;
						//formula din pdf
						int py = img->h - cur_y - g->bbyoff - g->bbh + i;
						// imaginea este de sus in jos

						if (px >= 0 && px < img->w && py >= 0 && py < img->h)
							img->imagine[py][px] = col;
					}
				}
			}
			cur_x += g->dwx;
			cur_y += g->dwy;
		}
	}
}

Glyph *deep_copy_glyph(Glyph *src)
{
	Glyph *dest = (Glyph *)calloc(1, sizeof(Glyph));

	dest->char_code = src->char_code;
	dest->dwx = src->dwx;
	dest->dwy = src->dwy;
	dest->bbw = src->bbw;
	dest->bbh = src->bbh;
	dest->bbxoff = src->bbxoff;
	dest->bbyoff = src->bbyoff;

	if (src->pixelmap) {
		dest->pixelmap = (int **)calloc(dest->bbh, sizeof(int *));
		for (int i = 0; i < dest->bbh; i++) {
			dest->pixelmap[i] = (int *)calloc(dest->bbw, sizeof(int));
			//copiem bit cu bit
			for (int j = 0; j < dest->bbw; j++)
				dest->pixelmap[i][j] = src->pixelmap[i][j];
		}
	}

	return dest;
}

Font *deep_copy_font(Font *src)
{
	if (!src)
		return NULL;
	Font *dest = (Font *)calloc(1, sizeof(Font));

	if (src->nume_font)
		dest->nume_font = my_strdup(src->nume_font);
	if (src->nume_fisier)
		dest->nume_fisier = my_strdup(src->nume_fisier);

	for (int i = 0; i < 256; i++) {
		if (src->chars[i])
			dest->chars[i] = deep_copy_glyph(src->chars[i]);
	}
	return dest;
}

//POPOVICI STEFAN 314CB
#include "turtle.h"
#include <math.h>
#include <stdlib.h>

typedef struct {
	double x;
	double y;
	double angle;
	double pas_curent;
	double unghi_rotire;
	Pixel culoare;
} turtle_stack;

void draw_line(ppmFile *img, int x0, int y0, int x1, int y1, Pixel col)
{
	int dx = abs(x1 - x0);
	int sx, sy;
	if (x0 < x1)
		sx = 1;
	else
		sx = -1;
	int dy = -abs(y1 - y0);
	if (y0 < y1)
		sy = 1;
	else
		sy = -1;
	int err = dx + dy;

	while (1) {
		if (x0 >= 0 && x0 < img->w && y0 >= 0 && y0 < img->h)
			img->imagine[img->h - 1 - y0][x0] = col;

		if (x0 == x1 && y0 == y1)
			break;
		int e2 = 2 * err;
		if (e2 >= dy) {
			err += dy;
			x0 += sx;
		}
		if (e2 <= dx) {
			err += dx;
			y0 += sy;
		}
	}
}

void turtle_movements(ppmFile *img, const char *comenzi, double x, double y,
			double unghi, double pas, double delta, Pixel col)
{
	int capacitate = 10;
	int top = -1;
	turtle_stack *stiva_testoasa = (turtle_stack *)
			calloc(capacitate, sizeof(turtle_stack));
	if (!stiva_testoasa) {
		perror("eroare alocare memorie stiva testoasa");
		return;
	}

	const double PI = 3.1415926;

	for (int i = 0; comenzi[i]; i++) {
		char c = comenzi[i];
		if (c == 'F' || c == 'f') {
			double x_nou = x + pas * cos(unghi * PI / 180);
			double y_nou = y + pas * sin(unghi * PI / 180);
			// cos si sin folosesc radiani

			if (c == 'F') {
				draw_line(img, (int)round(x), (int)round(y),
					(int)round(x_nou), (int)round(y_nou), col);
			}
			x = x_nou;
			y = y_nou;
		} else if (c == '+') {
			unghi = unghi + delta;
		} else if (c == '-') {
			unghi = unghi - delta;
		} else if (c == '[') {
			top++;
			if (top >= capacitate) {
				// s-a umplut stiva, o marim
				capacitate *= 2;
				stiva_testoasa = (turtle_stack *)
					realloc(stiva_testoasa, capacitate * sizeof(turtle_stack));
			}

			stiva_testoasa[top].x = x;
			stiva_testoasa[top].y = y;
			stiva_testoasa[top].angle = unghi;

			stiva_testoasa[top].pas_curent = pas;
			stiva_testoasa[top].culoare = col;
		} else if (c == ']') {
			if (top >= 0) {

				x = stiva_testoasa[top].x;
				y = stiva_testoasa[top].y;
				unghi = stiva_testoasa[top].angle;

				pas = stiva_testoasa[top].pas_curent;
				col = stiva_testoasa[top].culoare;
				top--;
			}
		}
	}

	free(stiva_testoasa);
}
//POPOVICI STEFAN 314CB
#include "ppm.h"

void free_ppm(ppmFile *ppm)
{
	if (!ppm)
		return;
	if (ppm->imagine) {
		for (int i = 0; i < ppm->h; i++)
			free(ppm->imagine[i]);
		free(ppm->imagine);
	}
	free(ppm);
}

ppmFile *alocare_ppm(int w, int h)
{
	ppmFile *ppm = (ppmFile *)malloc(sizeof(ppmFile));
	if (!ppm) {
		perror("eroare alocare memorie fisier ppm");
		return NULL;
	}

	ppm->w = w;
	ppm->h = h;
	ppm->imagine = (Pixel **)calloc(h, sizeof(Pixel *));
	if (!ppm->imagine) {
		perror("eroare alocare memorie imagine");
		free_ppm(ppm);
		return NULL;
	}

	for (int i = 0; i < h; i++) {
		ppm->imagine[i] = (Pixel *)calloc(w, sizeof(Pixel));
		if (!ppm->imagine[i]) {
			perror("eroare alocare memorie imagine");
			free_ppm(ppm);
			return NULL;
		}
	}
	return ppm;
}

ppmFile *load_ppm(const char *path)
{
	FILE *f = fopen(path, "rb");
	// b pentru ca sunt fisiere binare!!
	if (!f)
		return NULL;

	char magic[3]; // semnatura e garantat 2 caractere
	int w, h, max_val;

	fscanf(f, "%s %d %d %d", magic, &w, &h, &max_val);

	fgetc(f); // dupa ce citim variabilele de mai sus cu fscanf, aparent
	// functia lasa ultimul getline in pace, iar cand vom folosi fread
	// pentru a citi datele binare ale imaginii, apar bube :(

	ppmFile *ppm = alocare_ppm(w, h);
	if (!ppm) {
		perror("eroare alocare memorie fisier ppm");
		fclose(f);
		return NULL;
	}

	for (int i = 0; i < h; i++)
		fread(ppm->imagine[i], sizeof(Pixel), w, f);

	fclose(f);
	return ppm;
}

int save_ppm(const char *path, ppmFile *ppm)
{
	if (!ppm)
		return 0;
	FILE *f = fopen(path, "wb");
	if (!f)
		return 0;

	//datele de inceput, headerul
	fprintf(f, "P6\n%d %d\n255\n", ppm->w, ppm->h);

	for (int i = 0 ; i < ppm->h; i++)
		fwrite(ppm->imagine[i], sizeof(Pixel), ppm->w, f);

	fclose(f);
	return 1;
}

ppmFile *deep_copy_ppm(ppmFile *src)
{
	if (!src)
		return NULL;
	ppmFile *dest = alocare_ppm(src->w, src->h);
	if (!dest)
		return NULL;

	for (int i = 0; i < src->h; i++) {
		for (int j = 0; j <  src->w; j++)
			dest->imagine[i][j] = src->imagine[i][j];
	}
	return dest;
}

void errors_check(ppmFile *poza)
{
	if (!poza)
		return;

	int window = 0; // ultimii 4 biti cititi ii vom shifta la stanga mereu
	int bite_count = 0; // contror pentru a nu verifica inainte de 4 biti
	int prev_x = -1, prev_y = -1;
	int prev_channel_idx = -1;
	// pentru a retine coordonatele pixelului anterior


	for (int i = 0; i < poza->h; i++) {
		for (int j = 0; j < poza->w; j++) {
			//parcugem pixel cu pixel
			unsigned char r = poza->imagine[i][j].r;
			unsigned char g = poza->imagine[i][j].g;
			unsigned char b = poza->imagine[i][j].b;

			unsigned char culori[3];
			// vom procesa cele 3 canale pe rand
			culori[0] = r;
			culori[1] = g;
			culori[2] = b;

			
			for (int k = 0; k < 3; k++) {
				unsigned char val_curenta = culori[k];
				for (int bit = 7; bit >= 0; bit--) {
					// iteram bit cu bit in octetul curent
					// de la cel mai semnificativ la cel mai putin
					int bit_val = (val_curenta >> bit) & 1;
					//scoatem bitul curent, adaugam noul bit
					//in fereastra cu | si facem o mască cu & 0xF
					// pentru a pastra doar ultimii 4 biti
					window = (window << 1) | bit_val;
					window = window & 0xF;

					bite_count++;
					//verificam doar daca am numarat cel putin 4 biti
					if (bite_count >= 4) {
						int am_gasit = 0;
						int trebuie_sa_fie = -1; 
						// 0010 bitul 1 e gresit
						// 1101 bitul 0 e gresit
						if (window == 2) {
							am_gasit = 1;
							trebuie_sa_fie = 0; 
						} else if (window == 13) {
							am_gasit = 1;
							trebuie_sa_fie = 1;
						}

						if (am_gasit == 1) {
							//daca am gasit o eroare, afisam warning
							//pentru penultimul bit citit
							int x_afisare, y_afisare;
							int r_afis, g_afis, b_afis;

							if (bit < 7) {
								//eroare in octetul curent
								x_afisare = j;
								y_afisare = poza->h - 1 - i;
								// coordonata y trebuie inversata

								r_afis = poza->imagine[i][j].r;
								g_afis = poza->imagine[i][j].g;
								b_afis = poza->imagine[i][j].b;

								int masca = 1 << (bit + 1);
								int *val_tinta;
								// alegem canalul tinta
								if (k == 0)
									val_tinta = &r_afis;
								else if (k == 1)
									val_tinta = &g_afis;
								else
									val_tinta = &b_afis;
								//aplicam corectia
								if (trebuie_sa_fie == 0)
									*val_tinta &= ~masca;
								else
									*val_tinta |= masca;

							} else {
								//eroare in octetul anterior
								x_afisare = prev_x;
								y_afisare = poza->h - 1 - prev_y;
								// coordonata y trebuie inversata

								r_afis = poza->imagine[prev_y][prev_x].r;
								g_afis = poza->imagine[prev_y][prev_x].g;
								b_afis = poza->imagine[prev_y][prev_x].b;

								int *val_tinta;
								if (prev_channel_idx == 0)
									val_tinta = &r_afis;
								else if (prev_channel_idx == 1)
									val_tinta = &g_afis;
								else
									val_tinta = &b_afis;

								if (trebuie_sa_fie == 0)
									*val_tinta &= ~1;
								else
									*val_tinta |= 1;
							}

							printf("Warning: pixel at (%d, %d) may be read as (%d, %d, %d)\n",
								   x_afisare, y_afisare, r_afis, g_afis, b_afis);
						}
					}
				}
				//salvam pozitia curenta ca anterioara pentru a trece
				//la urmatoarea iteratie
				prev_x = j;
				prev_y = i;
				prev_channel_idx = k;
			}
		}
	}
}

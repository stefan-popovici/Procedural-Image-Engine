//POPOVICI STEFAN 314CB
#include "lsystem.h"
#include "turtle.h"
#include "commands.h"
#include "ppm.h"
//scopul acestui fisier este de a ma ajuta sa mentin limita de 120 de linii
//de cod in main. cu atatea comenzi posibile trec confortabil de limita
//pentru coding style.

void int_to_str(char *dest, int n)
{
	if (n == 0) {
		strcat(dest, "0");
		return;
	}

	char buffer[12];
	int poz = 0;
	int copie = n;
	//extragem cifrele in ordine inversa dintr-un buffer temporar
	while (copie > 0) {
		int cifra = copie % 10;
		buffer[poz++] = cifra + '0';
		copie = copie / 10;
	}

	//le copiem in ordine corecta in sirul final
	int len_dest = strlen(dest);
	for (int i = poz - 1; i >= 0; i--)
		dest[len_dest++] = buffer[i];
	dest[len_dest] = '\0';
}

void LOAD(char *path, LSystem *sys, UndoStack *u, UndoStack *r)
{
	scanf("%s", path);
	ppmFile *aux = load_ppm(path);
	if (aux) {
		push_stack(u, sys);
		golire_stiva(r);
		// daca avem o imagine mai veche deja o stergem
		if (sys->img)
			free_ppm(sys->img);
		sys->img = aux;

		char buffer[256] = "Loaded ";
		strcat(buffer, path);
		strcat(buffer, " (PPM image ");

		int_to_str(buffer, sys->img->w);
		strcat(buffer, "x");
		int_to_str(buffer, sys->img->h);
		strcat(buffer, ")");
		/// acum bufferul contine comanda completa
		if (sys->last_output)
			free(sys->last_output);
		sys->last_output = my_strdup(buffer);
		printf("%s\n", sys->last_output);

	} else {
		// nu avem la ce sa dam load
		printf("Failed to load %s\n", path);
	}
}

void LSYSTEM(char *path, LSystem *sys, UndoStack *u, UndoStack *r)
{
	if (scanf("%s", path) == 1) {
		/// verificam daca nu suntem in cazul cu exit, in care
		/// nu se primeste un path
		FILE *f_test = fopen(path, "r");
		if (f_test != NULL) {
			fclose(f_test);
			push_stack(u, sys); // salvam trecutul;
			golire_stiva(r); // viitorul nu mai e valid
			load_lsystem(path, sys);

			char buffer[256] = "Loaded ";
			strcat(buffer, path);
			strcat(buffer, " (L-system with ");
			int_to_str(buffer, sys->nr_reguli);
			strcat(buffer, " rules)");
			if (sys->last_output)
				free(sys->last_output);
			sys->last_output = my_strdup(buffer);
			printf("%s\n", sys->last_output);
		} else {
			printf("Failed to load %s\n", path);
		}
		/*
			verificam daca acea cale exista si este valida
			in caz contrar, eliberam sistemul vechi pentru ca
			nu vrem sa facem operatii de derivare pe sistemul vechi
		*/
	}
}

void SAVE(char *path, LSystem *sys)
{
	scanf("%s", path);
	//verificam daca nu avem ce salva
	if (!sys->img)
		printf("No image loaded\n");
	else {
		save_ppm(path, sys->img);
		printf("Saved %s\n", path);
	}
}

void DERIVE(LSystem *sys)
{
	int n;
	scanf("%d", &n);

	char *rezultat = derivare(n, sys);
	if (rezultat != NULL) {
		printf("%s\n", rezultat);
		free(rezultat);
	}
}

void TURTLE(LSystem *sys, UndoStack *u_stack, UndoStack *r_stack)
{
	double x, y, pas, unghi, delta;
	int n, r, g, b;

	scanf("%lf %lf %lf %lf %lf %d %d %d %d",
		&x, &y, &pas, &unghi, &delta, &n, &r, &g, &b);

	if (!sys->img)
		printf("No image loaded\n");
	else if (!sys || !sys->axioma) {
		printf("No L-system loaded\n");
	} else {
		char *sir = derivare(n, sys);
		if (sir != NULL) {
			push_stack(u_stack, sys);
			golire_stiva(r_stack);

			Pixel col;
			col.r = (unsigned char)r;
			col.g = (unsigned char)g;
			col.b = (unsigned char)b;

			turtle_movements(sys->img, sir, x, y, unghi, pas, delta, col);
			if (sys->last_output)
				free(sys->last_output);
			sys->last_output = my_strdup("Drawing done");
			printf("%s\n", sys->last_output);
			free(sir);
		}
	}
}

void UNDO(UndoStack *u, UndoStack *r, LSystem *sys)
{
	if (u->top >= 0) {
		push_stack(r, sys); // salvam prezentul in redo
		pop_stack(u, sys); // aduc trecutul
	} else {
		/// daca nu avem la ce sa dam undo
		printf("Nothing to undo\n");
	}
}

void REDO(UndoStack *u, UndoStack *r, LSystem *sys)
{
	if (r->top >= 0) {
		push_stack(u, sys); // salvam prezentul in undo
		pop_stack(r, sys); // aducem viitorul

		if (sys->last_output)
			printf("%s\n", sys->last_output);
		
	} else {
		//daca nu avem la ce sa dam redo
		printf("Nothing to redo\n");
	}
}

void EXIT(LSystem *sys, UndoStack *u, UndoStack *r,
		char *command, char *path)
{
	golire_stiva(u);
	free(u->stari);
	free(u);

	golire_stiva(r);
	free(r->stari);
	free(r);

	free_lsystem(sys);
	free(sys);

	free(command);
	free(path);
}

char *read_line(void)
{
	int cap = 256;
	int poz = 0;
	char *line = (char *)malloc(cap * sizeof(char));
	if (!line)
		return NULL;

	int c;
	while ((c = getchar()) != '\n' && c != EOF) {
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
	line[poz] = '\0';
	return line;
}

void TYPE(LSystem *sys, UndoStack *u, UndoStack *r, char *line)
{
	if (!sys->img) {
		printf("No image loaded\n");
		return;
	}
	if (!sys->font_curent) {
		printf("No font loaded\n");
		return;
	}

	char *p = line; // sarim peste "TYPE"
	while (*p && *p != '\"')
		p++; // gasim prima ghilimea
	if (*p != '\"')
		return;

	char *start_text = p + 1; // textul incepe dupa ghilimea
	char *end_text = start_text;
	while (*end_text && *end_text != '\"')
		end_text++; //gasim sfarsitul text
	if (*end_text != '\"')
		return;
	*end_text = '\0'; // inchidem sirul

	p = end_text + 1; // continuam de dupa text ca sa luam coordonatele
	int x = extract_int(&p);
	int y = extract_int(&p);
	int red, green, blue;
	red = extract_int(&p);
	green = extract_int(&p);
	blue = extract_int(&p);

	Pixel col; // construim pixelul. structura noastra este de tip
	// unsigned char, deci trebuie sa convertim
	col.r = (unsigned char)red;
	col.g = (unsigned char)green;
	col.b = (unsigned char)blue;

	//salvam starea curenta in undo
	push_stack(u, sys);
	golire_stiva(r); // viitorul nu mai e valid

	type(sys->img, sys->font_curent, start_text, x, y, col);
	if (sys->last_output)
		free(sys->last_output);
	sys->last_output = my_strdup("Text written");
	printf("%s\n", sys->last_output);
}

void FONT(char *path, LSystem *sys, UndoStack *u, UndoStack *r)
{
	scanf("%s", path);
	Font *aux = load_font(path);
	if (aux) {
		//salvam starea in undo
		push_stack(u, sys);
		golire_stiva(r);

		//daca exista deja un font in eliberam
		if (sys->font_curent)
			free_font(sys->font_curent);
		sys->font_curent = aux;

		int len = strlen(path) + strlen(sys->font_curent->nume_font) + 100;
		char *buffer = (char *)malloc(len * sizeof(char));
		strcpy(buffer, "Loaded ");
		strcat(buffer, path);
		strcat(buffer, " (bitmap font ");
		strcat(buffer, sys->font_curent->nume_font);
		strcat(buffer, ")");
		/// acum bufferul contine comanda completa

		if (sys->last_output)
			free(sys->last_output);
		sys->last_output = my_strdup(buffer);
		free(buffer);
		printf("%s\n", sys->last_output);
	} else {
		printf("Failed to load %s\n", path);
	}
}

void BITCHECK(LSystem *sys)
{
	if (!sys->img) {
		printf("No image loaded\n");
		return;
	}

	//verificam erorile din gravura
	errors_check(sys->img);

	if (sys->last_output)
		free(sys->last_output);
	sys->last_output = NULL;
}
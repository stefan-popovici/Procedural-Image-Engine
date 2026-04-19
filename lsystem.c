//POPOVICI STEFAN 314CB
#include "lsystem.h"
#define _POSIX_C_SOURCE 200809L /// am incercat sa includ strdup

//mi-l fac singur pana la urma
char *my_strdup(const char *s)
{
	if (!s)
		return NULL;
	char *nou = (char *)malloc((strlen(s) + 1) * sizeof(char));
	if (nou)
		strcpy(nou, s);
	return nou;
}

void free_lsystem(LSystem *sys)
{
	if (!sys)
		return;
	free(sys->axioma);
	if (sys->reguli) {
		for (int i = 0; i < sys->nr_reguli; i++)
			free(sys->reguli[i].succesor);
		free(sys->reguli);
	}
	free(sys->nume_fisier);

	if (sys->img)
		free_ppm(sys->img);
	if (sys->font_curent)
		free_font(sys->font_curent);
	if (sys->last_output)
		free(sys->last_output);

	sys->axioma = NULL;
	sys->reguli = NULL;
	sys->nr_reguli = 0;
	sys->nume_fisier = NULL;
	sys->img = NULL;
	sys->font_curent = NULL;
	sys->last_output = NULL;
}

void load_lsystem(const char *path, LSystem *sys)
{
	FILE *f;
	f = fopen(path, "r");
	if (!f)
		return;
	/// salvam imaginea existenta
	// comanda LSYSTEM imi stergea si imaginea incarcata anterior cu LOAD
	ppmFile *temp_img = sys->img;
	Font *temp_font = sys->font_curent;

	sys->font_curent = NULL;
	sys->img = NULL;

	free_lsystem(sys); /// stergem sistemul precedent

	sys->img = temp_img; // pun imaginea inpoi
	sys->font_curent = temp_font;

	sys->nume_fisier = my_strdup(path);

	char buffer[1001];
	fscanf(f, "%s", buffer);
	sys->axioma = my_strdup(buffer);
	/// am folosit acel buffer pentru ca am vrut o zona neglijabila de memorie
	/// in care sa citesc pentru inceput axioma. nu am facut asta direct
	/// in sys->axioma pentru ca nu stim dimensiunea textului pe care urmeaza
	/// sa-l primim deci nu stim la cat sa dam malloc
	/// am aflat ca strdup face si malloc si strcpy intr-o singura comanda

	fscanf(f, "%d", &sys->nr_reguli);
	sys->reguli = (Regula *)calloc(sys->nr_reguli, sizeof(Regula));

	for (int i = 0; i < sys->nr_reguli; i++) {
		fscanf(f, "%s", buffer);
		sys->reguli[i].simbol = buffer[0];

		fscanf(f, "%s", buffer);
		sys->reguli[i].succesor = my_strdup(buffer);
	}
	fclose(f);

}

char *derivare(int n, LSystem *sys)
{
	// am modificat functia sa returneze un char* care este
	// sirul rezultat dupa n derivari
	// daca nu avem un lsystem incarcat, returnam NULL
	// inainte era void, am facut schimbarea pentru comanda TURTLE
	if (!sys || !sys->axioma) {
		printf("No L-system loaded\n");
		return NULL;
	}
	char *copie = my_strdup(sys->axioma);
	for (int i = 0; i < n; i++) {
		/// pana la urma voi folosi realloc pentru derivare
		/// voi pleca de un numar modest de caractere pe care il voi
		/// mari in functie de schimbarile la dimensiunea axiomei in
		/// functie de regula

		int capacitate = strlen(copie) + 100;
		char *temp = (char *)malloc(capacitate * sizeof(char));
		temp[0] = '\0';
		int pozitie_curenta = 0;
		int lungime_veche = strlen(copie);

		for (int j = 0; j < lungime_veche; j++) {
			char litera_curenta = copie[j];
			char *regula_curenta = NULL;
			///gasim regula pentru litera curenta
			for (int k = 0; k < sys->nr_reguli; k++) {
				if (sys->reguli[k].simbol == litera_curenta) {
					regula_curenta = sys->reguli[k].succesor;
					break;
				}
			}
			///calculez de cat spatiu am nevoie
			int de_adaugat;
			if (regula_curenta)
				de_adaugat = strlen(regula_curenta);
			else
				de_adaugat = 1;

			if (pozitie_curenta + de_adaugat + 1 >= capacitate) {
				capacitate = capacitate * 2 + de_adaugat;
				temp = (char *)realloc(temp, capacitate * sizeof(char));
			}
			// acum punem caracterele
			if (regula_curenta) {
				///inlocuim litera cu regula gasita
				strcpy(temp + pozitie_curenta, regula_curenta);
				pozitie_curenta += de_adaugat;
			} else {
				/// nu exista regula pentru litera
				temp[pozitie_curenta] = litera_curenta;
				pozitie_curenta = pozitie_curenta + 1;
			}
		}
		temp[pozitie_curenta] = '\0'; // si null la final
		free(copie);
		copie = temp;
	}
	
	return copie;
}

LSystem *deep_copy_lsystem(LSystem *src)
{
	if (!src)
		return NULL;
	LSystem *dest = (LSystem *)malloc(1 * sizeof(LSystem));
	if (!dest)
		return NULL;

	dest->nr_reguli = src->nr_reguli;
	dest->axioma = my_strdup(src->axioma);
	dest->nume_fisier = my_strdup(src->nume_fisier);
	if (src->nr_reguli && src->reguli) {
		dest->reguli = (Regula *)malloc(dest->nr_reguli * sizeof(Regula));
		if (dest->reguli) {
			for (int i = 0; i < src->nr_reguli; i++) {
				dest->reguli[i].simbol = src->reguli[i].simbol;
				dest->reguli[i].succesor = my_strdup(src->reguli[i].succesor);
			}
		}
	} else {
		dest->reguli = NULL;
	}

	dest->img = deep_copy_ppm(src->img);
	dest->font_curent = deep_copy_font(src->font_curent);

	if (src->last_output)
		dest->last_output = my_strdup(src->last_output);
	else
		dest->last_output = NULL;

	return dest;
}

void push_stack(UndoStack *stiva, LSystem *sys)
{
	if (stiva->top + 1 == stiva->cap) {
		stiva->cap *= 2;
		stiva->stari = realloc(stiva->stari, stiva->cap * sizeof(LSystem *));
	}
	stiva->top++;
	stiva->stari[stiva->top] = deep_copy_lsystem(sys);
}

void pop_stack(UndoStack *stiva, LSystem *sys)
{
	if (stiva->top < 0)
		return;

	LSystem *vechi = stiva->stari[stiva->top];

	if (sys->img && sys->img != vechi->img)
		free_ppm(sys->img);
	sys->img = vechi->img;

	if (sys->font_curent)
		free_font(sys->font_curent);
	sys->font_curent = vechi->font_curent;

	free(sys->axioma);
	if (sys->reguli) {
		for (int i = 0; i < sys->nr_reguli; i++)
			free(sys->reguli[i].succesor);
		free(sys->reguli);
	}
	free(sys->nume_fisier);
	if (sys->last_output)
		free(sys->last_output);

	sys->axioma = vechi->axioma;
	sys->reguli = vechi->reguli;
	sys->nr_reguli = vechi->nr_reguli;
	sys->nume_fisier = vechi->nume_fisier;
	sys->img = vechi->img;
	sys->font_curent = vechi->font_curent;
	sys->last_output = vechi->last_output;

	vechi->axioma = NULL;
	vechi->reguli = NULL;
	vechi->nume_fisier = NULL;
	vechi->img = NULL;
	vechi->font_curent = NULL;
	vechi->last_output = NULL;

	free(vechi);
	stiva->top--;
}

void golire_stiva(UndoStack *stiva)
{
	for (int i = 0; i <= stiva->top; i++) {
		free_lsystem(stiva->stari[i]);
		free(stiva->stari[i]);
	}
	stiva->top = -1;
}

UndoStack *create_stack(int capacitate)
{
	UndoStack *stack = (UndoStack *)malloc(sizeof(UndoStack));
	if (!stack)
		return NULL;

	stack->top = -1;
	stack->cap = capacitate;
	stack->stari = (LSystem **)calloc(stack->cap, sizeof(LSystem *));

	if (!stack->stari) {
		free(stack);
		return NULL;
	}
	return stack;
}

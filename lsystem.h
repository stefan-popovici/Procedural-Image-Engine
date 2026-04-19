//POPOVICI STEFAN 314CB

#ifndef LSYS_H
#define LSYS_H

#include "ppm.h"
#include "font.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
	char simbol;
	char *succesor;
} Regula;

typedef struct {
	char *nume_fisier;
	int nr_reguli;
	Regula *reguli;
	char *axioma;
	//initial am incercat sa fac o strctura separata pentru
	//imagini ppm, dar e foarte mare bataie de cap, am avut probleme cu
	//undo/redo. voi modifica lsystem ca fiind o "stare intreaga a programului"

	ppmFile *img;
	Font *font_curent;

	char *last_output;
} LSystem;


typedef struct {
	int top; // ultimul element salvat
	int cap; //capacitate, dimensiunea actuala
	LSystem **stari;
} UndoStack;

char *my_strdup(const char *s);
void free_lsystem(LSystem *sys);
void load_lsystem(const char *path, LSystem *sys);
char *derivare(int n, LSystem *sys);
LSystem *deep_copy_lsystem(LSystem *src); // src = sursa
UndoStack *create_stack(int capacitate);
void push_stack(UndoStack *stiva, LSystem *sys);
void pop_stack(UndoStack *stiva, LSystem *sys);
void golire_stiva(UndoStack *stiva);

#endif
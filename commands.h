//POPOVICI STEFAN 314CB
#ifndef COMMANDS_H
#define COMMANDS_H

#include "lsystem.h"
#include "ppm.h"
#include "turtle.h"
#include "font.h"

void LOAD(char *path, LSystem *sys, UndoStack *u, UndoStack *r);
void LSYSTEM(char *path, LSystem *sys, UndoStack *u, UndoStack *r);
void SAVE(char *path, LSystem *sys);

void DERIVE(LSystem *sys);
void TURTLE(LSystem *sys, UndoStack *u, UndoStack *r);
void UNDO(UndoStack *u, UndoStack *r, LSystem *sys);
void REDO(UndoStack *u, UndoStack *r, LSystem *sys);

char *read_line(void);
void TYPE(LSystem *sys, UndoStack *u, UndoStack *r, char *line);
void FONT(char *path, LSystem *sys, UndoStack *u, UndoStack *r);

void BITCHECK(LSystem *sys);

void EXIT(LSystem *sys, UndoStack *u, UndoStack *r, char *c, char *p);
#endif

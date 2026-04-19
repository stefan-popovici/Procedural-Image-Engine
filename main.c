//POPOVICI STEFAN 314CB
#include "commands.h"
#include <stdio.h>
#include <stdlib.h>

int main(void)
{
	char *command, *path;
	command = (char *)malloc(100 * sizeof(char));///operatia(lsys,exit,derive)
	path = (char *)malloc(100 * sizeof(char));///calea primita

	LSystem *sys = (LSystem *)calloc(1, sizeof(LSystem));
	if (sys == NULL)
		return 1;

	UndoStack *u_stack = create_stack(10);
	UndoStack *r_stack = create_stack(10);
	/// la fel, incepem cu o
	/// capacitate modesta si marim in functie de nevoi
	if (!u_stack || !r_stack)
		return 1;

	while (scanf("%s", command) == 1) {
		if (strcmp(command, "LSYSTEM") == 0) {
			LSYSTEM(path, sys, u_stack, r_stack);
		} else if (strcmp(command, "DERIVE") == 0) {
			DERIVE(sys);
		} else if (strcmp(command, "EXIT") == 0) {
			EXIT(sys, u_stack, r_stack, command, path);
			return 0;
		} else if (strcmp(command, "UNDO") == 0) {
			UNDO(u_stack, r_stack, sys);
		} else if (strcmp(command, "REDO") == 0) {
			REDO(u_stack, r_stack, sys);
		} else if (strcmp(command, "LOAD") == 0) {
			LOAD(path, sys, u_stack, r_stack);
		} else if (strcmp(command, "SAVE") == 0) {
			SAVE(path, sys);
		} else if (strcmp(command, "TURTLE") == 0) {
			TURTLE(sys, u_stack, r_stack);
		} else if (strcmp(command, "TYPE") == 0) {
			char *line = read_line();
			if (line) {
				TYPE(sys, u_stack, r_stack, line);
				free(line);
			}
		} else if (strcmp(command, "FONT") == 0) {
			FONT(path, sys, u_stack, r_stack);
		} else if (strcmp(command, "BITCHECK") == 0) {
			BITCHECK(sys);
		} else {
			printf("Invalid command\n");
		}
	}
	return 0;
}
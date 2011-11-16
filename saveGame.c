#include <stdio.h>
#include <stdlib.h>
#include "prototypes.h"

void printPlayer(FILE *, const tPlayer *, const tTheme *);
void printFieldStatus(FILE *, const tPlayer *, const tTheme *);

void
saveGame(const char * fileName, const tGame * game)
{
	FILE * file;
	char * turn;
	file = fopen(fileName, "w+t");
	fprintf(file, "Tematica: %d\n\n", game->theme.themeNo);
	if (game->turn==MACHINE)
		turn = "Computadora";
	else
		turn = "Jugador";
	fprintf(file, "Proximo turno: %s\n\n", turn);
	fprintf(file, "Undos: %d\n\n", game->undos);
	fprintf(file, "Datos del jugador\n");
	printPlayer(file, &(game->user), &(game->theme));
	fprintf(file, "\n");
	printFieldStatus(file, &(game->user), &(game->theme));
	fprintf(file, "\n\n");
	fprintf(file, "Datos de la computadora\n");
	printPlayer(file, &(game->machine), &(game->theme));
	fprintf(file, "\n\n");
	printFieldStatus(file, &(game->machine), &(game->theme));
	fclose(file);
}

void
printFieldStatus(FILE * file, const tPlayer * player, const tTheme * theme)
{
	int i, j;
	char * status;
	for (i=0; i<theme->rows; i++)
	{
		for (j=0; j<theme->columns; j++)
		{
			if (player->enemyField[i][j] != BCHAR_UNKNOWN)
			{
				if(player->enemyField[i][j] == BCHAR_WATER)
					status = "AGUA";
				else if (player->enemyField[i][j] == BCHAR_IMPACTED)
					status = "IMPACTO";
				else
					status = "DESTRUCCION";
				fprintf(file, "(%d,%d) -> %s\n", i, j, status);
			}
		}
	}
}

void
printPlayer(FILE * file, const tPlayer * player, const tTheme * theme)
{
	char * status;
	int membAmount, i, j, k, row, col, size;
	for (i=0; i<theme->elemAmount; i++)
	{
		fprintf(file, "Elemento %d\n", i+1);
		membAmount = theme->elements[i].cant;
		for (j=0; j<membAmount; j++)
		{
			fprintf(file, "Miembro %d\n", j+1);
			size = theme->elements[i].size;
			for (k=0; k<size; k++)
			{
				row = player->statusArmy[i][j].position[k].position.row;
				col = player->statusArmy[i][j].position[k].position.column;
				if (player->statusArmy[i][j].position[k].status == BCHAR_ALIVE)
					status = "NO IMPACTADA";
				else
					status = "IMPACTADA";
				fprintf(file, "(%d,%d) -> %s\n", row, col, status);
			}
			fprintf(file, "\n");
		}
	}
}

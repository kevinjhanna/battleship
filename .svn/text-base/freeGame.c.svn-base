#include <stdlib.h>
#include "prototypes.h"

void
freeGame(tGame * game)
{
	freeStatusArmy(&(game->user.statusArmy), &(game->theme));
	freeStatusArmy(&(game->machine.statusArmy), &(game->theme));
	freeField(game->user.enemyField, game->theme.rows);
	freeField(game->machine.enemyField, game->theme.rows);
	freeField(game->user.defenseField, game->theme.rows);
	freeField(game->machine.defenseField, game->theme.rows);
}

void
freeStatusArmy(tStatusArmy * statusArmy, tTheme * theme)
{
	int i, j;
	for (i=0; i<theme->elemAmount; i++)
	{
		for (j=0; j<((theme->elements)[i].cant); j++)
		{
			free((*statusArmy)[i][j].position);
		}
		free((*statusArmy)[i]);
	}
	free(*statusArmy);
}

void
freeField(tField field, int rows)
{
	int i;
	for (i=0; i<rows; i++)
	{
		free(field[i]);
	}
	free(field);
}

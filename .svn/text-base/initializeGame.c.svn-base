#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "prototypes.h"

#define ENEMY_FIELD 1
#define DEFENSE_FIELD 2
#define STATUS_ARMY 3

int initializePlayer(tPlayer *, tTheme *);
int initializeEmptyField(int, int, tField *);
int initializeStatusArmy(tStatusArmy *, tTheme *);
void initializeStatus(tStatusArmy *, tTheme *, int, int);
void initializeLastMove(tPlayer *);
int totalAlive(const tTheme *);

int
initializeGame(tGame * game)
{
	int error = 0;
	
	/* Inicializar Ai */
  game->ai.lockWay = FALSE;
  game->ai.lockTarget = FALSE;
  game->ai.minAlive = 1;
  game->undos = 0;
	
  if ((error = initializePlayer(&(game->user), &(game->theme)))!=0)
  {
		if (error)
		{
			switch(error)
			{
				case STATUS_ARMY:
					freeField((game->user.enemyField), game->theme.rows);
					freeField((game->user.defenseField), game->theme.rows);
					break;
				case DEFENSE_FIELD:
					freeField((game->user.enemyField), game->theme.rows);
					break;
				case ENEMY_FIELD:
					break;
			}
			return NO_MEMORY;
		}
	}
	if ((error = initializePlayer(&(game->machine), &(game->theme)))!=0)
	{
		if (error)
		{
			switch(error)
			{
				case STATUS_ARMY:
					freeField((game->machine.enemyField), game->theme.rows);
					freeField((game->machine.defenseField), game->theme.rows);
					break;
				case DEFENSE_FIELD:
					freeField((game->machine.enemyField), game->theme.rows);
					break;
				case ENEMY_FIELD:
					break;
			}
			freeField((game->user.enemyField), game->theme.rows);
			freeField((game->user.defenseField), game->theme.rows);
			freeStatusArmy(&(game->user.statusArmy), &(game->theme));
			return NO_MEMORY;
		}
	}
	return error;
}
  
int
initializePlayer(tPlayer * player, tTheme * theme)
{
	int error = 0;
	player->totalAlive = totalAlive(theme);
	initializeLastMove(player);
	if ((error = initializeEmptyField(theme->rows, theme->columns,
																		&(player->enemyField)))!=0)
		return ENEMY_FIELD;
	if ((error = initializeEmptyField(theme->rows, theme->columns,
																		&(player->defenseField)))!=0)
		return DEFENSE_FIELD;
	if ((error = initializeStatusArmy(&(player->statusArmy), theme))!=0)
		return STATUS_ARMY;
	return error;
}

int
totalAlive(const tTheme * theme)
{
	int i, totalAlive = 0;
	for (i=0; i<theme->elemAmount; i++)
	{
		totalAlive += (((theme->elements)[i]).size)*(((theme->elements)[i]).cant);
	}
	return totalAlive;
}

int
initializeEmptyField(int nRows, int nColumns, tField * field)
{
    int i,j;
    tBoolean memory = TRUE;

    *field = malloc(nRows*sizeof(tRow));
    
    if (*field == NULL)
	   return NO_MEMORY;

    
    for (i = 0; i < nRows && memory; i++)
    {
			/*cambiar char para que sea mas generico*/
			(*field)[i] = malloc(nColumns*sizeof(tRow));
			memory = ((*field)[i] != NULL);
			/* Pone los puntos*/
			if(memory)
				memset((*field)[i], BCHAR_UNKNOWN, nColumns);
    }


    if(memory)
		return 0;

    /* ...no hubo memoria... */	
    /* liberar todo lo que se intento crear hasta ahora */
    for (j = 0; j < i; j++)
			free(*field[i]);

    free(*field);
    return NO_MEMORY;
}

int
initializeStatusArmy(tStatusArmy * statusArmy, tTheme * theme)
{
	int i, j, k, error = 0;
	
	if((*statusArmy = malloc(sizeof(tStatus*)*theme->elemAmount))==NULL)
		error=NO_MEMORY;
	for (i=0; i < theme->elemAmount && !error; i++)
	{
		if (((*statusArmy)[i]= malloc(sizeof(tStatus)*
																		((theme->elements)[i].cant)))==NULL)
			error = NO_MEMORY;
		for (j=0; j<((theme->elements)[i].cant) && !error; j++)
		{
			if(((*statusArmy)[i][j].position = malloc(sizeof(tPositionStatus)*
																		(theme->elements[i].size)))==NULL)
				error=NO_MEMORY;
			if (!error)
			{
				initializeStatus(statusArmy, theme, i, j);
				(*statusArmy)[i][j].alive = 0;
				(*statusArmy)[i][j].dim = 0;
			}
		}
	}
	
	if (error)
	{
		for (k=0; k<i; k++)
		{
			for (j=0; j<((theme->elements)[k].cant); j++)
			{
				free((*statusArmy)[k][j].position);
			}
			free((*statusArmy)[k]);
		}
		free(*statusArmy);
		
	}
	return error;
}

void
initializeStatus(tStatusArmy * statusArmy, tTheme * theme, 
								int elementNo, int memberNo)
{
	int i, size = theme->elements[elementNo].size;
	for (i=0; i<size; i++)
	{
		(*statusArmy)[elementNo][memberNo].position[i].status=BCHAR_UNKNOWN;
	}
}

void
initializeLastMove(tPlayer * player)
{
	player->lastMove.position.row = player->lastMove.element = 
					player->lastMove.member = player->lastMove.order = FIRST_PLAY;
}

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "prototypes.h"

int validateArea(tPosition *, tPosition *, tGame *, int);
int conflictedArea(tPosition *, tPosition *, tPlayer *, int, int);
int findPlace(int, int, tPosition *, tPosition *, tField, int, int);
int findStatusArmy(const tPosition *, tPlayer *, int *, int *, const tTheme *);
void insertStatusArmy(tStatusArmy *, tPosition *, const tTheme *, int, int, 
											int);
void setInField(char, tPosition *, tPosition *, tField *);
void setInGame(tPosition *, tPosition *, tPlayer *, const tTheme *, int, int);
int wasDestroyed(const tPosition *, tPlayer *, tPosition *, tPosition *, 
								const tTheme *, int *, int *, int *);


int
createNewRPlayer(tPlayer * player, const tTheme * theme)
{
	int elemAmount = theme->elemAmount, membAmount, error = 0;
	int length, i, j;
	tPosition p1 = {0,0};
	tPosition p2 = {0,0};
	
	/*por cada miembro busca una posición válida dónde colocar cada
	 * casillero */
	
	for (i=0; i<elemAmount && !error; i++)
	{
		membAmount = theme->elements[i].cant;
		length = theme->elements[i].size;
		for (j=0; j<membAmount && !error; j++)
		{
			error = findPlace(MAX_TRIES, length, &p1, &p2, 
										player->defenseField, theme->rows, theme->columns);
			if (!error)
			{
				setInField(BCHAR_ALIVE, &p1, &p2, &(player->defenseField));
				setInGame(&p1, &p2, player, theme, i, j);
			}
		}
	}
	if (error)
		return MAP_NOT_GENERATED;
	return 0;
}

int
userSet(tPosition * start, tPosition * end, tGame * game, int elementNo,
				int memberNo, int reposition)
{
	/*valida las posiciones que se le pasan y las coloca en el tablero y
	* statusArmy */

	int error = 0, size;
	tPosition oldStart = {0,0};
	tPosition oldEnd = {0,0};
	
	/* Si se está reposicionando un barco primero lo borra del tablero, 
	* después valida si las coordenadas son correctas y luego lo coloca*/
	
	if (start->row > game->theme.rows && start->column > game->theme.columns)
		return OUT_OF_BOUNDARIES;
	if (end->row > game->theme.rows && end->column > game->theme.columns)
		return OUT_OF_BOUNDARIES;

	if(reposition)
	{
		size = game->user.statusArmy[elementNo][memberNo].dim;
		oldStart = 
				game->user.statusArmy[elementNo][memberNo].position[0].position;
		oldEnd = 
				game->user.statusArmy[elementNo][memberNo].position[size-1].position;
		setInField(BCHAR_UNKNOWN, &oldStart, &oldEnd, &(game->user.defenseField));
	}
	
	error = validateArea(start, end, game, elementNo);
	
	if (error!=0)
	{
		if (reposition)
			setInField(BCHAR_ALIVE, &oldStart, &oldEnd, &(game->user.defenseField));
		return error;
	}
	
	setInGame(start, end, &(game->user), &(game->theme), elementNo, memberNo);
	return 0;
}



int 
validateArea(tPosition * start, tPosition * end, tGame * game, 
						int elementNo)
{
	
	/* valida que tenga el tamaño correcto y que no se toque con otro 
	 * miembro */
	 
	int error = 0, layout;
	
	layout = (start->column == end->column) ? VERTICAL : HORIZONTAL;
	
	if (start->row != end->row && start->column != end->column)
		/* el miembro está en diagonal */
		return NOT_ALLIGNED;
	if (layout == VERTICAL)
	{
		if (abs(end->row - start->row)+1 != 
															((game->theme).elements[elementNo]).size)
			/* el tamaño que se le pasó no coincide con el del miembro */
			return OVERSIZED;
	}
	else
	{
		if (abs(end->column - start->column)+1 != 
							   ((game->theme).elements[elementNo]).size)
			/* el tamaño que se le pasó no coincide con el del miembro */
			return OVERSIZED;
	}
	if ((error = conflictedArea(start, end, &(game->user), 
						game->theme.rows, (game->theme).columns))!=0)
		/*hay miembros en los casilleros aledaños */
		return CONFLICTED_SHIP;
	
	return 0;
}
	

int
conflictedArea(tPosition * start, tPosition * end, tPlayer * player, 
							int rows, int columns)
{
	/* valida si hay elementos en todos los casilleros alrededor */
	int i, layout, from, to, error = 0;
	tPosition position = {0,0};
	layout = (start->column == end->column) ? VERTICAL : HORIZONTAL;
	
	if(layout==VERTICAL)
	{
		from = MIN(start->row, end->row);
		to = MAX(start->row, end->row);
		position.column = start->column;
		for (i=from; i<=to && !error; i++)
		{
			/* por cada posicion llama a conflictedPosition */
			position.row = i;
			if(conflictedPosition(player->defenseField, &position, rows, 
																															columns))
			{
				error = CONFLICTED_SHIP;
			}
		}
	}
	else
	{
		from = MIN(start->column, end->column);
		to = MAX(start->column, end->column);
		position.row = start->row;
		for (i=from; i<=to && !error; i++)
		{
			/* por cada posicion llama a conflictedPosition */
			position.column = i;
			if(conflictedPosition(player->defenseField, &position, rows, 
																															columns))
			{
				error = CONFLICTED_SHIP;
			}
		}
	}
	return error;
}

int
conflictedPosition(tField field, tPosition * position, int rows, 
									int columns)
{
  /* Devuelve 0 si todos los elementos alrededor son BCHAR_UNKNOWN o 
   * BCHAR_WATER  UPDATE: o BCHAR_IMPACTED (se necesita? SI.)
   * BCHAR_IMPACTED, para calcular availableSpace, cuando esta bombardeando
   * no random, sino en bombAround
   * Devuelve diferente de cero de lo contrario */
  
  int i, j;
  int thisRow, thisColumn;
  tBoolean prescenceNearby = FALSE;
  
  for (i = -1; i <= 1 && !prescenceNearby; i++)
  {
    for (j = -1; j <= 1 && !prescenceNearby; j++)
    {
      thisRow = position->row + i;
      thisColumn = position->column + j;
      
      if (UP_TO(thisRow, rows-1) && UP_TO(thisColumn, columns-1))
      {
        /* Dentro de una posicion valida del field */
        
        if (field[thisRow][thisColumn] != BCHAR_UNKNOWN
            && field[thisRow][thisColumn] != BCHAR_WATER
            && field[thisRow][thisColumn] != BCHAR_IMPACTED)
          prescenceNearby = TRUE;
      }
    }
  }
  return prescenceNearby;
}

void
setInField(char bchar, tPosition * start, tPosition * end, 
					tField * field)
{
  /* asume la validez de los parametros */
  
  
  int i;
  int layout, from, to, fixed;

  layout = (start->column == end->column) ? VERTICAL : HORIZONTAL;


  if (layout == VERTICAL)
  { 
    /* es vertical */
  	from = MIN(start->row, end->row);
  	to = MAX(start->row, end->row);
    fixed = start->column;
    
    for (i = from; i <= to; i++)
      (*field)[i][fixed] = bchar;
      
  }
  else
  {
    /* es horizontal */
    from = MIN(start->column, end->column);
  	to = MAX(start->column, end->column);
    fixed = start->row;
    
    for (i = from; i <= to; i++)
      (*field)[fixed][i] = bchar;
  }
}

int
findStatusArmy(const tPosition * position, tPlayer * player, 
							int * element, int * member, const tTheme * theme)
{
	/* se le pasa una posición y devuelve por parámetro que elemento y
	 * miembro está ahí */
	
	int i, j, k, rRow, rCol, size, found =0, order=-1;
	int fRow = position->row, fCol = position->column;
	int totalElements = theme->elemAmount, totalMembers;
	
	for (i=0; i<totalElements && !found; i++)
	{
		totalMembers = theme->elements[i].cant;
		for (j=0; j<totalMembers && !found; j++)
		{
			size = player->statusArmy[i][j].dim;
			for (k=0; k<size && !found; k++)
			{
				rRow = player->statusArmy[i][j].position[k].position.row;
				rCol = player->statusArmy[i][j].position[k].position.column;
				if ((rRow == fRow) && (rCol == fCol))
				{
					*element = i;
					*member = j;
					found = 1;
					order = k;
				}
			}
		}
	}
	return order;
	/* si entra a esta funcion es porque encontro un @ en el tablero,
	 * es decir, siempre va a encontrar el barco */
}

void
insertStatusArmy(tStatusArmy * statusArmy, tPosition * coords, 
								const tTheme * theme, int elementNo, int memberNo, int i)
{
	/* guarda las coordenadas del nuevo barco en statusArmy */
	(*statusArmy)[elementNo][memberNo].dim = 
																		((theme->elements)[elementNo]).size;
	
	(*statusArmy)[elementNo][memberNo].alive = 
																(*statusArmy)[elementNo][memberNo].dim;
	
	(*statusArmy)[elementNo][memberNo].position[i].position.row = 
																														coords->row;
	(*statusArmy)[elementNo][memberNo].position[i].position.column =
																												coords->column;
	(*statusArmy)[elementNo][memberNo].position[i].status = BCHAR_ALIVE;
}

void
setInGame(tPosition * start, tPosition * end, tPlayer * player, 
					const tTheme * theme, int elementNo, int memberNo)
{
	/* asume coordenadas válidas, guarda la posición de cada casillero
	 * donde está ubicado el barco dentro de statusArmy y luego pone 
	 * los '@' en el tablero */

	int from, to, i, layout, order;
	tPosition coords = {0,0};
	layout = (start->column == end->column) ? VERTICAL : HORIZONTAL;
		
	if(layout == VERTICAL)
	{
		from = MIN(start->row, end->row);
		to = MAX(start->row, end->row);
		coords.column = start->column;
		order = 0;		
		for (i=from; i<=to; i++)
		{
			coords.row = i;
			insertStatusArmy(&(player->statusArmy), &coords, theme, elementNo, 
																										memberNo, order++);
		}
	}
	else
	{
		from = MIN(start->column, end->column);
		to = MAX(start->column, end->column);
		coords.row = start->row;
		order = 0;
		for (i=from; i<=to; i++)
		{
			coords.column = i;
			insertStatusArmy(&(player->statusArmy), &coords, theme, elementNo, 
																										memberNo, order++);
		}
	}
	setInField(BCHAR_ALIVE, start, end, &(player->defenseField));
}


int
randPosition(tPosition * position, int rows, int columns)
{
  /* asume que rows > 0 y columns > 0 */
  position->row = rand_int(0, rows - 1);
  position->column = rand_int(0, columns - 1);
  return 1;
}


int
movePosition(tWays way, tPosition * position, int rows, int columns)
{
  /* Desplaza la posicion un casillero NORTE, ESTE, SUR, OESTE */
  /* Devuelve 1 si pudo moverla o 0 si se va afuera del field */
  
  switch (way)
  {
    case NORTH:
      position->row--;
      break;
      
    case EAST:
      position->column++;
      break;
      
    case SOUTH:
      position->row++;
      break;
      
    case WEST:
      position->column--;
      break;
  }
  
  return (UP_TO(position->row, rows - 1) && 
					UP_TO(position->column, columns - 1));
}


int
findPlace(int maxTries, int length, tPosition * p1, tPosition * p2, 
					tField field, int rows, int columns)
{
	/* Encuentra un lugar donde poder poner el miembro de longitud length
   * (No haya miembros existentes al rededor)
   * Deja las posiciones en p1 y p2
  */
  
  int  placed = FALSE, i, found, tries = 0;
  tWays way;
  
  while(!placed && tries < maxTries)
  {
    tries++;
    
    found = FALSE;
    while (!found)
    {
      /* Buscar una posicion random en la que no haya miembros 
       * alrededor*/
      randPosition(p1, rows, columns);
      if (!conflictedPosition(field, p1, rows, columns))
        found = TRUE;
    }
    /*guarda la posición inicial en p2, se obtiene una dirección 
     * aleatoria y se sigue avanzando hasta llegar a i = length, que
     * quiere decir que encontró una posición final válida para el 
     * miembro */
    *p2 = *p1;
    way = RAND_WAY();
    
    i = 1;
    while (i < length
           && movePosition(way, p2, rows, columns)
           && !conflictedPosition(field, p2, rows, columns))
    {
      i++;
    }
    if(i == length)
      placed = TRUE;
  }
  
  return (maxTries - tries)?0:MAP_NOT_GENERATED;
}

int
bomb(const tPosition * position, tPlayer * attacker, 
		tPlayer * defender, const tTheme * theme)
{
	int row = position->row, col = position->column;
	int element, member, order;
	tPosition start, end;
	
	/*valida si está dentro del tablero*/
	if (!(UP_TO(col, theme->columns-1) && UP_TO(row, theme->rows-1)))
		return OUT_OF_BOUNDARIES;
	
	if (defender->defenseField[row][col]==BCHAR_ALIVE)
	{
		if (wasDestroyed(position, defender, &start, &end, theme, &element, 
										&member, &order))
		{
			/* si fue destruido pone todos los casilleros donde está el barco
			 * en 'D' */
			setInField(BCHAR_DESTROYED, &start, &end, 
											&(defender->defenseField));
			setInField(BCHAR_DESTROYED, &start,&end, &(attacker->enemyField));
			attacker->lastMove.position.row = row;
			attacker->lastMove.position.column = col;
			attacker->lastMove.element = element;
			attacker->lastMove.member = member;
			attacker->lastMove.order = order;
			(defender->totalAlive)--;
			return BCHAR_DESTROYED;
		}
		else
		{
			defender->defenseField[row][col]=BCHAR_IMPACTED;
			attacker->enemyField[row][col]=BCHAR_IMPACTED;
			attacker->lastMove.position.row = row;
			attacker->lastMove.position.column = col;
			attacker->lastMove.element = element;
			attacker->lastMove.member = member;
			attacker->lastMove.order = order;
			(defender->totalAlive)--;
			return BCHAR_IMPACTED;
		}
	}
	else if (defender->defenseField[row][col]==BCHAR_UNKNOWN)
	{
		/* si fue agua solo se inserta la 'A' en el tablero y se guardan los
		 * datos en lastMove */
		attacker->enemyField[row][col]=BCHAR_WATER;
		defender->defenseField[row][col]=BCHAR_WATER;
		attacker->lastMove.position.row = row;
		attacker->lastMove.position.column = col;
		return BCHAR_WATER;
	}
	return 0;
}
																																							
int
wasDestroyed(const tPosition * position, tPlayer * player, tPosition * start,
						tPosition * end, const tTheme * theme, int * element, int * member,
						int * order)
{
	/* indica si al bombardear se destruyó el miembro o no (devuelve 1 o 0
	 * según el caso), y devuelve por parametro el número de elemento, 
	 * miembro y orden del barco que se alcanzó...también actualiza
	 * los datos de statusArmy */
	 
	int size, i;
	*order = findStatusArmy(position, player, element, member, theme);
	(player->statusArmy[*element][*member].alive)--;
	if(player->statusArmy[*element][*member].alive==0)
	{
		size = player->statusArmy[*element][*member].dim;
		*start= player->statusArmy[*element][*member].position[0].position;
		*end= player->statusArmy[*element][*member].position[size-1].position;
		for (i=0; i<size; i++)
		{
			player->statusArmy[*element][*member].position[i].status = 
																											BCHAR_DESTROYED;
		}
		return 1;
	}
	player->statusArmy[*element][*member].position[*order].status = 
																												BCHAR_IMPACTED;
	return 0;
}

void
undo(tPlayer * attack, tPlayer * defense)
{
	int row = attack->lastMove.position.row;
	int col = attack->lastMove.position.column;
	int element, member, order, size;
	tPosition startPos, endPos;
	char result = attack->enemyField[row][col];
	if (result != BCHAR_WATER)
	{
		/* si en la última jugada se impactó o destruyó algún barco, se
		 * guardan los datos del barco, después se vuelve atrás cambiando
		 * los tableros y la información en statusArmy */
		element = attack->lastMove.element;
		member = attack->lastMove.member;
		order = attack->lastMove.order;
		if (result == BCHAR_IMPACTED)
		{
			(defense->statusArmy[element][member].alive)++;
			(defense->totalAlive)++;
		}
		else
		{
			if ((size = defense->statusArmy[element][member].dim) > 1)
			{
				startPos = 
							defense->statusArmy[element][member].position[0].position;
				endPos = 
				 defense->statusArmy[element][member].position[size-1].position;
				setInField(BCHAR_IMPACTED, &startPos, &endPos, 
									&(defense->defenseField));
				setInField(BCHAR_IMPACTED, &startPos, &endPos, 
									&(attack->enemyField));
			}
			defense->statusArmy[element][member].alive = 1;
			defense->totalAlive += 1;
		}
		defense->statusArmy[element][member].position[order].status = 
																														BCHAR_ALIVE;
		defense->defenseField[row][col] = BCHAR_ALIVE;
	}
	else if (result == BCHAR_WATER)
		defense->defenseField[row][col] = BCHAR_UNKNOWN;
	/* esto se hace sea agua, impacto o destruido */
	attack->enemyField[row][col] = BCHAR_UNKNOWN;
	attack->lastMove.position.row = UNDOED; /* indica que ya se realizó
																					 * en este turno */
}

int
canUndo(const tPlayer * user, int undos)
{
	/* verifica que si se puede hacer el undo y devuelve el código de 
	 * error */
	if (undos == 3)
		return MAX_UNDOS;
	if (user->lastMove.position.row == FIRST_PLAY)
		return FIRST_PLAY;
	if (user->lastMove.position.row == UNDOED)
		return UNDOED;
	return 1;
}

int
hasWon(int userAlive, int machineAlive)
{
	if (userAlive==0)
	{
		if (machineAlive==0)
			return TIE;
		return MACHINE;
	}
	else if (machineAlive==0)
		return USER;
	else
		return NONE;
}

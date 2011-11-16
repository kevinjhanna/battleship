#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "prototypes.h"


/* formato de cargada [FORMAT_SAVED_] */
#define FMT_S_THEME "Tematica: %2d%c"
#define FMT_S_UNDOS "Undos: %2d%c"
#define FMT_S_ELMNT "Elemento %2d%c"
#define FMT_S_MBMR "Miembro %2d%c"


int wakeAi(tAi *, tPlayer *, tField, tTheme *);
int waitPhrase(FILE *, tBuffer *, char *);
int setTurn(FILE *, tBuffer *, int *);
char memberStatusToBchar(char *);
char coordStatusToBchar(char *);
int loadPlayer(FILE *, tBuffer *, tPlayer *, tTheme *, tField, int);
int getAttackField(FILE *, tBuffer *, tTheme *, tField, tField, int);
int readControlLine(tBuffer *, FILE *);
int getStatus(FILE *, tBuffer *, tTheme *, int *, int *, char *);


int
loadGame(tGame * game, tAllThemes * themes, FILE * savedFile)
{
  int error = 0;
  /* ***************** Buffer ********************
    *
    * bufferSize es el size "virtual" del buffer 
    * El tamaño físico está en buffer->size
    *
  */
  
  tBuffer buffer;
	char phrase[80];
  
  int themeNo = -1, undos = -1;	   

  /* Inicializar el buffer */
  buffer.size = 0;
  buffer.text = NULL;
  

  
  
  /* Conseguimos el numero de tematica que utiliza */
  error = getNumberFormat(savedFile, &buffer, FMT_S_THEME, &themeNo);
  if (themeNo < 1)
    error = THEME_NEGATIVE_VALUE;
  if (themeNo > themes->totalThemes)
    error = SAVED_THEME_NOT_IN_FILE;
  
  
  if (error)
  {
    free(buffer.text);
    return error;
  }
  
  /* Cargar la tematica en game
   * Defasaje humano, las tematicas empiezan en 1
  */
  game->theme = (themes->themesList)[themeNo-1];
  (game->theme).themeNo = themeNo;
    
  /* Ahora podemos allocar memoria para el game */
  initializeGame(game);  
    
  
  /* Conseguir de quien es el proximo turno */
  error = setTurn(savedFile, &buffer, &(game->turn));
  if(error)
    return error;
    
  /* Conseguir undos*/
  error = getNumberFormat(savedFile, &buffer, FMT_S_UNDOS, &undos);
  if (undos > MAX_UNDOS || undos < 0)
    error = EXCEEDED_UNDOS;
  
  if(error)
    return error;
  
  game->undos = undos;

  
  /* Cargar Jugador */
  strcpy(phrase, "Datos del jugador");
  error = waitPhrase(savedFile, &buffer, phrase);
  if(error)
	 return error;
  
  error = loadPlayer(savedFile, &buffer, &(game->user), &(game->theme), game->machine.defenseField, USER);
  if(error)
    return error;
  

  error = loadPlayer(savedFile, &buffer, &(game->machine), &(game->theme), game->user.defenseField, MACHINE);
 
 if (!error)
	error = wakeAi(&(game->ai), &(game->user), game->machine.enemyField, &(game->theme));

   free(buffer.text);

     return error;
}
char
coordStatusToBchar(char * string)
{
  /* Devuelve un BCHAR segun el string
  *	BCHAR_IMPACTED, BCHAR_DESTROYED, BCHAR_WATER, BCHAR_INVALID
  */
  if (!strcmp(string, "IMPACTO"))
    return BCHAR_IMPACTED;
  else if (!strcmp(string, "AGUA"))
   return BCHAR_WATER;
  else if (!strcmp(string, "DESTRUCCION"))
    return BCHAR_DESTROYED;
  else
    return BCHAR_INVALID;
}

char
memberStatusToBchar(char * string)
{
  /* Devuelve un BCHAR segun el string
  *	BCHAR_IMPACTED, BCHAR_ALIVE, BCHAR_INVALID
  */
  if (!strcmp(string, "IMPACTADA"))
    return BCHAR_IMPACTED;
  else if (!strcmp(string, "NO IMPACTADA"))
   return BCHAR_ALIVE;
  else
    return BCHAR_INVALID;
}

int
loadPlayer(FILE * file, tBuffer * buffer, tPlayer * player, tTheme * theme, tField opponentField, int type)
{
	
  /* Carga el jugador y el tablero de ataque del oponente
   * type recibe MACHINE o USER
  */
  
  int i, j, k, error = 0;
  int elementNo;
  int row, column;
  char bchar;
  char status[MAX_STATUS + 1];
  for (i = 0; i < theme->elemAmount && !error; i++)
  {
    /* por cada elemento */
    error = getNumberFormat(file, buffer, FMT_S_ELMNT, &elementNo);
    if (elementNo != i + 1)
	    error = SAVED_ELEMENT_NOT_DECLARED;

    for (j = 0; j < ((theme->elements)[i]).cant && !error; j++)
    {
      /* cada miembro */
	   
	    error = getNumberFormat(file, buffer, FMT_S_MBMR, &elementNo);
	    
	    if (elementNo != j + 1)
				error = SAVED_MEMBER_NOT_DECLARED;
			if (!error)
				player->statusArmy[i][j].dim = theme->elements[i].size;
	    for (k = 0; k < theme->elements[i].size && !error; k++)
	    {
	      /*  cada coordenada*/
				error = getStatus(file, buffer, theme, &row, &column, status);
				if(!error)
				{
					if ((bchar = memberStatusToBchar(status)) == BCHAR_INVALID)
						error = INVALID_MEMBER_COORDINATE;
					
					if (!error)
					{
						/* inserta en statusArmy los datos del miembro impactado */
						player->statusArmy[i][j].position[k].status = bchar;
						player->statusArmy[i][j].position[k].position.row = row;
						player->statusArmy[i][j].position[k].position.column = column;
						player->defenseField[row][column] = bchar;
						/* si es coordenada no impactada */
						if (bchar==BCHAR_ALIVE)
							(player->statusArmy[i][j].alive)++;
						else
							(player->totalAlive)--;
					}
				}
	    }
	    /*si alive = 0, hay que poner todo en D*/
	    if (player->statusArmy[i][j].alive == 0)
	    {
				for (k = 0; k < theme->elements[i].size && !error; k++)
				{
					player->statusArmy[i][j].position[k].status = BCHAR_DESTROYED;
					row = player->statusArmy[i][j].position[k].position.row;
					column = player->statusArmy[i][j].position[k].position.column;
					player->defenseField[row][column] = BCHAR_DESTROYED;
				}
			}
    }
  }
  
  
  error = getAttackField(file, buffer, theme, player->enemyField, opponentField, type);
  
  return error;
}

int
getAttackField(FILE * file, tBuffer * buffer, tTheme * theme, tField field, tField opponentField, int type)
{
  /* va cargando el attack field hasta cierta phrase */
  int error = 0;
  tBoolean found = FALSE;
  int auxRow = -1, auxColumn = -1, params = 0;
  int bufferSize;
  char phrase[50];
  char fmt_coor[50]; 
  char auxStatus[MAX_STATUS + 1];  /* IMPACTADA o NO IMPACTADA*/
	char bchar, ctrlC;
	
  sprintf(fmt_coor, "(%%2d,%%2d) -> %%19[^%c]%%c", CONTROL_CHAR);

  sprintf(phrase, "Datos de la computadora%c", CONTROL_CHAR);

	/* control char al final del buffer */
  ctrlC = !CONTROL_CHAR;
  
  
  while (READ_AND_FIND(file, found, buffer, bufferSize))
  {
    if (bufferSize > 1)
    {
			if((type==USER && strcmp(buffer->text, phrase) == 0 ))
			{
				
				found = TRUE;
			}
			else
			{
				params = sscanf(buffer->text, fmt_coor, &auxRow, &auxColumn, auxStatus, &ctrlC);
				if (params != 4)
					error = SYMBOL_NOT_EXPECTED;
	
				if (ctrlC != CONTROL_CHAR)
		  error = SYMBOL_NOT_EXPECTED;
  
	   if(!UP_TO(auxRow, theme->rows - 1) || !UP_TO(auxColumn, theme->columns - 1))
		error = OUT_OF_BOUNDARIES;
	   
	   if(error)
				return error;
	   
	   /* cargar en status army */
	   
	   if ((bchar = coordStatusToBchar(auxStatus)) != BCHAR_INVALID)
	   {
			field[auxRow][auxColumn] = bchar;
			if (bchar == BCHAR_WATER)
				opponentField[auxRow][auxColumn] = bchar;
			}
			else
				return INVALID_ATTACK_COORDINATE;

	 }
    }
  
  }
  if(type == MACHINE && bufferSize == EOF)
    found = TRUE;
  
  if (!found )
    error = SAVED_COORD_NOT_DECLARED;
  
   if (bufferSize == UNEXPECTED_EOF)
	 error =  UNEXPECTED_EOF;
	
   if (bufferSize == BUFFER_MEMORY)
	 error = BUFFER_MEMORY;
	 

  
  return error;

}


int
getStatus(FILE * file, tBuffer * buffer, tTheme * theme, int * row, int * column, char * string)
{
 /* deja en row, column y bchar cada coordenada */
 int error = 0;
 int auxRow = -1, auxColumn = -1, params = 0;
 int bufferSize;

	  /* control char al final del buffer */
  char ctrlC = !CONTROL_CHAR;

  tBoolean found = FALSE;
  char fmt_coor[50]; 
  char auxStatus[MAX_STATUS + 1];  /* IMPACTADA o NO IMPACTADA*/
  
  sprintf(fmt_coor, "(%%2d,%%2d) -> %%19[^%c]%%c", CONTROL_CHAR);
    
  while (READ_AND_FIND(file, found, buffer, bufferSize))
	{
		if(bufferSize > 1)
		{
			params = sscanf(buffer->text, fmt_coor, &auxRow, &auxColumn, auxStatus, &ctrlC);
			found = TRUE;
		}
	}
	
  if(!UP_TO(auxRow, theme->rows - 1) || !UP_TO(auxColumn, theme->columns - 1))
    error = OUT_OF_BOUNDARIES;

  if (bufferSize == UNEXPECTED_EOF)
	 return UNEXPECTED_EOF;
	
  if (bufferSize == BUFFER_MEMORY)
	 return BUFFER_MEMORY;
	 
  if (!found)
	error = SAVED_COORD_NOT_DECLARED;

  if (params != 4)
		error = SYMBOL_NOT_EXPECTED;
	
  if (ctrlC != CONTROL_CHAR)
	error = SYMBOL_NOT_EXPECTED;
  
  

    
  if (error)
	return error;
  *row = auxRow;
  *column = auxColumn;
  strcpy(string, auxStatus);
	/* Esta todo OK */
	
	
	
  return 0;
}




int
waitPhrase(FILE * file, tBuffer * buffer, char * phrase)
{
  /* Setea el turno segun JUGADOR o MAQUINA */
  int error = 0;
  int params = 0;
  int bufferSize;
  char textPhrase[80];
  char fmt_phrase[10];
  char ctrlC;
  tBoolean found = FALSE;
  
  sprintf(fmt_phrase, "%%79[^%c]%%c", CONTROL_CHAR);
  /* control char al final del buffer */
  ctrlC = !CONTROL_CHAR;


    
  while (READ_AND_FIND(file, found, buffer, bufferSize))
	  {
  		if(bufferSize > 1)
  		{
  			params = sscanf(buffer->text, fmt_phrase, textPhrase, &ctrlC);
  			found = TRUE;
  		}
	}
	

  if (bufferSize == UNEXPECTED_EOF)
	 return UNEXPECTED_EOF;
    
  if (bufferSize == BUFFER_MEMORY)
	 return BUFFER_MEMORY;;
    
    
	if (!found)
		error = PHRASE_NOT_FOUND;

	if (params != 1)
		error = SYMBOL_NOT_EXPECTED;
	
	if (ctrlC != CONTROL_CHAR)
		error = SYMBOL_NOT_EXPECTED;

  
  
  /* Esta todo OK */
  return strcmp(phrase, textPhrase);
}

int
setTurn(FILE * file, tBuffer * buffer, int * turn)
{
  /* Setea el turno segun JUGADOR o MAQUINA */
  int error = 0;
  int params = 0;
  int bufferSize;

  /* control char al final del buffer */
  char ctrlC = !CONTROL_CHAR;

  tBoolean found = FALSE;
  char fmt_turn[80];
  char turnString[MAX_STATUS + 1];/* estatico: JUGADOR o COMPUTADORA */

      
  sprintf(fmt_turn, "Proximo turno: %%20[^%c]%%c", CONTROL_CHAR);
    
  while (READ_AND_FIND(file, found, buffer, bufferSize))
	  {
  		if(bufferSize > 1)
  		{
  			params = sscanf(buffer->text, fmt_turn, turnString, &ctrlC);
  			found = TRUE;
  		}
	}
	

    if (bufferSize == UNEXPECTED_EOF)
	 return UNEXPECTED_EOF;
    if (bufferSize == BUFFER_MEMORY)
	 return BUFFER_MEMORY;;
    
    
	if (!found)
		error = TURN_NOT_FOUND;

	if (params != 2)
		error = SYMBOL_NOT_EXPECTED;
	
	if (ctrlC != CONTROL_CHAR)
		error = SYMBOL_NOT_EXPECTED;

	if (error)
	{
		return error;
	}

  if(strcmp(turnString, USER_NAME) == 0)
    *turn = USER;
  else if (strcmp(turnString, MACHINE_NAME) == 0)
    *turn = MACHINE;
  else
    return TURN_NOT_FOUND;
    
	
  

  /* Esta todo OK */
  return error;
}

int
wakeAi(tAi * ai, tPlayer * user, tField attackField, tTheme * theme)
{
	int i, j, membAmount, counter = 0, elementNo, memberNo;
	tBoolean found = FALSE;
	tPosition positionAux;
	
	/* demostramos existencia y unicidad de un miembro impactado pero no
	 * destruido */
	 
	for (i = 0; i < theme->elemAmount; i++)
	{
		membAmount = theme->elements[i].cant;
		for (j = 0; j < membAmount && !found; j++)
		{
			if (user->statusArmy[i][j].alive != user->statusArmy[i][j].dim)
			{
				elementNo = i;
				memberNo = j;
				counter++;
			}
		}
	}
	
	if (counter > 1)
		return CORRUPTED_SAVED_FILE;
	
	if (counter == 1)
	{
		for (i = 0; i < user->statusArmy[elementNo][memberNo].dim && !found; i++)
		{
			if (user->statusArmy[elementNo][memberNo].position[i].status == 
					BCHAR_IMPACTED)
			{
				found = TRUE;
				ai->target = 
							user->statusArmy[elementNo][memberNo].position[i].position;
				ai->lockTarget = TRUE;
			}
		}
	}
	
	found = FALSE;
	
	for (i = NORTH; i <= WEST && !found; i++)
	{
		positionAux = ai->target;
		if (movePosition(i, &positionAux, theme->rows, theme->columns))
		{
			if (attackField[positionAux.row][positionAux.column] == BCHAR_IMPACTED)
			{
				ai->lockWay = TRUE;
				ai->way = i;
				found = TRUE;
			}
		}
	}
	return 0;
}

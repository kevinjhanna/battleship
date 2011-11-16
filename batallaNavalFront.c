#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include "prototypes.h"

void printMembers(tTheme *, tStatusArmy);
int userPlay(const tPosition *, tPlayer *, tPlayer *, tTheme *);
void printField(tField, tTheme *);

void
printThemes(const tAllThemes* themes)
{
	int i, j, membAmount, size;
	char * name;
	for (i=0; i < themes->totalThemes; i++)
	{
		printf("\n");
		printf("Tematica: %d\n", i+1);
		printf("Nombre: %s\n", themes->themesList[i].name);
		printf("Dimension del tablero: %dx%d\n\n", themes->themesList[i].rows, themes->themesList[i].columns);
		printf("Barcos disponibles:\n");
		for (j=0; j<themes->themesList[i].elemAmount; j++)
		{
			membAmount = themes->themesList[i].elements[j].cant;
			name = themes->themesList[i].elements[j].name;
			size = themes->themesList[i].elements[j].size;
			printf("%d %s, longitud: %d\n", membAmount, name, size);
		}
		printf("\n");
		printf("---------------------------------------------------------------\n");
	}
}

int 
createNewGame(tGame * game, const tAllThemes * themes, tBoolean randomField)
{
	/* devuelve 0 si esta todo bien o el numero de error */
  int error = 0, i, themeNo;
  
  
  printThemes(themes);
  
  themeNo = askTheme(themes->totalThemes);
  
  
  game->theme = (themes->themesList)[themeNo-1];
  game->theme.themeNo = themeNo;
  error = initializeGame(game);
  if(error)
    return error;
  
  game->turn = rand_int(MACHINE, USER);
  
  if (randomField)
  {
		error = MAP_NOT_GENERATED;
    for (i=0; i<3 && error == MAP_NOT_GENERATED; i++)
    {
			error = createNewRPlayer(&(game->user), &(game->theme));
		}
	}
  else
		error = createNewIPlayer(game);
  
  error = MAP_NOT_GENERATED;
  for (i=0; i<3 && error == MAP_NOT_GENERATED; i++)
  {
		error = createNewRPlayer(&(game->machine), &(game->theme));
	}
  
  return error;
}

int
createNewIPlayer(tGame * game)
{
	char * s = malloc(15);
	int elementNo=0, memberNo=0, arg, command = NONE, error, totalMembers;
	tPosition start = {0,0};
	tPosition end = {0,0};
	int sRow , sCol, fRow, fCol, reposition, remaining=0, i;
	
	for(i=0; i<game->theme.elemAmount; i++)
		remaining += game->theme.elements[i].cant;
			
	do
	{
		error = 0;
		reposition = 0;
		printf("\n");
		printMembers(&game->theme, game->user.statusArmy);
		printf("Ingresar comando:\n");
		scanf("%s", s);
		ERASE_BUFFER
		printf("\n");
		arg = sscanf(s, "set(%2d,%2d)(%2d,%2d)(%2d,%2d)", &elementNo, &memberNo, &sRow, 
								&sCol, &fRow, &fCol);
		if (arg == 6)
		{
			start.row = sRow;
			start.column = sCol;
			end.row = fRow;
			end.column = fCol;
			elementNo -= 1;
			memberNo -= 1;
			if (elementNo >= game->theme.elemAmount || elementNo < 0)
			{
				error = NO_SHIP; 
			}
			totalMembers = game->theme.elements[elementNo].cant;
			if (!error)
			{
				if (memberNo >= totalMembers || memberNo < 0)
				{
					error = NO_SHIP;
				}
			}
			/* Si el miembro fue posicionado anteriormente, la validación
			 * de la posición es distinta */
			if (!error)
			{
				if (game->user.statusArmy[elementNo][memberNo].alive != 0)
					reposition = 1;
			
			/* userSet valida las coordenadas y las inserta en el juego */
				error = userSet(&start, &end, game, elementNo, memberNo, reposition);
			}
			if(!error)
			{
				remaining--;
				printField(game->user.defenseField, &(game->theme));
			}
			else
				printError(error);
		}
		else if (!strcmp(s, "done"))
		{
			if (remaining == 0)
				command = DONE;
			else
				printf("No se han ingresado todos los barcos.\n\n");
		}
		else
			printf("Comando invalido.\n");
	} while (command != DONE);
	
	/*printf("Tablero enemigo:\n\n");
	printField(game->user.enemyField, &(game->theme));
	printf("Su tablero:\n\n");
	printField((game->user).defenseField, &(game->theme));*/
	free(s);
	return 0;
}

int
playGame(tGame * game)
{
	char * s = malloc(MAX_FILE_SIZE+1), c;
	char * fileName = malloc(MAX_FILE_SIZE+1);
	int row, col, command = NONE, arg=0, result;
	tPosition position;
	
  tPosition machineBomb;
	
	if (game->turn==MACHINE)
	{
		
		printf("Juega la maquina\n\n");
		
		game->turn = USER;
    
    
    aiBomb(&(game->ai), &machineBomb, &(game->machine), &(game->user), 
					&(game->theme));
		
		minAlive(&(game->ai), &(game->user), &(game->theme));
		
		printf("La maquina bombardeo en: (%d, %d)\n\n", machineBomb.row, 
					machineBomb.column);
	}
	else
	{
		printf("Tablero enemigo: \n\n");
		printField(game->user.enemyField, &(game->theme));
		printf("Su tablero: \n\n");
		printField(game->user.defenseField, &(game->theme));
		do
		{
			printf("Su turno.-\n");
			do
			{
				arg=scanf("%20[^\n]", s);
				ERASE_BUFFER
			} while (arg!=1);
			printf("\n");
			arg = sscanf(s, "bomb(%3d,%3d%c", &row, &col, &c);
			if (arg==3 && c == ')')
			{
				position.column = col;
				position.row = row;
				result = userPlay(&position, &(game->user), &(game->machine), 
													&(game->theme));
				if (result != OUT_OF_BOUNDARIES)
				{
					
					printf("Tablero enemigo: \n\n");
					printField(game->user.enemyField, &(game->theme));
					printf("Su tablero: \n\n");
					printField(game->user.defenseField, &(game->theme));
					printf("Presione ENTER para continuar:");
					ERASE_BUFFER
					printf("\n");
					command = BOMB;
					game->turn = MACHINE;
				}
			}
			else if (!strcmp(s, "quit"))
			{
				free(s);
				free(fileName);
				return QUIT;
			}
			else if ((arg=sscanf(s, "save %s", fileName))==1)
			{
				saveGame(fileName, game);
			}
			else if (!strcmp(s, "undo"))
			{
				if ((result=canUndo(&(game->user), game->undos))==1)
				{
					undo(&(game->user), &(game->machine));
					undo(&(game->machine), &(game->user));
					(game->undos)++;
					
					printf("Utilizo el comando UNDO, tiene %d UNDOs restantes\n\n", 
								(3-(game->undos)));
					printf("Tablero enemigo: \n\n");
					printField(game->user.enemyField, &(game->theme));
					printf("Su tablero: \n\n");
					printField(game->user.defenseField, &(game->theme));
					printf("Presione ENTER para continuar:");
					ERASE_BUFFER
					printf("\n");
				}
				else
				{
					printError(result);
				}
			}
			else
			{
				printf("Comando inval�do.\n");
			}
		} while (command==NONE);
	}
	free(fileName);
	free(s);
	return hasWon(game->user.totalAlive, game->machine.totalAlive);
}


int
userPlay(const tPosition * position, tPlayer * attacker, tPlayer * defender, 
				tTheme * theme)
{
	int result;
	result = bomb(position, attacker, defender, theme);
	switch(result)
	{
		case BCHAR_WATER:
			printf("AGUA!\n\nPresione ENTER para continuar:");
			ERASE_BUFFER
			printf("\n");
			break;
		case BCHAR_IMPACTED:
			printf("IMPACTO!\n\nPresione ENTER para continuar:");
			ERASE_BUFFER
			printf("\n");
			break;
		case BCHAR_DESTROYED:
			printf("DESTRUIDO!\n\nPresione ENTER para continuar:");
			ERASE_BUFFER
			printf("\n");
			break;
		case OUT_OF_BOUNDARIES:
			printf("Comando invalido\n\n");
			return OUT_OF_BOUNDARIES;
	}
	return result;
}



void
printField(tField field, tTheme * theme)
{
	int i, j;
	/* header del tablero */
	printf("  ");
	for (i = 0; i < theme->columns; i++)
		printf("%2d",i);
	printf("\n");

  
	for (i = 0; i < theme->rows; i++)
	{
		for (j = 0; j < theme->columns; j++)
		{
			if (j == 0 ) 
				printf("%2d ", i);
			printf("%c ", field[i][j]);
		}
		printf("\n");
	}
	printf("\n");
}

void
printMembers(tTheme * theme, tStatusArmy statusArmy)
{
	/*Imprime una lista con los miembros restantes cada vez que se 
	 * coloca uno nuevo */
	 
	int membAmount, i, j, totalAmount, size;
	char * name;
	printf("Barcos disponibles:\n\n");
	for (i=0; i<theme->elemAmount; i++)
	{
		membAmount = theme->elements[i].cant;
		name = theme->elements[i].name;
		size = theme->elements[i].size;
		totalAmount = 0;
		for (j=0; j<membAmount; j++)
		{
			if (statusArmy[i][j].alive == 0)
				totalAmount += 1;
		}
		printf("%d %s, longitud: %d\n", totalAmount, name, size);
	}
	printf("\n");
}

tBoolean 
askIfRandomField(void)
{
	int option, randomField, arg;
	printf("Desea que el tablero se genere aleatoriomente?\n");
	printf("Elija su opcion:\n1)Si\n2)No\n");
	do
	{
		arg = scanf("%d",&option);
		ERASE_BUFFER
	} while (arg!=1 || (option<1 || option>2));

	
	if (option==1)
	{
		randomField=TRUE;
	}
	else
	{
		randomField=FALSE;
	}
	return randomField;
}

int
askMode(void) /*solo levanta la opcion, el case lo hace el main*/
{
  int mode, arg=0;
  printf("Bienvenido al juego, por favor elija una de las siguientes ");
  printf("opciones:\n1)Comenzar una nueva partida\n2)Recuperar una partida ");
  printf("guardada\n3)Salir\nIngrese su opcion:\n");
  do
  {
		arg = scanf("%d",&mode);
   	ERASE_BUFFER
  } while (arg!=1 || (mode<1 || mode>MAX_MODE));
  return mode;
}

void
printError(int error)
{
  switch(error)
  {
    case NO_MEMORY:
      printf("Disculpe, no hay suficente memoria ");
      printf("en su computador para jugar a la batalla naval.");
      break;
      
    case THEMES_FILE_NOT_EXITS:
      printf("No se ha encontrado el archivo tematicas.dat.");
      break;
      
    case TOTAL_THEMES_NOT_DECLARED:
      printf("El archivo tematicas.dat no ");
      printf("indica la cantidad total de tematicas.");
      break;
	
    case TOTAL_THEMES_NOT_IN_RANGE:
      printf("El archivo tematicas.dat ");
      printf("indica un total de tematicas negativo.");
      break;
      
    case THEMES_SYMBOL_NOT_EXPECTED:
      printf("La tematica contiene simbolos inesperados.");
      break;
	  
    case THEME_NOT_EXISTS:
      printf("El archivo tematicas.dat ");
      printf("no contiene todas las tematicas indicadas.");
      break;
	  
    case THEME_ELEMENT_NOT_DECLARED:
      printf("El archivo tematicas.dat ");
      printf("contiene alguna tematica ");
      printf("con algun elemento no declarado");
      break;
	  
    case THEME_NO_ELEMENTS:
      printf("El archivo tematicas.dat ");
      printf("contiene alguna tematica ");
      printf("que indica un total de elementos negativo");
      break;
    
    case THEME_NEGATIVE_VALUE:
      printf("El archivo ");
      printf("contiene alguna valor negativo");
      break;
	  
    case THEME_ELEMENT_TOO_LARGE:
      printf("El archivo tematicas.dat ");
      printf("contiene alguna tematica ");
      printf("con algun elemento que no entra en el tablero.");
      break;
	  
    case OVERSIZED:
      printf("El miembro no cumple con las proporciones indicadas.");
      break;

    case NOT_ALLIGNED:
      printf("El miembro debe ser ubicado en forma horinzontal o vertical.");
      break;

    case TURN_NOT_FOUND:
      printf("El archivo de partida cargada ");
      printf("no indica de quien es el turno.");
      break;

    case EXCEEDED_UNDOS:
        printf("El archivo de partida cargada ");
        printf("indica una cantidad de UNDOS excedida.");
	      break;

    case PHRASE_NOT_FOUND:
        printf("El archivo que se esta cargando esta corrupto.");
	      break;
	      
    case CONFLICTED_SHIP:
      printf("El elemento no puede ser ubicado ya que se encuentra en ");
      printf("contacto con otros elementos.");
      break;
      
    case UNEXPECTED_EOF:
      printf("Hay un caracter no permitido en el archivo cargado. ");
      printf("(ver documentacion especifica de nuestro trabajo).");
      /* CONTROL_CHAR no permitido */
      break;

    case MAP_NOT_GENERATED:
	    printf("El mapa aleatorio no se ha podido generar ");
	    printf("con las dimensiones requeridas. Intente nuevamente.");
	    break;

    case OUT_OF_BOUNDARIES:
	    printf("Se ha escapado de los limites");
	    break;
    
    case TEXT_OVERFLOW:
	    printf("El archivo cargado no termina correctamente.");
	    break;
    
    case INVALID_MEMBER_COORDINATE:
	    printf("Algun miembro tiene una coordenada invalida.");
	    break;
	  
    case NO_SHIP:
	    printf("No existe tal elemento o tal miembro.");
	    break;
	  
    case INVALID_ATTACK_COORDINATE:
	    printf("La coordenada de ataque es invalida.");
	    break;
	    
    case SAVED_ELEMENT_NOT_DECLARED:
      printf("El archivo de partida cargada ");
	    printf("no contiene todos los elementos necesarios");
	    break;

    case SAVED_MEMBER_NOT_DECLARED:
      printf("El archivo de partida cargada ");
	    printf("no contiene todos los miembros necesarios.");
	    break;

    case SAVED_FILE_NOT_EXISTS:	  
      printf("El archivo de partida cargada ");
      printf("no existe.");
    	break;
	  
    case SAVED_INVALID_THEME:	  
      printf("El archivo de partida cargada ");
    	printf("contiene una tematica invalida.");
    	break;
	  
    case SAVED_THEME_NOT_IN_FILE:
      printf("El archivo de partida cargada ");
    	printf("contiene una tematica que no esta en tematicas.dat.");
    	break;

    case SAVED_COORD_NOT_DECLARED:
      printf("El archivo de partida cargada ");
    	printf("contiene alguna coordenada sin declarar.");
    	break;
    	
    case SAVED_PLAYER_HEADER_NOT_DECLARED:
      printf("El archivo de partida cargada ");
			printf("no define algun jugador.");
			break;
    
    case SYMBOL_NOT_EXPECTED:
	    printf("El archivo cargado contiene algun simbolo inesperado.");
			break;
	  
    case WRONG_DECLARED:
			printf("El archvio cargado esta corrupto.");
			break;
		case EXCEEDED_MAX_FIELD_SIZE:
  		printf("Una tematica no puede ser mas grande que 99x99.");
  		break;
  }
  printf("\n");
  
}



int
askTheme(int totalThemes)
{
	int themeNo = -1, arg;
	do
	{
		printf("Por favor ingrese el numero de la tematica elegida:");
		arg = scanf("%d", &themeNo);
		ERASE_BUFFER;
	} while (themeNo < 1 || themeNo > totalThemes);	
	printf("\n\n");
	return themeNo;
}


int
askFile(FILE ** file) 
{
	/*pregunta el archivo, si el nombre no 
	 * es correcto fopen va a devolver null*/
	
  tBoolean error;
  char fileName[MAX_FILE_SIZE+1];
  do
  {
		error = FALSE;
		printf("Por favor ingrese un nombre valido de partida");
		printf(" o ingrese q:\n");
		scanf("%[^\n]", fileName);
		ERASE_BUFFER
		if (!strcmp(fileName, "q"))
			return QUIT;
		if ((*file = fopen(fileName, "r+t"))==NULL)
		{
			printf("No existe el archivo, intente nuevamente\n");
			error = TRUE;
		}
	} while (error);
	
	return 0;
}

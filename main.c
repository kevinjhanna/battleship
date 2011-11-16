#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <unistd.h>
#include "prototypes.h"

int loadGame(tGame *, tAllThemes *, FILE *);
int getFile(tBuffer *);
int askFile(FILE **);

int
main(void)
{
	tAllThemes themes;
	tGame game;
	tBoolean randomField;
	int error = 0, mode, winner = NONE;
	FILE * file;
	
	rand_initialize();
	
	printf("Cargando tematicas...\n");
	sleep(1);
	
	error = loadThemes(&themes);
	
	if (error)
	{
		printError(error);
		return 0;
	}
	
	mode = askMode();


  switch(mode)
  {
		case NEW:
		{
			randomField = askIfRandomField();
      
      error = createNewGame(&game, &themes, randomField);
			break;
		}
    case LOAD:
    {
      error = askFile(&file);  /*devuelve 0 si lo encuentra*/
      if(!error)
      {
        printf("Cargando partida...\n");
        sleep(1);
				error = loadGame(&game, &themes, file);
				fclose(file);
			}
			else
				return 0;
			break;
		}
    case QUIT:
    {
			freeAllThemes(&themes, themes.totalThemes);
      return 0;
		}
  }
  
  if(error)
  {
    printError(error);
		freeAllThemes(&themes, themes.totalThemes);
    return 0;
  }
  
  do
  {
		winner = playGame(&game);
	} while(winner==NONE);
  
  if (winner!=QUIT)
		printf("%s ha ganado! Gracias por jugar.\n", 
					winner?"El usuario":"La computadora");
	else
		printf("Gracias por jugar\n");
  
  freeGame(&game);
  freeAllThemes(&themes, themes.totalThemes);
  
  return 0;
}

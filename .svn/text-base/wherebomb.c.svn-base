#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include "prototypes.h"

#define RAND_VERTICAL() (rand_int(NORTH, SOUTH))
#define RAND_HORIZONTAL() (rand_int(EAST, WEST))

int availableSpace(tWays way, tPosition *, tField, int, int);
int conflictedVision(tField, tPosition *, int, int, tWays way);


void
turnWay(tWays * way)
{
  /* pensar forma matematica*/
  switch(*way)
  {
    case NORTH:
      *way = SOUTH;
      break;
    case SOUTH:
      *way = NORTH;
      break; 
    case EAST:
      *way = WEST;
      break;
    case WEST:
      *way = EAST;
      break;
  }
}




tPosition
bombAround(tPosition * target, tField attackField, int layout, tWays * way, 
					int rows, int columns)
{
  /* 
   * Devuelve una posicion donde bombardear, aleatoria alrededor del target
   * En way deja la direccion donde intento
   *
   * Recibe en layout, si intentar vertical o horizontal
   * Lo que reciba, sabe que una de las dos ways es posible
  */

  tBoolean found = FALSE;
  tPosition try;
  int i;
  
  if (layout == VERTICAL)
    /* Intentar solo NORTH o SOUTH */
    *way = RAND_VERTICAL();
  else
    /* Intentar solo WEST o EAST */
    *way = RAND_HORIZONTAL();
  
  /* (NORTH && SOUTH) o (WEST && EAST): son dos intentos  */
  for (i = 0; i < 2 && !found; i++)
  {
    try = *target;
    
    if (movePosition(*way, &try, rows, columns)
        && !conflictedVision(attackField, &try, rows, columns, *way)
        && attackField[try.row][try.column] == BCHAR_UNKNOWN)
    {
      found = TRUE;
    }
    else
    {
      turnWay(way);
    }
  }
  
  return try;
}




tPosition
bombInLine(tPosition * target, tField attackField, tWays * way, int rows, 
					int columns)
{
  /* 
   * En way recibe por que lado comenzar
   * Devuelve una posicion donde bombardear, en linea del target
   * En way deja la direccion donde intento
   *
  */
  tBoolean found = FALSE;
  tPosition try;

  try = *target;

  
  while (!found)
  {
    
    if (movePosition(*way, &try, rows, columns) && 
				!conflictedVision(attackField, &try, rows, columns, *way))
    {
      if(attackField[try.row][try.column] == BCHAR_UNKNOWN)
      {
        found = TRUE;

      }
    }
    else
    {
      turnWay(way);
      movePosition(*way, &try, rows, columns);
      

    }
  }
  
  return try;
  
}


void
minAlive(tAi * ai, tPlayer * user, tTheme * theme)
{
	int membAmount, i, j, found = 0;
	ai->minAlive = user->totalAlive; /* inicializa minAlive en totalAlive
																		* porque nunca va a ser más grande*/
	
	/* recorre la matriz statusArmy almacenando en minAlive los barcos
	 * que tengan una cantidad de elementos vivos menor a la almacenada 
	 * anteriormente hasta que encuentre un barco con la cantidad minima 
	 * de casilleros vivos (1) o termine de recorrerla */
	
	for (i=0; i<theme->elemAmount && !found; i++)
	{
		membAmount = theme->elements[i].cant;
		for (j=0; j<membAmount && !found; j++)
		{
			if (user->statusArmy[i][j].alive == 1)
				found = 1;
			if (user->statusArmy[i][j].alive < ai->minAlive)
				ai->minAlive = user->statusArmy[i][j].alive;
		}
	}
}






int
conflictedVision(tField field, tPosition * position, int rows, int columns, 
								tWays way)
{
  /* Basada en conflictedPosition, si x es la posicion, O es lo que verifica
  * 
  *	Segun el WAY, en este caso NORTH
  *
  *	[ ][o][o][o][ ]
  *  [ ][o][x][o][ ]
  *	[ ][ ][ ][ ][ ]
  * Devuelve 0 si todos los elementos son BCHAR_UNKNOWN o BCHAR_WATER o 
  * 	BCHAR_IMPACTED  
   * Devuelve diferente de cero de lo contrario
  */
  
  int i, j;
  int thisRow, thisColumn;
  int fromRow, fromColumn, toRow, toColumn;
  tBoolean prescenceNearby = FALSE;
  
  switch(way)
  {
    case NORTH:
	   fromRow = -1;
	   toRow = 0;
	   fromColumn = -1;
	   toColumn = 1;
	 break;
	 
    case SOUTH:
	   fromRow = 0;
	   toRow = 1;
	   fromColumn = -1;
	   toColumn = -1;
	 break;
	 
    case EAST:
	   fromRow = 0;
	   toRow = 1;
	   fromColumn = 0;
	   toColumn = 1;
	 break;
	 
     case WEST:
	   fromRow = 0;
	   toRow = 1;
	   fromColumn = -1;
	   toColumn = 0;
	 break;
  }
  
  
  for (i = fromRow; i <= toRow && !prescenceNearby; i++)
  {
    for (j = fromColumn; j <= toColumn && !prescenceNearby; j++)
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


int
randBombPosition(tField field, tPosition * bombPosition, int rows, int columns,
								int minElement, tAi * ai)
{
  /* deja en Ai, hSpace espacio horizontal para ubicar un barco y en vSpace el
   * espacio vertical */
  
  tBoolean found = FALSE;
  int horizontalSpace, verticalSpace;
  while(!found)
  {
    /* inicilizar el espacio disponible por cada posicion */
    horizontalSpace = 1;
    verticalSpace = 1;
    
    /* Buscar una posicion que se pueda bombarderar */
    randPosition(bombPosition, rows, columns);
    
    if(field[bombPosition->row][bombPosition->column] == BCHAR_UNKNOWN
      && !conflictedPosition(field, bombPosition, rows, columns))
	  {
      horizontalSpace += availableSpace(EAST, bombPosition, field, rows, 
																				columns);
      horizontalSpace += availableSpace(WEST, bombPosition, field, rows, 
																				columns);

      verticalSpace += availableSpace(NORTH, bombPosition, field, rows,
																			columns);
      verticalSpace += availableSpace(SOUTH, bombPosition, field, rows, 
																			columns);
    /*
      * en cada Space, queda cuantas posiciones libres hay hasta chocar 
       * contra un elemento */
      if(horizontalSpace >= minElement || verticalSpace >= minElement)
      {
        /* guarda los ai*/
        ai->hSpace = horizontalSpace;
        ai->vSpace = verticalSpace;
        found = TRUE;
      }
    }
  }
  return 0;
}


/* devuelve una posicion random CORRECTA para bombarderar */

void
aiBomb(tAi * ai,tPosition * bombPosition, tPlayer * machine, tPlayer * user, 
			tTheme * theme)
{

  tField attackField = machine->enemyField; /*tablero de ataque de la maquina*/
  
  int layout;
  int minElement = ai->minAlive;
  int bombResult;
  
  if (ai->lockTarget)
  {
    /* En los turnos anteriores impactamos un elemento
     * Vamos a seguir destruyendolo
    */
    if (ai->lockWay)
    {
      /* Ya sabemos si es horizontal o vertical */
      *bombPosition = bombInLine(&(ai->target), attackField, &(ai->way), 
																theme->rows, theme->columns);
      
      bombResult = bomb(bombPosition, machine, user, theme);
      
      switch(bombResult)
      {
        case BCHAR_WATER:
          turnWay(&(ai->way));
          break;
          
        case BCHAR_DESTROYED:
          ai->lockWay = FALSE;
          ai->lockTarget = FALSE;
          break;
      }
      
      
      
    }
    else
    {
      /* Bombardear alrededor para descubrir si es horizontal o vertical
       * en Ai sabe si hay suficiente espacio
      */
      
      if(ai->hSpace >= minElement && ai->vSpace >= minElement)
      {
        /* el minimo elemento entra tanto en horizontal como vertical */
        layout = rand_int(0,1);
      }
      else if (ai->hSpace >= minElement)
      {
        /* solo entra en horizontal */
        layout = HORIZONTAL;
      }
      else
      {
        /* solo entra vertical*/
        layout = VERTICAL;
      }
      
      *bombPosition = bombAround(&(ai->target), attackField, layout, 
																&(ai->way), theme->rows, theme->columns);

      bombResult = bomb(bombPosition, machine, user, theme);
      
      switch(bombResult)
      {
        case BCHAR_IMPACTED:
          ai->lockWay = TRUE;
          break;
          
        case BCHAR_DESTROYED:
          ai->lockWay = FALSE;
          ai->lockTarget = FALSE;
          break;
      }
      
      /* Hay que volver a calcular hSpace y vSpace
	  * Esta vez no contamos el casillero target (inicializar space en 0)
	 */
      ai->hSpace = 0;
      ai->hSpace += availableSpace(EAST, &(ai->target), attackField,
																	theme->rows, theme->columns);
      ai->hSpace += availableSpace(WEST, &(ai->target), attackField, 
																	theme->rows, theme->columns);

      ai->vSpace = 0;
      ai->vSpace += availableSpace(NORTH, &(ai->target), attackField, 
																	theme->rows, theme->columns);
      ai->vSpace += availableSpace(SOUTH, &(ai->target), attackField, 
																	theme->rows, theme->columns);
      
    }
  }
  else
  {
    /* Posicion al azar */

    randBombPosition(attackField, bombPosition, theme->rows, theme->columns,
										minElement, ai);
    if (bomb(bombPosition, machine, user, theme) == BCHAR_IMPACTED)
    {
          ai->lockTarget = TRUE;
          ai->target = *bombPosition;
    }
  }
}

int
availableSpace(tWays way, tPosition * position, tField field, int rows, 
							int columns)
{
  /* Devuelve la cantidad de casilleros habilitados para ocupar 
   * (sin contar position)
   * En sentido way
   * no cambia position dada
   * ej: |@|.|.|.|position|.|.|@|@| EAST devuelve 1, WEST devuelve 2
   *
   * ej: |@|.|.|A|position|A|.|@|@| EAST devuelve 0, WEST devuelve 0
   */
  
  tBoolean found = FALSE;
  int availableSpace = 0;
  tPosition lastPosition;
  lastPosition = *position;

  while(!found && movePosition(way, &lastPosition, rows, columns))
  {
    
    if(!conflictedPosition(field, &lastPosition, rows, columns)
      && field[lastPosition.row][lastPosition.column]!= BCHAR_WATER)
    {
      availableSpace++;
    }
    else
      found = TRUE;
  }
  return availableSpace;
}

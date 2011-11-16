#ifndef _prototypes_h
  #define _prototypes_h
  


#include "random.h"
#include <stdio.h>


#define NEW 1
#define LOAD 2
#define QUIT 3
#define MAX_MODE 3
#define ERASE_BUFFER while(getchar() != '\n') ;

#define SET 5
#define DONE 6
#define BOMB 4
#define NONE -1

#define MAX(a, b) (((a) > (b)) ? (a):(b))
#define MIN(a, b) (((a) < (b)) ? (a):(b))

#define RAND_WAY() (rand_int(NORTH, WEST))

#define UP_TO(var, lastN) ((var) >= 0 && (var) <= (lastN))

/* Board chars */
#define BCHAR_UNKNOWN '.'/*desconocido para el oponente o agua para el propio*/
#define BCHAR_ALIVE '@'
#define BCHAR_IMPACTED 'x'
#define BCHAR_DESTROYED 'D'
#define BCHAR_WATER 'A'
#define BCHAR_INVALID 0

#define VERTICAL 0
#define HORIZONTAL !VERTICAL
#define BLOCK 10
#define MAX_TRIES 10
#define MAX_UNDOS 3
#define FIRST_PLAY -1
#define UNDOED -2
#define MAX_FILE_SIZE 30



/*********************** Lectura de archivos ******************************/
#define READ(file, buffer, bufferSize)\
					  (bufferSize = readControlLine(buffer, file))
					  
#define READ_AND_FIND(file, found, buffer, bufferSize)\
				  (!found && (READ(file, buffer, bufferSize)) > 0)
				  
/*Buffer*/
#define COMMENT_START ';'
#define CONTROL_CHAR '_'
#define BLOCK 10

/* Buffer Errors*/
#define BUFFER_MEMORY -3
#define UNEXPECTED_EOF -2
/****************************************************************************/

#define MAX_NAME 200
#define MAX_STATUS 20 /* IMPACTADA, NO IMPACTADA, DESTRUCCION, ETC*/
#define MAX_FIELD_SIZE 99


enum errors { NO_MEMORY=1,

							SYMBOL_NOT_EXPECTED,
							WRONG_DECLARED,
							
							THEMES_FILE_NOT_EXITS,
							THEME_NOT_EXISTS,
							THEME_NO_ELEMENTS,
							TOTAL_THEMES_NOT_DECLARED,
							TOTAL_THEMES_NOT_IN_RANGE,
							THEMES_SYMBOL_NOT_EXPECTED,
							THEME_NEGATIVE_VALUE,
							THEME_ELEMENT_NOT_DECLARED,
							THEME_ELEMENT_TOO_LARGE,
							
							OVERSIZED,
							NOT_ALLIGNED,
							TURN_NOT_FOUND,
							EXCEEDED_UNDOS,
							PHRASE_NOT_FOUND,
							CONFLICTED_SHIP,
							MAP_NOT_GENERATED,
							OUT_OF_BOUNDARIES,

							TEXT_OVERFLOW,
							INVALID_MEMBER_COORDINATE,
							NO_SHIP,
							INVALID_ATTACK_COORDINATE,
							
							SAVED_ELEMENT_NOT_DECLARED,
							SAVED_MEMBER_NOT_DECLARED,
							SAVED_FILE_NOT_EXISTS,
							SAVED_INVALID_THEME,
							SAVED_THEME_NOT_IN_FILE,
							SAVED_COORD_NOT_DECLARED,
							SAVED_PLAYER_HEADER_NOT_DECLARED, CORRUPTED_SAVED_FILE,
							THEMES_FILE_NOT_EXISTS,
							
							EXCEEDED_MAX_FIELD_SIZE
							};

typedef enum way {NORTH = 1, SOUTH, EAST, WEST} tWays;
typedef enum {FALSE = 0, TRUE} tBoolean;
enum {MACHINE = 0, USER} tTurn;
#define TIE 2;
#define USER_NAME "Jugador"
#define MACHINE_NAME "Computadora"







typedef struct
{
  int row;
  int column;
} tPosition;

typedef struct
{
  tPosition position;
  int status; /* ALIVE, IMPACTED, DESTROYED */
} tPositionStatus;

typedef struct
{
  int alive; /* cantidad de casilleros vivos */
  int dim; /* dimensión de la nave */
  tPositionStatus * position;
} tStatus;

typedef tStatus ** tStatusArmy;

/**********************************************************************/

typedef struct
{
  tPosition position;
  int element;
  int member;
  int order; /* índice que indica el orden en el que se encuentra el 
							* casillero dentro del elemento */
} tLastMove;

typedef char * tRow;
typedef tRow * tField;

typedef struct
{
  tField enemyField;
  tField defenseField;
  tStatusArmy statusArmy;
  int totalAlive; /* cantidad de casilleros vivos del jugador */
  tLastMove lastMove;
} tPlayer;

typedef struct
{
  int size;
  int cant;
  char * name;
} tElement;

typedef struct
{
	char * text;
	int size;
} tBuffer;

typedef struct
{
  char * name;
  tElement * elements;
  int elemAmount;
  int themeNo;
  int rows;
  int columns;
} tTheme;

typedef struct
{
  tTheme * themesList;
  int totalThemes;
} tAllThemes;


typedef struct
{
 tWays way; /* la direccion por la cual intentar el proximo turno */
 tPosition target; /* ultimo lugar donde bombardeo correctamente */
 tBoolean lockTarget; /* TRUE si ya sabe que el target esta por ahi*/
 tBoolean lockWay; /* TRUE si  ya descubrio si es horizontal o vertical */
 int hSpace; /* Casilleros disponibles para ubicar un barco, HORIZONTAL */
 int vSpace; /* Casilleros disponibles para ubicar un barco, VERTICAL */
 int minAlive;
} tAi;


typedef struct
{
  tTheme theme;
  tPlayer user;
  tPlayer machine;
  tAi ai;
  int turn;
  int undos;
} tGame;


int initializeGame(tGame *);
int createNewGame(tGame *, const tAllThemes *, tBoolean);
int createNewIPlayer(tGame *);
int conflictedPosition(tField, tPosition *, int, int);
int createNewRPlayer(tPlayer *, const tTheme *);
int movePosition(tWays way, tPosition *, int, int);
int askMode(void);
tBoolean askIfRandomField(void);
int bomb(const tPosition *, tPlayer *, tPlayer *, const tTheme *);
int playGame(tGame *);
void undo(tPlayer *, tPlayer *);
int canUndo(const tPlayer *, int);
void printError(int error);
void freeGame(tGame *);
void saveGame(const char *, const tGame *);
int loadThemes(tAllThemes *);
int askTheme(int totalThemes);
void freeStatusArmy(tStatusArmy *, tTheme *);
void freeField(tField, int);
int userSet(tPosition *, tPosition *, tGame *, int, int, int);
int hasWon(int, int);
void freeAllThemes(tAllThemes *, int);
int readControlLine(tBuffer *, FILE *);
tBoolean keepOnReading(FILE *, tBuffer *);
int getNumberFormat(FILE *, tBuffer *, char *, int *);
void aiBomb(tAi *,tPosition *, tPlayer *, tPlayer *, tTheme *);
void minAlive(tAi *, tPlayer *, tTheme *);
int randPosition(tPosition *, int, int);

#endif

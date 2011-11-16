#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "prototypes.h"


#define THEMES_FILE_NAME "tematicas.dat"

/* Formatos del archivo */
#define FMT_TOTAL_THEMES "Numero de tematicas: %3d%c"
#define FMT_THEME_START "Tematica  %3d%c"
#define FMT_THEME_FIELD_SIZE "Tablero: %2dx%2d%c"
#define FMT_THEME_TOTAL_ELEMENTS "Numero de elementos de la flota: %3d%c"


int getThemeName(FILE *, tBuffer *, char **);
int getElement(FILE *, tBuffer *, int, char **, int *, int *);



int
getTotalThemes(FILE * themesFile, tBuffer * buffer, tAllThemes * themes)
{
  
  /* Deja en themes la cantidad de themes indicada por el encabezado
   * del archivo de tematicas.dat
   * Calloquea el espacio necesario
   * Devuelve 0 si esta todo OK, o el número de error 
  */
  
  int error = 0;
  int auxTotalThemes = -1, params = 0;
  int bufferSize;
	
	
	/* control char al final del buffer */
	char ctrlC = !CONTROL_CHAR;
	
	tBoolean found = FALSE;
	
	
  while (READ_AND_FIND(themesFile, found, buffer, bufferSize))
  {
  	if(bufferSize > 1)
  	{
  		params = sscanf(buffer->text, FMT_TOTAL_THEMES, &auxTotalThemes, &ctrlC);
  		found = TRUE;
  	}
  }

  /* Validar totalThemes*/
  if (bufferSize == UNEXPECTED_EOF)
    return UNEXPECTED_EOF;
    
  if (bufferSize == BUFFER_MEMORY)
	 return BUFFER_MEMORY;
	 
  if (!found)
  	error = TOTAL_THEMES_NOT_DECLARED;

  if (params != 2)
  	error = SYMBOL_NOT_EXPECTED;
  	
  if (auxTotalThemes < 0)
  	error = TOTAL_THEMES_NOT_IN_RANGE;
	
  if (ctrlC != CONTROL_CHAR)
  	error = SYMBOL_NOT_EXPECTED;

  if (error)
  	return error;
	
  themes->themesList = calloc(auxTotalThemes, sizeof(tTheme));
  
  if (themes->themesList == NULL)
  	return NO_MEMORY;

  /* Esta todo OK */
  themes->totalThemes = auxTotalThemes;
  return 0;
}


int
getFieldSize(FILE * themesFile, tBuffer * buffer, int * rows, int * columns)
{
	/* Deja en rows y columns el size del field,
	 * Devuelve 0 si no hay error
	 * o el numero de error
	*/
	  
	int error = 0;
	int auxRows = -1, auxColumns = -1, params = 0;
	int bufferSize;

 	/* control char al final del buffer */
	char ctrlC = !CONTROL_CHAR;

	tBoolean found = FALSE;
   
  while (READ_AND_FIND(themesFile, found, buffer, bufferSize))
	{
		if(bufferSize > 1)
		{
	
			params = sscanf(buffer->text, FMT_THEME_FIELD_SIZE, &auxRows, &auxColumns, &ctrlC);
			found = TRUE;
		}
	}
	
	if (auxRows < 1 || auxRows > MAX_FIELD_SIZE
	    || auxColumns < 1 || auxColumns > MAX_FIELD_SIZE)
      error = MAX_FIELD_SIZE; 
  
	
	
	if (bufferSize == UNEXPECTED_EOF)
		return UNEXPECTED_EOF;
	
	if (bufferSize == BUFFER_MEMORY)
		return BUFFER_MEMORY;
	
    
	if (!found)
		error = THEME_ELEMENT_NOT_DECLARED;

	if (params != 3)
		error = SYMBOL_NOT_EXPECTED;
	
	if (ctrlC != CONTROL_CHAR)
		error = SYMBOL_NOT_EXPECTED;
  
  
	if (error)
		return error;
	
	*rows = auxRows;
	*columns = auxColumns;
  /* Esta todo OK */
  return 0;
}



void
freeElements(tElement * elements, int dim)
{
  /* Libera memoria de los elementos
   * inicializados en null
  */
  int i;
  for (i = 0; i < dim; i++)
  {
   if(elements[i].name != NULL)
	 free(elements[i].name);
  }
 if(elements != NULL)
  free(elements);
}

void
freeTheme(tTheme * theme)
{
  if (theme->name != NULL)
    free(theme->name);
  if (theme->elements != NULL) 
    freeElements(theme->elements, theme->elemAmount);
}

void
freeAllThemes(tAllThemes * allThemes, int dim)
{
  int i;
  for (i = 0; i < dim; i++)
  {
    if (&(allThemes->themesList[i]) != NULL)
	 freeTheme(&((allThemes->themesList)[i]));
  }
  if (allThemes->themesList != NULL)
    free(allThemes->themesList);
}

int
loadThemes(tAllThemes * themes)
{
	int i, j;
	int error = 0;
	tBoolean textOverflow = FALSE; /* para ver si hay texto de mas al final del archivo*/
	/* ***************** Buffer ********************
	 *
	 * bufferSize es el tamaño "virtual" del buffer 
	 * El tamaño físico está en buffer->size
	 *
	*/
	tBuffer buffer;

	
	char * themeName; 
	
	FILE * themesFile;
	
	
	int rows, columns;
	char * name;
	int length, quantity;
	int themeNo = -1;
	int totalElements = -1;
		
	/* Abrir archivo de themes */
	themesFile = fopen(THEMES_FILE_NAME, "rt");
	if (themesFile == NULL)
	  return THEMES_FILE_NOT_EXISTS;

	/* Inicializar el buffer */
	buffer.size = 0;
	buffer.text = NULL;
	
  /* Conseguir la cantidad de themes indicados por el archivo
   * getTotalThemes malloquea la cantidad necesaria
  */
  error = getTotalThemes(themesFile, &buffer, themes);
  if (error)
  {
    free(buffer.text);
    	fclose(themesFile);

    return error;
  }
  

  for (i = 0; i < themes->totalThemes && !error; i++)
  {
  	/* Recorrer cada theme */
  	error = getNumberFormat(themesFile, &buffer, FMT_THEME_START, &themeNo);

  	if(themeNo != i + 1)
  	  error = THEME_NOT_EXISTS;
    
  	if (error)
  	{
  	  free(buffer.text);
    		fclose(themesFile);

  	
  		return error;
  	}
  	
  	

  	error = getThemeName(themesFile, &buffer, &themeName);
    
  	if(error)
  	{
  	  free(buffer.text);
    	
  	  	fclose(themesFile);

  	  /* si hay error, la memoria de el nombre es liberada por getThemeName */
  	  return error;
    }
  	
  	
  	
  	error = getFieldSize(themesFile, &buffer, &rows, &columns);
  	if (error)
  	{
  	    free(buffer.text);
    		fclose(themesFile);
  		return error;
  	}

  	/* Conseguir el total de elementos de la flota */
  	error = getNumberFormat(themesFile, &buffer, FMT_THEME_TOTAL_ELEMENTS, &totalElements);
  	if (totalElements < 1)
  	    error = THEME_NO_ELEMENTS;

  	if (error)
  	{
  	  free(buffer.text);
  		fclose(themesFile);
  	  return error;
  	}

  	((themes->themesList)[i]).name = themeName;
  	((themes->themesList)[i]).rows = rows;
  	((themes->themesList)[i]).columns = columns;

		
	/* ahora inicializamos la cantidad de elementos */
	((themes->themesList)[i]).elements = calloc(totalElements, sizeof(tElement));
	((themes->themesList)[i]).elemAmount = totalElements;
		
		
		
		if(((themes->themesList)[i]).elements == NULL)
		{
		  free(buffer.text);
  	  fclose(themesFile);
      return NO_MEMORY;
		}
		
		
		
		
		for (j = 0; j < totalElements && !error; j++)
		{  
		  error = getElement(themesFile, &buffer, j +1, &name, &length, &quantity);
		  
		  if (length < 1 || quantity < 1)
		    error = THEME_NEGATIVE_VALUE;
		  if(length > rows && length > columns)
		    error = THEME_ELEMENT_TOO_LARGE;
		  
		  if(!error)
		  {
		    (((themes->themesList)[i].elements)[j]).size = length;
		    (((themes->themesList)[i].elements)[j]).cant = quantity;
		    (((themes->themesList)[i].elements)[j]).name = name;
		  }
		}
		
		if (error)
		{
		  free(buffer.text);  	  
		  fclose(themesFile);

		  return error;
		}
		else
		{
		  /* la tematica todo ok */
    	
		}
	}
	
	
	textOverflow = keepOnReading(themesFile, &buffer);
	if(textOverflow)
	  error = TEXT_OVERFLOW;
	
	/* Todo ok */
	fclose(themesFile);

	free(buffer.text);
	
	
	return error;
}


int
getElement(FILE * themesFile, tBuffer * buffer, int element, char ** name, 
					int * length, int * quantity)
{
   int error = 0;
   int auxLength = -1, auxQuantity = -1, params = 0;
   int bufferSize;

 	/* control char al final del buffer */
    char ctrlC = !CONTROL_CHAR;

    tBoolean found = FALSE;
    char fmt_element[80];
    
    /* Hacemos un string lo suficientemente grande
    *  Documentacion: un barco puede tener de nombre maximo 200 caracteres
    */
    char * string;
    string = calloc(MAX_NAME + 1, sizeof(char));
    if(string == NULL)
	 return NO_MEMORY;
    
    
	sprintf(fmt_element, "Elemento %d: %%200[^-]- Longitud: %%2d - Cantidad: %%3d%%c",
					element);
    
  while (READ_AND_FIND(themesFile, found, buffer, bufferSize))
	{
		if(bufferSize > 1)
		{
			params = sscanf(buffer->text, fmt_element, string, &auxLength, 
											&auxQuantity, &ctrlC);
			found = TRUE;
		}
	}
	

	 if (bufferSize == UNEXPECTED_EOF)
	 {
	   	  free(string);

	   return UNEXPECTED_EOF;
	 }
      if (bufferSize == BUFFER_MEMORY)
	 {
	   	  free(string);

	   return BUFFER_MEMORY;
	 }
	if (!found)
		error = THEME_ELEMENT_NOT_DECLARED;

	if (params != 4)
		error = SYMBOL_NOT_EXPECTED;
	
	if (ctrlC != CONTROL_CHAR)
		error = SYMBOL_NOT_EXPECTED;

	if (error)
	{
	  free(string);
		return error;
	}
	
	/* eliminamos los espacios no necesarios */
	string = realloc(string, strlen(string) + 1);
  
	*name = string;
	*length = auxLength;
	*quantity = auxQuantity;

	/* Esta todo OK */
  return 0;
}

int
getThemeName(FILE * themesFile, tBuffer * buffer, char ** name)
{
  int error = 0;
  int params = 0;
  int bufferSize;

 	/* control char al final del buffer */
  char ctrlC = !CONTROL_CHAR;

  tBoolean found = FALSE;
  char fmt_element[80];
    
  /* Hacemos un string lo suficientemente grande
  *  Documentacion: un barco puede tener de nombre maximo 200 caracteres
  */
    char * string;
    string = calloc(MAX_NAME + 1, sizeof(char));
    if (string == NULL)
      return NO_MEMORY;
      
    sprintf(fmt_element, "Nombre:%%[^%c]%%c", CONTROL_CHAR);
    
    while (READ_AND_FIND(themesFile, found, buffer, bufferSize))
	  {
  		if(bufferSize > 1)
  		{
  			params = sscanf(buffer->text, fmt_element, string, &ctrlC);
  			found = TRUE;
  		}
	}
	

    if (bufferSize == UNEXPECTED_EOF)
	 return UNEXPECTED_EOF;
    if (bufferSize == BUFFER_MEMORY)
	 return BUFFER_MEMORY;;
    
    
	if (!found)
		error = THEME_ELEMENT_NOT_DECLARED;

	if (params != 2)
		error = SYMBOL_NOT_EXPECTED;
	
	if (ctrlC != CONTROL_CHAR)
		error = SYMBOL_NOT_EXPECTED;

	if (error)
	{
		return error;
	}
	
  /* eliminamos los espacios no necesarios */
  string = realloc(string, strlen(string) + 1);
	
  *name = string;
	/* Esta todo OK */
  return 0;
}


#include "prototypes.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>



int
getNumberFormat(FILE * file, tBuffer * buffer, char * format, int * number)
{
  
  /* Devuelve 0, si es el formato para conseguir el numero es correcto
   *
   *
   * En caso de error devuelve diferente de cero, el numero de error 
  */
  
   int error = 0;
   int auxNumber = -1, params = 0;
   int bufferSize;


 	/* control char al final del buffer */
 	char ctrlC = !CONTROL_CHAR;

 	tBoolean found = FALSE;

	while (READ_AND_FIND(file, found, buffer, bufferSize))
	{
		if(bufferSize > 1)
		{
			params = sscanf(buffer->text, format, &auxNumber, &ctrlC);
			found = TRUE;
		}
	}
	
    if (bufferSize == UNEXPECTED_EOF)
	 return UNEXPECTED_EOF;
    if (bufferSize == BUFFER_MEMORY)
	 return BUFFER_MEMORY;
    
	if (!found)
		error = WRONG_DECLARED;

	if (params != 2)
		error = SYMBOL_NOT_EXPECTED;

	if (ctrlC != CONTROL_CHAR)
		error = SYMBOL_NOT_EXPECTED;

	if (error)
		return error;
  /* Esta todo OK */
  *number = auxNumber;

  return 0;
}


tBoolean
keepOnReading(FILE * file, tBuffer * buffer)
{
  /* devuelve TRUE si encuentra algo que no sea comentario */
  tBoolean found = FALSE;
  int bufferSize;
  while (READ_AND_FIND(file, found, buffer, bufferSize))
  {
    if(bufferSize > 1)
    {
	 found = TRUE;
    }
  }
  return found;
}




int
readControlLine(tBuffer * buffer, FILE * file)
{
	/* Lee una linea eliminando comentarios y la guarda en destine 
	 * Al final de cada linea pone CONTROL_CHAR y 0
	 * Si aparece un CONTROL_CHAR en la linea devuelve UNEXPECTED_EOF
	 *
	 * Devuelve EOF si ya proceso TODAS las lineas
	 *
	 * Devuelve BUFFER_MEMORY si no hay suficiente memoria
	 */
	
	/* 
	
	* buffer dinámico, en destine 
	
	 *Para la documentacion: 
	 * Supone que espacio al final está mal
	 * No guarda espacios al principio de una linea
	 * No puede haber CONTROL_CHAR en el nombre de un barco
	 * ej:"Tematica 1 ;comentario" es invalido
	*/
	
	int c;
	int bufferIndex = 0;
	tBoolean inComment = FALSE;
  char * auxText = NULL;
	
	while ((c = fgetc(file)) != EOF && c != '\n' && c != CONTROL_CHAR)
	{
	   /* Asegurarse lugar tambien para CONTROL_CHAR y 0 */
	  if (bufferIndex + 2 >= buffer->size)
	  {
		  auxText = realloc(buffer->text, buffer->size + BLOCK);
		  buffer->size += BLOCK;
	    
	    if(auxText == NULL)
  		  return BUFFER_MEMORY;
  		  
      buffer->text = auxText;
	  
	  }	  
	  	  
	  if (c == COMMENT_START)
		  inComment = TRUE;
		
		if(!inComment && (c!=' ' || bufferIndex > 0))
		{	
			/* Guardar el char en el buffer si no es comentario*/
			/* No guarda espacios al principio */
			(buffer->text)[bufferIndex] = c;
			bufferIndex++;
		}
	}
	
	if (c == CONTROL_CHAR)
	  return UNEXPECTED_EOF;
	
	if (c == EOF && bufferIndex == 0)
		return EOF;
    
  (buffer->text)[bufferIndex] = CONTROL_CHAR; /* caracter de control */
  (buffer->text)[++bufferIndex] = 0; /* final de string */
  return bufferIndex;
}

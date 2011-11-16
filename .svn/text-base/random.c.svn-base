#include <stdlib.h>
#include <time.h>

float
rand_normalize(){
	/* Generates between [0, 1) */
	return rand()/(RAND_MAX + 1.0);
}

int
rand_int(int n1, int n2)
{
	return (n2 - n1 + 1) * rand_normalize() + n1;
}

void
rand_initialize()
{
	srand(time(NULL));
	/* llamar una vez rand_int porque el primer llamado nunca lo hace random */
	rand_normalize();
}
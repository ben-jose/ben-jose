
#include <stdio.h>

#include "ben_jose.h"

#define MARK_USED(X)  ((void)(&(X)))

int main(void)
{
	bj_satisf_val_t vv2 = k_yes_satisf;
	bj_error_t verr1 = bje_file_cannot_open;
	
	MARK_USED(vv2);
	MARK_USED(verr1);
    printf("Hola Jose L Quiroga 0\n");
}


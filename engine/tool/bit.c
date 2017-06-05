/*
 * bit.c
 *
 *  Created on: 2015年10月14日
 *      Author: Random
 */


#include <stdlib.h>
#include "bit.h"


struct bit_array*
create_bit_array(unsigned int size)
{
	struct bit_array* array;
	array = (struct bit_array*)malloc(sizeof(struct bit_array));
	if(!array)
	{
		return 0;
	}
	array->uarray = (BIT_TYPE*)malloc(sizeof(BIT_TYPE) * (size + sizeof(BIT_TYPE) - 1)/sizeof(BIT_TYPE));
	if(!array->uarray)
	{
		free(array);
		return 0;
	}
	array->array_size = size;
	return array;
}

void
release_bit_array(struct bit_array* array)
{
	if(!array) return;
	free(array->uarray);
	array->uarray = 0;
	free(array);
}

char
get_bit(struct bit_array* array, unsigned int uIndex)
{
	unsigned int u1, u2;
	BIT_TYPE bt = 1;
	if(uIndex >= array->array_size) return 0;
	u1 = uIndex / sizeof(BIT_TYPE);
	u2 = uIndex % sizeof(BIT_TYPE);
	return ((array->uarray[u1] & (bt << u2)) != 0) ? 1 : 0;
}

char
set_bit(struct bit_array* array, unsigned int uIndex, char b)
{
	unsigned int u1, u2;
	BIT_TYPE bt = 1;
	u1 = uIndex / sizeof(BIT_TYPE);
	u2 = uIndex % sizeof(BIT_TYPE);
	if(uIndex >= array->array_size) return 0;
	if(b)
	{
		array->uarray[u1] |= (bt << u2);
	}
	else
	{
		array->uarray[u1] &= ~(bt << u2);
	}
	return 1;
}


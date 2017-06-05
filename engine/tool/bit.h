/*
 * bit.h
 *
 *  Created on: 2015年10月14日
 *      Author: Random
 */

#ifndef BIT_H_
#define BIT_H_

typedef unsigned int BIT_TYPE;

struct bit_array
{
	BIT_TYPE			*uarray;
	unsigned int		array_size;
};

struct bit_array*		create_bit_array(unsigned int size);
void					release_bit_array(struct bit_array* array);
char					get_bit(struct bit_array* array, unsigned int uIndex);
char					set_bit(struct bit_array* array, unsigned int uIndex, char b);


#endif /* BIT_H_ */

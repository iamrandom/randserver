/*
 * heap.h
 *
 *  Created on: 2015年10月14日
 *      Author: Random
 */

#ifndef HEAP_H_
#define HEAP_H_

#ifdef __cplusplus
extern "C" {
#endif
//union heap_value{
//		void*				point;
//		int64_t			i64;
//		uint64_t	ui64;
//		char				buff[8];
//	};

#define key_type int64_t

typedef struct
{
	key_type				key;
//	union heap_value		value;
}heap_node;

struct heap
{
	unsigned int			size;
	unsigned int			max_size;
	heap_node*				nodes;
	int						gc_tick;
	char					big_head;
};

void						heap_init(struct heap* hp, char big_head);
void						heap_release(struct heap*);
void						heap_push(struct heap* hp, key_type key);
char						heap_pop(struct heap* hp, key_type* pkey, int index);
char						heap_get(struct heap* hp, key_type* pkey, int index);

#ifdef __cplusplus
}
#endif

#endif /* HEAP_H_ */

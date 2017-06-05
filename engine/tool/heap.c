/*
 * heap.c
 *
 *  Created on: 2015年10月14日
 *      Author: Random
 */
#include <stdint.h>
#include <stdlib.h>
#include "heap.h"

#ifdef __cplusplus
extern "C" {
#endif

void
heap_init(struct heap* hp, char big_head)
{
	hp->big_head = big_head;
	hp->size = 0;
	hp->max_size = 0;
	hp->nodes = 0;
	hp->gc_tick = 0;
}

void
heap_release(struct heap* hp)
{
	if(!hp) return;
	free(hp->nodes);
	free(hp);
}

void
check_gc(struct heap* hp)
{
	unsigned int new_size;

	if(hp->size * 2 >= hp->max_size)
	{
		hp->gc_tick = 0;
		return;
	}
	++hp->gc_tick;
	if(hp->gc_tick >= 8)
	{
		new_size = (hp->max_size + 1) / 2;
		hp->nodes = (heap_node*)realloc(hp->nodes, new_size * sizeof(heap_node));
		if(!hp->nodes)
		{
			hp->max_size = 0;
			hp->size = 0;
		}
		else
		{
			hp->max_size = new_size;
		}
		hp->gc_tick = 0;
	}
}

void
heap_push(struct heap* hp, key_type key)
{
	unsigned int new_size;
	unsigned int index, parent_index;
	heap_node* nodes;
	if(!hp) return;
	if(hp->max_size == hp->size)
	{
		new_size = hp->max_size;
		if(new_size < 8)
		{
			new_size = 8;
		}
		else
		{
			new_size *= 2;
		}
		hp->nodes = (heap_node*)realloc(hp->nodes, new_size * sizeof(heap_node));
		if(!hp->nodes)
		{
			hp->max_size = 0;
			hp->size = 0;
			return;
		}
		hp->max_size = new_size;
	}
	nodes = hp->nodes;
	index = hp->size;
	while(index > 0)
	{
		parent_index = (index - 1) / 2;
		if(hp->big_head && key <= nodes[parent_index].key)
		{
			break;
		}
		if(!hp->big_head && key >= nodes[parent_index].key)
		{
			break;
		}
		nodes[index] = nodes[parent_index];
		index = parent_index;
	}
	nodes[index].key = key;
//	nodes[index].value.ui64 = value.ui64;
	++hp->size;
	check_gc(hp);
}

char
heap_pop(struct heap* hp, key_type* pkey, int index)
{
	heap_node* nodes;
	key_type key;
	unsigned int parent_index;
	unsigned int child_index;

	if(!hp || index < 0 || hp->size <= (unsigned int)index) return 0;
	--hp->size;
	nodes = hp->nodes;
	if(pkey)
	{
		*pkey = nodes[index].key;
	}
	if((unsigned int)index != hp->size)
	{
		key = nodes[hp->size].key;
		parent_index = index;
		while(parent_index < hp->size)
		{
			child_index = parent_index * 2 + 1;
			if(child_index >= hp->size) break;
			if((child_index + 1) <= (hp->size-1) )
			{
				if ( (hp->big_head && (nodes[child_index].key < nodes[child_index + 1].key))
							|| (!hp->big_head && (nodes[child_index + 1].key < nodes[child_index].key)) )
				{
					 child_index = child_index + 1;
				}
			}
			if( (hp->big_head && (key < nodes[child_index].key))
					|| (!hp->big_head && (nodes[child_index].key < key)) )
			{
				nodes[parent_index] = nodes[child_index];
				parent_index = child_index;
			}
			else
			{
				break;
			}
		}
		nodes[parent_index] = nodes[hp->size];
	}
	check_gc(hp);
	return 1;
}

char
heap_get(struct heap* hp, key_type* pkey, int index)
{
	if(!hp || index < 0 || hp->size <= (unsigned int)index) return 0;
	*pkey = hp->nodes[index].key;
	return 1;
}


#ifdef __cplusplus
}
#endif



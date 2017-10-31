#include <string.h>
#include "cpack.h"



void 
pack_init(struct cpack* cp, unsigned int max_size)
{
	cp->max_size = max_size;
	cp->size = 0;
}

int
pack_struct(struct cpack* cp, const void* pvalue, size_t size)
{
	if(cp->size < 0)
	{
		return - 1;
	}
	if(((size_t)cp->size + size) > cp->max_size)
	{
		// maybe size = 0
		cp->size *= -1;
		return -1;
	}
	memcpy(cp->data + cp->size, pvalue, size);
	cp->size += (unsigned int)size;
	return 0;
}

int 
pack_char(struct cpack* cp, char value)
{
	return pack_struct(cp, &value, sizeof(value));
}

int
pack_uchar(struct cpack* cp, unsigned char value)
{
	return pack_struct(cp, &value, sizeof(value));
}

int 
pack_short(struct cpack* cp, short value)
{
	return pack_struct(cp, &value, sizeof(value));
}

int 
pack_ushort(struct cpack* cp, unsigned short value)
{
	return pack_struct(cp, &value, sizeof(value));
}

int	
pack_int(struct cpack* cp, int value)
{
	return pack_struct(cp, &value, sizeof(value));
}

int 
pack_uint(struct cpack* cp, unsigned int value)
{
	return pack_struct(cp, &value, sizeof(value));
}

int	
pack_ll(struct cpack* cp, long long value)
{
	return pack_struct(cp, &value, sizeof(value));
}

int 
pack_ull(struct cpack* cp, unsigned long long value)
{
	return pack_struct(cp, &value, sizeof(value));
}

int	
pack_float(struct cpack* cp, float value)
{
	return pack_struct(cp, &value, sizeof(value));
}

int 
pack_double(struct cpack* cp, double value)
{
	return pack_struct(cp, &value, sizeof(value));
}

int 
pack_string(struct cpack* cp, unsigned short length, const char* data)
{
	int ret = 0;
	ret = pack_struct(cp, &length, sizeof(length));
	if(ret == 0)
	{
		ret = pack_struct(cp, data, length);
	}
	return ret;
}



void 
unpack_init(struct cunpack* cunp,  unsigned int length, const char* msg)
{
	cunp->size = length;
	cunp->unpack_size = 0;
	cunp->data = msg;
}

int 
unpack_struct(struct cunpack* cunp, void* data, size_t length)
{
	if((size_t)(cunp->size - cunp->unpack_size) < length) return -1;
	memcpy(data, cunp->data + cunp->unpack_size, length);
	cunp->unpack_size += (unsigned short)length;
	return 0;
}

int 
unpack_char(struct cunpack* cunp, char* value)
{
	return unpack_struct(cunp, value, sizeof(*value));
}
int 
unpack_uchar(struct cunpack* cunp, unsigned char* value)
{
	return unpack_struct(cunp, value, sizeof(*value));
}

int 
unpack_short(struct cunpack* cunp, short* value)
{
	return unpack_struct(cunp, value, sizeof(*value));
}

int 
unpack_ushort(struct cunpack* cunp, unsigned short* value)
{
	return unpack_struct(cunp, value, sizeof(*value));
}

int	
unpack_int(struct cunpack* cunp, int* value)
{
	return unpack_struct(cunp, value, sizeof(*value));
}

int 
unpack_uint(struct cunpack* cunp, unsigned int* value)
{
	return unpack_struct(cunp, value, sizeof(*value));
}

int	
unpack_ll(struct cunpack* cunp, long long* value)
{
	return unpack_struct(cunp, value, sizeof(*value));
}

int 
unpack_ull(struct cunpack* cunp, unsigned long long* value)
{
	return unpack_struct(cunp, value, sizeof(*value));
}

int	
unpack_float(struct cunpack* cunp, float* value)
{
	return unpack_struct(cunp, value, sizeof(*value));
}

int 
unpack_double(struct cunpack* cunp, double* value)
{
	return unpack_struct(cunp, value, sizeof(*value));
}

int 
unpack_string(struct cunpack* cunp, unsigned short* length, const char** data)
{
	int ret;
	ret = unpack_struct(cunp, length, sizeof(*length));
	if(ret == 0)
	{
		if((size_t)(cunp->size - cunp->unpack_size) < (size_t)(*length)) return -1;
		*data = cunp->data + cunp->unpack_size;
		cunp->unpack_size += *length;
		return 0;
	}
	return ret;
}


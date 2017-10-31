#ifndef CPACK_H
#define CPACK_H


struct cpack
{
	unsigned int   		max_size;
	unsigned int   		size;
	char  				data[];
};

void 	pack_init(struct cpack* cp, unsigned int max_size);
int 	pack_char(struct cpack* cp, char value);
int 	pack_uchar(struct cpack* cp, unsigned char value);
int 	pack_short(struct cpack* cp, short value);
int 	pack_ushort(struct cpack* cp, unsigned short value);
int		pack_int(struct cpack* cp, int value);
int 	pack_uint(struct cpack* cp, unsigned int value);
int		pack_ll(struct cpack* cp, long long value);
int 	pack_ull(struct cpack* cp, unsigned long long value);
int		pack_float(struct cpack* cp, float value);
int 	pack_double(struct cpack* cp, double value);
int 	pack_string(struct cpack* cp, unsigned short length, const char* data);
int 	pack_struct(struct cpack* cp, const void* pvalue, size_t size);

struct cunpack
{
	unsigned int 		 unpack_size;
	unsigned int 		 size;
	const char*		 	 data;
};

void 	unpack_init(struct cunpack* cunp,  unsigned int length, const char* msg);
int 	unpack_char(struct cunpack* cunp, char* value);
int 	unpack_uchar(struct cunpack* cunp, unsigned char* value);
int 	unpack_short(struct cunpack* cunp, short* value);
int 	unpack_ushort(struct cunpack* cunp, unsigned short* value);
int		unpack_int(struct cunpack* cunp, int* value);
int 	unpack_uint(struct cunpack* cunp, unsigned int* value);
int		unpack_ll(struct cunpack* cunp, long long* value);
int 	unpack_ull(struct cunpack* cunp, unsigned long long* value);
int		unpack_float(struct cunpack* cunp, float* value);
int 	unpack_double(struct cunpack* cunp, double* value);
int 	unpack_string(struct cunpack* cunp, unsigned short* length, const char** data);
int 	unpack_struct(struct cunpack* cunp, void* data, size_t length);

#endif
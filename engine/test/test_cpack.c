
#include <stdio.h>
#include <stdlib.h>

#include "../tool/cpack.h"


int main(int argc, char** argv)
{
	struct cpack *cp;
	struct cunpack cunp;

	char a;
	unsigned char b;
	short c;
	unsigned short d;
	int e;
	unsigned int f;
	long long g;
	unsigned long long h;
	float j;
	double k;
	const char* psss = 0;;
	unsigned short length = 0;
	char sss[] = "fwsafefxvzcxvsgewgewfwewfwefwe";

	printf(" %d \n", sizeof(struct cpack));
	cp = (struct cpack *)malloc(65536);
	pack_init(cp, 65536 - sizeof(struct cpack));
	pack_char(cp, -1);
	pack_uchar(cp, 2);
	pack_short(cp, -25533);
	pack_ushort(cp, 65533);
	pack_int(cp, -32132131);
	pack_uint(cp, 321321321);
	pack_ll(cp, -32132132144213211);
	pack_ull(cp, 392139213213213213);
	pack_float(cp, -3213.42131f);
	pack_double(cp, -3213.4213213);
	pack_string(cp, (unsigned short)sizeof(sss), sss);
	printf(" %d \n", cp->size);
	unpack_init(&cunp, cp->size, cp->data);
	unpack_char(&cunp, &a);
	unpack_uchar(&cunp, &b);
	unpack_short(&cunp, &c);
	unpack_ushort(&cunp, &d);
	unpack_int(&cunp, &e);
	unpack_uint(&cunp, &f);
	unpack_ll(&cunp, &g);
	unpack_ull(&cunp, &h);
	unpack_float(&cunp, &j);
	unpack_double(&cunp, &k);
	unpack_string(&cunp, &length, &psss);
	printf("%d, %d %d %lld\n", (int)a ,(int)c, e, g);
	printf("%u, %u %u %llu\n", (unsigned int)b, (unsigned int)d, f, h);
	printf("%f, %f\n", j, k);
	printf("%u, %s\n", length, psss);
	free(cp);
	return 0;
}
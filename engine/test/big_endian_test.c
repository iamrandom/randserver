

#include <stdio.h>
#include <stdlib.h>


int main(int argc, char** argv)
{
    int a = 1000000000;
    printf("%x  %d\n", a, a);
    printf("%x  %d\n", -a, -a);
    return 0;
}
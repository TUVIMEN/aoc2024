#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <err.h>
#include <assert.h>

typedef unsigned long long int ull;

ull
newnumber(ull num) {
    #define PRUNE(x) ((x)%16777216)

    num ^= num*64;
    num = PRUNE(num);

    num ^= num/32;
    num = PRUNE(num);

    num ^= num*2048;
    num = PRUNE(num);

    return num;
}

ull
getlast(ull num)
{
    for (size_t i = 0; i < 2000; i++) {
        num = newnumber(num);
    }
    return num;
}

int
main(int argc, char *argv[])
{
    if (argc < 2) {
        fprintf(stderr,"%s <FILE>\n",argv[0]);
        return -1;
    }

    FILE *f = fopen(argv[1],"rb");
    if (!f)
        err(-1,"%s",argv[1]);

    char *line = NULL;
    size_t linel = 0;

    size_t sum = 0;

    while (getline(&line,&linel,f) != -1) {
        ull num = atoll(line);

        num = getlast(num);
        sum += num;
    }

    if (line)
        free(line);
    fclose(f);

    printf("%llu\n",sum);

    return 0;
}

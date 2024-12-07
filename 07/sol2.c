#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <err.h>
#include <assert.h>

typedef unsigned long long int ull;
typedef unsigned char uchar;

ull
numconcat(ull x, ull y)
{
    ull t = y;
    while (t) {
        t /= 10;
        x *= 10;
    }
    return x+y;
}

uchar
ispossible(ull res, ull *nums, size_t numsl, ull prev, size_t lvl)
{
    if (lvl >= numsl-1)
        return (res == prev) ? 1 : 0;


    if (ispossible(res,nums,numsl,((prev == 0) ? 1 : prev)*nums[lvl],lvl+1))
        return 1;

    if (ispossible(res,nums,numsl,prev+nums[lvl],lvl+1))
        return 1;

    if (ispossible(res,nums,numsl,numconcat(prev,nums[lvl]),lvl+1))
        return 1;

    return 0;
}

int
main(int argc, char *argv[])
{
    if (argc < 2) {
        fprintf(stderr,"%s <FILE>\n",argv[0]);
        return -1;
    }

    FILE *f = fopen(argv[1],"r");
    if (!f)
        err(-1,"%s",argv[1]);

    char *line = NULL;
    size_t linel = 0;

    const size_t maxnums = 512;
    ull nums[maxnums];
    size_t numsl;
    ull sum=0;

    while (getline(&line,&linel,f) != -1) {
        char *ptr = line;
        char *endptr;
        ull res = strtoull(ptr,&endptr,10);
        ptr = endptr;
        numsl = 0;

        while (*endptr) {
            ptr++;
            assert(numsl != maxnums);
            nums[numsl++] = strtoull(ptr,&endptr,10);
            ptr = endptr;
        }

        if (ispossible(res,nums,numsl,0,0))
            sum += res;
    }

    printf("%llu\n",sum);

    if (line)
        free(line);
    fclose(f);

    return 0;
}

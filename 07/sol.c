#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <err.h>
#include <assert.h>

typedef unsigned long long int ull;
typedef unsigned char uchar;

uchar
ispossible(ull res, ull *nums, size_t numsl, ull prev, size_t lvl)
{
    if (lvl >= numsl-1)
        return (res == prev) ? 1 : 0;

    size_t t = prev;
    if (t == 0)
        t = 1;
    t *= nums[lvl];

    uchar ret = ispossible(res,nums,numsl,t,lvl+1);
    if (ret)
        return ret;

    t = prev+nums[lvl];
    return ispossible(res,nums,numsl,t,lvl+1);
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

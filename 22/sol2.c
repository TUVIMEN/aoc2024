#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <err.h>
#include <assert.h>

typedef unsigned long long int ull;
typedef unsigned short ushort;
typedef unsigned char uchar;

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

#define MAXSEQUENCES (32*32*32*32)

void
fillsequence(ull num, ushort *sequences, uchar *nsequences)
{
    unsigned int seq = 0;
    unsigned int changes[2000] = {0};
    for (size_t i = 0; i < 2000; i++) {
        int prev = num%10;
        num = newnumber(num);
        int digit = num%10;
        int diff = digit-prev;
        uchar n = diff+10;
        seq = ((seq<<5)&(MAXSEQUENCES-1))|n;
        if (i > 2) {
            if (!nsequences[seq]) {
                sequences[seq] += digit;
                nsequences[seq] = 1;
                changes[i] = seq;
            }
        }
    }

    for (size_t i = 3; i < 2000; i++) {
        if (!changes[i])
            continue;

        nsequences[changes[i]] = 0;
    }
}

ushort
maxbananas(ushort *sequences)
{
    ushort max = 0;
    for (size_t i = 0; i < MAXSEQUENCES; i++)
        if (sequences[i] > max)
            max = sequences[i];
    return max;
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

    ushort *sequences = calloc(MAXSEQUENCES,sizeof(ushort));
    uchar *nsequences = calloc(MAXSEQUENCES,sizeof(uchar));

    while (getline(&line,&linel,f) != -1) {
        ull num = atoll(line);

        fillsequence(num,sequences,nsequences);
    }

    if (line)
        free(line);
    fclose(f);

    printf("%u\n",maxbananas(sequences));

    free(sequences);
    free(nsequences);

    return 0;
}

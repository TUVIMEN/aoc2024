#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <err.h>
#include <assert.h>

typedef unsigned char uchar;

typedef struct {
    char *b;
    size_t s;
} str;

size_t
ispossible(str *towels, size_t towelsl, str pat)
{
    size_t *vals = calloc(pat.s,sizeof(size_t));

    for (size_t i = 0; i < pat.s; i++) {
        char *b = pat.b+pat.s-i-1;
        size_t s = i+1;
        size_t *val = vals+i;

        for (size_t j = 0; j < towelsl; j++) {
            size_t ts = towels[j].s;
            if (s < ts || memcmp(towels[j].b,b,ts) != 0)
                continue;

            if (s == ts) {
                (*val)++;
            } else
                *val += vals[s-ts-1];
        }
    }

    size_t ret = vals[pat.s-1];
    free(vals);
    return ret;
}

int
str_comp(const str *s1, const str *s2)
{
    size_t s = s1->s;
    if (s2->s < s)
        s = s2->s;
    return memcmp(s1->b,s2->b,s);
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

    const size_t maxtowels = 512;
    str towels[maxtowels];
    size_t towelsl = 0;

    ssize_t s = getline(&line,&linel,f);
    char *token,*ptr=line,*saveptr;
    if (s && line[s-1] == '\n')
        line[s-1] = '\0';

    for (; (token = strtok_r(ptr," ,",&saveptr)); ptr=NULL) {
        assert(towelsl < maxtowels);
        size_t len = strlen(token);
        towels[towelsl++] = (str){
            memcpy(malloc(len),token,len),
            len
        };
    }

    getline(&line,&linel,f);

    size_t possible = 0;

    while ((s = getline(&line,&linel,f)) != -1) {
        if (s && line[s-1] == '\n')
            s--;
        str pat = {
            line,
            s
        };

        possible += ispossible(towels,towelsl,pat);
    }

    printf("%lu\n",possible);

    for (size_t i = 0; i < towelsl; i++)
        free(towels[i].b);

    if (line)
        free(line);
    fclose(f);


    return 0;
}

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <err.h>
#include <assert.h>

typedef unsigned long long int ull;
typedef unsigned char uchar;

ull regA = 0,
    regB = 0,
    regC = 0;

size_t instrptr = 0;

#define MAXOUTPUT 256
const size_t maxoutput = MAXOUTPUT;
uchar output[MAXOUTPUT];
size_t outputl = 0;

enum instrName {
    adv,
    bxl,
    bst,
    jnz,
    bxc,
    out,
    bdv,
    cdv
};

int
instr_adv(ull *valp)
{
    ull v = *valp;
    regA >>= v;

    return 1;
}

int
instr_bxl(ull *valp)
{
    ull v = *valp;
    regB ^= v;

    return 1;
}

int
instr_bst(ull *valp)
{
    ull v = *valp;
    regB = v%8;

    return 1;
}

int
instr_jnz(ull *valp)
{
    ull v = *valp;
    if (regA != 0) {
        instrptr = v;
        return 0;
    }

    return 1;
}

int
instr_bxc(ull *valp)
{
    ull v = *valp;
    regB ^= regC;

    return 1;
}

int
instr_out(ull *valp)
{
    ull v = *valp;
    assert(outputl < maxoutput);
    output[outputl++] = v%8;

    return 1;
}

int
instr_bdv(ull *valp)
{
    ull v = *valp;
    regB  = regA >> v;

    return 1;
}

int
instr_cdv(ull *valp)
{
    ull v = *valp;
    regC = regA >> v;

    return 1;
}


struct {
    enum instrName code;
    int (*func)(ull*);
    uchar combo;
} instructions[] = {
    {adv,instr_adv,1},
    {bxl,instr_bxl,0},
    {bst,instr_bst,1},
    {jnz,instr_jnz,0},
    {bxc,instr_bxc,0},
    {out,instr_out,1},
    {bdv,instr_bdv,1},
    {cdv,instr_cdv,1}
};

void
combo(ull **valp)
{
    ull v = **valp;
    assert(v < 7);
    if (v == 4)
        *valp = &regA;
    if (v == 5)
        *valp = &regB;
    if (v == 6)
        *valp = &regC;
}

void
execinstr(char *src)
{
    char code = src[0];
    assert(code >= '0' && code-'0' < 9);
    code -= '0';

    char c = src[1];
    assert(c >= '0' && c <= '9');
    ull val = c-'0';

    ull *valp = &val;
    if (instructions[code].combo)
        combo(&valp);

    if (instructions[code].func(valp))
        instrptr += 2;
}

void
readreg(FILE *f, char **line, size_t *linel, ull *reg)
{
    assert(getline(line,linel,f) != -1);
    char *d = strchr(*line,':');
    assert(d != NULL);
    *reg = atoll(d+1);
}

char *
readinput(char *input, size_t *inputl)
{
    size_t s = 0;
    for (size_t i = 0; input[i] != '\0' && input[i] != '\n'; i++) {
        if (input[i] == ',')
            continue;
        input[s++] = input[i];
    }

    input[s] = 0;
    *inputl = s-1;
    return input;
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

    readreg(f,&line,&linel,&regA);
    readreg(f,&line,&linel,&regB);
    readreg(f,&line,&linel,&regC);

    getline(&line,&linel,f);

    assert(getline(&line,&linel,f) != -1);
    char *delim = strchr(line,':');
    assert(delim != NULL);

    size_t inputl;
    char *input = readinput(delim+2,&inputl);

    fclose(f);

    while (instrptr < inputl) {
        execinstr(input+instrptr);
    }


    if (outputl) {
        if (outputl > 1)
            for (size_t i = 0; i < outputl-1; i++)
                printf("%d,",output[i]);
        printf("%d\n",output[outputl-1]);
    }

    if (line)
        free(line);

    return 0;
}

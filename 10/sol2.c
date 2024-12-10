#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <err.h>
#include <assert.h>

size_t
findtrail(char **lines, size_t linesl, size_t x, size_t y, char prev)
{
    if (x >= linesl)
        return 0;
    if (prev == (char)-1)
        prev = '0';

    if (lines[x][y] != prev)
        return 0;

    if (prev == '9') {
        return 1;
    }

    size_t ret = 0;

    if (x > 0)
        ret += findtrail(lines,linesl,x-1,y,prev+1);
    if (y > 0)
        ret += findtrail(lines,linesl,x,y-1,prev+1);
    ret += findtrail(lines,linesl,x+1,y,prev+1);
    if (lines[x][y])
        ret += findtrail(lines,linesl,x,y+1,prev+1);
        
    return ret;
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

    const size_t maxlines = 500;
    char *lines[maxlines];
    size_t linesl=0,linel=0;
    char *line = NULL;

    while((getline(&line,&linel,f)) != -1) {
        lines[linesl++] = line;
        assert(linesl < maxlines);
        line = NULL;
        linel = 0;
    }

    fclose(f);

    size_t trails = 0;
    for (size_t i=0; i < linesl; i++) {
        for (size_t j=0; lines[i][j]; j++) {
            if (lines[i][j] == '0') {
                size_t t = findtrail(lines,linesl,i,j,-1);
                trails += t;
            }
        }

    }

    printf("%lu\n",trails);

    for (size_t i = 0; i < linesl; i++)
        free(lines[i]);

    return -1;
}

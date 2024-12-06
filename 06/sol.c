#include <stdlib.h>
#include <stdio.h>
#include <err.h>
#include <string.h>

typedef unsigned char uchar;

void
getinput(const char *filename, char **data, size_t *datal)
{
    *datal = 0;
    FILE *f = fopen(filename,"rb");
    if (!f)
        err(-1,"%s",filename);

    char *line = NULL;
    size_t linel = 0;
    ssize_t s;

    while ((s = getline(&line,&linel,f)) != -1) {
        if (s && line[s-1] == '\n')
            line[s-1] = 0;
        data[(*datal)++] = line;

        line = NULL;
        linel = 0;
    }

    fclose(f);

}

uchar
findguard(char **data, const size_t datal, size_t *x, size_t *y)
{
    for (size_t i = 0; i < datal; i++) {
        for (size_t j = 0; data[i][j]; j++) {
            if (data[i][j] == '^') {
                *x = i;
                *y = j;
                return 1;
            }
        }
    }
    return 0;
}


size_t
process(char **data, const size_t datal)
{
    size_t ret=0,x,y;

    if (!findguard(data,datal,&x,&y))
        return ret;

    uchar dir = 0;
     while (1) {
        if ((dir == 2 && x == datal-1) ||
            (dir == 1 && data[x][y+1] == 0) ||
            (dir == 0 && x == 0) ||
            (dir == 3 && y == 0)) {
            ret++;
            break;
        }
        size_t pos[4][2] = {
            {x-1,y},
            {x,y+1},
            {x+1,y},
            {x,y-1},
        };

        size_t *d = pos[dir];
        if (data[d[0]][d[1]] == '#') {
            dir++;
            if (dir == 4)
                dir = 0;
            continue;
        }

        if (data[x][y] != 'X')
            ret++;
        data[x][y] = 'X';
        x = d[0];
        y = d[1];
     }

    return ret;
}

int
main(int argc, char *argv[])
{
    if (argc < 2) {
        fprintf(stderr,"%s <FILE>\n",argv[0]);
        return -1;
    }

    char *data[256];
    size_t datal;

    getinput(argv[1],data,&datal);

    size_t area = process(data,datal);
    printf("%lu\n",area);

    for (size_t i =0; i < datal; i++) {
        free(data[i]);
    }

    return 0;
}

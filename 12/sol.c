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

void
getpatch(char **data, const size_t datal, size_t x, size_t y, size_t *area, size_t *perimeter, char type)
{
    if (x >= datal || !data[x][y]) {
        (*perimeter)++;
        return;
    }
    if (data[x][y] == '.')
        return;
    if (data[x][y] != type) {
        (*perimeter)++;
        return;
    }

    data[x][y] = '.';
    (*area)++;

    if (x == 0) {
        (*perimeter)++;
    } else
        getpatch(data,datal,x-1,y,area,perimeter,type);
    if (y == 0) {
        (*perimeter)++;
    } else
        getpatch(data,datal,x,y-1,area,perimeter,type);
    getpatch(data,datal,x+1,y,area,perimeter,type);
    getpatch(data,datal,x,y+1,area,perimeter,type);
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

    size_t price = 0;

    for (size_t i=0; i < datal; i++) {
        for (size_t j=0; data[i][j]; j++) {
            if (data[i][j] == '#')
                continue;
            size_t area=0,perimeter=0;
            char t = data[i][j];
            getpatch(data,datal,i,j,&area,&perimeter,data[i][j]);
            price += area*perimeter;
            for (size_t g=0; g < datal; g++) {
                for (size_t h = 0; data[g][h]; h++)
                    if (data[g][h] == '.')
                        data[g][h] = '#';
            }
        }
    }

    printf("%lu\n",price);

    for (size_t i =0; i < datal; i++) {
        free(data[i]);
    }

    return 0;
}

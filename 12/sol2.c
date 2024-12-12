#include <stdlib.h>
#include <stdio.h>
#include <err.h>
#include <string.h>
#include <assert.h>

typedef unsigned char uchar;
typedef unsigned short ushort;

typedef struct {
    ushort x;
    ushort y;
} point;

int
xsides_comp(const point *p1, const point *p2)
{
    int x = p1->x-p2->x;
    if (x != 0)
        return x;
    return p1->y-p2->y;
}

int
ysides_comp(const point *p1, const point *p2)
{
    int y = p1->y-p2->y;
    if (y != 0)
        return y;
    return p1->x-p2->x;
}

size_t
getsides(point *borders, size_t bordersl)
{
    size_t ret = 0;

    //border points are first sorted by x axis and non unique values are removed
    //the same repeats for y axis

    qsort(borders,bordersl,sizeof(point),(int(*)(const void*,const void*))xsides_comp);

    for (size_t i = 0; i < bordersl;) {
        size_t prevx=borders[i].x,prevy=borders[i].y;

        borders[ret++] = (point){borders[i].x,borders[i].y};
        i++;

        while (i < bordersl && prevx == borders[i].x) {
            int d = borders[i].y-prevy;
            if (d > 6)
                break;
            prevy = borders[i].y;
            i++;
        }
    }

    bordersl = ret;
    ret = 0;

    qsort(borders,bordersl,sizeof(point),(int(*)(const void*,const void*))ysides_comp);

    for (size_t i = 0; i < bordersl;) {
        size_t prevx=borders[i].x,prevy=borders[i].y;
        borders[ret++] = (point){borders[i].x,borders[i].x};
        i++;

        while (i < bordersl && prevy == borders[i].y) {
            int d = borders[i].x-prevx;
            if (d > 6)
                break;
            prevx = borders[i].x;
            i++;
        }
    }

    return ret;
}

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
addborder(point *borders, size_t *bordersl, ushort x, ushort y)
{
    assert(*bordersl < 2048);
    borders[(*bordersl)++] = (point){(ushort)x,(ushort)y};
}

void
getpatch(char **data, const size_t datal, size_t x, size_t y, size_t *area, point *borders, size_t *bordersl, char type, ushort dir)
{
    //added borders have directions increased by 1 so that 0 may exists and then multiplied by 4 and direction is added
    //this is so values are unique
    if (x >= datal || !data[x][y]) {
        addborder(borders,bordersl,((x+1)*4)+dir,((y+1)*4)+dir);
        return;
    }
    if (data[x][y] == '.')
        return;
    if (data[x][y] != type) {
        addborder(borders,bordersl,((x+1)*4)+dir,((y+1)*4)+dir);
        return;
    }

    data[x][y] = '.';
    (*area)++;

    if (x == 0) {
        addborder(borders,bordersl,x+0,((y+1)*4)+0);
    } else
        getpatch(data,datal,x-1,y,area,borders,bordersl,type,0);
    if (y == 0) {
        addborder(borders,bordersl,((x+1)*4)+dir+1,y+1);
    } else
        getpatch(data,datal,x,y-1,area,borders,bordersl,type,1);
    getpatch(data,datal,x+1,y,area,borders,bordersl,type,2);
    getpatch(data,datal,x,y+1,area,borders,bordersl,type,3);
}

void
clearprevious(char **data, size_t datal)
{
    for (size_t i=0; i < datal; i++)
        for (size_t j = 0; data[i][j]; j++)
            if (data[i][j] == '.')
                data[i][j] = '#';
}

size_t
patchprice(char **data,size_t datal, size_t x, size_t y)
{
    size_t area=0,perimeter=0;

    const size_t maxborders = 2048;
    point borders[maxborders];
    size_t bordersl=0;

    char t = data[x][y];

    getpatch(data,datal,x,y,&area,borders,&bordersl,data[x][y],3);
    clearprevious(data,datal);
    size_t sides = getsides(borders,bordersl);

    return area*sides;
}

size_t
getprice(char **data, size_t datal)
{
    size_t ret = 0;
    for (size_t i=0; i < datal; i++) {
        for (size_t j=0; data[i][j]; j++) {
            if (data[i][j] == '#')
                continue;
            ret += patchprice(data,datal,i,j);
        }
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

    size_t price = getprice(data,datal);


    printf("%lu\n",price);

    for (size_t i =0; i < datal; i++) {
        free(data[i]);
    }

    return 0;
}

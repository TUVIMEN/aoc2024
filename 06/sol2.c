#include <stdlib.h>
#include <stdio.h>
#include <err.h>
#include <string.h>

//given test case was horrible

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

uchar
move_up(char **data, const size_t datal, size_t x, size_t y, size_t *nx, size_t *ny)
{
    if (x == 0)
        return 0;
    *nx = x-1;
    *ny = y;
    return 1;
}

uchar
move_right(char **data, const size_t datal, size_t x, size_t y, size_t *nx, size_t *ny)
{
    if (data[x][y+1] == 0)
        return 0;
    *nx = x;
    *ny = y+1;
    return 1;
}

uchar
move_down(char **data, const size_t datal, size_t x, size_t y, size_t *nx, size_t *ny)
{
    if (x == datal-1)
        return 0;
    *nx = x+1;
    *ny = y;
    return 1;
}

uchar
move_left(char **data, const size_t datal, size_t x, size_t y, size_t *nx, size_t *ny)
{
    if (y == 0)
        return 0;
    *nx = x;
    *ny = y-1;
    return 1;
}

const uchar (*moves[])(char**,const size_t,size_t,size_t,size_t*,size_t*) = {
    move_up,
    move_right,
    move_down,
    move_left
};

uchar
checkbounce(char **data, const size_t datal, size_t x, size_t y, size_t dir, size_t bx, size_t by)
{
    size_t used[1024][3];
    size_t usedl = 0;
    size_t firstdir = dir;
    while (1) {
        size_t nx,ny;
        if (!moves[dir](data,datal,x,y,&nx,&ny))
            break;

        if (data[nx][ny] == '#') {
            for (size_t i = 0; i < usedl; i++)
                if (used[i][0] == nx && used[i][1] == ny && used[i][2] == dir)
                    return 1;
            used[usedl][0] = nx;
            used[usedl][1] = ny;
            used[usedl][2] = dir;
            usedl++;

            dir++;
            if (dir == 4)
                dir = 0;
            continue;
        }

        x = nx;
        y = ny;
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
    size_t firstx=x,firsty=y;
     while (1) {
        size_t nx,ny;
        if (!moves[dir](data,datal,x,y,&nx,&ny))
            break;

        if (data[nx][ny] == '#') {
            dir++;
            if (dir == 4)
                dir = 0;
            continue;
        }

        char ndir = dir+1;
        if (ndir == 4)
            ndir = 0;

        if ((nx != firstx || ny != firsty) && data[nx][ny] != 'X') {
            data[nx][ny] = '#';
            if (checkbounce(data,datal,x,y,ndir,nx,ny))
                ret++;
        }

        data[nx][ny] = 'X';

        x = nx;
        y = ny;
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

    for (size_t i =0; i < datal; i++)
        free(data[i]);

    return 0;
}

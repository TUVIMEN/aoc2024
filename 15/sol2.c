#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <err.h>
#include <assert.h>

typedef unsigned char uchar;

uchar
findrobot(char **map, size_t mapl, size_t *px, size_t *py)
{
    for (size_t i = 0; i < mapl; i++) {
        for (size_t j = 0; map[i][j]; j++) {
            if (map[i][j] == '@') {
                *px = i;
                *py = j;
                return 1;
            }
        }
    }

    return 0;
}

char *
remap(char *line, ssize_t linel)
{
    if (line[linel-1] == '\n')
        linel--;
    size_t retl = linel*2+1;
    char *ret = malloc(retl);
    ret[retl-1] = '\0';

    for (ssize_t i = 0; i < linel; i++) {
        size_t j = i<<1;
        switch (line[i]) {
            case '@':
                ret[j] = '@';
                ret[j+1] = '.';
                break;
            case 'O':
                ret[j] = '[';
                ret[j+1] = ']';
                break;
            default:
                ret[j] = line[i];
                ret[j+1] = line[i];
        }
    }

    return ret;
}

uchar
move_next(char type, size_t px, size_t py, size_t *nx, size_t *ny)
{
    *nx = px;
    *ny = py;
    switch (type) {
        case '^':
            *nx = px-1;
            break;
        case '>':
            *ny = py+1;
            break;
        case 'v':
            *nx = px+1;
            break;
        case '<':
            *ny = py-1;
            break;
        default:
            return 0;
    }
    return 1;
}

uchar
move_possible(char **map, char type, size_t px, size_t py)
{
    size_t nx,ny;
    if (!move_next(type,px,py,&nx,&ny))
        return 0;

    if (map[nx][ny] == '#')
        return 0;

    if (map[nx][ny] == '[' || map[nx][ny] == ']') {
        if (!move_possible(map,type,nx,ny))
            return 0;
        if (type == 'v' || type == '^') {
            size_t ny2 = ny;
            if (map[nx][ny] == '[') {
                ny2++;
            } else
                ny2--;
            if (!move_possible(map,type,nx,ny2))
                return 0;
        }
    }

    return 1;
}

uchar
move_move(char **map, char type, size_t *px, size_t *py, uchar brother)
{
    size_t nx,ny;

    if ((map[*px][*py] == '[' || map[*px][*py] == ']') && (type == 'v' || type == '^') && !brother) {
        nx=*px,ny=*py;
        if (map[nx][ny] == '[') {
            ny++;
        } else {
            ny--;
        }
        size_t tx=nx,ty=ny;
        if (!move_move(map,type,&tx,&ty,1))
            return 0;

        map[nx][ny] = '.';
    }

    if (!move_next(type,*px,*py,&nx,&ny))
        return 0;

    if (map[nx][ny] == '#')
        return 0;

    if (map[nx][ny] == '[' || map[nx][ny] == ']') {
        size_t tx=nx,ty=ny;
        if (!move_move(map,type,&tx,&ty,0))
            return 0;
    }

    char c = map[nx][ny];
    map[nx][ny] = map[*px][*py];
    map[*px][*py] = c;

    *px = nx;
    *py = ny;

    return 1;
}

uchar
move(char **map, size_t mapl, char type, size_t *px, size_t *py)
{
    if (!move_possible(map,type,*px,*py))
        return 0;
    return move_move(map,type,px,py,0);
}

int
main(int argc, char *argv[])
{
    if (argc < 2) {
        fprintf(stderr,"%s <FILe>\n",argv[0]);
        return -1;
    }

    FILE *f = fopen(argv[1],"rb");
    if (!f)
        err(-1,"%s",argv[1]);

    char *line = NULL;
    size_t linel = 0;
    ssize_t s = 0;

    const size_t maxlines = 64;
    char *map[maxlines];
    size_t mapl = 0;

    while ((s = getline(&line,&linel,f)) != -1) {
        if (s == 1 && line[0] == '\n')
            break;

        map[mapl++] = remap(line,s);
    }

    size_t px,py;
    if (!findrobot(map,mapl,&px,&py))
        goto END;

    while ((s = getline(&line,&linel,f)) != -1) {
        for (size_t i = 0; i < s && line[s] != '\n'; i++) {
            move(map,mapl,line[i],&px,&py);
        }
    }

    size_t sum = 0;
    for (size_t i = 0; i < mapl; i++) {
        for (size_t j = 0; map[i][j]; j++) {
            if (map[i][j] == '[') {
                sum += i*100+j;
            }
        }
    }

    printf("%lu\n",sum);

    END: ;
    for (size_t i = 0; i < mapl; i++)
        free(map[i]);

    if (line)
        free(line);
    fclose(f);

    return 0;
}

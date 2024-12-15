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

uchar
move(char **map, size_t mapl, char type, size_t *px, size_t *py)
{
    size_t nx=*px,ny=*py;
    switch (type) {
        case '^':
            nx--;
            break;
        case '>':
            ny++;
            break;
        case 'v':
            nx++;
            break;
        case '<':
            ny--;
            break;
        default:
            return 0;
    }

    if (map[nx][ny] == '#')
        return 0;
    size_t tx=nx,ty=ny;
    if (map[nx][ny] == 'O' && !move(map,mapl,type,&tx,&ty))
        return 0;

    char c = map[nx][ny];
    map[nx][ny] = map[*px][*py];
    map[*px][*py] = c;

    *px = nx;
    *py = ny;

    return 1;
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

        map[mapl++] = line;
        line = NULL;
        linel = 0;
    }

    size_t px,py;
    if (!findrobot(map,mapl,&px,&py))
        goto END;

    while ((s = getline(&line,&linel,f)) != -1)
        for (size_t i = 0; i < s && line[s] != '\n'; i++)
            move(map,mapl,line[i],&px,&py);

    size_t sum = 0;
    for (size_t i = 0; i < mapl; i++) {
        for (size_t j = 0; map[i][j]; j++) {
            if (map[i][j] == 'O') {
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

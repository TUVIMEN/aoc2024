#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <err.h>
#include <assert.h>

typedef unsigned char uchar;
typedef unsigned int uint;

ssize_t
readgame(FILE *f, char **line, size_t *linel, uint out[6])
{
    ssize_t s;
    uchar group = 0;
        char *saveptr,*token;

    for (size_t i = 0; i < 2; i++) {
        s = getline(line,linel,f);
        assert(s != -1);
        for (char *ptr=*line; (token = strtok_r(ptr,"+",&saveptr)); ptr = NULL) {
            if (ptr)
                continue;
            out[group++] = (uint)atoi(token);
        }
    }
    s = getline(line,linel,f);
    assert(s != -1);
    for (char *ptr=*line; (token = strtok_r(ptr,"=",&saveptr)); ptr = NULL) {
        if (ptr)
            continue;
        out[group++] = (uint)atoi(token);
    }
    return getline(line,linel,f);
}

size_t
play(uint game[6])
{
    uint x=0,y=0;
    size_t tokens=0;
    uint steps = 0;
    int dx=0,dy=0;

    while (1) {
        if (steps >= 100) {
            return 0;
        }
        if (game[4] < x) {
            return 0;
        }
        if (game[5] < y) {
            return 0;
        }
        dx = game[4]-x;
        dy = game[5]-y;

        if (dx%game[2] == 0 && dy%game[3] == 0 && (dx/game[2]) == (dy/game[3])) {
            break;
        }

        x += game[0];
        y += game[1];
        tokens += 3;
        steps++;
    }

    uint steps2 = dx/game[2];
    if (steps2 >= 100) {
        return 0;
    }
    tokens += steps2;

    return tokens;
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

    uint game[6];
    char *line = NULL;
    size_t linel = 0;
    size_t tokens = 0;

    while (1) {
        ssize_t s = readgame(f,&line,&linel,game);
        tokens += play(game);
        if (s == -1)
            break;
    }

    printf("%lu\n",tokens);

    if (line)
        free(line);
    fclose(f);

    return 0;
}

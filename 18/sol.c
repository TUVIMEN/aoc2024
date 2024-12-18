#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <err.h>
#include <assert.h>

typedef unsigned char uchar;
typedef unsigned int uint;

#define MAPMAXX 71
#define MAPMAXY 71
/*#define MAPMAXX 7
#define MAPMAXY 7*/
const size_t mapmaxx = MAPMAXX;
const size_t mapmaxy = MAPMAXY;
const size_t maxbytes = 1024;
//const size_t maxbytes = 12;

uchar map[MAPMAXX][MAPMAXY];

typedef struct {
    void *v;
    size_t size;
    size_t asize;
    size_t elsize;
    size_t inc_r;
} farray;

farray *
farray_init(const size_t elsize, const size_t inc_r)
{
    farray f = {
        .elsize = elsize,
        .inc_r = inc_r
    };
    return memcpy(malloc(sizeof(farray)),&f,sizeof(farray));
}

void *
farray_inc(farray *f)
{
    if (f->size >= f->asize) {
        f->v = realloc(f->v,(f->asize+=f->inc_r)*f->elsize);
    }
    return f->v+((f->size++)*f->elsize);
}

void
farray_conv(farray *f, void **data, size_t *datal)
{
    *data = NULL;
    if (f->asize) {
        if (f->size) {
            if (f->size != f->asize)
                f->v = realloc(f->v,f->size*f->elsize);
                *data = f->v;
        } else
            free(f->v);
    }
    *datal = f->size;
    free(f);
}

void
farray_free(farray *f)
{
    free(f->v);
    free(f);
}

typedef struct {
    int x;
    int y;
} point;

uchar
movepath(int x, int y, farray *points)
{
    if (x < 0 || y < 0 || y >= mapmaxy || x >= mapmaxx)
        return 0;
    if (map[x][y] != 0)
        return 0;
    if (x == mapmaxx-1 && y == mapmaxy-1)
        return 1;

    map[x][y] = 1;

    int pos[4][2] = {
        {x+1,y},
        {x-1,y},
        {x,y-1},
        {x,y+1}
    };

    for (size_t i = 0; i < 4; i++)
        *(point*)farray_inc(points) = (point){pos[i][0],pos[i][1]};

    return 0;
}

size_t
findend()
{
    size_t ret = 0;
    uchar found = 0;

    farray *p1 = farray_init(sizeof(point),1024),
           *p2 = farray_init(sizeof(point),1024);

    *(point*)farray_inc(p1) = (point){0,0};

    while (1) {
        point *p1v = p1->v;
        for (size_t i = 0; i < p1->size; i++) {
            if (movepath(p1v[i].x,p1v[i].y,p2)) {
                found = 1;
                goto END;
            }
        }

        if (p2->size == 0)
            break;

        p1->size = 0;
        farray *t = p2;
        p2 = p1;
        p1 = t;
        ret++;
    }
    END: ;

    farray_free(p1);
    farray_free(p2);
    if (!found)
        ret = 0;
    return ret;
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

    memset(map,0,sizeof(uchar)*mapmaxx*mapmaxy);

    char *line = NULL;
    size_t linel = 0;

    for (size_t i = 0; i < maxbytes && getline(&line,&linel,f) != -1; i++) {
        int x,y;
        x = atoi(line);
        char *n = strchr(line,',');
        assert(n != NULL);
        y = atoi(n+1);

        map[x][y] = 1;
    }

    if (line)
        free(line);
    fclose(f);

    assert(map[0][0] == 0);
    assert(map[mapmaxx-1][mapmaxy-1] == 0);

    printf("%lu\n",findend());

    return 0;
}

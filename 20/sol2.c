#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <err.h>
#include <assert.h>

typedef unsigned char uchar;
typedef unsigned int uint;

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
pointinbounds(uint **map, size_t mapl, size_t mapyl, const point *p)
{
    int x = p->x,
        y = p->y;
    if (x < 0 || y < 0 || x >= mapl || y >= mapyl)
        return 0;
    return 1;
}

point
point_up(const point *p)
{
    return (point){p->x+1,p->y};
}

point
point_down(const point *p)
{
    return (point){p->x-1,p->y};
}

point
point_left(const point *p)
{
    return (point){p->x,p->y-1};
}

point
point_right(const point *p)
{
    return (point){p->x,p->y+1};
}

uchar
movepath(uint **map, size_t mapl, size_t mapyl, const point *p, farray *points, uint lvl)
{
    int x = p->x,
        y = p->y;
    if (!pointinbounds(map,mapl,mapyl,p))
        return 0;

    uint c = map[x][y];
    if (c >= 'Z' || c == '#')
        return 0;

    map[x][y] = 'Z'+lvl;

    if (c == 'E')
        return 2;

    point pos[4] = {
        point_up(p),
        point_down(p),
        point_left(p),
        point_right(p)
    };

    for (size_t i = 0; i < 4; i++)
        *(point*)farray_inc(points) = pos[i];

    return 1;
}

point
findstart(uint **map, size_t mapl)
{
    for (size_t i = 0; i < mapl; i++)
        for (size_t j = 0; map[i][j]; j++)
            if (map[i][j] == 'S')
                return (point){i,j};

    assert(0);
}

point
findend(uint **map, size_t mapl, size_t mapyl)
{
    farray *p1 = farray_init(sizeof(point),1024),
           *p2 = farray_init(sizeof(point),1024);

    size_t lvl = 1;
    *(point*)farray_inc(p1) = findstart(map,mapl);
    uchar found = 0;
    point ret;

    while (1) {
        point *p1v = p1->v;
        for (size_t i = 0; i < p1->size; i++) {
            if (movepath(map,mapl,mapyl,&p1v[i],p2,lvl) == 2) {
                found = 1;
                ret = p1v[i];
            }
        }

        if (p2->size == 0)
            break;

        p1->size = 0;
        farray *t = p2;
        p2 = p1;
        p1 = t;
        lvl++;
    }

    farray_free(p1);
    farray_free(p2);

    assert(found);
    return ret;
}

void
checkcheat(uint **map, size_t mapl, size_t mapyl, const point *p, uint prev, size_t *cheats, int radius)
{
    int x = p->x,
        y = p->y;

    for (int i = x-radius; i <= x+radius; i++) {
        int k = abs(i-x);
        for (int j = y-radius+k; j <= y+radius-k; j++) {
            if (i == x && j == y)
                continue;

            point n = {i,j};
            if (!pointinbounds(map,mapl,mapyl,&n))
                continue;

            if (map[i][j] < 'Z')
                continue;

            int distance = abs(x-i)+abs(y-j);

            int d = ((int)map[i][j])-((int)prev)-distance;
            if (d >= 100)
                (*cheats)++;
        }
    }
}

void
backtrack(uint **map, size_t mapl, size_t mapyl, const point *p, uint prev, size_t *cheats)
{
    int x = p->x,
        y = p->y;

    if (!pointinbounds(map,mapl,mapyl,p))
        return;

    uint c = map[x][y];
    if (prev-1 != c)
        return;

    checkcheat(map,mapl,mapyl,p,c,cheats,20);

    if (c == 'Z')
        return;

    point pos[4] = {
        point_up(p),
        point_down(p),
        point_left(p),
        point_right(p)
    };

    for (size_t i = 0; i < 4; i++)
        backtrack(map,mapl,mapyl,&pos[i],c,cheats);
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
    const size_t maxmap = 256;
    uint *map[maxmap];
    size_t mapl = 0,mapyl=0;
    ssize_t s;

    while ((s = getline(&line,&linel,f)) != -1) {
        assert(mapl < maxmap);

        if (s && line[s-1] == '\n')
            s--;

        uint *m = map[mapl++] = malloc((s+1)*sizeof(uint));
        mapyl = s;
        m[s] = '\0';
        for (ssize_t i = 0; i < s; i++)
            m[i] = line[i];
    }

    if (line)
        free(line);
    fclose(f);

    point end = findend(map,mapl,mapyl);
    size_t cheats = 0;
    backtrack(map,mapl,mapyl,&end,map[end.x][end.y]+1,&cheats);

    printf("%lu\n",cheats);

    for (size_t i = 0; i < mapl; i++)
        free(map[i]);

    return 0;
}

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <err.h>

void
search(const char **data, size_t datal, size_t x, size_t y, size_t *found)
{
    if (data[x][y] != 'A')
        return;

    if (x == 0 || x >= datal-1 || y == 0 || data[x][y] == 0)
        return;

    if ((
            (data[x-1][y-1] == 'S' && data[x+1][y+1] == 'M')
            ||
            (data[x-1][y-1] == 'M' && data[x+1][y+1] == 'S')
        ) && (
            (data[x-1][y+1] == 'S' && data[x+1][y-1] == 'M')
            ||
            (data[x-1][y+1] == 'M' && data[x+1][y-1] == 'S')
        ))
        (*found)++;
}

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

void
getinput(const char *filename, char ***data, size_t *datal)
{
    *data = NULL;
    *datal = 0;
    FILE *file = fopen(filename,"r");
    if (!file)
        err(-1,"%s",filename);

    farray *f = farray_init(sizeof(char*),1000);

    char *line = NULL;
    size_t linel = 0;

    while (getline(&line,&linel,file) > 0) {
        *(char**)farray_inc(f) = line;
        line = NULL;
        linel = 0;
    }

    farray_conv(f,(void**)data,datal);

    fclose(file);
}

int
main(int argc, char *argv[])
{
    if (argc < 2) {
        fprintf(stderr,"%s <FILE>\n",argv[0]);
        return -1;
    }

    char **data;
    size_t datal;

    getinput(argv[1],&data,&datal);

    if (!data)
        return -1;

    size_t found = 0;

    for (size_t x = 0; x < datal; x++) {
        for (size_t y = 0; data[x][y]; y++) {
            search(data,datal,x,y,&found);
        }
    }

    printf("%lu\n",found);

    for (size_t i = 0; i < datal; i++)
        free(data[i]);
    if (data)
        free(data);

    return 0;
}

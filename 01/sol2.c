#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <err.h>

typedef unsigned int uint;

#define ARR_INC 1000

typedef struct {
    void *v;
    size_t size;
    size_t asize;
    size_t inc_r;
    size_t elsize;
} farray;

farray *
farray_init(size_t elsize, size_t inc_r)
{
    farray ret = {
        .elsize = elsize,
        .inc_r = inc_r
    };
    return memcpy(malloc(sizeof(farray)),&ret,sizeof(farray));
}

void *
farray_inc(farray *f) {
    if (f->size++ >= f->asize)
        f->v = realloc(f->v,(f->asize+=f->inc_r)*f->elsize);
    return f->v+(f->size-1)*f->elsize;
}

void
farray_conv(farray *f, void **arr, size_t *arrl) {
    if (f->size != f->asize) {
        if (f->size == 0) {
            free(f->v);
            *arr = NULL;
        } else
            *arr = realloc(f->v,f->size*f->elsize);
    } else
        *arr = f->v;
    *arrl = f->size;
    free(f);
}

void
farray_free(farray *f) {
    if (f->v)
        free(f->v);
    free(f);
}

void
readinput(const char *file, uint **arr1, size_t *arr1l, uint **arr2, size_t *arr2l)
{
    FILE *in = fopen(file,"r");
    if (in == NULL)
        err(-1,"%s",file);

    farray *arr[2];
    arr[0] = farray_init(sizeof(uint),ARR_INC);
    arr[1] = farray_init(sizeof(uint),ARR_INC);

    uint x,y;
    while (fscanf(in,"%u %u",&x,&y) != EOF) {
        *(uint*)farray_inc(arr[0]) = x;
        *(uint*)farray_inc(arr[1]) = y;
    }

    farray_conv(arr[0],(void*)arr1,arr1l);
    farray_conv(arr[1],(void*)arr2,arr2l);
}

void
my_qsort(uint *arr, const size_t begin, const size_t end)
{
    if (begin >= end)
        return;

    const uint pivot = arr[end];
    size_t i = begin;
    uint t;

    for (size_t j = begin; j < end; j++) {
        if (arr[j] < pivot) {
            t = arr[i];
            arr[i] = arr[j];
            arr[j] = t;
            i++;
        }
    }
    t = arr[i];
    arr[i] = arr[end];
    arr[end] = t;

    if (i)
        my_qsort(arr,begin,i-1);
    my_qsort(arr,i+1,end);
}

size_t
binsearch(uint target, const uint *arr, const size_t size)
{
    size_t begin=0,end=size-1;

    while (begin <= end) {
        size_t mid = begin+(end-begin)/2;
        uint val = arr[mid];
        if (val == target)
            return mid;
        if (!mid)
            break;
        if (val < target) {
            begin = mid+1;
        } else
            end = mid-1;
    }

    return -1;
}

size_t
findsimilarity(uint target, const uint *arr, const size_t size)
{
    size_t index = binsearch(target,arr,size);
    if (index == -1)
        return 0;

    uint occurance = 1;
    size_t i;
    if (index) {
        i = index-1;
        while (arr[i] == target) {
            occurance++;
            if (!i)
                break;
            i--;
        }
    }
    for (i = index+1; i < size && arr[i] == target; i++)
        occurance++;
    return target*occurance;
}

int
main(int argc, char *argv[])
{
    if (argc < 2) {
        fprintf(stderr,"%s <FILE>\n",argv[0]);
        return -1;
    }

    uint *arr[2] = {NULL};
    size_t arrl[2] = {0};
    readinput(argv[1],&arr[0],&arrl[0],&arr[1],&arrl[1]);
    my_qsort(arr[0],0,arrl[0]-1);
    my_qsort(arr[1],0,arrl[1]-1);

    size_t similarity = 0;

    for (size_t i = 0; i < arrl[0]; i++)
        similarity += findsimilarity(arr[0][i],arr[1],arrl[1]);

    free(arr[0]);
    free(arr[1]);

    printf("%lu\n",similarity);

    return 0;
}

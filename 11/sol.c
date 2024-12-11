#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <err.h>

typedef unsigned char uchar;
typedef unsigned long long int ull;

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
    if (f->size >= f->asize)
        f->v = realloc(f->v,(f->asize+=f->inc_r)*f->elsize);
    return f->v+((f->size++)*f->elsize);
}

void
farray_conv(farray *f, void **data, size_t *datal)
{
    *data = NULL;
    if (f->asize) {
        if (f->size) {
            if (f->size != f->asize) {
                f->v = realloc(f->v,f->size*f->elsize);
                *data = f->v;
            }
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
    ull num;
    ull count;
} stone;

size_t
binsearch(const stone *arr, const size_t size, ull target)
{
    size_t begin=0,end=size;

    while (begin < end) {
        size_t mid = begin+(end-begin)/2;
        ull val = arr[mid].num;
        if (val == target)
            return mid;
        if (val < target) {
            begin = mid+1;
        } else {
            end = mid;
        }
    }

    return begin;
}

farray *
store_init()
{
    return farray_init(sizeof(stone),1024);
}

stone *
store_add(farray *stones, ull num, ull count)
{
    size_t index=0;
    if (!stones->size)
        goto ADD;
    index = binsearch(stones->v,stones->size,num);
    if (((stone*)stones->v)[index].num == num) {
        stone *ret = ((stone*)stones->v)+index;
        ret->count += count;
        return ret;
    }

    ADD: ;

    farray_inc(stones);
    stone *stonesv = (stone*)stones->v;
    if (stones->size > index)
        memmove(stonesv+index+1,stonesv+index,sizeof(stone)*((stones->size-1)-index));

    stone *ret = stonesv+index;
    ret->num = num;
    ret->count = count;
    return ret;
}

void
getinput(const char *filename, farray *stones)
{
    FILE *f = fopen(filename,"r");
    if (!f)
        err(-1,"%s",filename);

    char *line = NULL;
    size_t linel = 0;

    getline(&line,&linel,f);

    fclose(f);

    char *ptr=line,*saveptr,*token;

    while ((token = strtok_r(ptr," ",&saveptr))) {
        store_add(stones,(ull)atoll(token),1);

        ptr = NULL;
    }

    free(line);
}

uchar
numofdigits(ull n)
{
    uchar ret = 0;
    while (n) {
        n /= 10;
        ret++;
    }
    return ret;
}

void
blink(farray *s1, farray *s2)
{
    stone *v = s1->v;
    for (size_t i = 0; i < s1->size; i++) {
        if (v[i].num == 0) {
            store_add(s2,1,v[i].count);
            continue;
        }
        uchar n = numofdigits(v[i].num);
        if ((n&1) == 0) {
            uchar ns = n/2;
            ull o1=v[i].num,o2=0;
            for (size_t j=0,g=1; j < ns; j++, g*=10) {
                o2 += (o1%10)*g;
                o1 /= 10;
            }
            store_add(s2,o1,v[i].count);
            store_add(s2,o2,v[i].count);
            continue;
        }

        store_add(s2,v[i].num*2024,v[i].count);
    }
}

int
main(int argc, char *argv[])
{
    if (argc < 3) {
        fprintf(stderr,"%s <FILE> <BLINKS>\n",argv[0]);
        return -1;
    }


    farray *s1=store_init(),*s2=store_init();

    getinput(argv[1],s1);

    int blinks = atoi(argv[2]);
    for (int i = 0; i < blinks; i++) {
        blink(s1,s2);

        farray *t = s2;
        s2 = s1;
        s1 = t;
        s2->size = 0;
    }
    ull amount=0;
    stone *stonesv = s1->v;
    for (size_t i = 0; i < s1->size; i++) {
        amount += stonesv[i].count;
    }

    printf("%llu\n",amount);

    farray_free(s1);
    farray_free(s2);

    return 0;
}

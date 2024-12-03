#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <err.h>
#include <ctype.h>

void
getfile(const char *file, char **data, size_t *datal)
{
    *data = NULL;
    *datal = 0;

    FILE *f = fopen(file,"r");
    if (!f)
        err(-1,"%s",file);

    const size_t bufmax = (1<<16);
    char buf[bufmax];
    ssize_t bufl = 0;

    do {
        bufl = fread(buf,1,bufmax,f);
        if (bufl) {
            *data = realloc(*data,*datal+bufl);
            memcpy(*data+*datal,buf,bufl);
            *datal += bufl;
        }
    } while (bufl == bufmax);

    fclose(f);
}

int
getdigit(const char *data, size_t *pos, const size_t datal)
{
    int ret = 0;
    size_t i = *pos;

    while (i < datal && isdigit(data[i])) {
        ret = ret*10 + data[i]-'0';
        i++;
    }

    *pos = i;
    return ret;
}

int
main(int argc, char *argv[])
{
    if (argc < 2) {
        fprintf(stderr,"%s <FILE>\n",argv[0]);
        return -1;
    }

    char *data;
    size_t datal;
    getfile(argv[1],&data,&datal);

    long long int sum = 0;

    size_t i = 0;
    while (i+8 < datal) {
        if (memcmp(data+i,"mul(",4) == 0) {
            i += 4;
            int x,y;
            x = getdigit(data,&i,datal);
            if (i >= datal || data[i] != ',')
                continue;
            i++;
            y = getdigit(data,&i,datal);
            if (i >= datal || data[i] != ')')
                continue;
            sum += x*y;
        }
        i++;
    }

    printf("%lld\n",sum);

    return 0;
}

#include <stdio.h>
#include <stdlib.h>
#include <err.h>
#include <string.h>
#include <assert.h>

typedef unsigned char uchar;

void
process(const char *filename)
{
    FILE *f = fopen(filename,"r");
    if (!f)
        err(-1,"%s",filename);

    uchar connections[256][256] = {0};

    char *line = NULL;
    size_t linel = 0;

    ssize_t t;
    while ((t = getline(&line,&linel,f)) != -1) {
        if (t == 1)
            break;
        uchar x,y;
        x = atoi(line);
        char *next = memchr(line,'|',t);
        if (!next)
            continue;
        y = atoi(next+1);

        connections[x][y] = 1;
    }

    const size_t maxbuf = 256;
    uchar buf[maxbuf];
    size_t bufl;
    size_t found = 0;
    size_t sum = 0;
    uchar seen[256];

    while ((t = getline(&line,&linel,f)) != -1) {
        bufl = 0;
        char *token,*saveptr,*str=line;

        for (bufl = 0; (token = strtok_r(str,",",&saveptr)); str = NULL, bufl++) {
            buf[bufl] = atoi(token);
            assert(bufl < maxbuf);
        }

        assert((bufl&1) == 1);

        memset(seen,0,256);

        for (size_t i = 0; i < bufl; i++) {

            for (size_t j = 0; j < 256; j++) {
                if (seen[j] && connections[buf[i]][j]) {
                    goto SKIP;
                }
            }
            seen[buf[i]] = 1;
        }
        found++;
        sum += buf[bufl/2];
        SKIP: ;
    }

    printf("%lu\n",sum);

    if (line)
        free(line);
    fclose(f);
}

int
main(int argc, char *argv[])
{
    if (argc < 2) {
        fprintf(stderr,"%s <FILE>\n",argv[0]);
        return -1;
    }

    process(argv[1]);

    return 0;
}

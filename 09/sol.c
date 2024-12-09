#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <err.h>
#include <assert.h>

typedef unsigned char uchar;
typedef unsigned long long int ull;

int
main(int argc, char *argv[])
{
    if (argc < 2) {
        fprintf(stderr,"%s <FILE>\n",argv[0]);
        return -1;
    }

    FILE *f = fopen(argv[1],"r");
    if (!f)
        err(-1,"%s",argv[1]);

    char *line = NULL;
    size_t linel = 0;

    getline(&line,&linel,f);

    fclose(f);

    if (!line)
        goto END;

    linel = strlen(line);
    if (linel && line[linel-1] == '\n') {
        linel--;
        if (!linel)
            goto END;
    }

    uchar *space = malloc(sizeof(uchar)*(linel/2));
    uchar *block = malloc(sizeof(uchar)*(linel/2+(linel&1)));
    size_t spacel=0,blockl=0;

    for (size_t i = 0; i < linel; i++) {
        if (i&1) {
            space[spacel++] = line[i]-'0';
        } else {
            block[blockl++] = line[i]-'0';
        }
    }

    ull checksum = 0;
    size_t pos=0;
    for (size_t i=blockl-1,j=0,g=0; j < i;) {
        while (1) {
            while (block[j]) {
                checksum += j*pos;
                pos++;
                block[j]--;
            }

            j++;
            if (space[g] == 0) {
                g++;
                if (g >= spacel)
                    goto EX;
            } else
                break;
        }

        while (space[g] && j < i) {
            while (space[g] && block[i]) {
                checksum += i*pos;
                pos++;
                block[i]--;
                space[g]--;
            }
            if (block[i] == 0) {
                i--;
            }
        }
        if (++g >= spacel)
            break;
    }
    for (size_t i = 0; i < blockl; i++) {
        while (block[i]) {
            checksum += i*pos;
            pos++;
            block[i]--;
        }
    }

    free(space);
    free(block);

    printf("%llu\n",checksum);

    END: ;
    if (line)
        free(line);

    return 0;
}

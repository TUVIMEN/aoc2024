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
    char *block = malloc(sizeof(uchar)*(linel/2+(linel&1)));
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
    for (size_t i=blockl-1,j=0,g=0; j <= i;) {
        if (block[j] > 0) {
            for (size_t l = 0; l < block[j]; l++) {
                checksum += j*pos;
                pos++;
            }
            block[j] *= -1;
        } else
            pos += block[j]*-1;
        if (j == i)
            break;
        j++;

        size_t k = i;
        while (space[g] && g < spacel) {
            for (; j <= k; k--) {
                if (block[k] > 0 && block[k] <= space[g]) {
                    for (size_t l = 0; l < block[k]; l++) {
                        checksum += k*pos;
                        pos++;
                        space[g]--;
                    }
                    block[k] *= -1;
                    if (k == i)
                        i--;
                    break;
                }
            }
            if (j > k) {
                pos += space[g];
                space[g] = 0;
            }
        }
        g++;
    }

    free(space);
    free(block);
    EX:;

    printf("%llu\n",checksum);

    END: ;
    if (line)
        free(line);

    return 0;
}

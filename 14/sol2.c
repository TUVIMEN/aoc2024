#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <err.h>
#include <assert.h>

const int height = 103;
const int width = 101;
/*const int height = 7;
const int width = 11;*/
const int maxrobots = 1024;

typedef unsigned char uchar;

void
getinput(const char *filename, int (*robots)[4], size_t *robotsl)
{
    FILE *f = fopen(filename,"rb");
    if (!f)
        err(-1,"%s",filename);

    char *line = NULL;
    size_t linel = 0;

    while (getline(&line,&linel,f) != -1) {
        char *token,*saveptr;

        int (*o)[4] = robots+((*robotsl)++);
        assert(*robotsl <= maxrobots);
        uchar i = 0;

        for (char *ptr = line; (token = strtok_r(ptr,"=",&saveptr)); ptr = NULL) {
            if (ptr)
                continue;
            char *token2,*saveptr2;
            for (char *ptr2 = token; (token2 = strtok_r(ptr2,",",&saveptr2)); ptr2 = NULL) {
                (*o)[i] = atoi(token2);
                i++;
                assert(i <= 4);
            }
        }
    }

    if (line)
        free(line);
    fclose(f);
}

int
robots_comp(const int (*r1)[4], const int (*r2)[4])
{
    int d = (*r1)[0]-(*r2)[0];
    if (d != 0)
        return d;
    return (*r1)[1]-(*r2)[1];
}

int
main(int argc, char *argv[0])
{
    if (argc < 2) {
        fprintf(stderr,"%s <FILE>\n",argv[0]);
        return -1;
    }

    int robots[maxrobots][4];
    size_t robotsl = 0;

    getinput(argv[1],robots,&robotsl);

    for (size_t sec = 1; sec < 999999; sec++) {
        for (size_t i = 0; i < robotsl; i++) {
            robots[i][0] = (robots[i][0]+robots[i][2])%width;
            robots[i][1] = (robots[i][1]+robots[i][3])%height;
            if (robots[i][0] < 0)
                robots[i][0] += width;
            if (robots[i][1] < 0)
                robots[i][1] += height;
        }

        qsort(robots,robotsl,sizeof(int)*4,(int (*)(const void*,const void*))robots_comp);

        size_t are_inline = 0;
        for (size_t j = 0; j < robotsl;) {
            int prevx = robots[j][0];
            int prevy = robots[j][1];
            j++;
            while (j < robotsl && robots[j][0] == prevx) {
                int d = prevy-robots[j][1];
                if (d != 0 && d != -1)
                    break;
                if (d != 0)
                    are_inline++;
                prevy = robots[j][1];
                j++;
            }
        }
        if (are_inline > 200) {
            printf("%lu\n",sec);
            break;
        }
    }

    return 0;
}

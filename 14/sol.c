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
main(int argc, char *argv[0])
{
    if (argc < 2) {
        fprintf(stderr,"%s <FILE>\n",argv[0]);
        return -1;
    }

    int robots[maxrobots][4];
    size_t robotsl = 0;

    getinput(argv[1],robots,&robotsl);

    size_t tiles[4] = {0};
    size_t tiles_p[4][4] = {
        {0,width/2,0,height/2},
        {0,width/2,height/2+height%2,height+1},
        {width/2+width%2,width+1,0,height/2},
        {width/2+width%2,width+1,height/2+height%2,height+1},
    };

    for (size_t i = 0; i < robotsl; i++) {
        robots[i][0] = (robots[i][0]+robots[i][2]*100)%width;
        robots[i][1] = (robots[i][1]+robots[i][3]*100)%height;
        if (robots[i][0] < 0)
            robots[i][0] += width;
        if (robots[i][1] < 0)
            robots[i][1] += height;

        for (size_t j = 0; j < 4; j++) {
            if (robots[i][0] >= tiles_p[j][0] && robots[i][0] < tiles_p[j][1]
                && robots[i][1] >= tiles_p[j][2] && robots[i][1] < tiles_p[j][3]) {
                tiles[j]++;
            }
        }
    }

    size_t out = 1;
    for (size_t j = 0; j < 4; j++)
        out *= tiles[j];
    printf("%lu\n",out);

    return 0;
}

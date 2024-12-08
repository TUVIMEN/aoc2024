#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <err.h>
#include <assert.h>

size_t
makeindex(char c)
{
    if (c >= '0' && c <= '9')
        return c-'0';
    if (c >= 'A' && c <= 'Z')
        return (c-'A')+10;
    if (c >= 'a' && c <= 'z')
        return (c-'a')+36;

    assert(c == 0);
    return 0;
}

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

    const size_t antennas_kinds = 62;
    const size_t antennas_maxsize = 20;
    int antennas[antennas_kinds][antennas_maxsize][2];
    size_t antennasl[antennas_kinds];
    memset(antennasl,0,sizeof(size_t)*antennas_kinds);

    int xsize=0,ysize=0;

    char *line = NULL;
    size_t linel = 0;
    ssize_t s;
    size_t i = 0;

    for (; (s = getline(&line,&linel,f)) != -1; i++) {
        if (xsize == 0) {
            xsize = s;
            if (s && line[s-1] == '\n')
                xsize--;
        }

        for (size_t j = 0; j < s && line[j] != '\n'; j++) {
            if (line[j] == '.')
                continue;
            size_t index = makeindex(line[j]);
            assert(index < antennas_kinds);
            int *a = antennas[index][antennasl[index]++];
            a[0] = j;
            a[1] = i;
        }
    }

    ysize = i;

    if (line)
        free(line);
    fclose(f);

    size_t antinodes = 0;
    unsigned char *things = calloc(ysize*xsize,1);

    for (size_t i = 0; i < antennas_kinds; i++) {
        int (*ant)[2] = antennas[i];
        size_t len = antennasl[i];
        if (!len)
            continue;
        if (len > 1) {
            for (size_t j = 0; j < len; j++) {
                size_t index = ant[j][1]*ysize+ant[j][0];
                if (things[index] == 0) {
                    antinodes++;
                    things[index] = 1;
                }
            }
        }

        for (size_t j = 0; j < len-1; j++) {
            for (size_t g = j+1; g < len; g++) {
                int xdif,ydif;
                xdif = ant[j][0]-ant[g][0];
                ydif = ant[j][1]-ant[g][1];

                int nx,ny,px=ant[j][0],py=ant[j][1];

                while (1) {
                    nx = px+xdif;
                    ny = py+ydif;

                    if (nx >= 0 && nx < xsize && ny >= 0 && ny < ysize) {
                        if (things[ysize*ny+nx] == 0) {
                            antinodes++;
                            things[ysize*ny+nx] = 1;
                        }
                    } else
                        break;
                    px = nx;
                    py = ny;
                }

                px=ant[g][0],py=ant[g][1];

                while (1) {
                    nx = px-xdif;
                    ny = py-ydif;

                    if (nx >= 0 && nx < xsize && ny >= 0 && ny < ysize) {
                        if (things[ysize*ny+nx] == 0) {
                            antinodes++;
                            things[ysize*ny+nx] = 1;
                        }
                    } else
                        break;
                    px = nx;
                    py = ny;
                }
            }
        }
    }

    printf("%lu\n",antinodes);

    free(things);

    return 0;
}

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <err.h>
#include <assert.h>

typedef unsigned char uchar;
typedef long long int ll;

ssize_t
readgame(FILE *f, char **line, size_t *linel, ll out[6])
{
    ssize_t s;
    uchar group = 0;
        char *saveptr,*token;

    for (size_t i = 0; i < 2; i++) {
        s = getline(line,linel,f);
        assert(s != -1);
        for (char *ptr=*line; (token = strtok_r(ptr,"+",&saveptr)); ptr = NULL) {
            if (ptr)
                continue;
            out[group++] = (ll)atoi(token);
        }
    }
    s = getline(line,linel,f);
    assert(s != -1);
    for (char *ptr=*line; (token = strtok_r(ptr,"=",&saveptr)); ptr = NULL) {
        if (ptr)
            continue;
        out[group++] = 10000000000000+(ll)atoi(token);
    }
    return getline(line,linel,f);
}

size_t
play(ll game[6])
{
    /*
    Ax * Sa + Bx * Sb = x
    Sa = (x - Bx * Sb)/Ax

    Ay * Sa + By * Sb = y
    Sa = (y - By * Sb)/Ay

    - Bx * Sb = (y - By * Sb)*Ax/Ay - x
    -Bx*Sb = y*Ax/Ay - By*Sb*Ax/Ay - x
    -Bx*Sb*Ay + By*Sb*Ax = y*Ax - x*Ay
    Sb*(By*Ax - Bx*Ay) = y*Ax - x*Ay

    Sb = (y*Ax - x*Ay)/(By*Ax - Bx*Ay)

    T = Sa*3 + Sb
    */

    ll Sb1 = game[5]*game[0]-game[4]*game[1],
        Sb2 = game[0]*game[3]-game[1]*game[2];
    if (Sb1%Sb2 != 0)
        return 0;

    ll Sb = Sb1/Sb2;
    ll Sa1 = game[4]-game[2]*Sb,
        Sa2 = game[0];
    if (Sa1%Sa2 != 0)
        return 0;

    ll Sa = Sa1/Sa2;

    return Sa*3 + Sb;
}

int
main(int argc, char *argv[])
{
    if (argc < 2) {
        fprintf(stderr,"%s <FILE>\n",argv[0]);
        return -1;
    }

    FILE *f = fopen(argv[1],"rb");
    if (!f)
        err(-1,"%s",argv[1]);

    ll game[6];
    char *line = NULL;
    size_t linel = 0;
    size_t tokens = 0;

    while (1) {
        ssize_t s = readgame(f,&line,&linel,game);
        tokens += play(game);
        if (s == -1)
            break;
    }

    printf("%lu\n",tokens);

    if (line)
        free(line);
    fclose(f);

    return 0;
}

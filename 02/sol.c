#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <err.h>
#include <assert.h>

typedef unsigned char uchar;

typedef struct lnode lnode;
struct lnode {
    int v;
    lnode *n;
};

const size_t maxreports = 1024;

void
lnode_free(lnode *ln)
{
    while (ln) {
        lnode *n = ln->n;
        free(ln);
        ln = n;
    }
}

void
getinput(const char *filename, lnode *reports, size_t *reportsl)
{
    FILE *f = fopen(filename,"rb");
    if (!f)
        err(-1,"%s",filename);
    char *line = NULL;
    size_t linel = 0;

    while (getline(&line,&linel,f) != -1) {
        char *token,*saveptr;
        lnode *r = reports+((*reportsl)++);
        assert(*reportsl <= maxreports);
        r->n = NULL;
        for (char *ptr = line; (token = strtok_r(ptr," ",&saveptr)); ptr = NULL) {
            r->v = atoi(token);
            r->n = memset(malloc(sizeof(lnode)),0,sizeof(lnode));
            r = r->n;
        }
    }

    if (line)
        free(line);
    fclose(f);
}

uchar
iscorrect(lnode *r)
{
    if (!r->n)
        return 0;

    int prev = r->v;
    r = r->n;
    int dir = 0;

    while (r->n) {
        int dif = r->v-prev;
        if (dif == 0)
            return 0;
        int cdir = 1;
        if (dif < 0)
            cdir = -1;
        if (dir == 0) {
            dir = cdir;
        } else if (dir != cdir)
            return 0;

        if (abs(dif) > 3)
            return 0;

        prev = r->v;
        r = r->n;
    }
    return 1;
}

int
main(int argc, char *argv[])
{
    if (argc < 2) {
        fprintf(stderr,"%s <FILE>\n",argv[0]);
        return -1;
    }

    lnode reports[maxreports];
    size_t reportsl = 0;
    getinput(argv[1],reports,&reportsl);

    size_t correct = 0;
    for (size_t i = 0; i < reportsl; i++) {
        correct += iscorrect(reports+i);
    }
    printf("%lu\n",correct);

    for (size_t i = 0; i < reportsl; i++)
        if (reports[i].n)
            lnode_free(reports[i].n);

    return 0;
}

#include <stdlib.h>
#include <stdio.h>
#include <sys/times.h>
#include <unistd.h>
#include <omp.h>

#define INITIAL_BUFFER_SIZE 4096

char *readfile(FILE *file, size_t *readn)
{
    char *buf = NULL;
    size_t size = INITIAL_BUFFER_SIZE;
    size_t pos = 0;
    *readn = 0;
    while(!feof(file))
    {
        char *tmp = realloc(buf, size);
        if(!tmp)
        {
            free(buf);
            perror("Can't allocate memory");
            return NULL;
        }
        buf = tmp;
        *readn += fread(buf + size - INITIAL_BUFFER_SIZE, 1, INITIAL_BUFFER_SIZE, file);
        size += INITIAL_BUFFER_SIZE;
    }
    return buf;
}

char downtable[] = "spxlrzimayedwtbgvnjocfhquk";
char uptable[] = "SPXLRZIMAYEDWTBGVNJOCFHQUK";

int main(int argc, char *argv[])
{
    FILE *in, *out;
    if(argc < 2)
    {
        in = stdin;
    }
    else
    {
        in = fopen(argv[1], "r");
        if(!in)
        {
            perror("can't open input file");
            return 1;
        }
    }
    if(argc < 3)
    {
        out = stdout;
    }
    else
    {
        out = fopen(argv[2], "w");
        if(!out)
        {
            perror("Can't open output file");
            return 1;
        }
    }
    size_t size;
    char *buf = readfile(in, &size);
    omp_set_num_threads(4);
    struct tms tms;
    clock_t time = times(&tms);
    char *c;
    #pragma omp parallel for
    for(c = buf; c < buf + size; c++)
    {
        if(*c >= 'a' && *c <= 'z')
        {
            *c = uptable[*c - 'a'];
        }
        if(*c >= 'a' && *c <= 'z')
        {
            *c = downtable[*c - 'a'];
        }
    }
    time = times(&tms) - time;
    fwrite(buf, 1, size, out);
    free(buf);
    printf("%f\n", (float)time/sysconf(_SC_CLK_TCK));
    return 0;
}

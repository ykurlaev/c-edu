#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BLOCK_SIZE 4096
#define TAG 13

char downtable[] = "spxlrzimayedwtbgvnjocfhquk";
char uptable[] = "SPXLRZIMAYEDWTBGVNJOCFHQUK";

void process(FILE *in, FILE *out, int ps, int io)
{
    for(;;)
    {
        MPI_Status status;
        char sbuf[BLOCK_SIZE * ps];
        memset(sbuf, 0, BLOCK_SIZE * ps);
        char rbuf[BLOCK_SIZE];
        int size;
        if(io)
        {
            size = fread(sbuf, 1, BLOCK_SIZE * ps, in);
        }
        MPI_Scatter(sbuf, BLOCK_SIZE, MPI_CHAR, rbuf, BLOCK_SIZE, MPI_CHAR, 0, MPI_COMM_WORLD);
        char *c;
        for(c = rbuf; c < rbuf + BLOCK_SIZE; c++)
        {
            if(*c >= 'A' && *c <= 'Z')
            {
                *c = uptable[*c - 'A'];
            }
            if(*c >= 'a' && *c <= 'z')
            {
                *c = downtable[*c - 'a'];
            }
        }
        MPI_Gather(rbuf, BLOCK_SIZE, MPI_CHAR, sbuf, BLOCK_SIZE, MPI_CHAR, 0, MPI_COMM_WORLD);
        fwrite(sbuf, 1, size, out);
        int end;
        if(io)
        {
            end = feof(in);
        }
        MPI_Bcast(&end, 1, MPI_INT, 0, MPI_COMM_WORLD);
        if(end)
        {
            return;
        }
    }
}

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
            perror("Can't open input file");
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
    int rank, size;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    process(in, out, size, (rank == 0));
    MPI_Finalize();
    return 0;
}

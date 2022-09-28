#include <mpi.h>
#include <stdio.h>

#define BLOCK_SIZE 4096
#define TAG 13

void ioprocess(FILE *in, FILE *out, int ps)
{
    for(;;)
    {
        MPI_Status status;
        char buf[BLOCK_SIZE];
        int size;
        int p;
        for(p = 1; p < ps; p++)
        {
            size = fread(buf, 1, BLOCK_SIZE, in);
            MPI_Send(&size, 1, MPI_INT, p, TAG, MPI_COMM_WORLD);
            MPI_Send(buf, size, MPI_CHAR, p, TAG, MPI_COMM_WORLD);
        }
        for(p = 1; p < ps; p++)
        {
            MPI_Recv(&size, 1, MPI_INT, p, TAG, MPI_COMM_WORLD, &status);
            MPI_Recv(buf, size, MPI_CHAR, p, TAG, MPI_COMM_WORLD, &status);
            fwrite(buf, 1, size, out);
        }
        if(feof(in))
        {
            for(p = 1; p < ps; p++)
            {
                int z = -1;
                MPI_Send(&z, 1, MPI_INT, p, TAG, MPI_COMM_WORLD);
            }
            return;
        }
    }
}

char downtable[] = "spxlrzimayedwtbgvnjocfhquk";
char uptable[] = "SPXLRZIMAYEDWTBGVNJOCFHQUK";

void calcprocess()
{
    for(;;)
    {
        MPI_Status status;
        int size;
        MPI_Recv(&size, 1, MPI_INT, 0, TAG, MPI_COMM_WORLD, &status);
        if(size == -1)
        {
            return;
        }
        char buf[size];
        MPI_Recv(buf, size, MPI_CHAR, 0, TAG, MPI_COMM_WORLD, &status);
        char *c;
        for(c = buf; c < buf + size; c++)
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
        MPI_Send(&size, 1, MPI_INT, 0, TAG, MPI_COMM_WORLD);
        MPI_Send(buf, size, MPI_CHAR, 0, TAG, MPI_COMM_WORLD);
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
    if(size < 2)
    {
        MPI_Finalize();
        fprintf(stderr, "Invalid thread count");
        return 1;
    }
    if(rank == 0)
    {
        ioprocess(in, out, size);
    }
    else
    {
        calcprocess();
    }
    MPI_Finalize();
    return 0;
}

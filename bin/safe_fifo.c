#include <stdio.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>

#define BUF_SIZE 16384

int main(int argc, char *argv[])
{
    char *fifo_file_name;
    char buf[BUF_SIZE];
    struct stat st;
    int fdr = 0, fdw = 0, n;

    if (argc < 2)
    {
        fifo_file_name = "/tmp/audio";
    }
    else
    {
        fifo_file_name = argv[1];
    }

    if (mkfifo(fifo_file_name, 0666) < 0 && errno != EEXIST)
    {
        fprintf(stderr, "error creating fifo buffer: %d\n", errno);
        goto err;
    }

    // hacky solution to prevent ENXIO
    if ((fdr = open(fifo_file_name, O_RDONLY | O_NONBLOCK, 0)) < 0)
    {
        fprintf(stderr, "error opening fifo buffer read only: %d\n", errno);
        goto err;
    }

    if ((fdw = open(fifo_file_name, O_WRONLY | O_NONBLOCK, 0)) < 0)
    {
        fprintf(stderr, "error opening fifo buffer write only: %d\n", errno);
        goto err;
    }

    if (fstat(fdw, &st) < 0)
    {
        fprintf(stderr, "error running fstat on fifo buffer: %d\n", errno);
        goto err;
    }

    if (!S_ISFIFO(st.st_mode))
    {
        fprintf(stderr, "%s is no a fifo buffer\n", fifo_file_name);
        goto err;
    }

    while (!feof(stdin) && (n = read(STDIN_FILENO, buf, BUF_SIZE)) > 0)
    {
        write(fdw, buf, n);
    }

    close(fdr);
    close(fdw);

    return 0;

err:
    close(fdr);
    close(fdw);

    return 1;
}

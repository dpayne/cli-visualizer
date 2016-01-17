#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <time.h>

#define BUF_SIZE 16384

char write_buffer[BUF_SIZE];
char read_buffer[BUF_SIZE];
char default_fifo_name[] = "/tmp/audio";

static struct timespec k_read_attempt_sleep_timespec = {0, 1L * 1000000L};

int main(int argc, char *argv[])
{
    char *fifo_file_name;
    if (argc < 2)
    {
        fifo_file_name = default_fifo_name;
    }
    else
    {
        fifo_file_name = argv[1];
    }

    int retval = mkfifo(fifo_file_name, 777);
    if (retval < 0)
    {
        fprintf(stderr, "error creating fifo buffer: %d", errno);
        return -1;
    }

    int fd = open(fifo_file_name, O_WRONLY | O_NONBLOCK, 0);

    if (fd < 0)
    {
        fprintf(stderr, "error opening fifo buffer: %d", errno);
        return -1;
    }

    // mark stdin as binary
    freopen(NULL, "rb", stdin);

    while (!feof(stdin))
    {
        // read input into buffer
        int bytes_read = read(STDIN_FILENO, write_buffer, BUF_SIZE);
        if (bytes_read > 0)
        {
            // pipe buffer to fifo
            write(fd, write_buffer, bytes_read);

            // EAGAIN means the buffer is full, clear it and try again
            if (errno == EAGAIN)
            {
                bytes_read = read(fd, read_buffer, BUF_SIZE);
                if (bytes_read < 0)
                {
                    fprintf(stderr, "error flushing fifo buffer: %d", errno);
                }
            }
        }
        else
        {
            break;
        }
    }

    if (close(fd) < 0)
    {
        fprintf(stderr, "error closing fifo buffer: %d", errno);
        return -1;
    }
    exit(0);
}

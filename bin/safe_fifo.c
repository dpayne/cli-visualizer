#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>

#define BUF_SIZE 16384

char write_buffer[BUF_SIZE];
char read_buffer[BUF_SIZE];
char default_fifo_name [] = "/tmp/audio";

int main(int argc, char *argv[])
{
    char * fifo_file_name;
    if (argc < 2)
    {
        fifo_file_name = default_fifo_name;
    }
    else
    {
        fifo_file_name = argv[1];
    }


    mkfifo(fifo_file_name, 0666);

    int fd = open(fifo_file_name, O_WRONLY | O_NONBLOCK, 0);

    //mark stdin as binary
    freopen(NULL, "rb", stdin);

    while (!feof(stdin))
    {
        // read input into buffer
        int bytes_read = read(STDIN_FILENO, write_buffer, BUF_SIZE);
        if ( bytes_read > 0 )
        {
            // pipe buffer to fifo
            write(fd, write_buffer, bytes_read);

            // EAGAIN means the buffer is full, clear it and try again
            if (errno == EAGAIN)
            {
                read(fd, read_buffer, BUF_SIZE);
            }
        }
    }

    close(fd);
}

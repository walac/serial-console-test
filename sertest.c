#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

static char data[] = "abcdefghjklmnopqrstuvwxyz0123456789";
static const int data_size = sizeof(data) - sizeof(data[0]);

int main(int argc, char **argv)
{
    int opt, i, k, n = 0;
    char *buf;
    int fd;

    while ((opt = getopt(argc, argv, "n:")) != -1) {
        switch (opt) {
            case 'n':
                n = atoi(optarg);
                k = n / data_size;
                break;
        }
    }

    if (n <= 0)
        return EXIT_FAILURE;

    buf = calloc(n, sizeof(char));
    if (!buf)
        return EXIT_FAILURE;

    for (i = 0; i < k; ++i)
        memcpy(buf + i * data_size, data, data_size);

    memcpy(buf + i * data_size, data, n % data_size);

    fd = open(argv[optind], O_WRONLY);
    if (fd < 0) {
        perror("open");
        return EXIT_FAILURE;
    }

    if (write(fd, buf, n) < 0)
        perror("write");

    close(fd);

    return 0;
}

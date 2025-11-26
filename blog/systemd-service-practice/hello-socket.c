#include <stdio.h>
#include <string.h>
#include <systemd/sd-daemon.h>
#include <sys/socket.h>
#include <unistd.h>

int main(int argc, char* argv[])
{
    if (sd_listen_fds(1) > 0)
    {
        /* systemd woke us up. we should never see more than one FD passed to us. */
        int fd_listen = SD_LISTEN_FDS_START;
        if (listen(fd_listen, 511) < 0) {
            perror("Failed to listen");
            return -1;
        }
	fprintf(stderr, "succeed to listen with fd: %i\n", fd_listen);
        for (int i = 0; i < 8; ++i)
        {
            int fd_accept = accept(fd_listen, NULL, NULL);
            if (fd_accept < 0)
            {
                perror("Failed to accept");
                return -1;
            }
	    fprintf(stderr, "succeed to connect with fd:  %i\n", fd_accept);
            char buffer[32] = {0};
            snprintf(buffer, sizeof(buffer), "Hello, World: %d\n", i);
            write(fd_accept, buffer, strlen(buffer));
            close(fd_accept);
        }
    }
    else
    {
        perror("Failed to sd_listen_fds");
        return -1;
    }

    return 0;
}

/*
 * yum install systemd-devel
 * pkg-config --cflags --libs libsystemd
 * gcc -std=c99 -lsystemd -o hello-socket.exe hello-socket.c
 * */

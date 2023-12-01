#include <stdio.h>
#include <string.h>
#include <systemd/sd-daemon.h>
#include <sys/socket.h>
#include <unistd.h>

int main(int argc, char* argv[])
{
    if (sd_listen_fds(1) > 0)
    {
        int fd_accept = SD_LISTEN_FDS_START;
        fprintf(stderr, "succeed to connect with fd:  %i\n", fd_accept);
        char buffer[32] = {0};
        snprintf(buffer, sizeof(buffer), "Hello, World: %d\n", fd_accept);
        write(fd_accept, buffer, strlen(buffer));
        close(fd_accept);
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
 * gcc -std=c99 -lsystemd -o hello-accept.exe hello-accept.c
 * */

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/select.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <signal.h>
#include <time.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <getopt.h>
#include "networking.h"
#include "notify.h"

#ifdef SOCK_VERBOSE
#define VERSION_STRING  "SS5, v1.1\n"
#endif

extern int errno;
extern char* optarg;

uint16_t port = 1337;
int sock;
struct sockaddr_in srv;
sig_atomic_t sigcaught;

void cleanup()
{
    close(sock);
#ifdef SOCK_VERBOSE
    notify_port_closed();
#endif
}

void sigcallback(int signal_)
{
    if (sigcaught == signal_) {
        cleanup();
        exit(1);
    }
    sigcaught = signal_;
}

int runsocks5()
{
    if ((sock = create_socket(0)) == -1)
        return complain(errno);
#ifdef SOCK_VERBOSE
    printf("\nSig caught: '%s', exiting.\n", strsignal((int)accept_loop(sock)));
    printf("SOCKS5 server by sierinjs.\n");
#else
    accept_loop(sock);
#endif
    cleanup();
    return 0;
}

int main(int argc, char** argv)
{
    int opt;
    while ((opt = getopt(argc, argv, "vp:")) != -1) {
        switch (opt)
        {
#ifdef SOCK_VERBOSE
            case 'v':
                printf(VERSION_STRING);
                exit(1);
#endif
            case 'p':
                port = atoi(optarg);
                break;
            default:
#ifdef SOCK_VERBOSE
                printf("Usage: %s [-v] [-p <port>]\n", argv[0]);
#endif
                exit(1);
        }
    }

    sigcaught = 0;
    signal(SIGINT, sigcallback);
    signal(SIGHUP, SIG_IGN);
    return runsocks5();
}

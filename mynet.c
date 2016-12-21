// mynet.c
// Weborf copyright 2010 (GPL3+) Salvo "LtWorf" Tomaselli <tiposchi@tiscali.it>

#include "options.h"

#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <syslog.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdlib.h>

#include "types.h"
#include "mynet.h"

extern weborf_configuration_t weborf_conf;

// Creates the server socket and performs some setsockopt on it.
int net_create_server_socket() {
    int s;
    int val;

    // Creates the socket
#ifdef IPV6
    if (weborf_conf.ipv6) s = socket(PF_INET6, SOCK_STREAM, 0);
    else
#endif
        s = socket(PF_INET, SOCK_STREAM, 0);

#ifdef IPV6
    // This futile system call is here just because a debian mantainer decided
    // that the default behavior must be to listen only to IPv6 connections
    // excluding IPv4 ones.
    // So this restores the logic and normal behavior of accepting connections
    // without being racist about the client's address.
    if (weborf_conf.ipv6) {
        val=0;
        setsockopt(s, IPPROTO_IPV6, IPV6_V6ONLY, (char *)&val, sizeof(val));
    }
#endif

    val = 1;
    // Makes port reusable immediately after termination.
    if (setsockopt(s, SOL_SOCKET, SO_REUSEADDR, &val, sizeof(val)) < 0) {
        perror("ruseaddr(any)");
        syslog(LOG_ERR, "reuseaddr(any)");
#ifdef IPV6
        if (weborf_conf.ipv6) {
            char *suggestion = "If you don't have IPv6 support in kernel, don't use the -e/--ipv6 commandline argument\n";
            write(2, suggestion, strlen(suggestion));
        }
#endif
        return -1;
    }

    return s;
}

void net_bind_and_listen(int s) {

    // Not used when the -e/--ipv6 commandline argument is active
    struct sockaddr_in locAddr;
    int ipAddrL = sizeof(struct sockaddr_in);

#ifdef IPV6
    struct sockaddr_in6 locAddr6; // Local and remote address
    if (weborf_conf.ipv6) {
        // Bind
        memset(&locAddr6, 0, sizeof(locAddr6));
        locAddr6.sin6_family = AF_INET6;
        locAddr6.sin6_port = htons(strtol(weborf_conf.port, NULL, 0));
        if (weborf_conf.ip == NULL) { // Default ip, listens to all the interfaces
            locAddr6.sin6_addr = in6addr_any;
        } else { // Custom ip
            if (inet_pton(AF_INET6, weborf_conf.ip, &locAddr6.sin6_addr) == 0) {
                printf("Invalid IPv6 address: %s\n", weborf_conf.ip);
                exit(2);
            }
        }
        if (bind(s, (struct sockaddr *) &locAddr6, sizeof(locAddr6)) < 0) {
            perror("trying to bind");
#ifdef SOCKETDBG
            syslog(LOG_ERR, "Port %d already in use", ntohs(locAddr6.sin6_port));
#endif
            exit(3);
        }
    }
    else {
#endif
        // Prepares socket's address
        locAddr.sin_family = AF_INET; // Internet socket
        {
            // Check the validity of port param and uses it
            unsigned int p = strtol(weborf_conf.port, NULL, 0);
            if (p < 1 || p > 65535) {
                printf("Invalid port number: %d\n", p);
                exit(4);
            }
            locAddr.sin_port = htons(p);
        }
        if (weborf_conf.ip == NULL) weborf_conf.ip = "0.0.0.0"; // Default ip address
        if (inet_aton(weborf_conf.ip, &locAddr.sin_addr) == 0) {
            // Converts ip to listen in binary format
            printf("Invalid IPv4 address: %s\n", weborf_conf.ip);
            exit(2);
        }
#ifdef SOCKETDBG
        syslog(LOG_INFO, "Listening on address: %s:%d",
               inet_ntoa(locAddr.sin_addr), ntohs(locAddr.sin_port));
#endif
        // Bind
        if (bind(s, (struct sockaddr *) &locAddr, ipAddrL) == -1) {
            perror("trying to bind");
#ifdef SOCKETDBG
            syslog(LOG_ERR, "Port %d already in use", ntohs(locAddr.sin_port));
#endif
            exit(3);
        }
#ifdef IPV6
    }
#endif

    listen(s, MAXQ); // Listen to the socket

}

void net_getpeername(int socket,char* buffer) {

#ifdef IPV6
    if (weborf_conf.ipv6) {
        struct sockaddr_storage t_addr;
        socklen_t addr_l=sizeof(t_addr);
        getpeername(socket, (struct sockaddr *)&t_addr, &addr_l);
        if (t_addr.ss_family==AF_INET) {
            struct sockaddr_in *addr =(struct sockaddr_in *)&t_addr;
            char temp_buffer[INET_ADDRSTRLEN];
            inet_ntop(AF_INET, &(addr->sin_addr), temp_buffer, INET_ADDRSTRLEN);
            snprintf(buffer,INET6_ADDRSTRLEN,"::ffff:%s",temp_buffer);
        } else {
            struct sockaddr_in6 *addr =(struct sockaddr_in6 *)&t_addr;
            inet_ntop(AF_INET6, &(addr->sin6_addr), buffer, INET6_ADDRSTRLEN);
        }
    }
    else {
#endif
        struct sockaddr_in addr;
        socklen_t addr_l=sizeof(struct sockaddr_in);
        getpeername(socket, (struct sockaddr *)&addr,&addr_l);
        inet_ntop(AF_INET, &addr.sin_addr, buffer, INET_ADDRSTRLEN);
#ifdef IPV6
    }
#endif
}

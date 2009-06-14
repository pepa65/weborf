/*
Weborf
Copyright (C) 2007  Giuseppe Pappalardo

Weborf is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.

@author Giuseppe Pappalardo <pappalardo@dmi.unict.it>
 */

#ifndef WEBORF_QUEUE_H
#define WEBORF_QUEUE_H

#include <stdlib.h>
#include <pthread.h>
#include "options.h"

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <unistd.h>
#include <stdlib.h>

#include <stdio.h>
#include <stdlib.h>
#include <syslog.h>
#include <getopt.h>


#include <stdlib.h>
#include <pthread.h>




typedef struct {
    int	num, size;//Filled positions in the queue, and its maximum size
    int head, tail;//pointers to head and tail of the round queue
    int * data;//Socket with client

#ifdef IPV6
    struct sockaddr_in6 * addr;//Local and remote address
#else
    struct sockaddr_in * addr;
#endif

    pthread_mutex_t mutex;//mutex to modify the queue
    pthread_cond_t for_space, for_data;
    int n_wait_sp, n_wait_dt;
} syn_queue_t;

int q_init(syn_queue_t * q, int size);

#ifdef IPV6
int q_put(syn_queue_t * q, int val,struct sockaddr_in6 addr_);
int q_get(syn_queue_t * q, int * val,struct sockaddr_in6 * addr_);
#else
int q_put(syn_queue_t * q, int val,struct sockaddr_in addr_);
int q_get(syn_queue_t * q, int * val,struct sockaddr_in * addr_);
#endif

void q_free(syn_queue_t * q);

#endif

// queue.h
// Weborf copyright 2007 (GPL3+) Salvo "LtWorf" Tomaselli <tiposchi@tiscali.it>

#ifndef WEBORF_QUEUE_H
#define WEBORF_QUEUE_H

#include "types.h"

int q_init(syn_queue_t * q, int size);
int q_put(syn_queue_t * q, int val);
int q_get(syn_queue_t * q, int *val);
void q_free(syn_queue_t * q);

#endif

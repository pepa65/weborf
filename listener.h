// listener.h
// Weborf copyright 2007 (GPL3+) Salvo "LtWorf" Tomaselli <tiposchi@tiscali.it>

#ifndef WEBORF_LISTENER_H
#define WEBORF_LISTENER_H

#define NOMEM 7

void init_threads(unsigned int count);
void quit();
void print_queue_status();
void *t_shape(void *nulla);
void set_authsocket(char *);
void chn_thread_count(int val);
void set_new_uid(int uid);

#endif

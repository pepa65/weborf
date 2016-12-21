// cachedir.h
// Weborf copyright 2010 (GPL3+) Salvo "LtWorf" Tomaselli <tiposchi@tiscali.it>

#ifndef WEBORF_CACHEDIR_H
#define WEBORF_CACHEDIR_H


#include "types.h"

bool cache_send_item(unsigned int uprefix,connection_t* connection_prop);
int cache_get_item_fd(unsigned int uprefix,connection_t* connection_prop);
int cache_get_item_fd_wr(unsigned int uprefix,connection_t *connection_prop);
void cache_store_item(unsigned int uprefix,connection_t* connection_prop, char *content, size_t content_len);
void cache_init(char *dir);
int cache_clear();
bool cache_is_enabled();

#endif

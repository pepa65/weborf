// mynet.h
// Weborf copyright 2010 (GPL3+) Salvo "LtWorf" Tomaselli <tiposchi@tiscali.it>

#ifndef WEBORF_MYNET_H
#define WEBORF_MYNET_H

#include "types.h"
#include "options.h"

int net_create_server_socket();
void net_bind_and_listen(int s);
void net_getpeername(int,char*);

#endif

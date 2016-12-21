// auth.h
// Weborf copyright 2010 (GPL3+) Salvo "LtWorf" Tomaselli <tiposchi@tiscali.it>

#ifndef WEBORF_AUTH_H
#define WEBORF_AUTH_H

#include "types.h"
#include "options.h"

void auth_set_socket(char *u_socket);
int auth_check_request(connection_t* connection_prop);

#endif

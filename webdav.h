// webdav.h
// Weborf copyright 2009 (GPL3+) Salvo "LtWorf" Tomaselli <tiposchi@tiscali.it>

#ifndef WEBORF_WEBDAV_H
#define WEBORF_WEBDAV_H

#include "types.h"

int propfind(connection_t* connection_prop,string_t *post_param);
int mkcol(connection_t* connection_prop);
int copy_move(connection_t* connection_prop);

#endif

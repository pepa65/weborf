// mime.h
// Weborf copyright 2010 (GPL3+) Salvo "LtWorf" Tomaselli <tiposchi@tiscali.it>

#ifndef WEBORF_MIME_H
#define WEBORF_MIME_H

#include "options.h"
#include "types.h"

int mime_init(magic_t *token);
void mime_release(magic_t token);
const char* mime_get_fd (magic_t token,int fd,struct stat *sb);

#endif

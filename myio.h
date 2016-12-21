// myio.h
// Weborf copyright 2010 (GPL3+) Salvo "LtWorf" Tomaselli <tiposchi@tiscali.it>

#ifndef WEBORF_MYIO_H
#define WEBORF_MYIO_H

#include "types.h"
#include "options.h"

int fd_copy(int from, int to, off_t count);
int dir_remove(char * dir);
bool file_exists(char *file);

#ifdef WEBDAV
int dir_move_copy (char* source, char* dest,int method);
int file_copy(char* source, char* dest);
int file_move(char* source, char* dest);
int dir_move (char* source, char* dest);
int dir_copy (char* source, char* dest);
#endif

#endif

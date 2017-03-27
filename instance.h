// instance.h
// Weborf copyright 2007 (GPL3+) Salvo "LtWorf" Tomaselli <tiposchi@tiscali.it>

#ifndef WEBORF_INSTANCE_H
#define WEBORF_INSTANCE_H

#ifndef O_LARGEFILE //Needed to compile on Mac, where this doesn't exist
#define O_LARGEFILE 0
#endif

#include "types.h"
#include "buffered_reader.h"

#ifdef WEBDAV
#include "webdav.h"
#endif

// Requests
#define INVALID -1
#define GET 0
#define POST 1
#define PUT 2
#define DELETE 3
#define OPTIONS 8

#ifdef WEBDAV
#define PROPFIND 4
#define COPY 5
#define MOVE 6
#define MKCOL 7
#endif

#define NO_ACTION -120

// Errors
#define ERR_PRECONDITION_FAILED -14
#define ERR_NOT_ALLOWED -13
#define ERR_INSUFFICIENT_STORAGE -12
#define ERR_CONFLICT -11
#define ERR_SERVICE_UNAVAILABLE -10
#define ERR_FORBIDDEN -9
#define ERR_NOTIMPLEMENTED -8
#define ERR_NODATA -7
#define ERR_NOTHTTP -6
#define ERR_NOAUTH -5
#define ERR_SOCKWRITE -4
#define ERR_NOMEM -3
#define ERR_FILENOTFOUND -2
#define ERR_BRKPIPE -1

// OK
#define OK_CREATED 1
#define OK_NOCONTENT 2

// Protocol version
#define HTTP_0_9 57
#define HTTP_1_0 48
#define HTTP_1_1 2

void inetd();
void *instance(void*);
int write_file(connection_t*);
int send_err(connection_t*,int,char*);
string_t read_post_data(connection_t*,buffered_read_t*);
char *get_basedir(char*);
int send_http_header(int,unsigned long long int,char*,bool,time_t,connection_t*);
int delete_file(connection_t*);
int read_file(connection_t*,buffered_read_t*);
void wsock(int,char*,size_t);
#endif

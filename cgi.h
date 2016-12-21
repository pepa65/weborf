// cgi.h
// Weborf copyright 2010 (GPL3+) Salvo "LtWorf" Tomaselli <tiposchi@tiscali.it>

#ifndef WEBORF_CGI_H
#define WEBORF_CGI_H

#include "options.h"
#include "types.h"

#define STDIN 0
#define STDOUT 1
#define STDERR 2

int exec_page(char * executor,string_t* post_param,char* real_basedir,connection_t* connection_prop);

#endif

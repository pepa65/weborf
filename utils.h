// utils.h
// Weborf copyright 200y (GPL3+) Salvo "LtWorf" Tomaselli <tiposchi@tiscali.it>

#ifndef WEBORF_UTILS_H
#define WEBORF_UTILS_H

#include "types.h"
#include "options.h"

int list_dir(connection_t *connection_prop, char *html, unsigned int bufsize, bool parent);
void help();
void version();
void moo();
void print_start_disclaimer(int argc, char *argv[]);
bool get_param_value(char *http_param, char *parameter, char *buf, ssize_t size,ssize_t param_len);
void daemonize();
void print_capabilities();

#endif

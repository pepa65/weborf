// mystring.h
// Weborf copyright 2007 (GPL3+) Salvo "LtWorf" Tomaselli <tiposchi@tiscali.it>

#ifndef WEBORF_MYSTRING_H
#define WEBORF_MYSTRING_H

#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <ctype.h>

#include "instance.h"
#include "types.h"

void split_get_params(connection_t* connection_prop);
bool endsWith(char *str, char *end,ssize_t len_str,ssize_t len_end);
void delChar(char *string, int pos, int n);
void strReplace(char *string, char *substr, char with);
void replaceEscape(char *string);
void strToUpper(char *str);

#endif

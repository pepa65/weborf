// buffered_reader.h
// Weborf copyright 2009 (GPL3+) Salvo "LtWorf" Tomaselli <tiposchi@tiscali.it>

#ifndef BUFFERED_READER_H
#define BUFFERED_READER_H

#include <unistd.h>
typedef struct {
    char *buffer; // Buffer where the reader stores the read data
    char *start; // Pointer to non-consumed data
    char *end; // Pointer to 1st byte after end of the data. A read must continue after end.
    int size; // Size of the buffer
} buffered_read_t;

void buffer_reset (buffered_read_t * buf);
int buffer_init(buffered_read_t * buf, ssize_t size);
void buffer_free(buffered_read_t * buf);
ssize_t buffer_read(int fd, void *b, ssize_t count, buffered_read_t * buf);
size_t buffer_strstr(int fd, buffered_read_t * buf, char * needle);
#endif

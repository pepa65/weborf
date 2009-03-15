/*
Weborf
Copyright (C) 2009  Salvo "LtWorf" Tomaselli

Weborf is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.

@author Salvo "LtWorf" Tomaselli <tiposchi@tiscali.it>
 */

#include "buffered_reader.h"
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

/**
This funcion inits the struct allocating a buffer of the specified size.
It will return 0 on success and 1 on fail.
*/
int buffer_init(buffered_read_t * buf, int size) {
    buf->buffer = malloc(sizeof(char*) * size);
    buf->start= buf->buffer;
    buf->end=buf->buffer;
    buf->size=size;

    return (buf->buffer == NULL) ? 1 : 0;
}

/**
This function will free the memory allocated by the buffer used in the struct.
*/
void buffer_free(buffered_read_t * buf) {
    if (buf->buffer != NULL)
        free(buf->buffer);
}

/**
This function is designed to be similar to a normal read, but it uses an internal
buffer.
When the buffer is empty, it will try to fill it.
An important difference from the normal read is that this function will wait until
the requested amount of bytes are available.
On some special cases, the read data could be less than the requested one. For example if
end of file is reached and it is impossible to do further reads.
*/
ssize_t buffer_read(int fd, void *b, ssize_t count,buffered_read_t * buf) {
    ssize_t wrote=0;//Count of written bytes
    ssize_t available, needed;
    ssize_t r;

    while (wrote<count) {
        available=buf->end - buf-> start;
        needed=count-wrote;
        
        if (needed <= available) {//More data in buffer than needed
            memcpy(b, buf->start, needed );
            buf->start+=needed;
            return count;
        } else {//Requesting more data than available
            memcpy(b, buf->start, available );
            b+=available;
            buf->start+=available;
            wrote+=available;

            //Filing the buffer again
            buf->start= buf->buffer;
            r = read(fd,buf->buffer,buf->size);
            if (r==0) {
                buf->end=buf->start;
                return wrote;
                
            }
            buf->end=buf->start+r;
        }


    }
    return wrote;
}


//#include <sys/types.h>
//#include <sys/stat.h>
//#include <fcntl.h>

//int main () {
//    buffered_read_t buf;
//    
//    buffer_init(&buf, 30);
    
//    int fp=open("/home/salvo/.bash_history",O_RDONLY);
//    char * k=malloc (600);
    
//    int end=0;
//    while ((end=buffer_read(fp,k,500,&buf))>=500 ) {
        
//        k[end]=0;
//        printf("---- %s\n",k);
//    }
//    printf("%d",end);
    
//    free(k);
//    buffer_free(&buf);
//}
// mime.c
// Weborf copyright 2010 (GPL3+) Salvo "LtWorf" Tomaselli <tiposchi@tiscali.it>

#include "options.h"

#include <string.h>
#include <unistd.h>

#include "mime.h"

#ifdef SEND_MIMETYPES
#include <magic.h>
#endif

// Returns a token for the libmagic. buffer must NOT be shared amongst threads.
// The size of buffer is assumed to be at least MIMETYPELEN
int mime_init(magic_t *token) {
#ifdef SEND_MIMETYPES
    *token=magic_open(MAGIC_SYMLINK | MAGIC_MIME_TYPE);
    if (*token==NULL) return 1;
    return magic_load(*token,NULL); // Load the database
#else
    *token=NULL;
    return 0;
#endif

}

// Releases the token for libmagic. If the token is null, it will do nothing.
void mime_release(magic_t token) {
    if (token==NULL) return;
#ifdef SEND_MIMETYPES
    magic_close(token);
#endif
}

// returns mimetype of an opened file descriptor
// the cursor can be located at any position
// fd is the descriptor to an open file
// sb is the stat to the same file
const char* mime_get_fd (magic_t token,int fd,struct stat *sb) {

#ifdef SEND_MIMETYPES
    if (token==NULL) return NULL;

    // If fd is a directory, send the mimetype without attempting to read it

    if (sb->st_mode & S_IFREG) {
        // Seek file to 0 and read its header to know its mime type
        // then seek again to the previous position
        char buf[64];

        // Get the current cursor position
        off_t prev_pos=lseek(fd,0,SEEK_CUR);

        // Set the cursor to the beginning of the file
        lseek(fd,0,SEEK_SET);

        // Reads 64 bytes to determine the type
        int r=read(fd,&buf,64);

        // Reset the position
        lseek(fd,prev_pos,SEEK_SET);

        const char* mime=magic_buffer(token,&buf,r);

        return mime;
    } else if (sb->st_mode & S_IFDIR)
        return "application/x-directory";
    else if (sb->st_mode & S_IFSOCK)
        return "application/x-socket";
    else if (sb->st_mode & S_IFLNK)
        return "application/x-symlink";
    else if (sb->st_mode & S_IFBLK)
        return "application/x-block-device";
    else if (sb->st_mode & S_IFCHR)
        return "application/x-character-device";
    ///else if (sb->st_mode & S_IFIFO)
    return "application/x-fifo";

#else
    return NULL;
#endif
}

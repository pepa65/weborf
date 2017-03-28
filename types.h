// types.h
// Weborf copyright 2010 (GPL3+) Salvo "LtWorf" Tomaselli <tiposchi@tiscali.it>

#ifndef WEBORF_TYPES_H
#define WEBORF_TYPES_H

#include "options.h"

#include <stdbool.h> // Adds boolean type
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <netinet/in.h>

#ifdef SEND_MIMETYPES
#include <magic.h>
#else
typedef void* magic_t;
#endif

typedef struct {
    long int id; // ID of the thread
    magic_t mime_token; // Token for libmagic
} thread_prop_t;

typedef struct {
    ssize_t len; // length of the array
    char *data[MAXINDEXCOUNT]; // Array containing pointers
    int data_l[MAXINDEXCOUNT]; // Array containing len of strings
} array_ll;

typedef struct {
    int num, size; // Filled positions in the queue, and its maximum size
    int head, tail; // pointers to head and tail of the round queue
    int *data; // Socket with client
#ifdef IPV6
    struct sockaddr_in6 *addr; // Local and remote address
#else
    struct sockaddr_in *addr;
#endif
    pthread_mutex_t mutex; // mutex to modify the queue
    pthread_cond_t for_space, for_data;
    int n_wait_sp, n_wait_dt;
} syn_queue_t;

typedef struct {
    int sock; // File descriptor for the socket
#ifdef IPV6
    char ip_addr[INET6_ADDRSTRLEN]; // ip address in string format
#else
    char ip_addr[INET_ADDRSTRLEN];
#endif
    bool keep_alive; // True if we are using pipelining
    short int protocol_version; // See defines like HTTP_something
    int method_id; // Index of the http method used (GET, POST)
    char *method; // String version of the http method used
    char *http_param; // Param string
    char *page; // Requested URI
    ssize_t page_len; // Lengh of the page string
    char *get_params; // Params in the URI, after the ? char
    char *strfile; // File on filesystem
    ssize_t strfile_len; // Length of string strfile
    struct stat strfile_stat; // Stat of strfile
    int strfile_fd; // File descriptor for strfile
    char *basedir; // Basedir for the host
    unsigned int status_code; // HTTP status code
} connection_t;

typedef struct {
    ssize_t len; // length of the string
    char *data; // Pointer to string
} string_t;

typedef struct {
    pthread_mutex_t mutex; // Mutex to access this struct
    unsigned int free; // Free threads
    unsigned int count; // thread count
} t_thread_info;

typedef struct {
    char *basedir;
    char* authsock; // Executable that will authenticate
    uid_t uid; //UID to use after bind
    gid_t gid; //GID to use after bind
    bool full_basedir; // True: show the full path of basedir
    bool hide; // True: hide the 'hidden' dot-files
#ifdef SEND_MIMETYPES
    bool send_content_type; // True: send the content type
#endif
    bool is_inetd; //True: executed by inetd
    array_ll cgi_paths; // Paths to cgi binaries
    bool virtual_host; // True: check for virtual hosts
    bool exec_script; // True: CGI enabled
    bool tar_directory; // True: sends directories compressed into tar-files
    bool zip; // True: compress to zip instead of tgz
    char *ip; // IP addr with default value
    char *port; // port with default value
    char *user; // Username for authentication
    char *pass; // Password for authentication
    char *name; // Name of the application
    char *sig; // Signature of the application
    char *favlink; // the html link element with the URL to a favicon
    char *css; // The CSS style definitions for the webpage
    char *indexes[MAXINDEXCOUNT]; // List of pointers to index files
    int indexes_l; // Count of the list
} weborf_configuration_t;

#endif

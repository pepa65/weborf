// utils.c
// Weborf copyright 2007 (GPL3+) Salvo "LtWorf" Tomaselli <tiposchi@tiscali.it>

#include "options.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <time.h>
#include <stdbool.h>
#include <netdb.h>
#include <dirent.h>
#include <unistd.h>
#include <syslog.h>
#include <signal.h>

#include "mystring.h"
#include "utils.h"
#include "embedded_auth.h"

extern weborf_configuration_t weborf_conf;

// This function reads the directory dir, putting inside the html string an
// html page with links to all the files within the directory.
// Buffer for html must be allocated by the calling function.
// bufsize is the size of the buffer allocated for html
// parent is true when the dir has a parent dir
// Returns the size of the html; -1: unable to open; -2: out of memory
int list_dir(connection_t *connection_prop, char *html, unsigned int bufsize, bool parent) {
    int pagesize=0; // Written bytes on the page
    int maxsize = bufsize - 1; // String's max size
    int printf_s;
    char *color; // Depending on row count chooses a background color
    char *measure; // contains measure unit for file's size (B, KiB, MiB)
    int counter = 0;
    int errcode = 0;

    char path[INBUFFER]; // Buffer to contain element's absolute path

    struct dirent **namelist = NULL;
    counter = scandir(connection_prop->strfile, &namelist, 0, alphasort);


    if (counter <0) { //Open not succesfull
        errcode = -1;
        goto escape;
    }

    // Specific header table
    pagesize=printf_s=snprintf(html+pagesize,maxsize,"%s%s</title>%s\n<style type=\"text/css\">%s</style></head>\n<body><h4>%s</h4><div class=\"list\"><table><tr class=\"i\"><td></td><td>Name</td><td>Size</td><td>Last Modified</td></tr>",HTMLHEAD,weborf_conf.name,weborf_conf.favlink,weborf_conf.css,connection_prop->strfile);
    maxsize-=printf_s;

    // Cycles trough dir's elements
    int i;
    struct tm ts;
    struct stat f_prop; // File's property
    char last_modified[URI_LEN];

    // Print link to parent directory, if there is any
    if (parent) {
        char *page = connection_prop->page;
        page++;
        page[strlen(page)-1] = 0;
			  printf_s=snprintf(html+pagesize,maxsize,"<tr class=\"darker\"><td class=\"b\"><a href=\"..\">up</a></td><td><a href=\"..\">%s</a></td><td></td><td></td></tr>",page);
        maxsize-=printf_s;
        pagesize+=printf_s;
        color = "light";
    }
    else color = "dark";

    for (i=0; i<counter; i++) {
        // Skipping hidden files
        if (namelist[i]->d_name[0] == '.' || errcode) {
            free(namelist[i]);
            continue;
        }

        snprintf(path, INBUFFER,"%s/%s", connection_prop->strfile, namelist[i]->d_name);

        // Stat on the entry

        stat(path, &f_prop);
        int f_mode = f_prop.st_mode; // Get's file's mode

        // get last modified
        localtime_r(&f_prop.st_mtime,&ts);
        strftime(last_modified,URI_LEN, "%a, %d %b %Y %H:%M:%S", &ts);
        char ext[7];
        strcpy(ext, (weborf_conf.zip ? "zip" : "tar.gz"));

        if (S_ISREG(f_mode)) { // Regular file

            // Table row for the file
            // Scaling the file's size
            unsigned long long int size = f_prop.st_size;
            if (size < 1024) {
                measure=" B";
            } else if ((size = (size / 1024)) < 1024) {
                measure=" KiB";
            } else if ((size = (size / 1024)) < 1024) {
                measure=" MiB";
            } else {
                size = size / 1024;
                measure=" GiB";
            }
            char *name = namelist[i]->d_name;
            printf_s=snprintf(html+pagesize,maxsize,
                    "<tr class=\"%s\"><td><a href=\"%s?/\" title=\"%s.%s\">file</a></td><td><a href=\"%s\">%s</a></td><td>%lld%s</td><td>%s</td></tr>\n",
                    color, name, name, ext, name, name, (long long int)size, measure, last_modified);
            maxsize-=printf_s;
            pagesize+=printf_s;
            color = (color == "dark" ? "light" : "dark");

        } else if (S_ISDIR(f_mode)) { // Directory entry
            // Table row for the dir
            char *name = namelist[i]->d_name;
            printf_s=snprintf(html+pagesize,maxsize,
                    "<tr class=\"%s\"><td class=\"b\"><a href=\"%s?/\" title=\"%s.%s\">dir</a></td><td class=\"b\"><a href=\"%s\">%s</a></td><td></td><td>%s</td></tr>\n",
                    color, name, name, ext, name, name, last_modified);
            maxsize-=printf_s;
            pagesize+=printf_s;
            color = (color == "dark" ? "light" : "dark");
        }

        free(namelist[i]);
        if (maxsize <= 0) errcode = -2; // Out of memory
    }
escape:
    free(namelist);
    if (errcode == 0) {
        printf_s=snprintf(html+pagesize,maxsize,"</table>%s",HTMLFOOT);
        pagesize+=printf_s;
        return pagesize;
    } else return errcode;
}

// Prints version information
void version() {
    printf("Weborf " VERSION "\n"
           "Copyright 2007 Salvo 'LtWorf' Tomaselli.\n"
           "This is free software.  You may redistribute copies of it under the terms of\n"
           "the GNU General Public License <http://www.gnu.org/licenses/gpl.html>.\n"
           "There is NO WARRANTY, to the extent permitted by law.\n\n"

           "Written by Salvo 'LtWorf' Tomaselli and Salvo Rinaldi.\n"
           "Synchronized queue by Prof. Giuseppe Pappalardo\n"
           "Modifications by pepa65 <" PACKAGE_BUGREPORT ">\n" PACKAGE_URL "\n");
    exit(0);
}

void print_capabilities() {
    printf("version:" VERSION "\n");

    printf("ipv:"
#ifdef IPV6
    "6"
#else
    "4"
#endif
    "\n"
    );

    printf("webdav:"
#ifdef WEBDAV
    "yes"
#else
    "no"
#endif
    "\n"
    );

    printf("mime:"
#ifdef SEND_MIMETYPES
    "yes"
#else
    "no"
#endif
    "\n"
    );

    exit(0);
}

// Prints command line help
void help() {

    printf("\tStart the weborf webserver\n"
           "\tUsage: weborf [OPTIONS]\n\n"
#ifdef IPV6
           "\tCompiled for IPv6\n"
#else
           "\tCompiled for IPv4\n"
#endif

#ifdef WEBDAV
           "\tHas webdav support\n"
#endif

#ifdef SEND_MIMETYPES
           "\tHas MIME support\n"
#endif

           "Default port: %s\n"
           "Default base directory: %s\n"
           "Signature used: %s\n\n"
           "CSS stylesheet used: %s\n\n",PORT,
#ifdef BASEDIR
                                         BASEDIR,
#else
                                         "$PWD",
#endif
                                         PACKAGE_STRING,CSS);

    printf("  -a, --auth    followed by absolute path of authentication program\n"
           "  -b, --basedir followed by the path of basedir\n"
           "  -C, --cache   sets the directory to use for cache files\n"
           "  -c, --cgi     list of cgi files+binaries to execute, comma-separated\n"
           "  -d, --daemon  keep running, as a daemon\n"
#ifdef IPV6
           "  -e, --ipv6    sets extended IPv6 addressing mode only, no IPv4\n"
#endif
           "  -f, --favicon followed by the URL of a favicon for the webpage\n"
           "  -g, --gid     followed by valid GID; if started by root,\n"
           "                this will be the group of the process\n"
           "  -h, --help    display this help and exit\n"
           "  -I, --index   list of index files, comma-separated\n"
           "  -i, --ip      followed by IP address to listen on\n"
           "  -k, --caps    lists the capabilities of the binary\n"
           "  -l, --login   filename with login credentials: first line is username,\n"
           "                second line is password (overridden by -U and -P)\n"
           "  -m, --mime    sends content type header to clients\n"
           "  -n, --name    the name of the application as in the webpage title\n"
           "  -P, --pass    followed by the password for access authentication\n"
           "                (both username and password must be set for it to work)\n"
           "  -p, --port    followed by the port number to listen on\n"
           "  -S, --css     additional CSS for the stylesheet of the webpage\n"
           "  -s, --sig     the signature as at the bottom of the webpage\n"
           "  -T  --inetd   must be specified when using weborf with inetd or xinetd\n"
           "  -t  --tar     will send the directories as .tar.gz files\n"
           "  -U, --user    followed by the username for access authentication\n"
           "  -u, --uid     followed by a valid UID; if started by root,\n"
           "                this user will read and execute files for weborf\n"
           "  -V, --virtual list of virtualhosts in the form host=basedir, comma-separated\n"
           "  -v, --version print program version\n"
           "  -X, --exec    instead of sending each file, execute the script and send output\n"
           "  -z, --zip     compress as zip instead of tgz\n\n"


           "Report bugs here: " PACKAGE_URL "\n"
           "or to: " PACKAGE_BUGREPORT "\n");
    exit(0);
}

// Searching for easter eggs within the code isn't fair!
void moo() {
    printf(" _____________________________\n"
           "< Weborf has supercow powers! >\n"
           " -----------------------------\n"
           "        \\   ^__^\n"
           "         \\  (oo)\\_______\n"
           "            (__)\\       )\\/\\\n"
           "                ||----w |\n"
           "                ||     ||\n");
    exit(0);
}

// This function prints the start disclaimer on stdout.
// It wants the command line parameters
void print_start_disclaimer(int argc, char *argv[]) {
    char *host;
    if (weborf_conf.ip == NULL) host = "localhost";
    else host = weborf_conf.ip;
    printf("Weborf %s is Free software (GPLv3)\n"
           "Serving %s on http://%s:%s\n"
           "For options: %s --help\n",
            VERSION, weborf_conf.basedir, host, weborf_conf.port, argv[0]);
}

// Detaches the process from the shell, it is re-implemented because it is not
// included in POSIX
// It shouldn't be executed after launching other threads. In that case the
// effects are not specified.
void daemonize() {
    if (fork() == 0)
        signal(SIGHUP, SIG_IGN);
    else
        exit(0);
}

// This function retrieves the value of an http field within the header
// http_param: string containing the header
// parameter: searched parameter
// buf: buffer for the value
// size: maximum size of the buffer
// param_len: lenght of the parameter
// Returns false if the parameter isn't found, or true otherwise
bool get_param_value(char *http_param, char *parameter, char *buf, ssize_t size,ssize_t param_len) {
    char *val = strstr(http_param, parameter); // Locates the requested parameter information

    if (val == NULL) { // No such field
        return false;
    }

    // It is very important for this line to be here, for security reasons.
    // It moves the pointer forward, assuming "Field: Value\r\n"
    // If the field is malformed like "Field0\r\n" the subsequent strstr
    // will fail and the function will return false.
    // Moving this line after the next strstr would introduce a security
    // vulnerability.
    // The strstr will not cause a segfault because at this point the header
    // string must at least terminate with "\r\n\r", the last '\r' is changed
    // to 0 so there is enough space to perform the operation
    val += param_len + 2; // Moves the begin of the string to exclude the name of the field

    char *field_end = strstr(val, "\r\n"); // Searches the end of the parameter
    if (field_end==NULL) {
        return false;
    }

    if ((field_end - val + 1) < size) {
        // If the parameter's length is less than buffer's size
        memcpy(buf, val, field_end - val);
    } else { // Parameter string is too long for the buffer
        return false;
    }
    buf[field_end - val] = 0; // Ends the string within the destination buffer

    return true;
}

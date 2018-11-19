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

#include "utils.h"
#include "embedded_auth.h"

extern weborf_configuration_t weborf_conf;

// This function encodes characters in 'name' to %hh form into 'encoded'
static void href_encode(const char *name, char *encoded, const int alloc) {
    // 'encoded' needs pre-allocation of length passed into 'alloc'
    encoded[0]=0;
    if (name==NULL) return;

    char *code=" \"#&\\<\?:%>";
    size_t len_name=strlen(name);
    size_t len_code=strlen(code);
    size_t len_encoded=0;
    size_t inc;
    char *format;
    size_t i, j;

    for (i=0; i < len_name; i++) { // Check every character of name
        format="%c";
        inc=1;
        for (j=0; j < len_code; j++) // Encode every character of code
            if (name[i]==code[j]) {
                format="%%%02x";
                inc=3;
                break;
            }
        if (len_encoded + inc >= alloc) break;
        len_encoded+=sprintf(encoded + len_encoded, format, name[i]);
    }
}

// This function encodes the characters in 'name' to &code; into 'encoded'
static void html_encode(const char *name, char *encoded, const size_t alloc) {
    // 'encoded' needs pre-allocation of length into 'alloc'!
    size_t len=0;
    size_t inc;
    char *code;
    size_t i;
    char ch[2]=" ";

    for (i=0; i < strlen(name); i++) {
        ch[0]=name[i];
        code=ch;
        switch(ch[0]) {
            case '&': code="&amp;"; break;
            case '<': code="&lt;"; break;
            case '>': code="&gt;"; break;
            case ' ': code="&nbsp;"; break;
        }
        inc=strlen(code);
        if (len+inc >= alloc) break;
        strncpy(encoded + len, code, inc);
        len+=inc;
    }
    encoded[len]=0;
}

// This function reads the directory dir, putting inside the html string an
// html page with links to all the files within the directory.
// Buffer for html must be allocated by the calling function.
// bufsize is the size of the buffer allocated for html
// parent is true when the dir has a parent dir
// Returns the size of the html; -1: unable to open; -2: out of memory
int list_dir(connection_t *connection_prop, char *html, unsigned int bufsize, bool parent) {
    int pagesize=0; // Written bytes on the page
    int maxsize=bufsize-1; // String's max size
    int printf_s;
    bool color=0; // flip row background colour
    // size and unit for file's size (B, KiB, MiB, GiB) and remainder for GiB
    unsigned long long int size;
    char path[INBUFFER]; // Buffer to contain element's absolute path
    struct dirent **namelist=NULL; // UNIX filenames
    int i;
    int nfiles=scandir(connection_prop->strfile, &namelist, 0, alphasort);
    if (nfiles<0) return -1; // Open not succesful
    char *sizeu=malloc(SIZEU); // max: len("9999.99 GiB")
    char *basename=malloc(HTML_NLEN);
    char *basedir=weborf_conf.full_basedir ? connection_prop->strfile : connection_prop->page;
    char name_href[HREF_NLEN];
    char name_html[HTML_NLEN];
    href_encode(basedir, name_href, HREF_NLEN);
    html_encode(basedir, name_html, HTML_NLEN);

    i=strlen(name_html)-1;
    int j=HTML_NLEN;
    basename[--j]=0;
    while (i > 0 && name_html[--i] != '/') basename[--j]=name_html[i];
    name_html[++i]=0;
    basename=basename+j;

    // Specific header table
    printf_s=snprintf(html+pagesize, maxsize,
            "%s%s</title>%s<style type=\"text/css\">\n%s\n</style></head>"
            "<body><table class=\"head\"><tr><td><a href=\"/\" title=\"back to root\">root</a></td>"
            "<td><a href=\"..\" title=\"One back to here\">%s</a></td>"
            "</tr></table><div class=\"list\"><table>"
            "<tr class=\"i\"><td>type</td><td>name</td><td>size</td><td>last modified</td></tr>",
            HTMLHEAD, weborf_conf.name, weborf_conf.favlink, weborf_conf.css,
            weborf_conf.full_basedir || parent ? name_html : "");
printf("1- %d %d %d\n", pagesize, printf_s, maxsize);
    pagesize+=printf_s;
    maxsize-=printf_s;
    if (maxsize < 0) return -2; // Out of memory

    char *name;
    printf_s=snprintf(html+pagesize,maxsize,"<tr class=\"darker\"><td><b><a href=\"%s?\">dir</a></b></td><td><b>%s</b></td><td class=\"size\">%d</td><td></td></tr>",
            name_href, basename, nfiles-2);
printf("2- %d %d %d\n", pagesize, printf_s, maxsize);
    pagesize+=printf_s;
    maxsize-=printf_s;
    if (maxsize < 0) return -2; // Out of memory

    // Cycle through dir's elements
    struct stat f_prop; // File's property
    char last_modified[URI_LEN];
    for (i=0; i<nfiles; i++) {
        name=namelist[i]->d_name;
        if (name[0]=='.')
            // Skip hidden files? Skip . and .. for sure
            if (weborf_conf.hide || name[1]==0 || (name[1]=='.' && name[2]==0)) {
                free(namelist[i]);
                continue;
            }

        snprintf(path, INBUFFER,"%s/%s", connection_prop->strfile, name);

        // Stat on the entry
        stat(path, &f_prop);
        int f_mode=f_prop.st_mode; // Get file mode
        // Get last modified
        strftime(last_modified, URI_LEN, "%Y-%m-%d %H:%M:%S %Z %A", localtime(&f_prop.st_mtime));
        html_encode(name, name_html, HTML_NLEN);
        href_encode(name, name_href, HREF_NLEN);

        if (S_ISREG(f_mode)) { // Regular file
            // Table row for the file, scaling the file's size
            size=f_prop.st_size;
            if (size < 1024) snprintf(sizeu, SIZEU, "%d B", (int)size);
            else if (size < 1048576)
                snprintf(sizeu, SIZEU, "%.2f KiB", (double)size/1024);
            else if ((size=size/1024) < 1048576)
                snprintf(sizeu, SIZEU, "%.2f MiB", (double)size/1024);
            else snprintf(sizeu, SIZEU, "%.2f GiB", (double)size/1048576);
            printf_s=snprintf(html+pagesize, maxsize,
                    "<tr class=\"%s\"><td><a href=\"%s?\" title=\"%s.%s\">file</a></td><td><a href=\"%s\">%s</a></td><td class=\"size\">%s</td><td class=\"date\">%s</td></tr>\n",
                    color ? "dark" : "light", name_href, name_html,
                    weborf_conf.zip ? "zip" : "tar.gz", name_href, name_html,
                    sizeu, last_modified);
        } else if (S_ISDIR(f_mode)) // Directory entry
            // Table row for the dir
            printf_s=snprintf(html+pagesize, maxsize,
                    "<tr class=\"%s b\"><td><a href=\"%s?\" title=\"%s.%s\">dir</a></td><td><a href=\"%s/\">%s</a></td><td></td><td class=\"date\">%s</td></tr>\n",
                    color ? "dark" : "light", name_href, name_html,
                    weborf_conf.zip ? "zip" : "tar.gz", name_href, name_html,
                    last_modified);
        else {
            printf("No file/directory:%s\n", name);
            free(namelist[i]);
            continue;
        }
        color^=color;
printf("3- %d %d %d\n", pagesize, printf_s, maxsize);
        pagesize+=printf_s;
        maxsize-=printf_s;
        if (maxsize < 0) return -2; // Out of memory
        free(namelist[i]);
    }
    free(namelist);
    free(sizeu);
    printf_s=snprintf(html+pagesize,maxsize,"</table>%s",HTMLFOOT);
printf("4- %d %d %d\n", pagesize, printf_s, maxsize);
    pagesize+=printf_s;
    maxsize-=printf_s;
    if (maxsize < 0) return -2; // Out of memory
    return pagesize;
}

// Prints version information
void version() {
    printf("Weborf " VERSION "\n" COPYRIGHT);
    exit(0);
}

void print_capabilities() {
    printf("version:" VERSION "\n");
    printf("ipv6:"
#ifdef IPV6
    "1"
#else
    "0"
#endif
    "\n");
    printf("webdav:"
#ifdef WEBDAV
    "1"
#else
    "0"
#endif
    "\n");
    printf("mime:"
#ifdef SEND_MIMETYPES
    "1"
#else
    "0"
#endif
    "\n");
    exit(0);
}

// Prints command line help
void help() {
    int i;
    printf(" %s\n"
           " Usage: weborf [OPTIONS]\n\n Compiled-in features: "
#ifdef IPV6
           "IPv6 "
#else
           "IPv4 "
#endif

#ifdef WEBDAV
           "WebDAV "
#endif

#ifdef SEND_MIMETYPES
           "MIME "
#endif
#ifndef IPV6
#ifndef WEBDAV
#ifndef SEND_MIMETYPES
           "[none]"
#endif
#endif
#endif
           "\n Default port: %s\n"
           " Default base directory: %s\n\n"
           " Default CSS stylesheet used: %s\n\n", PACKAGE_STRING, PORT,
#ifdef BASEDIR
                                         BASEDIR,
#else
                                         "$PWD",
#endif
                                         CSS);
    printf(" Default index files: ");
    for (i=0; i<weborf_conf.indexes_l; i++) printf("%s ", weborf_conf.indexes[i]);
    if (weborf_conf.indexes_l==0) printf("[none]");
    printf("\n\n  -a, --auth    followed by absolute path of authentication program\n"
           "  -b, --basedir followed by the path of basedir\n"
           "  -C, --cache   sets the directory to use for cache files\n"
           "  -c, --cgi     list of cgi files+binaries to execute, comma-separated\n"
           "  -d, --daemon  keep running, as a daemon\n"
           "  -F, --full    show the full path of basedir\n"
           "  -f, --favicon followed by the URL of a favicon for the webpage\n"
           "  -g, --gid     followed by valid GID; if started by root,\n"
           "                this will be the group of the process\n"
           "  -h, --help    display this help and exit\n"
           "  -H  --hide    hide 'hidden files' that start with a dot\n"
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

           " Report bugs here: " PACKAGE_URL "\n"
           " or to: " PACKAGE_BUGREPORT "\n");
    exit(0);
}

// Searching for easter eggs within the code isn't fair!
void moo() {
    printf("  _____________________________\n"
           " { Weborf has supercow powers! }\n"
           "  -----------------------------\n"
           "         \\   (__)\n"
           "          \\ /(oo)\\_______\n"
           "             (__)\\       )\\/\\\n"
           "                 ||----w\\|\n"
           "                 ||     ||\n"
           "          \"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\n");
    exit(0);
}

// This function prints the start disclaimer on stdout.
// It wants the command line parameters
void print_start_disclaimer(int argc, char *argv[]) {
    char *host;
    if (weborf_conf.ip==NULL) host="localhost";
    else host=weborf_conf.ip;
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
    if (fork()==0)
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
    char *val=strstr(http_param, parameter); // Locates the requested parameter information

    if (val==NULL) { // No such field
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
    val+=param_len + 2; // Moves the begin of the string to exclude the name of the field

    char *field_end=strstr(val, "\r\n"); // Searches the end of the parameter
    if (field_end==NULL) {
        return false;
    }

    if ((field_end - val + 1) < size) {
        // If the parameter's length is less than buffer's size
        memcpy(buf, val, field_end - val);
    } else { // Parameter string is too long for the buffer
        return false;
    }
    buf[field_end - val]=0; // Ends the string within the destination buffer

    return true;
}

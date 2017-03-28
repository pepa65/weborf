// configuration.c
// Weborf copyright 2010 (GPL3+) Salvo "LtWorf" Tomaselli <tiposchi@tiscali.it>

#include "options.h"

#include <sys/types.h>
#include <unistd.h>
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "configuration.h"
#include "types.h"
#include "utils.h"
#include "cachedir.h"
#include "auth.h"

char *realpath(const char*, char*);
char *get_current_dir_name(void);

weborf_configuration_t weborf_conf={
    .tar_directory=false,
    .zip=false,
    .is_inetd=false,
    .virtual_host=false,
    .exec_script=false,
    .ip=NULL,
    .port=PORT,
#ifdef BASEDIR
    .basedir=BASEDIR,
#else
    .basedir="",
#endif
    .full_basedir=false,
    .hide=false,
    .uid=ROOTUID,
    .gid=ROOTGID,
    .user=NULL,
    .pass=NULL,
    .name=PACKAGE_NAME,
    .sig=PACKAGE_STRING,
    .favlink="",
    .css=CSS,
#ifdef SEND_MIMETYPES
    .send_content_type=false,
#endif
};

// Enables sending mime types in response to GET requests
// or prints an error and exits if the support was not compiled
static void configuration_enable_sending_mime() {
#ifdef SEND_MIMETYPES
    weborf_conf.send_content_type=true;
#else
    fprintf(stderr,"Support for MIME is not available\n");
    exit(19);
#endif
}

// Sets the base dir, making sure that it is really a directory.
static void configuration_set_basedir(char * bd) {
    struct stat stat_buf;
    stat(bd, &stat_buf);

    if (!S_ISDIR(stat_buf.st_mode)) {
        // Not a directory
        printf("%s must be a directory\n",bd);
        exit(1);
    }
    weborf_conf.basedir=realpath(bd, NULL);
//    weborf_conf.basedir=bd;
}

// Sets default CGI configuration, run .php and .py as CGI
static void configuration_set_default_CGI() {
    weborf_conf.cgi_paths.len=4;
    weborf_conf.cgi_paths.data[0]=".php";
    weborf_conf.cgi_paths.data[1]=CGI_PHP;
    weborf_conf.cgi_paths.data[2]=".py";
    weborf_conf.cgi_paths.data[3]=CGI_PY;
    weborf_conf.cgi_paths.data_l[0]=strlen(".php");
    weborf_conf.cgi_paths.data_l[1]=strlen(CGI_PHP);
    weborf_conf.cgi_paths.data_l[2]=strlen(".py");
    weborf_conf.cgi_paths.data_l[3]=strlen(CGI_PY);
}

// Sets the default index file
static void configuration_set_default_index() {
#ifdef INDEX
    weborf_conf.indexes[0]=INDEX;
    weborf_conf.indexes_l=1;
#else
    weborf_conf.indexes[0]=NULL;
    weborf_conf.indexes_l=0;
#endif
}

static void configuration_set_cgi(char *optarg) {
    int i=0;
    weborf_conf.cgi_paths.len=1; // count of indexes
    weborf_conf.cgi_paths.data[0]=optarg; // 1st one points to begin of param
    while (optarg[i++]!=0) { // Reads the string
        if (optarg[i]==',') {
            optarg[i++]=0; // Nulling the comma
            // Increasing counter, making next item point to char after comma
            weborf_conf.cgi_paths.data[weborf_conf.cgi_paths.len++]=&optarg[i];
            if (weborf_conf.cgi_paths.len==MAXINDEXCOUNT) {
                perror("Too many cgis, change MAXINDEXCOUNT in options.h and recompile to allow more");
                exit(6);
            }
        }
    }

    for (i=0; i<weborf_conf.cgi_paths.len; i++) {
        weborf_conf.cgi_paths.data_l[i]=strlen(weborf_conf.cgi_paths.data[i]);
    }
}

static void configuration_set_index_list(char *optarg) {
    // Setting list of indexes
    weborf_conf.indexes_l=0; // count of indexes
    int i=0;
    int l=strlen(optarg);
    while (i < l) {
        weborf_conf.indexes[weborf_conf.indexes_l++]=&optarg[i];
        if (weborf_conf.indexes_l >= MAXINDEXCOUNT) {
            perror("Too many indexes, change MAXINDEXCOUNT in options.h to allow for more");
            exit(6);
        }
        while (optarg[i]!=',' && i<l) i++;
        optarg[i++]=0; // Nulling the comma, increasing i to char after comma
    }
#ifdef THREADDBG
    for (i=0; i<weborf_conf.indexes_l; i++) printf("Index %d: %s\n", i+1, weborf_conf.indexes[i]);
#endif
}

static void configuration_set_virtualhost(char *optarg) {
    weborf_conf.virtual_host=true;

    int i=0;
    char *virtual=optarg; // 1st one points to begin of param

    while (optarg[i++]!=0) { // Reads the string
        if (optarg[i]==',') {
            optarg[i++]=0; // Nulling the comma
            putenv(virtual);
            virtual=&optarg[i];

        }
    }
    putenv(virtual);
}


void configuration_load(int argc, char *argv[]) {
    configuration_set_default_CGI();
    configuration_set_default_index();

    int c; // Identify the readed option
    int option_index=0;

    // Declares options
    struct option long_options[]={
        {"auth", required_argument, 0, 'a'},
        {"basedir", required_argument, 0, 'b'},
        {"cache", required_argument, 0, 'C'},
        {"cgi", required_argument, 0, 'c'},
        {"daemon", no_argument, 0, 'd'},
        {"full", no_argument,0,'F'},
        {"favicon", required_argument, 0, 'f'},
        {"gid", required_argument, 0, 'g'},
        {"hide", no_argument, 0, 'H'},
        {"help", no_argument, 0, 'h'},
        {"index", required_argument, 0, 'I'},
        {"ip", required_argument, 0, 'i'},
        {"caps", no_argument, 0, 'k'},
        {"login", required_argument, 0, 'l'},
        {"moo", no_argument, 0, 'M'},
        {"mime", no_argument,0,'m'},
        {"name", required_argument, 0, 'n'},
        {"pass", required_argument, 0, 'P'},
        {"port", required_argument, 0, 'p'},
        {"css", required_argument, 0, 'S'},
        {"sig", required_argument, 0, 's'},
        {"inetd", no_argument,0,'T'},
        {"tar", no_argument,0,'t'},
        {"user", required_argument, 0, 'U'},
        {"uid", required_argument, 0, 'u'},
        {"virtual", required_argument, 0, 'V'},
        {"version", no_argument, 0, 'v'},
        {"exec", no_argument, 0, 'X'},
        {"zip", no_argument,0,'z'},
        {0, 0, 0, 0}
    };

#ifndef BASEDIR
    weborf_conf.basedir=get_current_dir_name();
#endif
    char *loginfile=NULL;
    char *favicon=NULL;
    while (1) { // Block to read command line

        option_index=0;

        // Reading one option and telling what options are allowed and what
        // needs an argument
        c=getopt_long(argc, argv, "a:b:C:c:dFf:g:HhI:i:kl:Mmn:P:p:S:s:TtU:u:V:vXz",
                long_options, &option_index);

        // If there are no options it continues
        if (c==-1) {
            if (favicon) {
                weborf_conf.favlink=malloc(28+strlen(favicon));
                weborf_conf.favlink[0]=0;
                strcat(weborf_conf.favlink, "<link rel=\"icon\" href=\"");
                strcat(weborf_conf.favlink, favicon);
                strcat(weborf_conf.favlink, "\" />");
            }
            if (loginfile) {
                FILE *f=fopen(loginfile, "r");
                if (f!=NULL) {
                    size_t len=0;
                    if (!weborf_conf.user && getline(&weborf_conf.user, &len, f)!=-1)
                        strtok(weborf_conf.user, "\n");
                    if (!weborf_conf.pass && getline(&weborf_conf.pass, &len, f)!=-1)
                        strtok(weborf_conf.pass, "\n");
                    fclose(f);
                }
            }
            if (weborf_conf.user && weborf_conf.pass)
                weborf_conf.authsock="";
            break;
        }

        switch (c) {
        case 'a':
            auth_set_socket(optarg);
            break;
        case 'b':
            configuration_set_basedir(optarg);
            break;
        case 'C':
            cache_init(optarg);
            break;
        case 'c':
            configuration_set_cgi(optarg);
            break;
        case 'd':
            daemonize();
            break;
        case 'F':
            weborf_conf.full_basedir=true;
            break;
        case 'f':
            favicon=optarg;
            break;
        case 'g':
            weborf_conf.gid=strtol(optarg, NULL, 0);
            break;
        case 'H':
            weborf_conf.hide=true;
            break;
        case 'h':
            help();
            break;
        case 'I':
            configuration_set_index_list(optarg);
            break;
        case 'i':
            weborf_conf.ip=optarg;
            break;
        case 'k':
            print_capabilities();
            break;
        case 'l':
            loginfile=optarg;
            break;
        case 'M':
            moo();
            break;
        case 'm':
            configuration_enable_sending_mime();
            break;
        case 'n':
            weborf_conf.name=optarg;
            break;
        case 'P':
            weborf_conf.pass=optarg;
            break;
        case 'p':
            weborf_conf.port=optarg;
            break;
        case 's':
            weborf_conf.sig=optarg;
            break;
        case 'S':
            weborf_conf.css=malloc(strlen(weborf_conf.css)+strlen(optarg)+1);
            strcat(weborf_conf.css, optarg);
            break;
        case 'T':
            weborf_conf.is_inetd=true;
            break;
        case 't':
            weborf_conf.tar_directory=true;
            break;
        case 'U':
            weborf_conf.user=optarg;
            break;
        case 'u':
            weborf_conf.uid=strtol(optarg, NULL, 0);
            break;
        case 'V':
            configuration_set_virtualhost(optarg);
            break;
        case 'v':   //Show version and exit
            version();
            break;
        case 'X':
            weborf_conf.exec_script=true;
            break;
        case 'z':
            weborf_conf.zip=true;
            break;
        default:
            exit(19);
        }

    }
}

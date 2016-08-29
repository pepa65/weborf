/*
Weborf
Copyright (C) 2010  Salvo "LtWorf" Tomaselli

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
#include "options.h"

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

weborf_configuration_t weborf_conf = {
    .tar_directory = false,
    .zip = false,
    .is_inetd = false,
    .virtual_host = false,
    .exec_script = true,
#ifdef IPV6
    .ipv6 = false,
#endif
    .ip = NULL,
    .port = PORT,
    .basedir=BASEDIR,
    .uid = ROOTUID,
    .user = NULL,
    .pass = NULL,
    .name = NAME,
    .sig = SIGNATURE,
    .favlink = "",
    .css = CSS,

#ifdef SEND_MIMETYPES
    .send_content_type = false,
#endif
};

/**
 * Enables sending mime types in response to GET requests
 * or prints an error and exits if the support was not
 * compiled
 * */
static void configuration_enable_sending_mime() {
#ifdef SEND_MIMETYPES
    weborf_conf.send_content_type=true;
#else
    fprintf(stderr,"Support for MIME is not available\n");
    exit(19);
#endif
}


/**
Sets the base dir, making sure that it is really a directory.
 */
static void configuration_set_basedir(char * bd) {
    struct stat stat_buf;
    stat(bd, &stat_buf);

    if (!S_ISDIR(stat_buf.st_mode)) {
        //Not a directory
        printf("%s must be a directory\n",bd);
        exit(1);
    }
    weborf_conf.basedir = bd;
}

/**
 * Sets default CGI configuration,
 * run .php and .py as CGI
 * */
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

/**
 * Sets the default index file
 * */
static void configuration_set_default_index() {
    weborf_conf.indexes[0] = INDEX;
    weborf_conf.indexes_l = 1;
}

static void configuration_set_cgi(char *optarg) {
    int i = 0;
    weborf_conf.cgi_paths.len = 1; //count of indexes
    weborf_conf.cgi_paths.data[0] = optarg; //1st one points to begin of param
    while (optarg[i++] != 0) { //Reads the string
        if (optarg[i] == ',') {
            optarg[i++] = 0; //Nulling the comma
            //Increasing counter and making next item point to char after the comma
            weborf_conf.cgi_paths.data[weborf_conf.cgi_paths.len++] = &optarg[i];
            if (weborf_conf.cgi_paths.len == MAXINDEXCOUNT) {
                perror("Too much cgis, change MAXINDEXCOUNT in options.h to allow more");
                exit(6);
            }
        }
    }

    for (i=0; i<weborf_conf.cgi_paths.len; i++) {
        weborf_conf.cgi_paths.data_l[i]=strlen(weborf_conf.cgi_paths.data[i]);
    }

}

static void configuration_set_index_list(char *optarg) { //Setting list of indexes
    int i = 0;
    weborf_conf.indexes_l = 1; //count of indexes
    weborf_conf.indexes[0] = optarg; //1st one points to begin of param
    while (optarg[i++] != 0) { //Reads the string

        if (optarg[i] == ',') {
            optarg[i++] = 0; //Nulling the comma
            //Increasing counter and making next item point to char after the comma
            weborf_conf.indexes[weborf_conf.indexes_l++] = &optarg[i];
            if (weborf_conf.indexes_l == MAXINDEXCOUNT) {
                perror("Too much indexes, change MAXINDEXCOUNT in options.h to allow more");
                exit(6);
            }
        }
    }

}

static void configuration_set_virtualhost(char *optarg) {
    weborf_conf.virtual_host = true;

    int i = 0;
    char *virtual = optarg; //1st one points to begin of param

    while (optarg[i++] != 0) { //Reads the string
        if (optarg[i] == ',') {
            optarg[i++] = 0; //Nulling the comma
            putenv(virtual);
            virtual = &optarg[i];

        }
    }
    putenv(virtual);
}


void configuration_load(int argc, char *argv[]) {
    configuration_set_default_CGI();
    configuration_set_default_index();


    int c; //Identify the readed option
    int option_index = 0;

    //Declares options
    struct option long_options[] = {
        {"version", no_argument, 0, 'v'},
        {"caps", no_argument, 0, 'k'},
        {"help", no_argument, 0, 'h'},
        {"port", required_argument, 0, 'p'},
        {"ip", required_argument, 0, 'i'},
#ifdef IPV6
        {"ipv6", no_argument, 0, 'e'},
#endif
        {"uid", required_argument, 0, 'u'},
        {"daemon", no_argument, 0, 'd'},
        {"basedir", required_argument, 0, 'b'},
        {"index", required_argument, 0, 'I'},
        {"auth", required_argument, 0, 'a'},
        {"virtual", required_argument, 0, 'V'},
        {"moo", no_argument, 0, 'M'},
        {"noexec", no_argument, 0, 'x'},
        {"cgi", required_argument, 0, 'c'},
        {"cache", required_argument, 0, 'C'},
        {"mime", no_argument,0,'m'},
        {"inetd", no_argument,0,'T'},
        {"tar", no_argument,0,'t'},
        {"zip", no_argument,0,'z'},
        {"user", required_argument, 0, 'U'},
        {"pass", required_argument, 0, 'P'},
        {"login", required_argument, 0, 'l'},
        {"name", required_argument, 0, 'n'},
        {"sig", required_argument, 0, 's'},
        {"favicon", required_argument, 0, 'f'},
        {"css", required_argument, 0, 'S'},
        {0, 0, 0, 0}
    };


    char *loginfile = NULL;
    char *favicon = NULL;
    while (1) { //Block to read command line

        option_index = 0;

        //Reading one option and telling what options are allowed and what needs an argument
#ifdef IPV6
        c = getopt_long(argc, argv, "ktzTMmvhp:i:eI:u:dxb:a:V:c:C:U:P:l:n:s:f:S:", long_options,
#else
        c = getopt_long(argc, argv, "ktzTMmvhp:i:I:u:dxb:a:V:c:C:U:P:l:n:s:f:S:", long_options,
#endif
                        &option_index);

        //If there are no options it continues
        if (c == -1) {
            if (favicon) {
                weborf_conf.favlink = malloc(28+strlen(favicon));
                weborf_conf.favlink[0] = 0;
                strcat(weborf_conf.favlink, "<link rel=\"icon\" href=\"");
                strcat(weborf_conf.favlink, favicon);
                strcat(weborf_conf.favlink, "\" />");
            }
            if (loginfile) {
                FILE *f = fopen(loginfile, "r");
                if (f != NULL) {
                    size_t len = 0;
                    if (!weborf_conf.user && getline(&weborf_conf.user, &len, f) != -1)
                        strtok(weborf_conf.user, "\n");
                    if (!weborf_conf.pass && getline(&weborf_conf.pass, &len, f) != -1)
                        strtok(weborf_conf.pass, "\n");
                    fclose(f);
                }
            }
            if (weborf_conf.user && weborf_conf.pass)
                weborf_conf.authsock = "";
            break;
        }

        switch (c) {
        case 'k':
            print_capabilities();
            break;
        case 't':
            weborf_conf.tar_directory=true;
            break;
        case 'z':
            weborf_conf.zip=true;
            break;
        case 'T':
            weborf_conf.is_inetd=true;
            break;
        case 'C':
            cache_init(optarg);
            break;
        case 'm':
            configuration_enable_sending_mime();
            break;
        case 'c':
            configuration_set_cgi(optarg);
            break;
        case 'V':
            configuration_set_virtualhost(optarg);
            break;
        case 'I':
            configuration_set_index_list(optarg);
            break;
        case 'b':
            configuration_set_basedir(optarg);
            break;
        case 'x':
            weborf_conf.exec_script = false;
            break;
        case 'v':   //Show version and exit
            version();
            break;
        case 'h':   //Show help and exit
            help();
            break;
        case 'p':
            weborf_conf.port = optarg;
            break;
        case 'i':
            weborf_conf.ip = optarg;
            break;
#ifdef IPV6
        case 'e':
            weborf_conf.ipv6 = true;
            break;
#endif
        case 'u':
            weborf_conf.uid = strtol(optarg, NULL, 0);
            break;
        case 'd':
            daemonize();
            break;
        case 'a':
            auth_set_socket(optarg);
            break;
        case 'M':
            moo();
            break;
        case 'U':
            weborf_conf.user = optarg;
            break;
        case 'P':
            weborf_conf.pass = optarg;
            break;
        case 'l':
            loginfile = optarg;
            break;
        case 'n':
            weborf_conf.name = optarg;
            break;
        case 's':
            weborf_conf.sig = optarg;
            break;
        case 'f':
            favicon = optarg;
            break;
        case 'S':
            weborf_conf.css = malloc(strlen(weborf_conf.css)+strlen(optarg)+1);
            strcat(weborf_conf.css, optarg);
            break;
        default:
            exit(19);
        }

    }
}

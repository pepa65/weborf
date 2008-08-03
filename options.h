/*
Weborf
Copyright (C) 2007  Salvo "LtWorf" Tomaselli

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

#ifndef WEBORF_OPTIONS_H
#define WEBORF_OPTIONS_H

#define VERSION "0.9"

//----------System
//Uid for superuser
#define ROOTUID 0

//----------Network
//Queue for connect requests
#define MAXQ 512
//Default port
#define PORT "8080"
//Delete the following line to use IPv4 instead.
#define IPV6
//-----------Threads
//Max threads
#define MAXTHREAD 200
//Thread started when free threads are low and when starting
#define INITIALTHREAD 12
//If there are LOWTHREAD or less free threads, will start some new ones
#define LOWTHREAD 3
//If there are more than MAXFREETHREAD, one will be closed
#define MAXFREETHREAD 12
//Polling frequence
#define THREADCONTROL 60

//------------Server
#define INDEX "index.html"
#define BASEDIR "/var/www"

//------------Buffers
#define INBUFFER 2048
#define FILEBUF 50000
//Maximum size for a page generated by a script or internally
#define MAXSCRIPTOUT 1024000
//Buffer for headers
#define HEADBUF 500
//Max size for password
#define PWDLIMIT 300
//Buffer to contain the string representation of an integer
#define NBUFFER 15

//-------------LIMITS
#define POST_MAX_SIZE 20480

//-------------HTML
#define HTMLHEAD "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.01 Transitional//EN\"><html><head><title>Weborf</title></head><body>"
#define HTMLFOOT "<p>Generated by Weborf</p></body></html>"


//-------------Debug options
//#define THREADDBG
//#define SOCKETDBG
//#define SERVERDBG
//#define SENDINGDBG
#define REQUESTDBG
#endif

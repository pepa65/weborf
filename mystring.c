// mystring.c
// Weborf copyright 2007 (GPL3+) Salvo "LtWorf" Tomaselli <tiposchi@tiscali.it>

#include "options.h"
#include "mystring.h"

// This function converts a string to upper case
void strToUpper(char *str) {
    int i = -1;
    while (str[++i]) {
        str[i] = toupper(str[i]);
    }
}

// This function splits the page name from the GET params.
// It also sets the value for the page_len field
void split_get_params(connection_t* connection_prop) {
    int i=0;
    // Make sure we keep all '?' in filenames
    while (connection_prop->page[i]!=0) i++;
    connection_prop->get_params=NULL;
    while (i>0) {
        i--;
        if (connection_prop->page[i]=='?') {
            connection_prop->page[i]=0;
            connection_prop->get_params=&connection_prop->page[i+1];
            break;
        }
    }
    connection_prop->page_len=i;
}
///void split_get_params(connection_t* connection_prop) {
///    char *separator=strstr(connection_prop->page,"?");
///
///    if (separator==NULL) {
///        connection_prop->get_params=NULL;
///        connection_prop->page_len=strlen(connection_prop->page);
///
///    } else {
///
///        separator[0]=0;
///        connection_prop->get_params=separator+1;
///        connection_prop->page_len=separator-connection_prop->page;
///    }
///}


// Replaces escape sequences in the form %HEXCODE with the correct char
// This is used for URLs, after the transformation the URL will probably
// represent a file that exists on filesystem.
// Since after this replace the string will be unchanged or shorter, no
// additional buffer will be needed. This function is in-place, doesn't
// create copies but changes the original string.
void replaceEscape(char *string) {
    char e_seq[3];
    e_seq[2] = 0;

    // Parses the string
    while ((string=strstr(string,"%"))!=NULL) {
        e_seq[0] = string[1];
        e_seq[1] = string[2];

        delChar(string, 0, 2); // Deletes 2 chars from the url

        // Replaces the 3rd character with the char corresponding to the escape
        string[0] = strtol(e_seq, NULL, 16);
    }
}

// This function replaces, within the string string, the substring substr with
// the char with. This function is in-place, doesn't create copies but changes
// the original string.
void strReplace(char *string, char *substr, char with) {
    char *pointer;
    int substrlen = strlen(substr);

    while ((pointer = strstr(string, substr)) != NULL) {
        delChar(pointer, 0, substrlen - 1);
        pointer[0] = with;
        string=pointer;
    }
}

// This function deletes n chars from the string, starting from the position
// pos. In case deleting of more chars than the string's len itself, the
// string will be returned unchanged.
// This function doesn't create copies but changes the original string.
void delChar(char *string, int pos, int n) {
    size_t l=strlen(string+pos);

    if (l<n) return;
    l-=n;
    memmove(string+pos,string+pos+n,l);
    string[l]=0;

    return;
}

// Returns true if str ends with end
// str String to compare
// end second string
// size of str
// size of end
// If str ends with end, true is returned, false otherwise
bool endsWith(char *str, char *end,ssize_t len_str,ssize_t len_end) {
    return strcmp(str+len_str-len_end,end)==0;
}

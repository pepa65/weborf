// py_wrapper.c
// Weborf copyright 2009 (GPL3+) Salvo "LtWorf" Tomaselli <tiposchi@tiscali.it>

#include <stdlib.h>
#include <unistd.h>

int main() {
    execl("/usr/bin/python","python",getenv("SCRIPT_FILENAME"),NULL);
    exit(1); //Unable to exec the wanted stuff
}

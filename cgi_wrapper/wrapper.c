// wrapper.c
// Weborf copyright 2009 (GPL3+) Salvo "LtWorf" Tomaselli <tiposchi@tiscali.it>

#include <stdlib.h>
#include <unistd.h>

int main() {
    execl(getenv("SCRIPT_FILENAME"),getenv("SCRIPT_FILENAME"),NULL);
    exit(1); //Unable to exec the wanted stuff
}

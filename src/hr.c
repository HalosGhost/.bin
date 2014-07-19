/**************************************************\
* A port of LuRsT's hr to C                        *
* Sam Stuewe (C) 2014 Licensed under the terms of  *
* the GNU Public License version 2                 *
\**************************************************/

// Libraries //
#include <stdio.h>
#include <string.h>
#include <sys/ioctl.h>

// Main Function //
int 
main (int argc, char * argv []) {

    struct winsize w;
    ioctl(0, TIOCGWINSZ, &w);
    int COLS = (w.ws_col <= 0 ? 80 : w.ws_col);

    if ( argc <= 1 ) {
        for ( int i = 0; i < COLS; i ++ ) {
            printf("#");
        } printf("\n");
    } else {
        for ( int i = 1; i < argc; i ++ ) {
            for ( int j = 0; j < (COLS/strlen(argv[i])); j ++ ) {
                printf(argv[i]);
            } printf("\n");
        }
    } return 0;
}

// vim: set tabstop=4 shiftwidth=4 expandtab:

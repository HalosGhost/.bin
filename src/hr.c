/**************************************************\
* A port of LuRsT's hr to C                        *
* Sam Stuewe (C) 2014 Licensed under the terms of  *
* the GNU Public License version 2                 *
\**************************************************/

// Libraries //
#include <stdint.h>    // int32_t, uint16_6, uint64_t
#include <sys/ioctl.h> // struct winsize, ioctl()
#include <stdio.h>     // putchar()
#include <string.h>    // strlen()
#include <stdlib.h>    // EXIT_SUCCESS

// Main Function //
int32_t
main (int32_t argc, char * argv []) {

    struct winsize w;
    ioctl(0, TIOCGWINSZ, &w);
    uint16_t COLS = (w.ws_col <= 0 ? 80 : w.ws_col);

    if ( argc <= 1 ) {
        for ( uint16_t i = 0; i < COLS; i ++ ) { putchar('#'); } putchar('\n');
    } else {
        for ( int32_t i = 1; i < argc; i ++ ) {
            uint64_t len = strlen(argv[i]);
            for ( uint16_t j = 0; j < COLS; j ++ ) {
                putchar(argv[i][j%len]);
            } putchar('\n');
        }
    } return EXIT_SUCCESS;
}

// vim: set ts=4 sw=4 et:

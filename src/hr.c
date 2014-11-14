/**************************************************\
* A port of LuRsT's hr to C                        *
* Sam Stuewe (C) 2014 Licensed under the terms of  *
* the GNU Public License version 2                 *
\**************************************************/

// Libraries //
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <sys/ioctl.h>

// Main Function //
int32_t
main (int32_t argc, char * argv []) {

    struct winsize w;
    ioctl(0, TIOCGWINSZ, &w);
    uint16_t COLS = (w.ws_col <= 0 ? 80 : w.ws_col);

    if ( argc <= 1 ) {
        for ( uint16_t i = 0; i < COLS; i ++ ) { printf("#"); } puts("");
    } else {
        for ( int32_t i = 1; i < argc; i ++ ) {
            uint64_t len = strlen(argv[i]);
            for ( uint16_t j = 0; j < COLS; j ++ ) {
                printf("%c", argv[i][j%len]);
            } puts("");
        }
    } return 0;
}

// vim: set tabstop=4 shiftwidth=4 expandtab:

/**************************************************\
* Display ANSI Color Schemes with Space Invaders   *
* Inspired by a bash script by pfh and lolilolicon *
* Sam Stuewe (C) 2014 Licensed under the terms of  *
* the GNU Public License version 2                 *
\**************************************************/

// Includes //
#include <stdio.h>     // printf()
#include <stdlib.h>    // EXIT_SUCCESS
#include "ansicolor.h" // ANSI escapes

// Main Function //
signed
main (void) {

    /* Top Row Invaders */
    printf(ANSI_FG_BLACK   "  ▀▄   ▄▀   "
           ANSI_FG_RED     "  ▄▄▄████▄▄▄  "
           ANSI_FG_GREEN   "   ▄██▄   "
           ANSI_FG_YELLOW  "   ▀▄   ▄▀   "
           ANSI_FG_BLUE    "  ▄▄▄████▄▄▄  "
           ANSI_FG_MAGENTA "   ▄██▄   "
           ANSI_FG_CYAN    "   ▀▄   ▄▀   \n"

           ANSI_FG_BLACK   " ▄█▀███▀█▄  "
           ANSI_FG_RED     " ███▀▀██▀▀███ "
           ANSI_FG_GREEN   " ▄█▀██▀█▄ "
           ANSI_FG_YELLOW  "  ▄█▀███▀█▄  "
           ANSI_FG_BLUE    " ███▀▀██▀▀███ "
           ANSI_FG_MAGENTA " ▄█▀██▀█▄ "
           ANSI_FG_CYAN    "  ▄█▀███▀█▄  \n"

           ANSI_FG_BLACK   "█▀███████▀█ "
           ANSI_FG_RED     " ▀▀███▀▀███▀▀ "
           ANSI_FG_GREEN   " ▀█▀██▀█▀ "
           ANSI_FG_YELLOW  " █▀███████▀█ "
           ANSI_FG_BLUE    " ▀▀███▀▀███▀▀ "
           ANSI_FG_MAGENTA " ▀█▀██▀█▀ "
           ANSI_FG_CYAN    " █▀███████▀█ \n"

           ANSI_FG_BLACK   "▀ ▀▄▄ ▄▄▀ ▀ "
           ANSI_FG_RED     "  ▀█▄ ▀▀ ▄█▀  "
           ANSI_FG_GREEN   " ▀▄    ▄▀ "
           ANSI_FG_YELLOW  " ▀ ▀▄▄ ▄▄▀ ▀ "
           ANSI_FG_BLUE    "  ▀█▄ ▀▀ ▄█▀  "
           ANSI_FG_MAGENTA " ▀▄    ▄▀ "
           ANSI_FG_CYAN    " ▀ ▀▄▄ ▄▄▀ ▀ \n"
           ANSI_BOLD_ON    "\n"

    /* Bottom Row Invaders */
           ANSI_FG_BLACK   "▄ ▀▄   ▄▀ ▄ "
           ANSI_FG_RED     "  ▄▄▄████▄▄▄  "
           ANSI_FG_GREEN   "   ▄██▄   "
           ANSI_FG_YELLOW  " ▄ ▀▄   ▄▀ ▄ "
           ANSI_FG_BLUE    "  ▄▄▄████▄▄▄  "
           ANSI_FG_MAGENTA "   ▄██▄   "
           ANSI_FG_CYAN    " ▄ ▀▄   ▄▀ ▄ \n"

           ANSI_FG_BLACK   "█▄█▀███▀█▄█ "
           ANSI_FG_RED     " ███▀▀██▀▀███ "
           ANSI_FG_GREEN   " ▄█▀██▀█▄ "
           ANSI_FG_YELLOW  " █▄█▀███▀█▄█ "
           ANSI_FG_BLUE    " ███▀▀██▀▀███ "
           ANSI_FG_MAGENTA " ▄█▀██▀█▄ "
           ANSI_FG_CYAN    " █▄█▀███▀█▄█ \n"

           ANSI_FG_BLACK   "▀█████████▀ "
           ANSI_FG_RED     " ▀▀▀██▀▀██▀▀▀ "
           ANSI_FG_GREEN   " ▀▀█▀▀█▀▀ "
           ANSI_FG_YELLOW  " ▀█████████▀ "
           ANSI_FG_BLUE    " ▀▀▀██▀▀██▀▀▀ "
           ANSI_FG_MAGENTA " ▀▀█▀▀█▀▀ "
           ANSI_FG_CYAN    " ▀█████████▀ \n"

           ANSI_FG_BLACK   " ▄▀     ▀▄  "
           ANSI_FG_RED     " ▄▄▀▀ ▀▀ ▀▀▄▄ "
           ANSI_FG_GREEN   " ▄▀▄▀▀▄▀▄ "
           ANSI_FG_YELLOW  "  ▄▀     ▀▄  "
           ANSI_FG_BLUE    " ▄▄▀▀ ▀▀ ▀▀▄▄ "
           ANSI_FG_MAGENTA " ▄▀▄▀▀▄▀▄ "
           ANSI_FG_CYAN    "  ▄▀     ▀▄  \n"

           ANSI_RESET      "\n"

    /* Defender's Tank */
           "\t\t\t\t\t  ▌\n\n"
           "\t\t\t\t\t▌\n\n"
           "\t\t\t\t      ▄█▄\n"
           "\t\t\t\t  ▄█████████▄\n"
           "\t\t\t\t  ▀▀▀▀▀▀▀▀▀▀▀\n");

    return EXIT_SUCCESS;
}

// vim: set ts=4 sw=4 et:

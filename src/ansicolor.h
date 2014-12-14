/*************************************************\
* Shortuct Macros for ANSI text manipulation      *
* Sam Stuewe (C) 2014 Licensed under the terms of *
* the GNU Public License version 2                *
\*************************************************/

#ifndef ANSICOLOR_H
#define ANSICOLOR_H

// Definitions //
#define ANSI_RESET         "\x1b[0m"
#define ANSI_BOLD_ON       "\x1b[1m"
#define ANSI_BOLD_OFF      "\x1b[22m"

#define ANSI_FG_BLACK      "\x1b[30m"
#define ANSI_FG_RED        "\x1b[31m"
#define ANSI_FG_GREEN      "\x1b[32m"
#define ANSI_FG_YELLOW     "\x1b[33m"
#define ANSI_FG_BLUE       "\x1b[34m"
#define ANSI_FG_MAGENTA    "\x1b[35m"
#define ANSI_FG_CYAN       "\x1b[36m"
#define ANSI_FG_WHITE      "\x1b[37m"
#define ANSI_FG_DEFAULT    "\x1b[39m"

#define ANSI_BG_BLACK      "\x1b[40m"
#define ANSI_BG_RED        "\x1b[41m"
#define ANSI_BG_GREEN      "\x1b[42m"
#define ANSI_BG_YELLOW     "\x1b[43m"
#define ANSI_BG_BLUE       "\x1b[44m"
#define ANSI_BG_MAGENTA    "\x1b[45m"
#define ANSI_BG_CYAN       "\x1b[46m"
#define ANSI_BG_WHITE      "\x1b[47m"
#define ANSI_BG_DEFAULT    "\x1b[49m"

#endif // ANSICOLOR_H
// vim: set ts=4 sw=4 et:

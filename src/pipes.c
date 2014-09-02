/*******************************************************\
* A simple screensaver inspired by pipesX by Yu-Jie Lin *
* Sam Stuewe (C) 2014 Licensed under the terms of       *
* the GNU Public License version 2                      *
\*******************************************************/

// Libraries //
#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <getopt.h>

// Forward Declarations //
#define PIPE_LIMIT 32

void
usage (int status) {

    fputs("Usage: pipes [options]\n\n"
          "Options:\n"
          "-c, --color\tpaint with color\n"
          "-C, --nocolor\tpaint without color\n"
          "-h, --help\tprint this help and exit\n"
          "-r, --reset=X\tclear screen after X characters painted\n"
          "-p, --pipes\tnumber of pipes\n",
          (status == 0 ? stdout : stderr));
    exit(status);
}

// Main Function //
int 
main (int argc, char * argv []) {

    struct winsize w;
    ioctl(0, TIOCGWINSZ, &w);

    int cols = (w.ws_col <= 0 ? 80 : w.ws_col);
    int rows = (w.ws_row <= 0 ? 50 : w.ws_row);

    unsigned char flag_color = 1;
    unsigned char switch_pipes = 1;
    unsigned int switch_reset = (rows * cols) - 20;

    if ( argc <= 1 ) {
        usage(1);
    } else {

        static struct option os [] = {                   
            /* Flags */                                  
            { "help",      no_argument,         0, 'h' }, 
            { "color",     no_argument,         0, 'c' },
            { "nocolor",   no_argument,         0, 'C' },
            /* Switches */                               
            { "reset",     required_argument,   0, 'r' }, 
            { "pipes",     required_argument,   0, 'p' },         
        };                                               

        for ( int c = 0, i = 0; c != -1;
              c = getopt_long(argc, argv, "hcCr:p:", os, &i) ) {

            switch ( c ) {
                case 'h':
                    usage(0);
                    break;

                case 'c':
                    flag_color = 1;
                    break;

                case 'C':
                    flag_color = 0;
                    break;

                case 'r':
                    sscanf(optarg, "%u\n", &switch_reset);
                    break;

                case 'p':
                    sscanf(optarg, "%c\n", &switch_pipes);
                    break;

            }
        }
    }

	if ( flag_color ) {
		printf("printing with color\n");
	}

    // run animation
    return 0;
}

// vim: set tabstop=4 shiftwidth=4 expandtab:

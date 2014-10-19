/*****************************\
* Get external IP address     *
* Author: Sam Stuewe (C) 2014 *
* License: GPLv3              *
\*****************************/

// Includes //
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <curl/curl.h>

// Forward Definitions //
static void
usage (int status) __attribute__((noreturn));

static void
get_ip (long mode);

// Main Function //
int main (int argc, char * argv []) {
    char ip_mode = 0;

    static struct option os [] = {
        /* Flags */
        { "all",  no_argument,   0, 'a' },
        { "help", no_argument,   0, 'h' },
        { "ipv4", no_argument,   0, '4' },
        { "ipv6", no_argument,   0, '6' },
		{ 0,	  0,			 0,	0   },
    };

    for ( int c = 0, i = 0; c != -1; 
		  c = getopt_long(argc, argv, "ah46", os, &i) ) {
        switch ( c ) {
            case 'h':
                usage(0);

            case 'a':
                ip_mode = 1;
                break;

            case '4':
                ip_mode = 4;
                break;

            case '6':
                ip_mode = 6;
                break;
        }
    }

    curl_global_init(CURL_GLOBAL_ALL);
    switch ( ip_mode ) {
        case 0:
            get_ip(0);
            break;

        case 1:
            get_ip(CURL_IPRESOLVE_V4);
            get_ip(CURL_IPRESOLVE_V6);
            break;

        case 4:
            get_ip(CURL_IPRESOLVE_V4);
            break;

        case 6:
            get_ip(CURL_IPRESOLVE_V6);
            break;
    }

    curl_global_cleanup();
    return 0;
}

// Function Definitions //
static void usage (int status) {
    fputs("Usage: lsip [options]\n\n"
          "Options:\n"
          "  -a, --all\tcheck and print all addresses\n"
          "  -h, --help\tprint this message and exit\n"
          "  -4, --ipv4\tcheck only external ipv4 address\n"
          "  -6, --ipv6\tcheck only external ipv6 address\n"
          ,(status == 0 ? stdout : stderr));
    exit(status);
}

static void get_ip (long mode) {
    CURL * handle = curl_easy_init();
    CURLcode res;

    if ( !handle ) { fputs("Failed to get CURL handle", stderr); };

    curl_easy_setopt(handle, CURLOPT_URL, "https://icanhazip.com");
    curl_easy_setopt(handle, CURLOPT_FOLLOWLOCATION, 1);
    if ( mode ) { curl_easy_setopt(handle, CURLOPT_IPRESOLVE, mode); };

    res = curl_easy_perform(handle);

    if ( res != CURLE_OK ) {
        fprintf(stderr, "Failed to fetch address: %s\n", 
                curl_easy_strerror(res));
    }

    curl_easy_cleanup(handle);
}

// vim: set tabstop=4 shiftwidth=4 expandtab:

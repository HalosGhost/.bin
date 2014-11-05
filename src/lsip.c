/*****************************\
* Get external IP address     *
* Author: Sam Stuewe (C) 2014 *
* License: GPLv3              *
\*****************************/

// Includes //
#include <stdio.h>
#include <stdlib.h>
#include <argp.h>
#include <curl/curl.h>

// Forward Definitions //
const char * argp_program_version = "lsip 2.0.0";
const char * argp_program_bug_address = "<halosghost@archlinux.info>";
static char * doc = "lsip -- a simple tool to check external IP addresses";

static int
get_ip (long mode);

static error_t
parse_opt (int key, char * arg, struct argp_state * state);

// Main Function //
int 
main (int argc, char * argv []) {

    struct argp_option os [] = {
        { 0,      0,   0, 0, "Options:",          -1 },
        { "ipv4", '4', 0, 0, "check ipv4",         0 },
        { "ipv6", '6', 0, 0, "check ipv6",         0 },
        { "all",  'a', 0, 0, "equivalent to -46",  0 },
        { 0,      0,   0, 0, 0,                    0 }
    };

    struct argp argp = { os, parse_opt, "", doc, 0, 0, 0 };

    return (argc == 1 ? get_ip(0) : argp_parse(&argp, argc, argv, 0, 0, 0));
}

// Function Definitions //
static int
get_ip (long mode) {

    CURL * handle = curl_easy_init();
    CURLcode res;

    if ( !handle ) { 
        fputs("Failed to get CURL handle", stderr); 
        return 1;
    }

    curl_easy_setopt(handle, CURLOPT_URL, "https://icanhazip.com");
    curl_easy_setopt(handle, CURLOPT_FOLLOWLOCATION, 1);
    if ( mode ) { curl_easy_setopt(handle, CURLOPT_IPRESOLVE, mode); };

    res = curl_easy_perform(handle);

    if ( res != CURLE_OK ) {
        fprintf(stderr, "Failed to fetch address: %s\n", 
                curl_easy_strerror(res));
        return 2;
    }

    curl_easy_cleanup(handle);
    return 0;
}

static error_t
parse_opt (int key, char * arg, struct argp_state * state) {

    switch ( key ) {
        case '4':
            get_ip(CURL_IPRESOLVE_V4);
            break;

        case '6':
            get_ip(CURL_IPRESOLVE_V6);
            break;

        case 'a':
            get_ip(CURL_IPRESOLVE_V4);
            get_ip(CURL_IPRESOLVE_V6);
            break;

        default:
            return ARGP_ERR_UNKNOWN;
    } return 0;
}

// vim: set tabstop=4 shiftwidth=4 expandtab:

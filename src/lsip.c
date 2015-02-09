/*****************************\
* Get external IP address     *
* Author: Sam Stuewe (C) 2014 *
* License: GPLv3              *
\*****************************/

// Includes //
#include <stdio.h>
#include <stdlib.h>
#include <argp.h>
#include <stdint.h>
#include <curl/curl.h>

// Forward Definitions //
const char * argp_program_version = "lsip 1.1.0";
const char * argp_program_bug_address = "<halosghost@archlinux.info>";
static char * doc = "lsip -- a simple tool to check external IP addresses";

static inline int32_t
get_ip (int64_t);

static error_t
parse_opt (int32_t, char *, struct argp_state *);

// Main Function //
int32_t
main (int32_t argc, char * argv []) {

    struct argp_option os [] = {
        { 0,      0,   0, 0, "Options:",          -1 },
        { "ipv4", '4', 0, 0, "Check ipv4",         0 },
        { "ipv6", '6', 0, 0, "Check ipv6",         0 },
        { "all",  'a', 0, 0, "Equivalent to -46",  0 },
        { 0,      0,   0, 0, 0,                    0 }
    };

    struct argp argp = { os, parse_opt, "", doc, 0, 0, 0 };

    return (argc == 1 ? get_ip(0) : argp_parse(&argp, argc, argv, 0, 0, 0));
}

// Function Definitions //
static int32_t
get_ip (int64_t mode) {

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
parse_opt (int32_t key, char * arg, struct argp_state * state) {

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

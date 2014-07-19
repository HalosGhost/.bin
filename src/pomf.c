/*****************************\
* Upload files to pomf.se     *
* Author: Sam Stuewe (C) 2014 *
* License: GPLv3              *
\*****************************/

// Libraries //
#include <stdlib.h>
#include <stdio.h>
#include <curl/curl.h>

// TODO: Add getopt support for progress bars/verbosity
// TODO: Add json parsing and curl WRITE_FUNCTION for correctly 
//       returning the URL

// Forward Declarations //
void
usage (int status) {
    fputs("Usage: pomf <file>\n",
          (status == 0 ? stdout : stderr));
    exit(status);
}

// Main Function //
int
main (int argc, char * argv []) {
    if ( argc <= 1 ) { 
        usage(1);
    } else if ( argv[1][0] == '-' ) {
        if ( argv[1][1] == 'h' || argv[1][2] == 'h' ) {
            usage(0);
        }
    }

    curl_global_init(CURL_GLOBAL_ALL);
    CURL * handle = curl_easy_init();
    CURLcode res;

    if ( !handle ) { 
        fputs("Failed to get CURL handle", stderr); 
        exit(1);
    }

    struct curl_httppost * post = NULL;
    struct curl_httppost * postend = NULL;

    curl_formadd(&post, &postend,
                 CURLFORM_COPYNAME, "files[]",
                 CURLFORM_FILE, argv[1],
                 CURLFORM_END);

    curl_easy_setopt(handle, CURLOPT_URL, "http://pomf.se/upload.php");
    curl_easy_setopt(handle, CURLOPT_NOPROGRESS, 1L);  
    curl_easy_setopt(handle, CURLOPT_FAILONERROR, 1L);
    curl_easy_setopt(handle, CURLOPT_HTTPPOST, post);
    curl_easy_setopt(handle, CURLOPT_MAXREDIRS, 50L);
    curl_easy_setopt(handle, CURLOPT_TCP_KEEPALIVE, 1L);

    res = curl_easy_perform(handle);
    curl_easy_cleanup(handle);
    handle = NULL;
    curl_formfree(post);
    post = NULL;
    curl_global_cleanup();

    return (int )res;
}

// vim: set ts=4 sw=4 et:

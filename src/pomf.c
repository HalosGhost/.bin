/*****************************\
* Upload files to pomf.se     *
* Author: Sam Stuewe (C) 2014 *
* License: GPLv3              *
\*****************************/

// Libraries //
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <curl/curl.h>

/**
 * TODO: Getopt support?
 * TODO: stdin support
 */

// Forward Declarations //
#define BUFFER_SIZE 256
#define MAX_FILE_SIZE 52428800

void
usage (int status) {

    fputs("Usage: pomf <file>\n",
          (status == 0 ? stdout : stderr));
    exit(status);
}

static
size_t
write_function (char * buffer, size_t size, size_t nmemb, char * userp) {

    char * string = userp;
    size_t length = size * nmemb;
    strncat(string, buffer, length);
    return length;
}

// Main Function //
int
main (int argc, char * argv []) {

    //FILE * input;

    if ( argc <= 1 ) { 
        usage(1);
    } else if ( argv[1][0] == '-' ) {
        //if ( strlen(argv[1]) == 1 ) {
        //  input = stdin;
        //}
        if ( argv[1][1] == 'h' || argv[1][2] == 'h' ) {
            usage(0);
        }
    }

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

    char written_result [BUFFER_SIZE] = { '\0' };

    curl_easy_setopt(handle, CURLOPT_URL, "http://pomf.se/upload.php");
    curl_easy_setopt(handle, CURLOPT_NOPROGRESS, 1L);  
    curl_easy_setopt(handle, CURLOPT_FAILONERROR, 1L);
    curl_easy_setopt(handle, CURLOPT_HTTPPOST, post);
    curl_easy_setopt(handle, CURLOPT_MAXREDIRS, 50L);
    curl_easy_setopt(handle, CURLOPT_TCP_KEEPALIVE, 1L);
    curl_easy_setopt(handle, CURLOPT_WRITEFUNCTION, write_function);
    curl_easy_setopt(handle, CURLOPT_WRITEDATA, &written_result);

    res = curl_easy_perform(handle);
    curl_easy_cleanup(handle);
    handle = NULL;
    curl_formfree(post);
    post = NULL;

    char url [BUFFER_SIZE];
    sscanf(written_result, "%*[^[][%*[^,],%*[^,],%*[^:]:\"%[^\"]", url);
    printf("http://a.pomf.se/%s\n", url);

    return (int )res;
}

// vim: set ts=4 sw=4 et:

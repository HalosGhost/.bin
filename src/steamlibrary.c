
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <glob.h>
#include <errno.h>
#include <sys/stat.h>

int32_t
main (void) { // Add argp support

	const char * home = getenv("HOME");
	if ( home ) {
		size_t n = strlen(home) + 31;
		char steamdir [n + 1];
		strncpy(steamdir, home, n);
		strncat(steamdir, "/.local/share/Steam/steamapps/", n - strlen(steamdir));

		// do some sanity check
		errno = 0;
		int32_t r = chdir(steamdir);
		int32_t errsv = errno;
		if ( r ) {
			fprintf(stderr, "Something went horribly wrong!: %s\n", strerror(errsv));
			return 1;
		}

		glob_t glb;
		r = glob("./*.acf", NULL, NULL, &glb);
		if ( r ) {
			fprintf(stderr, "Something went horribly wrong (with the globbing)!\n");
		}
		// loop through the files and parse them
	} return 0;
}

// vim: set ts=4 sw=4 et:

/* assure.c - define macros assure(expr) and assure_errno(expr)
 *            for fatal error messages
 *
 * (C) 2007 - Pedro Zorzenon Neto
 *
 * To use this this you must agree with its licence terms
 * License: GNU Lesser General Public License
 *          http://www.fsf.org/licensing/licenses/lgpl.html
 *
 * It was based on ideas of /usr/include/assert.h from GNU libc
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include "assure.h"

void assure_failed (const char *assertion, const char *file,
		    unsigned int line, const char *function) {
  fprintf(stderr,"%s:%d %s: Assure `%s' failed.\n", file, line,
	  function, assertion);
  abort();
}

void assure_failed_errno (const char *assertion, const char *file,
			  unsigned int line, const char *function) {
  fprintf(stderr,"%s:%d %s: Assure `%s' failed: %s\n",
	  file, line, function, assertion, strerror(errno));
  abort();
}

/* assure.h - define macros assure(expr) and assure_strerror(expr)
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

#ifdef	_ASSURE_H
# undef	_ASSURE_H
# undef	assure
# undef	_assure
# undef	assure_strerror
# undef	_assure_strerror
#endif

#define	_ASSURE_H	1

#define assure(expr) _assure(expr)

#define _assure(expr)                             \
  if (!(expr)) {                                  \
    assure_failed(__STRING(expr), __FILE__,       \
                  __LINE__, __PRETTY_FUNCTION__); \
  }

#define assure_strerror(expr) _assure_strerror(expr)

#define _assure_strerror(expr)                          \
  if (!(expr)) {                                        \
    assure_failed_errno(__STRING(expr), __FILE__,       \
                        __LINE__, __PRETTY_FUNCTION__); \
  }

#ifndef _ASSURE_DECLS
#define _ASSURE_DECLS

void assure_failed (const char *assertion, const char *file,
		    unsigned int line, const char *function);
     
void assure_failed_errno (const char *assertion, const char *file,
			  unsigned int line, const char *function);

#endif

/* testutil.h */

#ifndef TESTUTIL_H
#define TESTUTIL_H

#include "rfile.h"

#define check( rc ) \
  check_rc(__FILE__, __LINE__, rc)

void check_rc( const char *file, int line, int rc );
void rand_fill( void *mem, size_t size, unsigned seed );
void tu_tmp( char **name );
rfile *tu_create( char **name );

#endif

/* vim:ts=2:sw=2:sts=2:et:ft=c 
 */

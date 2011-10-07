/* testutil.h */

#ifndef TESTUTIL_H
#define TESTUTIL_H

#include "rfile.h"

#define check( rc ) \
  check_rc(__FILE__, __LINE__, rc)

#define countof(ar) (sizeof(ar) / sizeof(ar[0]))

void check_rc( const char *file, int line, int rc );
void rand_fill( void *mem, size_t size, unsigned seed );
void tu_tmp( char **name );
rfile *tu_create( char **name );
uint64_t tu_bigrand( uint64_t max, unsigned *seed );
void tu_mk_range_list( rfile_range * rl, size_t rlcount, size_t dsize,
                       unsigned seed );
void tu_shuffle( void *base, size_t nel, size_t width, unsigned seed );

#endif

/* vim:ts=2:sw=2:sts=2:et:ft=c 
 */

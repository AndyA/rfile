/* testutil.c */

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>

#include "testutil.h"
#include "tap.h"

void
check_rc( const char *file, int line, int rc ) {
  if ( rc )
    die( "%s, %d: error: %s", file, line, strerror( errno ) );
}

void
rand_fill( void *mem, size_t size, unsigned seed ) {
  unsigned char *mp = ( unsigned char * ) mem;
  off_t i;
  for ( i = 0; i < size; i++ ) {
    mp[i] = rand_r( &seed );
  }
}

void
tu_tmp( char **name ) {
  char *tmp = tmpnam( NULL );
  if ( NULL == ( *name = strdup( tmp ) ) )
    die( "Out of memory" );
}

rfile *
tu_create( char **name ) {
  rfile *rf;

  tu_tmp( name );
  if ( NULL == ( rf = rfile_create( *name, 0600 ) ) )
    die( "Can't create %s: %s", *name, strerror( errno ) );

  return rf;
}

/* vim:ts=2:sw=2:sts=2:et:ft=c 
 */

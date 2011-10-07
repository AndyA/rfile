/* testutil.c */

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>

#include "testutil.h"
#include "tap.h"

#define ELTPT(base, i, w) \
  (void *) ((char *) (base) + (i) * (w))

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

uint64_t
tu_bigrand( uint64_t max, unsigned *seed ) {
  uint64_t rlim = 1;
  uint64_t rn = 0;
  while ( rlim < max ) {
    rn = rn * ( ( uint64_t ) RAND_MAX + 1 ) + rand_r( seed );
    rlim *= ( ( uint64_t ) RAND_MAX + 1 );
  }
  return rn * max / rlim;
}

static int
cmp_range( const void *a, const void *b ) {
  const rfile_range *ra = a;
  const rfile_range *rb = b;
  return ra->start < rb->start ? -1
      : ra->start > rb->start ? 1
      : ra->end < rb->end ? -1 : ra->end > rb->end ? 1 : 0;
}

/* Create an ordered list of start, end pairs */
void
tu_mk_range_list( rfile_range * rl, size_t rlcount, size_t dsize,
                  unsigned seed ) {
  unsigned rlpos = 0;

  /* build list of ranges */
  rl[rlpos].start = rl[rlpos].end = 0;
  rlpos++;
  while ( rlpos < rlcount ) {
    rl[rlpos].start = rl[rlpos].end = tu_bigrand( dsize, &seed );
    rlpos++;
  }
  qsort( rl, rlcount, sizeof( rl[0] ), cmp_range );
  for ( rlpos = 0; rlpos < rlcount - 1; rlpos++ ) {
    rl[rlpos].end = rl[rlpos + 1].start;
  }
  rl[rlpos].end = dsize;
}

void
tu_shuffle( void *base, size_t nel, size_t width, unsigned seed ) {
  unsigned pos;
  void *tmp;
  if ( NULL == ( tmp = malloc( width ) ) )
    die( "Out of memory" );
  for ( pos = 0; pos < nel - 1; pos++ ) {
    unsigned pick = ( unsigned ) tu_bigrand( nel - pos - 1, &seed );
    memcpy( tmp, ELTPT( base, width, pick + pos + 1 ), width );
    memcpy( ELTPT( base, width, pick + pos + 1 ),
            ELTPT( base, width, pos ), width );
    memcpy( ELTPT( base, width, pos ), tmp, width );
  }
  free( tmp );
}

/* vim:ts=2:sw=2:sts=2:et:ft=c 
 */

/* refs.c */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

#include "tap.h"
#include "testutil.h"
#include "rfile.h"

static void
test_001( void ) {
  char *ref = tu_make_file( 12345, 0 );
  char *tf = NULL;
  struct stat st, rst;

  /* write file */
  {
    rfile_range r[1];
    rfile_ref rfref;
    ssize_t done;

    check( stat( ref, &st ) );
    rfref.ref = ref;
    rfref.range = r;
    rfref.count = countof( r );
    rfref.flags = rfile_ref_REL;
    r[0].start = 0;
    r[0].end = st.st_size;

    rfile *rf = tu_create( &tf );
    done = rfile_writeref( rf, &rfref );
    is( done, st.st_size, "Size matches (write)" );
    check( rfile_close( rf ) );
  }

  {
    check( rfile_stat( tf, &rst ) );
    is( st.st_size, rst.st_size, "Size matches (stat)" );
  }

  {
    size_t wsz, gsz;
    void *want, *got;
    want = tu_load( ref, &wsz, 0 );
    got = tu_load( tf, &gsz, 1 );
    is( gsz, wsz, "Size matches (read)" );
    ok( !memcmp( got, want, gsz ), "Data matches" );
    free( want );
    free( got );
  }

  free( ref );
  free( tf );
}

int
test_main( int argc, char *argv[] ) {
  ( void ) argc;
  ( void ) argv;
  plan( 4 );
  test_001(  );
  return 0;
}

/* vim:ts=2:sw=2:sts=2:et:ft=c 
 */

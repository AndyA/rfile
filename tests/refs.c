/* tests/refs.c */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

#include "tap.h"
#include "testutil.h"
#include "rfile.h"

static char *
mkfile( void ) {
  char *tmp = tu_tmp(  );
  char *src = "/etc/hosts";
  char buf[1024];

  int rd, wd;
  if ( rd = open( src, O_RDONLY ), rd < 0 )
    die( "Failed to read %s: %s", src, strerror( errno ) );
  if ( wd = open( tmp, O_WRONLY ), rd < 0 )
    die( "Failed to write %s: %s", tmp, strerror( errno ) );

  for ( ;; ) {
    ssize_t got = read( rd, buf, sizeof( buf ) );
    ssize_t done;
    if ( got == 0 )
      break;
    if ( got < 0 )
      die( "Failed to read %s: %s", src, strerror( errno ) );
    done = write( rd, buf, got );
    if ( done != got )
      die( "Failed to write %s: %s", tmp, strerror( errno ) );
  }
  close( rd );
  close( wd );
  return tmp;
}

static void
test_001( void ) {
  char *ref = "/etc/hosts";
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

  free( tf );
}

int
main( void ) {
  plan( 4 );
  test_001(  );
  return 0;
}

/* vim:ts=2:sw=2:sts=2:et:ft=c 
 */

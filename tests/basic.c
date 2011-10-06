/* tests/basic.c */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include "tap.h"
#include "rfile.h"

static void
_check( const char *file, int line, int rc ) {
  if ( rc )
    die( "%s, %d: error: %s", file, line, strerror( errno ) );
}

#define check( rc ) \
  _check(__FILE__, __LINE__, rc)

static void
test_read( void ) {
  rfile *rf;
  char buf[128];

  rf = rfile_open( "foo.rfile", O_RDONLY );
  if ( !rf )
    check( -1 );
  for ( ;; ) {
    ssize_t got = rfile_read( rf, buf, sizeof( buf ) );
    if ( got < 0 )
      check( -1 );
    if ( got == 0 )
      break;
    fwrite( buf, 1, got, stdout );
  }

  check( rfile_close( rf ) );
}

static void
test_write( void ) {
  const char *msg = "Hello, World\n";
  rfile *rf;
  int i;

  rf = rfile_create( "foo.rfile", 0666 );
  if ( !rf )
    check( -1 );
  for ( i = 0; i < 10; i++ ) {
    if ( rfile_write( rf, msg, strlen( msg ) ) < 0 )
      check( -1 );
  }

  check( rfile_close( rf ) );
}

int
main( void ) {
  test_write(  );
  test_read(  );
  plan( 1 );
  ok( 1, "That's fine" );
  return 0;
}

/* vim:ts=2:sw=2:sts=2:et:ft=c 
 */

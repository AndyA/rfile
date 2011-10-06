/* tests/basic.c */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "tap.h"
#include "rfile.h"

static void
test_read( void ) {
  rfile *rf;
  char buf[128];

  rf = rfile_open( "foo.rfile", O_RDONLY );
  for ( ;; ) {
    ssize_t got = rfile_read( rf, buf, sizeof( buf ) );
    if ( got == 0 )
      break;
    if ( got < 0 ) {
      fprintf( stderr, "Read error: %d\n", errno );
      exit( 1 );
    }
    fwrite( buf, 1, got, stdout );
  }

  rfile_close( rf );
}

static void
test_write( void ) {
  const char *msg = "Hello, World\n";
  rfile *rf;
  int i;

  rf = rfile_create( "foo.rfile", 0666 );
  for ( i = 0; i < 10; i++ ) {
    rfile_write( rf, msg, strlen( msg ) );
  }

  rfile_close( rf );
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

/* tests/basic.c */

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
  char data[11327];
  int sn;
  int step[] = {
    1, 2, 3, 4, 8, 9, 12, 16, 32, 36, 64, 100, 128, 129, 200, 256, 300,
    512, 1000, 1024, 1111, 1234, 2000, 2048, 3000, 4000, 4096, 8192, 10000,
    11300, 11320, 11327, -1
  };

  rand_fill( data, sizeof( data ), 0 );

  for ( sn = 0; step[sn] > 0; sn++ ) {
    rfile *rf;
    char *tf = NULL;
    struct stat st;
    unsigned pos;
    size_t written = 0;

    rf = tu_create( &tf );
    check( rfile_fstat( rf, &st ) );
    is( st.st_size, 0, "step = %d, new file: length == 0", step[sn] );
    is( rfile_lseek( rf, 0, SEEK_CUR ), 0,
        "step = %d, new file: fptr == 0", step[sn] );

    for ( pos = 0; pos < sizeof( data ); pos += step[sn] ) {
      size_t done, avail = sizeof( data ) - pos;
      if ( avail > step[sn] )
        avail = step[sn];
      done = rfile_write( rf, data + pos, avail );
      written += done;
    }

    is( written, sizeof( data ),
        "step = %d, rfile_write returned %d", step[sn], sizeof( data ) );

    check( rfile_fstat( rf, &st ) );
    is( st.st_size, sizeof( data ),
        "step = %d, after write: %ld == %d bytes", step[sn],
        ( long ) st.st_size, sizeof( data ) );
    is( rfile_lseek( rf, 0, SEEK_CUR ), sizeof( data ),
        "step = %d, after write: fptr == %d", step[sn], sizeof( data ) );
    check( rfile_close( rf ) );
    check( unlink( tf ) );
    free( tf );
  }
}

int
main( void ) {
  plan( 32 * 5 );
  test_001(  );
  return 0;
}

/* vim:ts=2:sw=2:sts=2:et:ft=c 
 */

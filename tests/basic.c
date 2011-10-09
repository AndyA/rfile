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
    512, 1000, 1024, 1111, 1234, 2000, 2048, 3000, 4000, 4096, 8192,
    10000, 11300, 11320, 11327, -1
  };

  rand_fill( data, sizeof( data ), 0 );

  for ( sn = 0; step[sn] > 0; sn++ ) {
    char *tf = NULL;

    /* write file */
    {
      rfile *rf;
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
    }

    /* read file */
    {
      rfile *rf;
      int sn2;

      if ( NULL == ( rf = rfile_open( tf, O_RDONLY ) ) )
        check( -1 );

      for ( sn2 = 0; step[sn2] > 0; sn2++ ) {
        unsigned pos;
        size_t fetched = 0;
        unsigned char got[sizeof( data )];

        is( rfile_lseek( rf, 0, SEEK_SET ), 0, "seek to start" );

        memset( got, 0, sizeof( got ) );
        for ( pos = 0; pos < sizeof( data ); pos += step[sn2] ) {
          size_t done, avail = sizeof( data ) - pos;
          if ( avail > step[sn2] )
            avail = step[sn2];
          done = rfile_read( rf, got + pos, avail );
          fetched += done;
        }

        is( fetched, sizeof( data ),
            "step = %d, rfile_read returned %d", step[sn],
            sizeof( data ) );

        ok( !memcmp( data, got, sizeof( data ) ), "data read OK" );
      }

      check( rfile_close( rf ) );
    }

    free( tf );
  }
}

static void
test_002( void ) {
  char *tf = NULL;
  char data[13271];

  rand_fill( data, sizeof( data ), 0 );

  /* write file */
  {
    rfile *rf;

    rf = tu_create( &tf );
    if ( rfile_write( rf, data, sizeof( data ) ) < 0 )
      check( -1 );
    check( rfile_close( rf ) );
  }

  {
    rfile_range rl[2000];
    char got[sizeof( data )];
    unsigned pc, rlpos;

    for ( pc = 10; pc <= 100; pc += 10 ) {
      size_t slice = countof( rl ) * pc / 100;
      rfile *rf;

      tu_mk_range_list( rl, slice, sizeof( data ), 0 );
      tu_shuffle( rl, slice, sizeof( rl[0] ), 0 );

      if ( NULL == ( rf = rfile_open( tf, O_RDONLY ) ) )
        check( -1 );

      memset( got, 0, sizeof( got ) );
      for ( rlpos = 0; rlpos < slice; rlpos++ ) {
        size_t want = rl[rlpos].end - rl[rlpos].start;
        /* diag( "%4u: %7lu - %7lu", rlpos, rl[rlpos].start, rl[rlpos].end ); */
        if ( rfile_lseek( rf, rl[rlpos].start, SEEK_SET ) !=
             rl[rlpos].start )
          check( -1 );
        if ( rfile_read( rf, got + rl[rlpos].start, want ) != want )
          check( -1 );
      }

      check( rfile_close( rf ) );
      ok( !memcmp( data, got, sizeof( data ) ), "%lu chunks: data read OK",
          ( unsigned long ) slice );
    }
  }

  free( tf );
}

int
main( void ) {
  plan( 3242 );
  test_001(  );
  test_002(  );
  return 0;
}

/* vim:ts=2:sw=2:sts=2:et:ft=c 
 */

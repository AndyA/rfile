/* filename.c */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/param.h>
#include <sys/stat.h>
#include <unistd.h>

#include "tap.h"
#include "testutil.h"
#include "rfile.h"
#include "filename.h"

static void
test_001( void ) {
  ok( rfile_fn_is_url( "http://example.com" ), "is url" );
  ok( !rfile_fn_is_url( "/usr/local" ), "is file" );
  ok( !rfile_fn_is_url( "http/example.com" ), "is file" );
}

static void
test_002( void ) {
  char cwd[MAXPATHLEN];

  if ( !getcwd( cwd, sizeof( cwd ) ) )
    check( -1 );

  {
    char *abs = rfile_fn_rel2abs( "foo", NULL );
    char want[MAXPATHLEN];
    strcpy( want, cwd );
    strcat( want, "/foo" );
    ok( !strcmp( abs, want ), "Abs relative to cwd (%s)", abs );
    free( abs );
  }
}

int
test_main( int argc, char *argv[] ) {
  ( void ) argc;
  ( void ) argv;
  plan( 4 );
  test_001(  );
  test_002(  );
  return 0;
}

/* vim:ts=2:sw=2:sts=2:et:ft=c 
 */

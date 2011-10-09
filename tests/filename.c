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

  ok( rfile_fn_is_abs( "/usr/local" ), "is abs" );
  ok( !rfile_fn_is_abs( "http/example.com" ), "is relative" );
}

static int
tidy_ok( const char *in, const char *want, const char *msg ) {
  char *got = rfile_fn_tidy( in );
  int r = !strcmp( got, want );
  ok( r, "%s", msg );
  if ( !r ) {
    diag( " got: %s", got );
    diag( "want: %s", want );
  }
  free( got );
  return r;
}

static void
test_002( void ) {
  tidy_ok( "nothing/to/see/here", "nothing/to/see/here", "no tidy" );
  tidy_ok( "//usr///local//bin/foo", "/usr/local/bin/foo",
           "extra slashes" );

  tidy_ok( "/", "/", "root" );
  tidy_ok( "./", "./", "canonical 'here'" );
  tidy_ok( "././", "./", "tidy 'here'" );
  tidy_ok( "/usr/./local/.//././bin/.", "/usr/local/bin",
           "lots of dots" );

  tidy_ok( "../../foo/bar", "../../foo/bar", "legitimate ups" );
  tidy_ok( "../foo/../bar", "../bar", "down, up" );
  tidy_ok( "../foo/baz/../../bar", "../bar", "multi down, up" );
  tidy_ok( "/tmp/../", "/", "root" );
  tidy_ok( "/.", "/.", "slashdot" );
  tidy_ok( "foo/.", "foo", "trailing" );
  tidy_ok( "/../tmp", "/tmp", "silly root up" );
  tidy_ok( "foo/..", ".", "degenerate special case" );

  tidy_ok( ".//../foo/baz/../bim//../.././bar", "../bar", "pointless" );
}

static void
test_003( void ) {
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
  plan( 21 );
  test_001(  );
  test_002(  );
  test_003(  );
  return 0;
}

/* vim:ts=2:sw=2:sts=2:et:ft=c 
 */

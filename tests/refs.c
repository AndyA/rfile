/* tests/refs.c */

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
  ok( 1, "That's OK" );
}

int
main( void ) {
  plan( 1 );
  test_001(  );
  return 0;
}

/* vim:ts=2:sw=2:sts=2:et:ft=c 
 */

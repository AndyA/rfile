/* filename.c */

#include <ctype.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/param.h>

#include "filename.h"

int
rfile_fn_is_abs( const char *fn ) {
  if ( *fn == '/' )
    return 1;

  while ( *fn && isalpha( *fn ) )
    fn++;

  return strlen( fn ) > 3 && !memcmp( fn, "://", 3 );
}

char *
rfile_fn_rel2abs( const char *rel, const char *base ) {
  char bd[MAXPATHLEN * 2], *abs;
  size_t blen, rlen;

  if ( rfile_fn_is_abs( rel ) ) {
    blen = 0;
  }
  else {
    if ( base ) {
      blen = strlen( base );
      while ( blen > 0 && base[blen - 1] != '/' )
        blen--;
      if ( blen + 1 > sizeof( bd ) ) {
        errno = EINVAL;
        return NULL;
      }
      memcpy( bd, base, blen );
      bd[blen] = '\0';
    }
    else {
      if ( !getcwd( bd, sizeof( bd ) - 1 ) )
        return NULL;
      blen = strlen( bd );
      bd[blen++] = '/';
    }
  }

  rlen = strlen( rel ) + 1;

  if ( blen + rlen > sizeof( bd ) ) {
    errno = EINVAL;
    return NULL;
  }

  memcpy( bd + blen, rel, rlen );

  if ( ( abs = strdup( bd ) ) )
    return abs;

  errno = ENOMEM;
  return NULL;
}

/* vim:ts=2:sw=2:sts=2:et:ft=c 
 */

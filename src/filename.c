/* filename.c */

#include <ctype.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <sys/param.h>
#include <unistd.h>

#include "filename.h"

int
rfile_fn_is_url( const char *fn ) {
  const char *fnp;

  for ( fnp = fn; *fnp && isalpha( *fnp ); fnp++ ) ;
  return fnp > fn + 2 && strlen( fnp ) > 3 && !memcmp( fnp, "://", 3 );
}

int
rfile_fn_is_abs( const char *fn ) {
  return rfile_fn_is_url( fn ) || *fn == '/';
}

char *
rfile_fn_tidy( const char *name ) {
  char *fn, *pos;

  if ( fn = strdup( name ), !fn ) {
    errno = ENOMEM;
    return NULL;
  }

  for ( pos = fn; ( pos = strstr( pos, "//" ) ); )
    strcpy( pos, pos + 1 );

  for ( pos = fn; ( pos = strstr( pos, "./" ) ); ) {
    if ( ( pos == fn && pos[2] ) || pos[-1] == '/' ) {
      strcpy( pos, pos + 2 );
      continue;
    }
    pos++;
  }

  for ( pos = fn; ( pos = strstr( pos, "/.." ) ); ) {
    char *p2;

    /* start of absolute name */
    if ( pos == fn ) {
      strcpy( pos, pos + 3 );
      continue;
    }

    if ( pos[3] != '/' && pos[3] != '\0' ) {
      pos++;
      continue;
    }

    for ( p2 = pos; p2 != fn && p2[-1] != '/'; p2-- ) ;

    if ( p2 == pos - 2 && p2[0] == '.' && p2[1] == '.' ) {
      pos++;
      continue;
    }

    if ( p2 == fn ) {
      if ( pos[3] == '\0' ) {
        strcpy( fn, "." );
        return fn;
      }
      strcpy( p2, pos + 4 );
      pos = p2;
    }
    else {
      p2--;
      strcpy( p2, pos + 3 );
      pos = p2;
    }
  }

  {
    size_t l = strlen( fn );
    if ( l > 2 && !strcmp( fn + l - 2, "/." ) ) {
      fn[l - 2] = '\0';
    }
  }

  return fn;
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

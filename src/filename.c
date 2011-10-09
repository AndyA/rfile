/* filename.c */

#include <ctype.h>
#include <errno.h>
#include <libgen.h>
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

static void
_inplace_strcpy( char *dst, const char *src ) {
  size_t len = strlen( src );
  memmove( dst, src, len + 1 );
}

char *
rfile_fn_tidy( const char *name ) {
  char *fn, *pos;

  if ( fn = strdup( name ), !fn ) {
    errno = ENOMEM;
    return NULL;
  }

  for ( pos = fn; ( pos = strstr( pos, "//" ) ); )
    _inplace_strcpy( pos, pos + 1 );

  for ( pos = fn; ( pos = strstr( pos, "./" ) ); ) {
    if ( ( pos == fn && pos[2] ) || pos[-1] == '/' ) {
      _inplace_strcpy( pos, pos + 2 );
      continue;
    }
    pos++;
  }

  for ( pos = fn; ( pos = strstr( pos, "/.." ) ); ) {
    char *p2;

    /* start of absolute name */
    if ( pos == fn ) {
      _inplace_strcpy( pos, pos + 3 );
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
        _inplace_strcpy( fn, "." );
        return fn;
      }
      _inplace_strcpy( p2, pos + 4 );
      pos = p2;
    }
    else {
      p2--;
      _inplace_strcpy( p2, pos + 3 );
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
rfile_fn_dirname( const char *file ) {
  char *fn, *dn;
  if ( ( fn = strdup( file ), !fn )
       || ( dn = strdup( dirname( fn ) ), !dn ) ) {
    errno = ENOMEM;
    return NULL;
  }
  free( fn );
  return dn;
}

char *
rfile_fn_rel2abs( const char *rel, const char *base ) {
  char bd[MAXPATHLEN * 2];
  size_t blen, rlen;

  if ( rfile_fn_is_abs( rel ) ) {
    blen = 0;
  }
  else {
    if ( base ) {
      char *ab = rfile_fn_rel2abs( base, NULL );
      blen = strlen( ab );
      if ( blen + 1 > sizeof( bd ) ) {
        errno = EINVAL;
        return NULL;
      }
      memcpy( bd, ab, blen );
      free( ab );
    }
    else {
      if ( !getcwd( bd, sizeof( bd ) - 1 ) )
        return NULL;
      blen = strlen( bd );
    }

    if ( bd[blen] != '/' )
      bd[blen++] = '/';
  }

  rlen = strlen( rel ) + 1;

  if ( blen + rlen > sizeof( bd ) ) {
    errno = EINVAL;
    return NULL;
  }

  memcpy( bd + blen, rel, rlen );

  return rfile_fn_tidy( bd );
}

char *
rfile_fn_rel2abs_file( const char *rel, const char *base_file ) {
  char *base, *abs;
  if ( base = rfile_fn_dirname( base_file ), !base )
    return NULL;
  abs = rfile_fn_rel2abs( rel, base );
  free( base );
  return abs;
}

/* vim:ts=2:sw=2:sts=2:et:ft=c 
 */

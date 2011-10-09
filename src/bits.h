/* bits.h */

#ifndef __BITS_H
#define __BITS_H

#include "../config.h"

#ifdef HAVE_INTTYPES_H
#include <inttypes.h>
#endif

#ifdef HAVE_STDINT_H
#include <stdint.h>
#endif

#include <stdarg.h>
#include <stddef.h>
#include <sys/types.h>

#define rfile_ALIGNMENT   4
#define rfile_ALIGNDOWN(n) ((n) / rfile_ALIGNMENT * rfile_ALIGNMENT)
#define rfile_ALIGNUP(n)   rfile_ALIGNDOWN(n + rfile_ALIGNMENT - 1)

typedef struct rfile__bits rfile_bits;

typedef struct {
  int ( *destroy ) ( rfile_bits * bits );
  int ( *grow ) ( rfile_bits * bits, size_t size );
} rfile_bits_vt;

struct rfile__bits {
  unsigned char *buf;
  size_t used, size;
  off_t pos;
  rfile_bits_vt *vt;
};

int rfile_bits_buf( rfile_bits * bits, unsigned char *buf, size_t size );
int rfile_bits_init( rfile_bits * bits, size_t size );
int rfile_bits_destroy( rfile_bits * bits );
int rfile_bits_rewind( rfile_bits * bits );
int rfile_bits_grow( rfile_bits * bits, size_t size );
int rfile_bits_put( rfile_bits * bits, const unsigned char *buf,
                    size_t len );
int rfile_bits_pad( rfile_bits * bits, size_t len );
int rfile_bits_get( rfile_bits * bits, unsigned char *buf, size_t len );
int rfile_bits_put32( rfile_bits * bits, uint32_t v );
int rfile_bits_put64( rfile_bits * bits, uint64_t v );
int rfile_bits_get32( rfile_bits * bits, uint32_t * v );
int rfile_bits_get64( rfile_bits * bits, uint64_t * v );
int rfile_bits_put_data( rfile_bits * bits, const void *data, size_t len );
const void *rfile_bits_get_data( rfile_bits * bits, size_t * len );
int rfile_bits_puts( rfile_bits * bits, const char *s );
const char *rfile_bits_gets( rfile_bits * bits );
int rfile_bits_read( rfile_bits * bits, int fd, size_t len );
int rfile_bits_write( rfile_bits * bits, int fd, size_t len );
int rfile_bits_piddle( rfile_bits * bits, const char *spec, ... );
int rfile_bits_guzzle( rfile_bits * bits, const char *spec, ... );

#define rfile_bits__READER( name, type, spec, mptr, size ) \
  static int                                              \
  name( rfile * rf, type * obj ) {                        \
    rfile_bits b;                                         \
    unsigned char buf[size];                              \
    return rfile_bits_buf( &b, buf, sizeof( buf ) )       \
        || rfile_bits_read( &b, rf->fd, size )            \
        || rfile_bits_rewind( &b )                        \
        || rfile_bits_guzzle( &b, spec, mptr )            \
        ? -1 : 0;                                         \
  }

#define rfile_bits__WRITER( name, type, spec, memb, size ) \
  static int                                              \
  name( rfile * rf, const type * obj ) {                  \
    rfile_bits b;                                         \
    unsigned char buf[size];                              \
    return rfile_bits_buf( &b, buf, sizeof( buf ) )       \
        || rfile_bits_piddle( &b, spec, memb )            \
        || rfile_bits_rewind( &b )                        \
        || rfile_bits_write( &b, rf->fd, size )           \
        ? -1 : 0;                                         \
  }

#define rfile_bits__GUZZLE( name, type, spec, mptr ) \
  static int                                         \
  name( rfile_bits * b, type * obj ) {               \
    return rfile_bits_guzzle( b, spec, mptr );       \
  }

#define rfile_bits__PIDDLE( name, type, spec, memb ) \
  static int                                         \
  name( rfile_bits * b, const type * obj ) {         \
    return rfile_bits_piddle( b, spec, memb );       \
  }

#define rfile_bits_READER( name, type ) \
  rfile_bits__READER( name, type, type ## _SPEC, \
                      type ## _MPTR, type ## _SIZE )

#define rfile_bits_WRITER( name, type ) \
  rfile_bits__WRITER( name, type, type ## _SPEC, \
                      type ## _MEMB, type ## _SIZE )

#define rfile_bits_GUZZLE( name, type ) \
  rfile_bits__GUZZLE( name, type, type ## _SPEC, type ## _MPTR )

#define rfile_bits_PIDDLE( name, type ) \
  rfile_bits__PIDDLE( name, type, type ## _SPEC, type ## _MEMB )

#define MEMB(n) ((obj)->n)
#define MPTR(n) (&MEMB(n))

#endif

/* vim:ts=2:sw=2:sts=2:et:ft=c 
 */

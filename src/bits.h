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

#define rfile_ALIGNMENT   8
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

#endif

/* vim:ts=2:sw=2:sts=2:et:ft=c 
 */

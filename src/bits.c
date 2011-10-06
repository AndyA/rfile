/* bits.c */

#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "bits.h"

static int rfile__bits_destroy_static( rfile_bits * bits );
static int rfile__bits_grow_static( rfile_bits * bits, size_t size );
static int rfile__bits_destroy_dynamic( rfile_bits * bits );
static int rfile__bits_grow_dynamic( rfile_bits * bits, size_t size );

static rfile_bits_vt vt_static = {
  rfile__bits_destroy_static,
  rfile__bits_grow_static
};

static rfile_bits_vt vt_dynamic = {
  rfile__bits_destroy_dynamic,
  rfile__bits_grow_dynamic
};

int
rfile_bits_buf( rfile_bits * bits, unsigned char *buf, size_t size ) {
  memset( bits, 0, sizeof( *bits ) );
  bits->buf = buf;
  bits->size = size;
  bits->vt = &vt_static;
  return 0;
}

int
rfile_bits_init( rfile_bits * bits, size_t size ) {
  memset( bits, 0, sizeof( *bits ) );
  bits->vt = &vt_dynamic;
  return rfile_bits_grow( bits, size );
}

static int
rfile__bits_destroy_static( rfile_bits * bits ) {
  return 0;
}

static int
rfile__bits_grow_static( rfile_bits * bits, size_t size ) {
  if ( bits->size < size ) {
    errno = ENOMEM;
    return -1;
  }
  return 0;
}

static int
rfile__bits_destroy_dynamic( rfile_bits * bits ) {
  free( bits->buf );
  memset( bits, 0, sizeof( *bits ) );
  return 0;
}

static int
rfile__bits_grow_dynamic( rfile_bits * bits, size_t size ) {
  if ( bits->size < size ) {
    unsigned char *buf = malloc( size );
    if ( !buf ) {
      errno = ENOMEM;
      return -1;
    }
    if ( bits->buf ) {
      memcpy( buf, bits->buf, bits->size );
      free( bits->buf );
    }
    bits->buf = buf;
    bits->size = size;
  }
  return 0;
}

int
rfile_bits_destroy( rfile_bits * bits ) {
  return bits->vt->destroy( bits );
}

int
rfile_bits_grow( rfile_bits * bits, size_t size ) {
  return bits->vt->grow( bits, size );
}

int
rfile_bits_rewind( rfile_bits * bits ) {
  bits->pos = 0;
  return 0;
}

int
rfile_bits_put( rfile_bits * bits, const unsigned char *buf, size_t len ) {
  if ( rfile_bits_grow( bits, bits->pos + len ) < 0 )
    return -1;
  memcpy( bits->buf + bits->pos, buf, len );
  bits->pos += len;
  if ( bits->used < bits->pos )
    bits->used = bits->pos;
  return 0;
}

int
rfile_bits_get( rfile_bits * bits, unsigned char *buf, size_t len ) {
  if ( bits->pos + len > bits->used ) {
    errno = EIO;
    return -1;
  }
  memcpy( buf, bits->buf + bits->pos, len );
  bits->pos += len;
  return 0;
}

int
rfile_bits_put32( rfile_bits * bits, uint32_t v ) {
  unsigned char buf[4];
  buf[0] = v >> 0;
  buf[1] = v >> 8;
  buf[2] = v >> 16;
  buf[3] = v >> 24;
  return rfile_bits_put( bits, buf, sizeof( buf ) );
}

int
rfile_bits_put64( rfile_bits * bits, uint64_t v ) {
  unsigned char buf[8];
  buf[0] = v >> 0;
  buf[1] = v >> 8;
  buf[2] = v >> 16;
  buf[3] = v >> 24;
  buf[4] = v >> 32;
  buf[5] = v >> 40;
  buf[6] = v >> 48;
  buf[7] = v >> 56;
  return rfile_bits_put( bits, buf, sizeof( buf ) );
}

int
rfile_bits_get32( rfile_bits * bits, uint32_t * v ) {
  unsigned char buf[4];
  if ( rfile_bits_get( bits, buf, sizeof( buf ) ) < 0 )
    return -1;
  *v = ( ( uint32_t ) buf[0] << 0 )
      | ( ( uint32_t ) buf[1] << 8 )
      | ( ( uint32_t ) buf[2] << 16 )
      | ( ( uint32_t ) buf[3] << 24 );
  return 0;
}

int
rfile_bits_get64( rfile_bits * bits, uint64_t * v ) {
  unsigned char buf[8];
  if ( rfile_bits_get( bits, buf, sizeof( buf ) ) < 0 )
    return -1;
  *v = ( ( uint64_t ) buf[0] << 0 )
      | ( ( uint64_t ) buf[1] << 8 )
      | ( ( uint64_t ) buf[2] << 16 )
      | ( ( uint64_t ) buf[3] << 24 )
      | ( ( uint64_t ) buf[4] << 32 )
      | ( ( uint64_t ) buf[5] << 40 )
      | ( ( uint64_t ) buf[6] << 48 )
      | ( ( uint64_t ) buf[7] << 56 );
  return 0;
}

int
rfile_bits_read( rfile_bits * bits, int fd, size_t len ) {
  ssize_t sz;

  if ( rfile_bits_grow( bits, bits->pos + len ) < 0 )
    return -1;
  sz = read( fd, bits->buf + bits->pos, len );
  if ( sz < 0 )
    return -1;
  if ( sz < len ) {
    errno = EIO;
    return -1;
  }
  bits->pos += len;
  if ( bits->used < bits->pos )
    bits->used = bits->pos;
  return 0;
}

int
rfile_bits_write( rfile_bits * bits, int fd, size_t len ) {
  ssize_t sz;

  if ( bits->pos + len > bits->used ) {
    errno = EIO;
    return -1;
  }

  sz = write( fd, bits->buf + bits->pos, len );
  if ( sz < 0 )
    return -1;
  if ( sz < len ) {
    errno = EIO;
    return -1;
  }
  bits->pos += len;
  return 0;
}

/* vim:ts=2:sw=2:sts=2:et:ft=c 
 */
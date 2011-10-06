/* rfile.c */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "bits.h"
#include "rfile.h"

static int
rfile__ghdr( rfile * rf, rfile_chunk_header * hdr ) {
  rfile_bits b;
  unsigned char buf[rfile_HEADER_SIZE];
  return rfile_bits_buf( &b, buf, sizeof( buf ) )
      || rfile_bits_read( &b, rf->fd, rfile_HEADER_SIZE )
      || rfile_bits_rewind( &b )
      || rfile_bits_get32( &b, &hdr->sig )
      || rfile_bits_get32( &b, &hdr->version )
      || rfile_bits_get32( &b, &hdr->type )
      || rfile_bits_get64( &b, &hdr->length )
      || rfile_bits_get64( &b, &hdr->pos.start )
      || rfile_bits_get64( &b, &hdr->pos.end )
      ? -1 : 0;
}

static int
rfile__phdr( rfile * rf, const rfile_chunk_header * hdr ) {
  rfile_bits b;
  unsigned char buf[rfile_HEADER_SIZE];
  return rfile_bits_buf( &b, buf, sizeof( buf ) )
      || rfile_bits_put32( &b, hdr->sig )
      || rfile_bits_put32( &b, hdr->version )
      || rfile_bits_put32( &b, hdr->type )
      || rfile_bits_put64( &b, hdr->length )
      || rfile_bits_put64( &b, hdr->pos.start )
      || rfile_bits_put64( &b, hdr->pos.end )
      || rfile_bits_rewind( &b )
      || rfile_bits_write( &b, rf->fd, rfile_HEADER_SIZE )
      ? -1 : 0;
}

static void
rfile__init_hdr( rfile_chunk_header * hdr,
                 unsigned long type,
                 off_t length, off_t start, off_t end ) {
  memset( hdr, 0, sizeof( *hdr ) );
  hdr->sig = rfile_SIG;
  hdr->version = rfile_FMT_VER;
  hdr->type = type;
  hdr->length = length;
  hdr->pos.start = start;
  hdr->pos.end = end;
}

static int
rfile__extent( rfile * rf, off_t * extp ) {
  rfile_chunk_header hdr;
  off_t pos;
  int rc;

  pos = lseek( rf->fd, 0, SEEK_END );
  if ( pos < 0 )
    return -1;

  /* zero length file is OK */
  if ( pos == 0 ) {
    *extp = 0;
    return 0;
  }

  pos = lseek( rf->fd, -rfile_HEADER_SIZE, SEEK_END );
  if ( pos < 0 )
    return -1;

  rc = rfile__ghdr( rf, &hdr );
  *extp = hdr.pos.end;

  return 0;
}

static int
rfile__alloc( void **buf, size_t sz ) {
  if ( *buf = malloc( sz ), !*buf ) {
    errno = ENOMEM;
    *buf = NULL;
    return -1;
  }
  memset( *buf, 0, sz );
  return 0;
}

static int
rfile__new( rfile ** rf ) {
  if ( rfile__alloc( ( void ** ) rf, sizeof( rfile ) ) < 0 )
    return -1;
  ( *rf )->c_pos = -1;
  return 0;
}

static off_t
rfile__iov_len( const struct iovec *iov, int iovcnt ) {
  off_t sz = 0;
  int i;
  for ( i = 0; i < iovcnt; i++ ) {
    sz += iov[i].iov_len;
  }
  return sz;
}

static int
rfile__setpos( rfile * rf, off_t pos ) {
  if ( lseek( rf->fd, pos, SEEK_SET ) < 0 )
    return -1;
  if ( rfile__ghdr( rf, &rf->c_hdr ) < 0 )
    return -1;
  rf->c_pos = pos;
  return 0;
}

static int
rfile__seek( rfile * rf, off_t pos ) {
  if ( pos > rf->ext ) {
    errno = EINVAL;
    return -1;
  }

  if ( rf->ext == 0 )
    return 0;

  /* Load first chunk header */
  if ( rf->c_pos == -1 ) {
    if ( pos < rf->ext / 2 ) {
      if ( rfile__setpos( rf, 0 ) < 0 )
        return -1;
    }
    else {
      off_t len = lseek( rf->fd, 0, SEEK_END );
      if ( len < 0 )
        return -1;
      if ( rfile__setpos( rf, len - rfile_HEADER_SIZE ) < 0 )
        return -1;
      rf->c_pos = len - rf->c_hdr.length;
    }
  }

  while ( rf->c_hdr.pos.end <= pos ) {
    if ( rfile__setpos( rf, rf->c_pos + rf->c_hdr.length ) < 0 )
      return -1;
  }

  while ( rf->c_hdr.pos.start > pos ) {
    if ( rfile__setpos( rf, rf->c_pos - rfile_HEADER_SIZE ) < 0 )
      return -1;
    rf->c_pos += rfile_HEADER_SIZE - rf->c_hdr.length;
  }

  return 0;
}

int
rfile_fstat( rfile * rf, struct stat *buf ) {
  int rc;

  /* FIXME race - size is cached, fstat is hot */
  rc = fstat( rf->fd, buf );
  buf->st_size = rf->ext;

  return rc;
}

int
rfile_stat( const char *path, struct stat *buf ) {
  rfile *rf = rfile_open( path, O_RDONLY );
  int rc;

  if ( !rf )
    return -1;

  rc = rfile_fstat( rf, buf );
  rfile_close( rf );
  return rc;
}

rfile *
rfile_create( const char *name, mode_t mode ) {
  rfile *rf;

  if ( rfile__new( &rf ) < 0 )
    return NULL;

  if ( rf->fd =
       open( name, O_CREAT | O_TRUNC | O_RDWR, mode ), rf->fd < 0 )
    goto fail;

  if ( rfile__extent( rf, &rf->ext ) < 0 )
    goto fail;

  return rf;

fail:
  rfile_close( rf );
  return NULL;
}

rfile *
rfile_open( const char *name, int oflag ) {
  rfile *rf;

  if ( rfile__new( &rf ) < 0 )
    return NULL;

  if ( rf->fd = open( name, oflag ), rf->fd < 0 )
    goto fail;

  if ( rfile__extent( rf, &rf->ext ) < 0 )
    goto fail;

  return rf;

fail:
  rfile_close( rf );
  return NULL;
}

int
rfile_close( rfile * rf ) {
  if ( rf ) {
    if ( rf->fd )
      close( rf->fd );
    free( rf );
  }
  return 0;
}

off_t
rfile_lseek( rfile * rf, off_t offset, int whence ) {
  switch ( whence ) {
  case SEEK_SET:
    rf->fptr = offset;
    break;
  case SEEK_CUR:
    rf->fptr += offset;
    break;
  case SEEK_END:
    rf->fptr = rf->ext + offset;
    break;
  default:
    errno = EINVAL;
    return -1;
  }
  return rf->fptr;
}

ssize_t
rfile_read( rfile * rf, void *buf, size_t nbyte ) {
  struct iovec iov;
  iov.iov_base = ( char * ) buf;
  iov.iov_len = nbyte;
  return rfile_readv( rf, &iov, 1 );
}

typedef struct {
  const struct iovec *iov;
  int iovcnt;
  int pos;
  size_t carry;
} rfile__iov_state;

static void
rfile__init_iov_state( rfile__iov_state * st, const struct iovec *iov,
                       int iovcnt ) {
  st->iov = iov;
  st->iovcnt = iovcnt;
  st->pos = 0;
  st->carry = 0;
}

static int
rfile__fill_iov( rfile__iov_state * st, struct iovec *iov, int iovsz,
                 size_t * avail ) {
  int iovcnt = 0;

  while ( *avail > 0 && st->pos < st->iovcnt && iovcnt < iovsz ) {
    size_t cav = st->iov[st->pos].iov_len - st->carry;
    if ( cav > *avail )
      cav = *avail;
    iov[iovcnt].iov_base = st->iov[st->pos].iov_base + st->carry;
    iov[iovcnt++].iov_len = cav;
    *avail -= cav;
    st->carry += cav;
    if ( st->carry == st->iov[st->pos].iov_len ) {
      st->carry = 0;
      st->pos++;
    }
  }

  return iovcnt;
}

ssize_t
rfile_readv( rfile * rf, const struct iovec * iov, int iovcnt ) {
  rfile__iov_state st;
  struct iovec iv[256];
  int ivc;
  size_t got = 0;

  if ( rf->fptr > rf->ext ) {
    errno = EINVAL;
    return -1;
  }

  if ( rf->fptr == rf->ext )
    return 0;

  if ( rfile__seek( rf, rf->fptr ) < 0 )
    return -1;

  rfile__init_iov_state( &st, iov, iovcnt );

  for ( ;; ) {
    size_t avail;
    int fd;

    switch ( rf->c_hdr.type ) {
    case rfile_DATA_IN:
    case rfile_DATA_OUT:
      avail = rf->c_hdr.pos.end - rf->fptr;
      fd = rf->fd;
      if ( lseek
           ( fd,
             rf->c_pos + rfile_HEADER_SIZE + rf->fptr -
             rf->c_hdr.pos.start, SEEK_SET ) < 0 )
        return -1;
      break;
    case rfile_REF_IN:
    case rfile_REF_OUT:
      break;
    default:
      errno = EIO;
      return -1;
    }
    while ( avail > 0 && st.pos < st.iovcnt ) {
      size_t ov = avail;
      ssize_t cnt;
      ivc =
          rfile__fill_iov( &st, iv, sizeof( iv ) / sizeof( iv[0] ),
                           &avail );
      cnt = readv( fd, iv, ivc );
      if ( cnt < 0 )
        return cnt;
      if ( cnt != ov - avail ) {
        errno = EIO;
        return -1;
      }
      got += cnt;
      rf->fptr += cnt;
    }

    if ( st.pos == st.iovcnt || rf->fptr == rf->ext )
      return got;

    if ( rfile__setpos( rf, rf->c_pos + rf->c_hdr.length ) < 0 )
      return -1;
  }

  return 0;
}

ssize_t
rfile_write( rfile * rf, const void *buf, size_t nbyte ) {
  struct iovec iov;
  iov.iov_base = ( char * ) buf;
  iov.iov_len = nbyte;
  return rfile_writev( rf, &iov, 1 );
}

ssize_t
rfile_writev( rfile * rf, const struct iovec * iov, int iovcnt ) {
  rfile_chunk_header hdr;
  off_t pos, sz;
  ssize_t bc;

  /* TODO check seek pos */
  sz = rfile__iov_len( iov, iovcnt );
  if ( sz == 0 )
    return 0;

  rfile__init_hdr( &hdr, rfile_DATA_IN, sz + rfile_HEADER_SIZE * 2,
                   rf->ext, rf->ext + sz );

  pos = lseek( rf->fd, 0, SEEK_END );

  if ( rfile__phdr( rf, &hdr ) < 0 )
    goto fail;

  if ( bc = writev( rf->fd, iov, iovcnt ), bc != sz )
    goto fail;

  hdr.type = rfile_DATA_OUT;

  if ( rfile__phdr( rf, &hdr ) < 0 )
    goto fail;

  rf->ext += bc;

  return bc;

fail:
  ftruncate( rf->fd, pos );
  return -1;
}

ssize_t
rfile_writeref( rfile * rf, const char *url, const rfile_range * range,
                size_t count ) {
  return 0;
}

/* vim:ts=2:sw=2:sts=2:et:ft=c 
 */

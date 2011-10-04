/* src/rfile.h */

#ifndef __RFILE_H
#define __RFILE_H

#include "../config.h"

#define rfile_FOURCC(s0, s1, s2, s3) \
  ( (unsigned int) \
    ( ( (unsigned char) (s3) << 24 ) | \
      ( (unsigned char) (s2) << 16 ) | \
      ( (unsigned char) (s1) <<  8 ) | \
      ( (unsigned char) (s0) <<  0 ) ) )

#define rfile_FMT_VER  1
#define rfile_SIG      rfile_FOURCC('r', 'F', 'l', 'E')
#define rfile_DATA_IN  rfile_FOURCC('D', 'A', 'T', 'A')
#define rfile_DATA_OUT rfile_FOURCC('d', 'a', 't', 'a')
#define rfile_REF_IN   rfile_FOURCC('R', 'E', 'F', ' ')
#define rfile_REF_OUT  rfile_FOURCC('r', 'e', 'f', ' ')

typedef struct {
  off_t start;
  off_t end;
} rfile_range;

typedef struct {
  unsigned long sig;
  unsigned long version;
  unsigned long type;
  off_t length;
  rfile_range pos;
} rfile_chunk_header;

#define rfile_HEADER_SIZE sizeof(rfile_chunk_header)

typedef struct {
  int fd;
  off_t ext;
  off_t fptr;                   /* file offset */

  rfile_chunk_header c_hdr;     /* current chunk */
  off_t c_pos;                  /* pos of chunk in file */

} rfile;

rfile *rfile_create( const char *name, mode_t mode );
rfile *rfile_open( const char *name, int oflag );
int rfile_close( rfile * rf );
off_t rfile_lseek( rfile * rf, off_t offset, int whence );
ssize_t rfile_read( rfile * rf, void *buf, size_t nbyte );
ssize_t rfile_readv( rfile * rf, const struct iovec *iov, int iovcnt );
ssize_t rfile_write( rfile * rf, const void *buf, size_t nbyte );
ssize_t rfile_writev( rfile * rf, const struct iovec *iov, int iovcnt );
ssize_t rfile_writeref( rfile * rf, const char *url,
                        const rfile_range * range, size_t count );

#endif

/* vim:ts=2:sw=2:sts=2:et:ft=c 
 */

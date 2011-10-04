/* src/rfile.h */

#ifndef __RFILE_H
#define __RFILE_H

#include "../config.h"

#define rfile_FOURCC(s) \
  ( (unsigned long) \
    ( ( (unsigned char) ((s)[0]) << 24 ) | \
      ( (unsigned char) ((s)[1]) << 16 ) | \
      ( (unsigned char) ((s)[2]) <<  8 ) | \
      ( (unsigned char) ((s)[3]) <<  0 ) ) )

#define rfile_DATA_IN  rfile_FOURCC("DATA")
#define rfile_DATA_OUT rfile_FOURCC("data")
#define rfile_REF_IN   rfile_FOURCC("REF ")
#define rfile_REF_OUT  rfile_FOURCC("ref ")

typedef struct {
  off_t start;
  off_t end;
} rfile_range;

typedef struct {
  unsigned long sig;
  unsigned long version;
  unsigned long length;
  unsigned long type;
  rfile_range pos;
} rfile_chunk_header;

typedef struct {
  int fd;

} rfile;

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

/* src/rfile.c */

#include <stdio.h>
#include <stdio.h>
#include <sys/uio.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include "rfile.h"

static int
rfile__extent( rfile * rf, off_t * extp ) {
  return 0;
}

int
rfile_fstat( rfile * rf, struct stat *buf ) {
  return 0;
}

int
rfile_stat( const char *path, struct stat *buf ) {
  rfile *rf = rfile_open( path, O_RDONLY );
  off_t ext;
  int rc = 0;

  if ( !rf )
    return -1;

  if ( rc = fstat( rf->fd, buf ), rc )
    goto fail;

  if ( rc = rfile__extent( rf, &ext ), rc )
    goto fail;

  buf->st_size = ext;

fail:
  rfile_close( rf );
  return rc;
}

rfile *
rfile_open( const char *name, int oflag ) {
  return NULL;
}

int
rfile_close( rfile * rf ) {
  return 0;
}

off_t
rfile_lseek( rfile * rf, off_t offset, int whence ) {
  return 0;
}

ssize_t
rfile_read( rfile * rf, void *buf, size_t nbyte ) {
  return 0;
}

ssize_t
rfile_readv( rfile * rf, const struct iovec * iov, int iovcnt ) {
  return 0;
}

ssize_t
rfile_write( rfile * rf, const void *buf, size_t nbyte ) {
  return 0;
}

ssize_t
rfile_writev( rfile * rf, const struct iovec * iov, int iovcnt ) {
  return 0;
}

ssize_t
rfile_writeref( rfile * rf, const char *url, const rfile_range * range,
                size_t count ) {
  return 0;
}

int
main( void ) {
  printf( "Hello, World %lx\n", rfile_DATA_IN );
  return 0;
}

/* vim:ts=2:sw=2:sts=2:et:ft=c 
 */

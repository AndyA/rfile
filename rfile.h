/* src/rfile.h */

#ifndef __RFILE_H
#define __RFILE_H

#include <errno.h>
#include <fcntl.h>
#include <stdint.h>
#include <sys/stat.h>
#include <sys/uio.h>
#include <unistd.h>

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

/* SERIALIZE */
typedef struct {
  uint64_t start;
  uint64_t end;
} rfile_range;

/* SERIALIZE */
typedef struct {
  uint32_t sig;
  uint32_t version;
  uint32_t type;
  uint64_t length;
  rfile_range pos;
} rfile_chunk_header;

#define rfile_ref_ABS  1
#define rfile_ref_REL  2

typedef struct {
  char *ref;
  rfile_range *range;
  size_t count;
  unsigned flags;

  /* private */
  int fd;
} rfile_ref;

typedef struct {
  char *fname;
  int fd;
  off_t ext;
  off_t fptr;                   /* file offset */

  rfile_chunk_header c_hdr;     /* current chunk */
  off_t c_pos;                  /* pos of chunk in file */

} rfile;

int rfile_fstat(rfile *rf, struct stat *buf);
int rfile_stat(const char *path, struct stat *buf);
rfile *rfile_open(const char *name, int oflag, ...);
rfile *rfile_create(const char *name, mode_t mode);
int rfile_close(rfile *rf);
off_t rfile_lseek(rfile *rf, off_t offset, int whence);
ssize_t rfile_read(rfile *rf, void *buf, size_t nbyte);
ssize_t rfile_readv(rfile *rf, const struct iovec *iov, int iovcnt);
ssize_t rfile_write(rfile *rf, const void *buf, size_t nbyte);
ssize_t rfile_writev(rfile *rf, const struct iovec *iov, int iovcnt);
ssize_t rfile_writeref(rfile *rf, const rfile_ref *ref);

#endif

/* vim:ts=2:sw=2:sts=2:et:ft=c
 */

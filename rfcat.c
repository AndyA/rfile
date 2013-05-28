/* rfcat.c */

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>

#include "rfile.h"

static void die(const char *fmt, ...) {
  va_list ap;
  va_start(ap, fmt);
  vfprintf(stderr, fmt, ap);
  fprintf(stderr, "\n");
  va_end(ap);
  exit(1);
}

int main(int argc, char *argv[]) {
  int argn;

  for (argn = 1; argn < argc; argn++) {
    char buf[16384];
    rfile *rf = rfile_open(argv[argn], O_RDONLY);
    if (!rf)
      die("Can't read %s: %s", argv[argn], strerror(errno));

    for (;;) {
      ssize_t got = rfile_read(rf, buf, sizeof(buf));
      if (got == 0)
        break;
      if (got < 0)
        die("Read error: %s\n", strerror(errno));
      fwrite(buf, 1, got, stdout);
    }

    rfile_close(rf);
  }
  return 0;
}

/* vim:ts=2:sw=2:sts=2:et:ft=c
 */

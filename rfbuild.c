/* rfbuild.c */

#include <ctype.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "rfile.h"

struct parser {
  const char *ip;
};

static void parse_entry(struct parser *p, rfile *rf);

static void die(const char *fmt, ...) {
  va_list ap;
  va_start(ap, fmt);
  vfprintf(stderr, fmt, ap);
  fprintf(stderr, "\n");
  va_end(ap);
  exit(1);
}

static void oom(void) {
  die("Out of memory");
}

/* parse a reference expression
 *
 * range     := ':' start ':' length
 * ranges    := range
 *           |  range ranges
 *
 * relabs    := ';rel'
 *           |  ';abs'
 *
 * fileref   := filename
 *           |  filename ranges
 *
 * filespec  := fileref
 *           |  fileref relabs
 *
 * include   := '+' filename
 *
 * reference := '[' filespec ']'
 *
 * direct    := fileref
 *
 * entry     := include
 *           |  reference
 *           |  direct
 *
 */


static char *grab_str(const char *sp, const char *ep) {
  size_t len = ep - sp;
  char *s = malloc(len + 1);
  if (!s) oom();
  memcpy(s, sp, len);
  s[len] = '\0';
  return s;
}

static int parse_number(struct parser *p, uint64_t *vp) {
  if (*p->ip != ':') return 0;
  p->ip++;
  char *ep;
  unsigned long long vv = strtoull(p->ip, &ep, 10);
  if (ep == p->ip) die("Bad number");
  p->ip = ep;
  if (vp) *vp = (uint64_t) vv;
  return 1;
}

static int parse_flags(struct parser *p, unsigned *fp) {
  if (*p->ip != ';') return 0;
  p->ip++;
  if (strlen(p->ip) >= 3
      && (!memcmp("abs", p->ip, 3)
          || !memcmp("rel", p->ip, 3))) {
    if (fp) *fp = *p->ip == 'a' ? rfile_ref_ABS : rfile_ref_REL;
    p->ip += 3;
    return 1;
  }
  die("Bad options");
  return 1;
}

static char *parse_filename(struct parser *p) {
  const char *sp = p->ip;
  int c;
  while (c = *p->ip, c) {
    if (c == ':' || c == ';' || c == ']' || isspace(c)) break;
    p->ip++;
  }
  return grab_str(sp, p->ip);
}

static off_t file_size(const char *filename) {
  struct stat st;
  if (stat(filename, &st)) die("Can't stat %s: %m", filename);
  return st.st_size;
}

static void parse_fileref(struct parser *p, rfile_ref *ref) {
  char *fn = parse_filename(p);
  struct parser ptmp = *p;

  size_t count = 0;
  while (parse_number(&ptmp, NULL)) count++;
  parse_flags(&ptmp, NULL);

  ref->ref = fn;
  ref->count = count ? (count + 1) / 2 : 1;
  ref->range = calloc(ref->count, sizeof(rfile_range));
  if (!ref->range) oom();

  for (unsigned i = 0; i < ref->count; i++) {
    uint64_t start, length;
    if (!parse_number(p, &start))
      start = 0;
    if (!parse_number(p, &length))
      length = file_size(ref->ref) - start;
    ref->range[i].start = start;
    ref->range[i].end = start + length;
  }

  parse_flags(p, &ref->flags);
}

static void parse_include(struct parser *p, rfile *rf) {
  char *fn = parse_filename(p);
  char *buf = NULL;
  size_t buf_sz = 0;
  unsigned buf_pos;
  FILE *fl = fopen(fn, "r");
  if (!fl) die("Can't read %s: %m", fn);

  int c = getc(fl);
  while (c != EOF) {
    if (isspace(c))
      c = getc(fl);
    else if (c == '#') {
      c = getc(fl);
      while (c != EOF && c != '\n') c = getc(fl);
    }
    else {
      struct parser ptmp;
      buf_pos = 0;
      for (;;) {
        if (buf_pos == buf_sz) {
          buf_sz *= 2;
          if (buf_sz < 256) buf_sz = 256;
          char *nb = realloc(buf, buf_sz);
          if (!nb) oom();
          buf = nb;
        }
        if (c == EOF || isspace(c)) {
          buf[buf_pos++] = '\0';
          break;
        }
        buf[buf_pos++] = c;
        c = getc(fl);
      }
      ptmp.ip = buf;
      fprintf(stderr, "# %s\n", buf);
      parse_entry(&ptmp, rf);
    }
  }

  free(buf);
  fclose(fl);
}

static void parse_ref(struct parser *p, rfile *rf) {
  rfile_ref ref;
  parse_fileref(p, &ref);
  if (rfile_writeref(rf, &ref) < 0)
    die("Failed to append reference");
  free(ref.range);
  free(ref.ref);
}

static void parse_append(struct parser *p, rfile *rf) {
  char *fn = parse_filename(p);
  char buf[0x10000];
  int fd = open(fn, O_RDONLY);
  if (fd < 0) die("Can't read %s: %m", fn);

  for (;;) {
    ssize_t got = read(fd, buf, sizeof(buf));
    if (got < 0) die("Read error on %s: %m", fn);
    if (got == 0) break;
    rfile_write(rf, buf, got);
  }

  close(fd);
  free(fn);
}

static void parse_entry(struct parser *p, rfile *rf) {
  switch (*p->ip) {
  case '+':
    p->ip++;
    parse_include(p, rf);
    break;

  case '[':
    p->ip++;
    parse_ref(p, rf);
    if (*p->ip != ']') die("Missing ]");
    p->ip++;
    break;

  default:
    parse_append(p, rf);
    break;
  }
}

int main(int argc, char *argv[]) {
  int argn;
  rfile *rf;

  if (argc < 2) die("Need a filename");
  rf = rfile_create(argv[1], 0777);

  for (argn = 2; argn < argc; argn++) {
    struct parser p;
    p.ip = argv[argn];
    parse_entry(&p, rf);
  }

  rfile_close(rf);
  return 0;
}

/* vim:ts=2:sw=2:sts=2:et:ft=c
 */

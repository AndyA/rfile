# rfile

rfiles are reference files. They contain a series of chunks each of
which contains either literal data or a reference to a number of byte
ranges in another file.

Via this library they can be made to appear as regular files.

## features

Supports normal POSIX-like file I/O (open, read, write, lseek, close).

Random access reading.

Append only writing.

Andy Armstrong <andy@hexten.net>

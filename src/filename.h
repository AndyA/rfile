/* filename.h */

#ifndef __FILENAME_H
#define __FILENAME_H

int rfile_fn_is_abs( const char *fn );
char *rfile_fn_rel2abs( const char *rel, const char *base );

#endif

/* vim:ts=2:sw=2:sts=2:et:ft=c 
 */

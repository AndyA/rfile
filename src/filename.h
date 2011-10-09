/* filename.h */

#ifndef __FILENAME_H
#define __FILENAME_H

int rfile_fn_is_url( const char *fn );
int rfile_fn_is_abs( const char *fn );
char *rfile_fn_tidy( const char *name );
char *rfile_fn_dirname( const char *file );
char *rfile_fn_rel2abs( const char *rel, const char *base );
char *rfile_fn_rel2abs_file( const char *rel, const char *base_file );
char *rfile_fn_abs2rel( const char *abs, const char *base );
char *rfile_fn_abs2rel_file( const char *abs, const char *base_file );

#endif

/* vim:ts=2:sw=2:sts=2:et:ft=c 
 */

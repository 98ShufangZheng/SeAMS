#ifndef GETOPT_H
#define GETOPT_H

#ifndef _STDLIB_H_
extern int optind;
extern char *optarg;

#ifdef __cplusplus
extern "C" {
#endif

int getopt( int argc, char *const * argv, const char * optstring );

#ifdef __cplusplus
}
#endif

#endif

#endif

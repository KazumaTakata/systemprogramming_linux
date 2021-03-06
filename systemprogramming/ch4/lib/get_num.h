#ifndef GET_NUM_H
#define GET_NUM_H
#define GN_NONNEG 01
#define GN_GT_0 02
#define GN_ANY_BASE 0100
#define GN_BASE_8 0200
#define GN_BASE_16 0400
/*Valuemustbe>=0*/                                                             /*Valuemustbe>0*/
/* By default, integers are decimal */ /* Can use any base - like strtol(3) */ /* Value is expressed in octal */
/* Value is expressed in hexadecimal */
long getLong(const char *arg, int flags, const char *name);
int getInt(const char *arg, int flags, const char *name);
#endif
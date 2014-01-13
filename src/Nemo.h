#ifndef NEMO_H
#define NEMO_H

#include <stdio.h>

int     Nm_flength(FILE *f);
int     Nm_flength(const char *filename);
bool    Nm_fexist(const char *filename);
int     Nm_LoadFile(const char *filename, void **bufferptr);
bool    Nm_SaveFile(const char *filename, const char *bufptr);

#endif
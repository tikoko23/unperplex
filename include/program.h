#ifndef UNPERPLEX_PROGRAM_H
#define UNPERPLEX_PROGRAM_H

#include <stddef.h>

#include <CL/cl.h>

#include "clinit.h"

char *readEntireFile(const char *filename, size_t *out_len);
cl_int loadProgramFromSource(CL_Data *cl, cl_program *this, const char *source, size_t length);
cl_int loadProgramFromFile(CL_Data *cl, cl_program *this, const char *filename);

#endif

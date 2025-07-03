#include <CL/cl.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

#include "program.h"
#include "clinit.h"

char *readEntireFile(const char *filename, size_t *out_len) {
    FILE *file = fopen(filename, "r");

    fseek(file, 0, SEEK_END);
    size_t len = ftell(file);
    rewind(file);

    char *buf = malloc(len + 1);
    fread(buf, 1, len, file);
    buf[len] = '\0';

    fclose(file);

    if (out_len) {
        *out_len = len;
    }

    return buf;
}

cl_int loadProgramFromSource(CL_Data *cl, cl_program *this, const char *source, size_t length) {
    cl_int err;
    *this = clCreateProgramWithSource(cl->context, 1, &source, &length, &err);
    return err;
}

cl_int loadProgramFromFile(CL_Data *cl, cl_program *this, const char *filename) {
    size_t source_length;
    char *source = readEntireFile(filename, &source_length);

    cl_int status = loadProgramFromSource(cl, this, source, source_length);

    free(source);
    return status;
}


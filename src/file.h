#ifndef FILE_H

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

#include "err.h"
#include "str.h"
#include "vstr.h"

/******************************************************************************/
/* PUBLIC FUNCTION PROTOTYPES *************************************************/
/******************************************************************************/

typedef enum {
    FILE_TYPE_NONE,
    FILE_TYPE_FILE,
    FILE_TYPE_DIR,
    FILE_TYPE_ERROR,
} FileTypeList;

typedef int (*FileFunc)(Str *filename, void *);

#define FILE_PATH_MAX   4096

FileTypeList file_get_type(Str *filename);

int file_is_dir(const Str *filename);
size_t file_size(Str *filename);

#define ERR_FILE_STR_READ "failed to read file"
#define ERR_file_str_read(filename, content) "failed reading file '%.*s'", STR_F(filename)
ErrDecl file_str_read(Str *filename, Str *content);

#define ERR_file_str_write(filename, content) "failed writing file '%.*s'", STR_F(filename)
ErrDecl file_str_write(Str *filename, Str *content);

#define ERR_FILE_FP_READ "failed to read file"
#define ERR_file_fp_read(file, content) "failed reading file pointer '%p'", file
ErrDecl file_fp_read(FILE *file, Str *content);

#define FILE_H
#endif

#include <errno.h>
#include "file.h"

/******************************************************************************/
/* PUBLIC FUNCTION IMPLEMENTATIONS ********************************************/
/******************************************************************************/

#if defined(PLATFORM_WINDOWS)
#else
#include <sys/stat.h>
#include <sys/sysmacros.h>
#endif

FileTypeList file_get_type(Str *filename) {
#if defined(PLATFORM_WINDOWS)
    ASSERT("not implemented");
#else
    struct stat s;
    char path[FILE_PATH_MAX];
    str_cstr(filename, path, FILE_PATH_MAX);
    int r = lstat(path, &s);
    if(r) return FILE_TYPE_ERROR;
    if(S_ISREG(s.st_mode)) return FILE_TYPE_FILE;
    if(S_ISDIR(s.st_mode)) return FILE_TYPE_DIR;
#endif
    return 0;
}

int file_is_dir(const Str *filename)
{
#if defined(PLATFORM_WINDOWS)
    ASSERT("not implemented");
#else
    struct stat s;
    char path[FILE_PATH_MAX];
    str_cstr(filename, path, FILE_PATH_MAX);
    int r = lstat(path, &s);
    if(r) return 0;
    return S_ISDIR(s.st_mode);
#endif
    return 0;
}

size_t file_size(Str *filename) {/*{{{*/
    char path[FILE_PATH_MAX];
    str_cstr(filename, path, FILE_PATH_MAX);
    FILE *fp = fopen(path, "rb");
    size_t result = SIZE_MAX;
    if(fp) {
        if(!fseek(fp, 0L, SEEK_END)) {
            result = ftell(fp);
        }
        fclose(fp);
    }
    return result;
}/*}}}*/

int file_is_file(Str *filename)
{
#if defined(PLATFORM_WINDOWS)
    ASSERT("not implemented");
#else
    struct stat s;
    char path[4094];
    str_cstr(filename, path, FILE_PATH_MAX);
    int r = lstat(path, &s);
    if(r) return 0;
    return S_ISREG(s.st_mode);
#endif
    return 0;
}

int file_fp_write(FILE *file, Str *content)
{
    if(!file) THROW("invalid filename");
    if(!content) THROW("invalid output buffer");

    /* write file */
    size_t bytes_written = fwrite(str_iter_begin(content), 1, str_length(content), file);
    if(bytes_written != str_length(content)) {
        THROW("bytes written (%zu) mismatch bytes to write (%zu)!", bytes_written, str_length(content));
    }

    /* close file outside */
    return 0;
error:
    return -1;
}

int file_fp_read(FILE *file, Str *content)
{
    if(!file) THROW("invalid filename");
    if(!content) THROW("invalid output buffer");

    /* get file length */
    fseek(file, 0, SEEK_END);
    size_t bytes_file = (size_t)ftell(file);
    fseek(file, 0, SEEK_SET);

    /* allocate memory */
    TRY(str_reserve(content, bytes_file + 1), "couldn't increase capacity");

    /* read file */
    size_t bytes_read = fread(content->s, 1, bytes_file, file);
    //if(bytes_file != bytes_read) THROW("mismatch read vs expected bytes");
    content->s[bytes_read] = 0;
    content->last = bytes_read;

    /* close file outside */
    return 0;
error:
    return -1;
}

int file_str_read(Str *filename, Str *content)
{
    int err = 0;
    FILE *file = 0;
    if(!filename) THROW("invalid filename");
    if(!content) THROW("invalid output buffer");

    /* open the file */
    errno = 0;
    if(filename->last && (
                filename->s[filename->last - 1] == PLATFORM_CH_SUBDIR ||
                filename->s[filename->last - 1] == '/')) {
        THROW("won't open directories");
    }

    char path[FILE_PATH_MAX] = {0};
    str_cstr(filename, path, FILE_PATH_MAX);
    file = fopen(path, "r");
    if(!file || errno) {
        //goto clean;
        THROW("failed to open file named '%s'", path);
    }

    TRY(file_fp_read(file, content), ERR_FILE_FP_READ);
    /* close file */
clean:
    if(file) fclose(file);
    return err;
error: ERR_CLEAN;
}

#if 1
int file_str_write(Str *filename, Str *content)
{
    int err = 0;
    FILE *file = 0;
    if(!filename) THROW("invalid filename");
    if(!content) THROW("invalid output buffer");

    /* open the file */
    errno = 0;
    if(filename->last && (
                filename->s[filename->last - 1] == PLATFORM_CH_SUBDIR ||
                filename->s[filename->last - 1] == '/')) {
        THROW("won't open directories");
    }

    char path[FILE_PATH_MAX] = {0};
    str_cstr(filename, path, FILE_PATH_MAX);
    file = fopen(path, "w");
    if(!file || errno) {
        //goto clean;
        THROW("failed to open file named '%s'", path);
    }

    TRY(file_fp_write(file, content), ERR_FILE_FP_READ);
clean:
    if(file) fclose(file);
    return err;
error: ERR_CLEAN;
}
#endif

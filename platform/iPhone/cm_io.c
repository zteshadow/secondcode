
#include "cm_io.h"
#include "cm_debug.h"

#include <stdio.h>
#include <dirent.h>
#include <sys/stat.h>


CM_HANDLE cm_fopen(const S8 *path, const S8 *mode)
{
	FILE *fp;

    CM_ASSERT(path != NULL && mode != NULL);
    
    fp = fopen(path, mode);

    //CM_ASSERT(fp != NULL);
    
    return fp;
}

CM_SIZE cm_fread(void *p, CM_SIZE size, CM_SIZE n, CM_HANDLE file)
{
    CM_SIZE cnt;
    CM_ASSERT(p != NULL && file != NULL);

    cnt = fread(p, 1, size * n, (FILE *)file);

    return cnt;
}

CM_SIZE cm_fwrite(const void *p, CM_SIZE size, CM_SIZE n, CM_HANDLE file)
{
    CM_SIZE cnt;
    CM_ASSERT(p != NULL && file != NULL);

    cnt = fwrite(p, 1, size * n, (FILE *)file);

    return cnt;
}

S32 cm_fseek(CM_HANDLE file, S32 offset, S32 whence)
{
    CM_ASSERT(file != NULL);

    return fseek((FILE *)file, offset, whence);
}

S32 cm_ftell(CM_HANDLE file)
{
    CM_ASSERT(file != NULL);

    return ftell((FILE *)file);
}

S32 cm_fclose(CM_HANDLE file)
{
    CM_ASSERT(file != NULL);
    return fclose((FILE *)file);
}

S8 *cm_fgets(S8 *buffer, S32 size, CM_HANDLE fp)
{
    CM_ASSERT(buffer != 0 && size > 1 && fp != 0);

    return fgets(buffer, size, (FILE *)fp);
}

S32 cm_sprintf(S8 *str, const S8 *format, ...)
{
	va_list ap;

    CM_ASSERT(str != 0);

	va_start(ap, format);

	vsprintf(str, format, ap);

	va_end(ap);

	return 0;
}

S32 cm_vsprintf(S8 *str, const S8 *format, va_list ap)
{
    CM_ASSERT(str != 0);

    vsprintf(str, format, ap);
	return 0;
}

S32 cm_dir_exist(const S8 *dir)
{
    S32 value = 0;
    CM_ASSERT(dir != 0);
    
    if (dir)
    {
        DIR *directory = opendir(dir);
        if (directory != 0)
        {
            value = 1;
            closedir(directory);
        }
    }
    return value;
}

S32 cm_mkdir(const S8 *dir)
{
    S32 value = -1;
    
    CM_ASSERT(dir != 0);
    if (dir)
    {
        value = mkdir(dir, 0777);
    }
    
    return value;
}

S32 cm_rmdir(const S8 *dir)
{
    S32 value = -1;
    
    CM_ASSERT(dir != 0);
    if (dir)
    {
        value = remove(dir);
    }
    
    return value;
}

S32 cm_file_delete(const S8 *fname)
{
    S32 value = -1;
    
    CM_ASSERT(fname != 0);
    if (fname)
    {
        value = remove(fname);
    }
    
    return value;
}

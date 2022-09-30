#include "ff_wrapper.h"

#include <unistd.h>
#include <fcntl.h>
#include <dfs_posix.h>
int omvhal_f_open(omvhal_fil_t *f, const char *path, int mode)
{
    uint32_t oflag = 0;
    int ret = 0;

    if (mode & FA_WRITE)
        oflag |= O_WRONLY;
    if (mode & FA_READ)
        oflag |= O_RDONLY;
    if (mode & FA_CREATE_ALWAYS)
        oflag |= O_CREAT;

    f->fd = open(path, oflag);

    if (f->fd < 0)
        ret = -1;

    return ret;
}

int omvhal_f_close(omvhal_fil_t *f)
{
    close(f->fd);

    return 0;
}

int omvhal_f_write(omvhal_fil_t *fp, const void *buff, uint32_t btw, uint32_t *bw)
{
    int ret;

    ret = write(fp->fd, buff, btw);
    if (bw)
        *bw = ret;
    if (ret == btw)
        ret = 0;

    return ret;
}

int omvhal_f_read(omvhal_fil_t *fp, void *buff, uint32_t btr, uint32_t *br)
{
    int ret;

    ret = read(fp->fd, buff, btr);
    if (br)
        *br = ret;
    if (ret == btr)
        ret = 0;

    return ret;
}

int omvhal_f_eof(omvhal_fil_t *fp)
{
    struct stat st;
    uint32_t pos;

    fstat(fp->fd, &st);
    pos = lseek(fp->fd, 0, SEEK_CUR);

    return (pos >= st.st_size);
}

uint32_t omvhal_f_size(omvhal_fil_t *fp)
{
    struct stat st;

    fstat(fp->fd, &st);

    return st.st_size;
}

uint32_t omvhal_f_tell(omvhal_fil_t *fp)
{
    return lseek(fp->fd, 0, SEEK_CUR);
}

int omvhal_f_lseek(omvhal_fil_t*fp, uint32_t offset)
{
    int ret;

    ret = lseek(fp->fd, offset, SEEK_SET);
    if (ret == offset)
        ret = 0;

    return ret;
}

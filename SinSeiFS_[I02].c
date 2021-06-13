#define FUSE_USE_VERSION 28

#include <fuse.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <dirent.h>
#include <errno.h>
#include <sys/time.h>
#include <wait.h>
#include <sys/stat.h>
#include <errno.h>
#include <ctype.h>
#include <time.h>


int x = 0;

static const char *log = "/home/drigo/SinSeiFS.log";

static const char *dir = "/home/drigo/SinSei";


void filelog(char *level, char *command, const char *desc[], int descLen)

{

    FILE *file = fopen(log, "a");

    time_t t;

    struct tm *tmp;

    char timeString[100];


    time(&t);

    tmp = localtime(&t);

    strftime(timeString, sizeof(timeString), "%d%m%y-%H:%M:%S", tmp);


    fprintf(file, "%s::%s:%s", level, timeString, command);

    for (int i = 0; i < descLen; i++)

    {

        fprintf(file, "::%s", desc[i]);

    }

    

    fprintf(file, "\n");

    fclose(file);

}




static int xmp_getattr(const char *path, struct stat *stbuf)

{

    int res;

    char fpath[1024];


    if (strcmp(path, "/") == 0)

    {

        path = dir;

        sprintf(fpath, "%s", path);

    }else

    {

        sprintf(fpath, "%s%s", dir, path);

    }


    res = lstat(fpath, stbuf);

    if (res == -1)

        return -errno;


    const char *desc[] = {path};

    filelog("INFO", "GETATTR", desc, 1);


    return 0;

}


static int xmp_access(const char *path, int mask)

{

    int res;

    char fpath[1024];


    if (strcmp(path, "/") == 0)

    {

        path = dir;

        sprintf(fpath, "%s", path);

    }else

    {

        sprintf(fpath, "%s%s", dir, path);

    }


    res = access(fpath, mask);


    const char *desc[] = {path};

    filelog("INFO", "ACCESS", desc, 1);


    if (res == -1)

        return -errno;

    return 0;

}


static int xmp_readlink(const char *path, char *buf, size_t size)

{

    int res;

    char fpath[1024];


    if (strcmp(path, "/") == 0)

    {

        path = dir;

        sprintf(fpath, "%s", path);

    }else

    {

        sprintf(fpath, "%s%s", dir, path);

    }


    res = readlink(fpath, buf, size - 1);


    const char *desc[] = {path};

    filelog("INFO", "READLINK", desc, 1);


    if (res == -1)

        return -errno;


    buf[res] = '\0';

    return 0;

}



static int xmp_readdir(const char *path, void *buf, fuse_fill_dir_t filler, off_t offset, struct fuse_file_info *fi)

{

    char fpath[1024];


    if (strcmp(path, "/") == 0)

    {

        path = dir;

        sprintf(fpath, "%s", path);

    }else

    {

        sprintf(fpath, "%s%s", dir, path);

    }


    if (x != 24)

        x++;

    else

    {

        const char *desc[] = {path};

        filelog("INFO", "READDIR", desc, 1);

    }


    int res = 0;

    DIR *dp;

    struct dirent *de;


    (void)offset;

    (void)fi;


    dp = opendir(fpath);

    if (dp == NULL)

        return -errno;


    while ((de = readdir(dp)) != NULL)

    {

        if (strcmp(de->d_name, ".") == 0 || strcmp(de->d_name, "..") == 0)

            continue;


        struct stat st;

        memset(&st, 0, sizeof(st));

        st.st_ino = de->d_ino;

        st.st_mode = de->d_type << 12;


        res = (filler(buf, de->d_name, &st, 0));

        if (res != 0)

            break;

    }


    closedir(dp);

    return 0;

}




static int xmp_mknod(const char *path, mode_t mode, dev_t rdev)

{

    int res;

    char fpath[1024];


    if (strcmp(path, "/") == 0)

    {

        path = dir;

        sprintf(fpath, "%s", path);

    }else

    {

        sprintf(fpath, "%s%s", dir, path);

    }


    const char *desc[] = {path};

    filelog("INFO", "MKNOD", desc, 1);


    /* On Linux this could just be 'mknod(path, mode, rdev)' but this is more portable */

    if (S_ISREG(mode))

    {

        res = open(fpath, O_CREAT | O_EXCL | O_WRONLY, mode);

        if (res >= 0)

            res = close(res);

    }

    else if (S_ISFIFO(mode))

        res = mkfifo(fpath, mode);

    else

        res = mknod(fpath, mode, rdev);


    if (res == -1)

        return -errno;

    return 0;

}




static int xmp_mkdir(const char *path, mode_t mode)

{

    int res;

    char fpath[1024];


    if (strcmp(path, "/") == 0)

    {

        path = dir;

        sprintf(fpath, "%s", path);

    }else

    {

        sprintf(fpath, "%s%s", dir, path);

    }


    res = mkdir(fpath, mode);


    const char *desc[] = {fpath};

    filelog("INFO", "MKDIR", desc, 1);


    if (res == -1)

        return -errno;

    return 0;

}


static int xmp_unlink(const char *path)

{

    int res;

    char fpath[1024];


    if (strcmp(path, "/") == 0)

    {

        path = dir;

        sprintf(fpath, "%s", path);

    }else

    {

        sprintf(fpath, "%s%s", dir, path);

    }


    res = unlink(fpath);


    const char *desc[] = {path};

    filelog("WARNING", "UNLINK", desc, 1);


    if (res == -1)

        return -errno;

    return 0;

}



static int xmp_rmdir(const char *path)

{

    int res;

    char fpath[1024];


    if (strcmp(path, "/") == 0)

    {

        path = dir;

        sprintf(fpath, "%s", path);

    }

    else

    {

        sprintf(fpath, "%s%s", dir, path);

    }


    res = rmdir(fpath);


    const char *desc[] = {fpath};

    filelog("WARNING", "RMDIR", desc, 1);


    if (res == -1)

        return -errno;

    return 0;

}


static int xmp_symlink(const char *from, const char *to)

{

    int res;

    char frompath[1024], topath[1024];


    sprintf(frompath, "%s%s", dir, from);

    sprintf(topath, "%s%s", dir, to);


    res = symlink(frompath, topath);


    const char *desc[] = {from, to};

    filelog("INFO", "SYMLINK", desc, 2);


    if (res == -1)

        return -errno;

    return 0;

}


static int xmp_rename(const char *from, const char *to)

{

    int res;

    char frompath[1024], topath[1024];


    sprintf(frompath, "%s%s", dir, from);

    sprintf(topath, "%s%s", dir, to);


    res = rename(frompath, topath);

    if (res == -1)

        return -errno;


    const char *desc[] = {frompath, topath};

    filelog("INFO", "RENAME", desc, 2);



    return 0;

}


static int xmp_link(const char *from, const char *to)

{

    int res;

    char frompath[1024], topath[1024];


    sprintf(frompath, "%s%s", dir, from);

    sprintf(topath, "%s%s", dir, to);


    res = link(frompath, topath);


    const char *desc[] = {from, to};

    filelog("INFO", "LINK", desc, 2);


    if (res == -1)

        return -errno;

    return 0;

}



static int xmp_chmod(const char *path, mode_t mode)

{

    int res;

    char fpath[1024];

    char modeString[128];

    sprintf(modeString, "%d", mode);


    if (strcmp(path, "/") == 0)

    {

        path = dir;

        sprintf(fpath, "%s", path);

    }

    else

    {

        sprintf(fpath, "%s%s", dir, path);

    }


    res = chmod(fpath, mode);


    const char *desc[] = {path, modeString};

    filelog("INFO", "CHMOD", desc, 2);


    if (res == -1)

        return -errno;

    return 0;

}



static int xmp_chown(const char *path, uid_t uid, gid_t gid)

{

    int res;

    char fpath[1024];

    char uidString[128];

    char gidString[128];

    sprintf(uidString, "%d", uid);

    sprintf(gidString, "%d", gid);


    if (strcmp(path, "/") == 0)

    {

        path = dir;

        sprintf(fpath, "%s", path);

    }

    else

    {

        sprintf(fpath, "%s%s", dir, path);

    }


    res = lchown(fpath, uid, gid);


    const char *desc[] = {path, uidString, gidString};

    filelog("INFO", "CHOWN", desc, 3);


    if (res == -1)

        return -errno;

    return 0;

}


static int xmp_truncate(const char *path, off_t size)

{

    int res;

    char fpath[1024];


    if (strcmp(path, "/") == 0)

    {

        path = dir;

        sprintf(fpath, "%s", path);

    }

    else

    {

        sprintf(fpath, "%s%s", dir, path);

    }


    res = truncate(fpath, size);


    const char *desc[] = {path};

    filelog("INFO", "TRUNCATE", desc, 1);


    if (res == -1)

        return -errno;

    return 0;

}


static int xmp_utimens(const char *path, const struct timespec ts[2])

{

    int res;

    char fpath[1024];


    if (strcmp(path, "/") == 0)

    {

        path = dir;

        sprintf(fpath, "%s", path);

    }

    else

    {

        sprintf(fpath, "%s%s", dir, path);

    }


    struct timeval tv[2];


    tv[0].tv_sec = ts[0].tv_sec;

    tv[0].tv_usec = ts[0].tv_nsec / 1000;

    tv[1].tv_sec = ts[1].tv_sec;

    tv[1].tv_usec = ts[1].tv_nsec / 1000;


    res = utimes(fpath, tv);


    const char *desc[] = {path};

    filelog("INFO", "UTIMENSAT", desc, 1);


    if (res == -1)

        return -errno;

    return 0;

}


static int xmp_open(const char *path, struct fuse_file_info *fi)

{

    int res;

    char fpath[1024];


    if (strcmp(path, "/") == 0)

    {

        path = dir;

        sprintf(fpath, "%s", path);

    }

    else

    {

        sprintf(fpath, "%s%s", dir, path);

    }


    res = open(fpath, fi->flags);


    const char *desc[] = {path};

    filelog("INFO", "OPEN", desc, 1);


    if (res == -1)

        return -errno;


    close(res);

    return 0;

}


static int xmp_read(const char *path, char *buf, size_t size, off_t offset, struct fuse_file_info *fi)

{

    char fpath[1024];


    if (strcmp(path, "/") == 0)

    {

        path = dir;

        sprintf(fpath, "%s", path);

    }

    else

    {

        sprintf(fpath, "%s%s", dir, path);

    }


    int res = 0;

    int fd = 0;


    (void)fi;


    const char *desc[] = {fpath};

    filelog("INFO", "READ", desc, 1);


    fd = open(fpath, O_RDONLY);

    if (fd == -1)

        return -errno;


    res = pread(fd, buf, size, offset);

    if (res == -1)

        res = -errno;


    close(fd);

    return res;

}


static int xmp_write(const char *path, const char *buf, size_t size, off_t offset, struct fuse_file_info *fi)

{

    int fd;

    int res;

    char fpath[1024];


    if (strcmp(path, "/") == 0)

    {

        path = dir;

        sprintf(fpath, "%s", path);

    }

    else

    {

        sprintf(fpath, "%s%s", dir, path);

    }


    (void)fi;


    const char *desc[] = {path};

    filelog("INFO", "WRITE", desc, 1);


    fd = open(fpath, O_WRONLY);

    if (fd == -1)

        return -errno;


    res = pwrite(fd, buf, size, offset);

    if (res == -1)

        res = -errno;


    close(fd);

    return res;

}


static int xmp_statfs(const char *path, struct statvfs *stbuf)

{

    int res;

    char fpath[1024];


    if (strcmp(path, "/") == 0)

    {

        path = dir;

        sprintf(fpath, "%s", path);

    }

    else

    {

        sprintf(fpath, "%s%s", dir, path);

    }


    res = statvfs(fpath, stbuf);


    const char *desc[] = {path};

    filelog("INFO", "STATFS", desc, 1);


    if (res == -1)

        return -errno;

    return 0;

}


static int xmp_create(const char *path, mode_t mode, struct fuse_file_info *fi)

{

    int res;

    char fpath[1024];


    if (strcmp(path, "/") == 0)

    {

        path = dir;

        sprintf(fpath, "%s", path);

    }else

    {

        sprintf(fpath, "%s%s", dir, path);

    }


    (void)fi;


    const char *desc[] = {path};

    filelog("INFO", "CREATE", desc, 1);


    res = creat(fpath, mode);

    if (res == -1)

        return -errno;


    close(res);

    return 0;

}





static struct fuse_operations xmp_oper = {
.getattr = xmp_getattr,
.access = xmp_access,
.readlink = xmp_readlink,
.readdir = xmp_readdir,
.mknod = xmp_mknod,
.mkdir = xmp_mkdir,
.symlink = xmp_symlink,
.unlink = xmp_unlink,
.rmdir = xmp_rmdir,
.rename = xmp_rename,
.link = xmp_link,
.chmod = xmp_chmod,
.chown = xmp_chown,
.truncate = xmp_truncate,
.utimens = xmp_utimens,
.open = xmp_open,
.read = xmp_read,
.write = xmp_write,
.statfs = xmp_statfs,
.create = xmp_create,
};


int main(int argc, char *argv[])

{

umask(0);

return fuse_main(argc, argv, &xmp_oper, NULL);

}

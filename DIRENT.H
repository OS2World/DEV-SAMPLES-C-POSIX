/*

    POSIX directory stuff for Cset/2

    by Joe DeRosa 3/22/93 (1:141/42@fidonet.org)

    released in the public domain

    if you use this code you accept ALL responsibility for anything that
    happens.
*/

#define INCL_DOSFILEMGR
#include <os2.h>

#ifndef NULL
#define NULL 0
#endif

struct dirent
{
    char	*d_name;
};

typedef struct
{
    FILEFINDBUF3  *fb;
    HDIR	  dh;
    struct dirent d_dirent;
    char	  *d_dirname;
    char	  d_entnum;
    unsigned char d_valid;
} DIR;


DIR	      *opendir (char  *dname);
struct dirent *readdir (DIR  *dir);
int	      closedir (DIR  *dir);
void	      rewinddir(DIR  *dir);
long	      telldir(DIR *dir);
void	      seekdir(DIR *dir,long loc);


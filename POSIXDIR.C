/*
    POSIX directory stuff for Cset/2

    by Joe DeRosa 3/22/93 (1:141/42@fidonet.org)

    released in the public domain

    if you use this code you accept ALL responsibility for anything that
    happens.
*/

#define  INCL_DOSFILEMGR
#include <os2.h>
#include <dirent.h>
#include <string.h>
#include <stdlib.h>

#define DIRVALID 0x42
#define ALLFILES FILE_ARCHIVED | FILE_DIRECTORY | FILE_SYSTEM | FILE_HIDDEN | FILE_READONLY

DIR *opendir (char *dirname)
{
   char *name,*cp;
   DIR *dir;
   int length;
   ULONG count;

   length = strlen (dirname);
   if ((name = malloc (length + 3)) == NULL)
      return NULL;

   strcpy (name, dirname);

   if (length-- && name[length] != ':' && name[length] != '\\' && name[length] != '/')
      strcat (name, "\\*");
   else
      strcat (name, "*");


   if ((dir = malloc (sizeof (DIR))) == NULL) {
      free (name);
      return NULL;
   }

   if ((dir->fb = malloc (sizeof (FILEFINDBUF3))) == NULL) {
      free (name);
      free (dir);
      return NULL;
   }

   dir->dh = HDIR_CREATE;
   count = 1;

   if (DosFindFirst (name,&dir->dh,ALLFILES,dir->fb, sizeof (FILEFINDBUF3), &count, FIL_STANDARD) != 0) {
      free (name);
      free (dir->fb);
      free (dir);
      return NULL;
   }
   dir->d_dirname = name;
   dir->d_entnum = 1;
   dir->d_valid = DIRVALID;
   dir->d_dirent.d_name = dir->fb->achName;

   return dir;
}



void rewinddir (DIR * dir)
{
   ULONG count;

   if (dir->d_valid != DIRVALID)
      return;

   DosFindClose (dir->dh);

   dir->dh = HDIR_CREATE;
   count = 1;

   DosFindFirst (dir->d_dirname,&dir->dh,ALLFILES,
		 dir->fb, sizeof (FILEFINDBUF3), &count, FIL_STANDARD);

   dir->d_entnum = 1;
}



struct dirent *readdir (DIR * dir)
{

   ULONG count;

   if (dir->d_valid != DIRVALID)
      return NULL;

   count = 1;

   if (dir->d_entnum != 1)
      if (DosFindNext (dir->dh, dir->fb, sizeof (FILEFINDBUF3), &count) != 0)
	 return NULL;


   dir->d_entnum++;

   return &dir->d_dirent;
}



int closedir (DIR * dir)
{
   if (dir == NULL || dir->d_valid != DIRVALID)
      return (-1);


   dir->d_valid = 0;
   DosFindClose (dir->dh);
   free (dir->d_dirname);
   free (dir->fb);
   free (dir);

   return 0;
}



long telldir (DIR * dir)
{
   if (dir->d_valid != DIRVALID)
      return -1;

   return dir->d_entnum;
}



void seekdir (DIR * dir, long pos)
{
   if (dir->d_valid != DIRVALID)
      return;
   rewinddir (dir);
   while (readdir (dir) && dir->d_entnum != pos);
}

#ifdef TEST

#include <stdio.h>
main ()
{
   DIR *d;
   struct dirent *de;

   d = opendir ("c:/Os2/.");
   if (d) {
      while ((de = readdir (d)) != 0)
	 printf ("Entry %ld is %s\n", telldir (d) - 1, de->d_name);

      seekdir (d, 15);
      while ((de = readdir (d)) != 0)
	 printf ("Entry %ld is %s\n", telldir (d) - 1, de->d_name);

      rewinddir (d);
      while ((de = readdir (d)) != 0)
	 printf ("Next is %s\n", de->d_name);

      closedir (d);
   }
}
#endif /* TEST */

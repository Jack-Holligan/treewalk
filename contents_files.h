//
//  contents.h
//  
//
//  Created by Jack Holligan on 02/12/2019.
//

#ifndef contents_files_h
#define contents_files_h

#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>
#include <time.h>
#include <stdio.h>

void contents_files() // List files ONLY.
{
    DIR *d;
    struct dirent *dir;
    struct stat filestat;
    char *base;
    d = opendir(".");
    if (d)
    {
        while ( (dir = readdir(d)) )
        {
            stat(dir->d_name, &filestat);
            if (S_ISDIR(filestat.st_mode)) continue; //List only non-directories.
            base=pwd();
            printf("%s/%s\n", base, dir->d_name);
        }
        closedir(d);
    }
}

#endif /* contents_files_h */

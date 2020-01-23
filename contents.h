//
//  contents.h
//  
//
//  Created by Jack Holligan on 06/12/2019.
//

#ifndef contents_h
#define contents_h

#include <sys/stat.h>

unsigned long number_of_elements(char **array)

{
    if (array==NULL)
    {
        return 0;
    }
    else
    {
        int n=-1;
        while (array[++n]!=NULL){}
        
        return n;
    }
}

void contents_dir() // List directories ONLY.
{
    DIR *d;
    struct dirent *dir;
    struct stat filestat;
    d = opendir(".");
    if (d)
    {
        while ((dir = readdir(d)))
        {
            stat(dir->d_name, &filestat);
            if(!(S_ISDIR(filestat.st_mode)) || strcmp(dir->d_name, ".") == 0 || strcmp(dir->d_name, "..") == 0) continue;
            printf("%s/%s\n", pwd(), dir->d_name);
        }
        closedir(d);
    }
}

int number_of_dirs()
{
    int n=0;
    DIR *d;
    struct dirent *dir;
    struct stat filestat;
    d = opendir(".");

    //Determine the number of directories
    while((dir = readdir(d)) != NULL)
    {
        stat(dir->d_name, &filestat);
        if (!(S_ISDIR(filestat.st_mode)) || strcmp(dir->d_name, ".")==0 || strcmp(dir->d_name, "..")==0 ) continue;
            n++;
    }
    rewinddir(d);
    return n;
}

char** directories(char **filesList) //Appends all directories in "." to an array supplied in the argument. Doesn't work yet.
{
    int i=number_of_elements(filesList);
    DIR *d;
    struct dirent *dir;
    
    struct stat filestat;
    d = opendir(".");
    int n = number_of_dirs();
    printf("Contains %d, adding %d:\n",i,n);
    filesList = (char **) realloc (*filesList, sizeof (char*)*(i+n));
    //Put file names into the array
    while((dir = readdir(d)) != NULL)
    {
        stat(dir->d_name, &filestat);
        if (!(S_ISDIR(filestat.st_mode)) || !strcmp(dir->d_name, ".") || !strcmp(dir->d_name, "..") ) continue;
        filesList[i]=dir->d_name;
        printf("%s/%s\n",pwd(),dir->d_name);
        i++;
        printf("There are now %lu elements.\n", number_of_elements(filesList));
    }
    
    return filesList;
}


void contents_files_from(char *path,int threadno) // List files ONLY.
{
    DIR *d;
    struct dirent *dir;
    struct stat filestat;
    //char *base;
    printf("%d %s\n",threadno, path);
    d = opendir(path);
    if (d)
    {
        int cnt=0;
        while ( (dir = readdir(d))!=NULL )
        {
            cnt++;
            stat(dir->d_name, &filestat);
            if (strcmp(dir->d_name, ".")==0 || strcmp(dir->d_name, "..")==0) continue;
            //if (S_ISREG(filestat.st_mode)){ //List only non-directories.
                //base=pwd();
                printf("%d %d %d %d %s/%s\n", threadno, filestat.st_mode, filestat.st_mode&S_IFMT, S_IFREG, path, dir->d_name);
            //}
        }
        printf("%d read %d entries\n", threadno, cnt);
        closedir(d);
    }
    else
    {
        printf("%d unable to open path %s\n", threadno, path);
    }
}

#endif /* contents_h */

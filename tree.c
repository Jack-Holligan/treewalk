//set stuff up (obviously, error checking needed).
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <unistd.h>
#include <errno.h>
#include <stdio.h>
#include <sys/stat.h>
#include <apr-1/apr.h>
#include <apr-1/apr_thread_pool.h>
//include <lustreapi.h>
#include "pwd.h"
#include "contents.h"
#include "contents_files.h"

typedef struct{
apr_thread_pool_t *threadPool;
apr_pool_t *pool;
 char *curdir;
} ThreadInfo;

//push tasks
//char *curdir="/path/to/dir/";

//apr_thread_pool_push(threadPool,scanDirFunc,(void*)curdir,1,NULL);
/*
where scanDirFunc is a function that will scan through the param (curdir),
and print file names, and call apr_thread_pool_push for any directories.

//e.g.
void *scanDirFunc(apr_thread_t *thread,void *curdirptr, int threadno){
  char *curdir=(char*)curdirptr;
  //switching to python for clarity - though you'd need to write in C...
  contents_files_from(curdirptr, threadno)
    //files=listDirectory(curdir);
  for file in files:
    if IS_FILE(os.stat(file)):
      printf("%s\n",file);
    else
      apr_thread_pool_push(threadPool,scanDirFunc,(void*)file,1,NULL);
  return NULL;
}
*/

void *scanDirFunc(apr_thread_t *thread, void *threadInfoptr)
{
    DIR *d;
    struct dirent *dir;
    struct stat filestat;
    //struct statx filestat;
    ThreadInfo *threadInfo=(ThreadInfo*)threadInfoptr;
    char *curdir=threadInfo->curdir;
    ThreadInfo *newThreadInfo=NULL;
    int lcurdir=strlen(curdir);
    char *fname;
    d=opendir(curdir);
    if (d)
    {
        while ((dir=readdir(d)))
        {
	  int ldir=strlen(dir->d_name);
	  fname=malloc(lcurdir+2+ldir);
	  memcpy(fname,curdir,lcurdir);
	  fname[lcurdir]='/';
	  memcpy(&fname[lcurdir+1],dir->d_name,ldir+1);
	  if(stat(fname, &filestat)!=0)
	  //if(statx(AT_FDCWD,fname,0,STATX_TYPE|STATX_ATIME &filestat)!=0)
	    {
	      printf("Error statting %s: %s\n",fname,strerror(errno));
	      free(fname);
	    }
	  else if(strcmp(dir->d_name, ".") != 0 && strcmp(dir->d_name, "..") != 0)
		{
		  printf("%s\n",fname);
		  if(S_ISDIR(filestat.st_mode))// && strcmp(dir->d_name, ".") != 0 && strcmp(dir->d_name, "..") != 0)
		    {
		      printf("Putting into pool: %s\n",fname);
		      newThreadInfo=malloc(sizeof(ThreadInfo));
		      newThreadInfo->pool=threadInfo->pool;
		      newThreadInfo->threadPool=threadInfo->threadPool;
		      newThreadInfo->curdir=fname;
		      if(apr_thread_pool_push(threadInfo->threadPool, scanDirFunc, (void*)newThreadInfo, 1, NULL)!=APR_SUCCESS){
			printf("Error pushing work for %s\n",fname);
			free(fname);
		      }
		    }
		  else
		    {
		      free(fname);
		    }
		  printf("\tNo. of threads active: %ld\n",apr_thread_pool_busy_count(threadInfo->threadPool));
		  printf("\tNo. of tasks awaiting completion: %ld\n", apr_thread_pool_tasks_count(threadInfo->threadPool));
		}
	}
    }
    //todo: clean up memory upon task exit.
    free(threadInfo->curdir);
    free(threadInfo);
    return NULL;
}
    
int main(int argc, char **argv)
{
    char *curdir = NULL;
    ThreadInfo *threadInfo;
    apr_thread_pool_t *threadPool;
    apr_pool_t *pool;
    threadInfo=malloc(sizeof(ThreadInfo));

    if (argc==1)
    {
      curdir=pwd();//".";
    }
    else
    {
        curdir=argv[1];
    }

    threadInfo->curdir=strdup(curdir);
    apr_initialize();
    apr_pool_create(&pool, NULL);
    apr_thread_pool_create(&threadPool,10,20,pool);
    threadInfo->pool=pool;
    threadInfo->threadPool=threadPool;
    apr_thread_pool_push(threadInfo->threadPool,scanDirFunc,(void*)threadInfo,1,NULL);
    
    while(apr_thread_pool_busy_count(threadPool) != 0 && apr_thread_pool_tasks_count(threadPool) != 0)
      {
	sleep(1);
      }
    //printf("%s\n", pwd());
    //todo: quit once work has all been done.
    printf("Walk complete. Destroying pool.\n");
    apr_thread_pool_destroy(threadPool);
    apr_pool_destroy(pool);
    apr_terminate();
    return 0;
}

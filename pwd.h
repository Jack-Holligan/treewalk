//
//  pwd.h
//  
//
//  Created by Jack Holligan on 06/12/2019.
//

#ifndef pwd_h
#define pwd_h

char* pwd()
{
    long size;
    char *buf;
    char *ptr;

    size = pathconf(".", _PC_PATH_MAX)+1;
    
    buf = malloc((size_t)size);

    if (buf == NULL)
    {
        perror ("Couldn't determine current directory");
        exit(1);
    }
    else
    {
        ptr = getcwd(buf, (size_t)size);
        return ptr;
    }
}

#endif /* pwd_h */

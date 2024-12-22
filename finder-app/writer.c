#include<syslog.h>
#include<stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <stdlib.h>

int writeToFile(char *argv[])
{
    char *writePath = argv[1];
    char *writeStr = argv[2];
    printf("writePath = %s\n",writePath);
    printf("write string = %s\n",writeStr);
    syslog(LOG_USER|LOG_DEBUG,"Opening File");
    int fd = open(writePath, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH);
    if (fd == -1)
    {
        syslog(LOG_ERR,"Error in opening file.");
        return 1;
    }
    else
    {
        syslog(LOG_DEBUG,"File opened successfully");
    }

    ssize_t wr;
    ssize_t strSize = strlen(writeStr);

    syslog(LOG_DEBUG,"Writing string %s to file %s",argv[2],argv[1]);


    wr = write(fd,writeStr,strlen(writeStr));
    printf("WR size = %d  strSize = %d\n",wr,strSize);

    if(wr == -1)
    {
        syslog(LOG_ERR,"Error in writing string to file");
        return 1;
    }

    if(wr != strSize)
    {
        syslog(LOG_ERR,"Possible error in writing string to file but wr not set to error number -1");
        return 1;
    }

    else
    {
        syslog(LOG_DEBUG,"String written to file successfully");
    }

    if (close(fd) == 01)
    {
        syslog(LOG_ERR,"Error in closing file");
        return 1;
    }

    else
    {
        syslog(LOG_DEBUG,"Closed file successfully");
    }  

    return 0;

}

int main(int argc, char *argv[])
{

    openlog ("writerUtility", LOG_PID | LOG_CONS | LOG_NOWAIT,LOG_USER);
    printf("argc = %d\n",argc);
    if (argc !=3)
    {
        syslog(LOG_ERR,"Arguments passed incorrectly");
        exit(EXIT_FAILURE);
    }

    
    
    
    int writeStatus = writeToFile(argv);
    return writeStatus;

}
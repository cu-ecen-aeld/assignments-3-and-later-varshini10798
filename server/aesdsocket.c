#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>
#include <sys/file.h>
#include <syslog.h>




//#include "Practical.h"
static const int MAXPENDING = 4096;
#define PORT "9000"
#define RCVBUFSIZE 8192
#define MAX_WIRE_SIZE 8192

static const char DELIMITER = '\n';
char shutdownFlag = 0;
char shutdownSuccess = 0;
int sockNum = -1;
char *filename = "/var/tmp/aesdsocketdata";

// struct addrinfo {
//     int ai_flags; // AI_PASSIVE, AI_CANONNAME, ...
//     int ai_family; // AF_xxx
//     int ai_socktype; // SOCK_xxx
//     int ai_protocol; // 0 (auto) or IPPROTO_TCP, IPPROTO_UDP
//     socklen_t ai_addrlen; // length of ai_addr
//     char *ai_canonname; // canonical name for nodename
//     struct sockaddr *ai_addr; // binary address
//     struct addrinfo *ai_next; // next structure in linked list
//     };

    void PrintSocketAddress(const struct sockaddr *address, FILE *stream, char SysLogFlag) {
        // Test for address and stream
        if (address == NULL || stream == NULL)
        return;
        void *numericAddress; // Pointer to binary address
        // Buffer to contain result (IPv6 sufficient to hold IPv4)
        char addrBuffer[INET6_ADDRSTRLEN];
        in_port_t port; // Port to print
        // Set pointer to address based on address family
        switch (address->sa_family) {
        case AF_INET:
        numericAddress = &((struct sockaddr_in *) address)->sin_addr;
        port = ntohs(((struct sockaddr_in *) address)->sin_port);
        break;
        case AF_INET6:
        numericAddress = &((struct sockaddr_in6 *) address)->sin6_addr;
        port = ntohs(((struct sockaddr_in6 *) address)->sin6_port);
        break;
        default:
        fputs("[unknown type]", stream);
        // Unhandled type
        return;
        }
        // Convert binary to printable address
        if (inet_ntop(address->sa_family, numericAddress, addrBuffer,
        sizeof(addrBuffer)) == NULL)
        fputs("[invalid address]", stream); // Unable to convert
        else {
        fprintf(stream, "%s", addrBuffer);
        if(SysLogFlag == 1)
        {
            syslog(LOG_INFO,"Accepted Connection from %s",addrBuffer);
        }

        if(SysLogFlag == 2)
        {
            syslog(LOG_INFO,"Closed Connection from %s",addrBuffer);
        }
        if (port != 0)
        // Zero not valid in any socket addr
        fprintf(stream, "-%u", port);
        }
        }

        void shutdownSocket(int sockNo)
        {
            //shutdown(sockNo,SHUT_RDWR);
            //close(sockNo);
            printf("Shutdown socket\n");
            if (access(filename,F_OK))
            {
            if (remove(filename) == 0)
                printf("The file %s was deleted.\n", filename);
            else
                perror("could not delete file\n");
            }
        }

void catchSignal(int signo)
{
    printf("Signal received %d\n",signo);
    // if (sockNum!= -1)
    // {
    //     shutdownSocket(sockNum);
    // }
    //exit(EXIT_SUCCESS);
    exit(0);
}

//int GetNextMsg(FILE *in, uint8_t *buf, size_t bufSize) 
int GetNextMsg(FILE *in, char **buf, size_t bufSize, char **outBuf)
{
    int count = 0;

    
    

    // if(buf==NULL)
    //     {
    //         printf("buf null");
    //         return -1;
    //     }

        // if(*outBuf==NULL)
        // {
        //     printf("buf null");
        //     return -1;
        // }
    printf("within function");
    int nextChar;
    while (count < bufSize) 
    {
    
    nextChar = getc(in);
    if (nextChar == EOF) 
        {
        if (count > 0)
            //DieWithUserMessage("GetNextMsg()", "Stream ended prematurely");
            perror("GetNextMsg() Stream ended prematurely");
        else
            return -1;
        }   
    if (nextChar == DELIMITER)
    //if((char)nextChar == DELIMITER)
        //printf("breaking from loop");
        break;

    (*buf)[count++] = nextChar;

    if((count == bufSize) && (nextChar != DELIMITER))  
    {
        printf("increasing size\n");
        bufSize +=MAX_WIRE_SIZE;
        printf("new buf size = %lu",bufSize);
        *buf = realloc(*buf,bufSize*sizeof(uint8_t));
        //char *tmp = realloc(buf,bufSize*sizeof(char));
        // if(tmp != NULL)
        // {
        //     free(buf);
        //     buf = tmp;
        // }
        //free(tmp);
    }
    
    }
    if (nextChar != DELIMITER) 
    //if ((char)nextChar != DELIMITER) 
    { // Out of space: count==bufSize
        printf("in this case\n");
        return -count;
    } 
    else 
    { // Found delimiter
        printf("bufSize %lu count %d\n", bufSize,count);
        printf("returning count\n");
        //buf = realloc(buf,count*sizeof(uint8_t));
        *buf = realloc(*buf,count*sizeof(char));
        //*outBuf = realloc(*outBuf,count*sizeof(char)+1);
        *outBuf = malloc(count*sizeof(char));

        if(*buf==NULL)
        {
            printf("buf null");
            return -1;
        }

        if(*outBuf==NULL)
        {
            printf("buf null");
            return -1;
        }

        //*outBuf = *buf;

        
        // char *tmp1 = realloc(buf,count*sizeof(char) + 1);
        // char *tmp2 = realloc(bufOut,count*sizeof(char) + 1);
        // //bufOut = realloc(bufOut,count*sizeof(char) + 1);
        // //bufOut = buf;
        // // if(bufOut == NULL)
        // // {
        // //     printf("bufout null");
        // // }
        // if(tmp1 != NULL)
        // {
        //     //free(buf);
        //     buf = tmp1;
        //     //free(tmp1);
        //     //bufOut = tmp1;
            
        // }

        // if(tmp2 != NULL)
        // {
        //     tmp2 = tmp1;
        //     //free(bufOut);
        //     bufOut = tmp2;
        //     //free(tmp2);
        //     //bufOut = tmp1;
            
        // }
        for (int i=0;i<count;i++)
            {
                (*outBuf)[i] = (*buf)[i];
            }
        free(*buf);
        //printf("buf out from function = %s",bufOut);
        //free(tmp1);
        return count;
    }
}
    /* Write the given message to the output stream, followed by
    * the delimiter. Return number of bytes written, or -1 on failure.
    */
//int PutMsg(uint8_t buf[], size_t msgSize, FILE *out, FILE *sockFile) {
    int PutMsg(char *buf, size_t msgSize, FILE *out, FILE *sockFile,char **outBuf) {
// Check for delimiter in message
    int i = 0;
    //printf("in put msg output buff %s",*outBuf);
    
    // for (i = 0; i < msgSize; i++)
    // {
    //     if (buf[i] == DELIMITER)
    //     //if ((char)buf[i] == DELIMITER)
    //     {
    //      printf("in this error %d %lu %d\n",i,msgSize,(int)buf[i]);   
    //      fwrite(buf, 1, i, sockFile);
    //      fflush(sockFile);
    //      //return -1;
    //     }
    // }
    //if (fwrite(buf, 1, msgSize, out) != msgSize)
    //return -1;

    if (fwrite(*outBuf, 1, msgSize, sockFile) != msgSize)
    {
    printf("in this size error\n");
    return -1;
    }

    //fputc(DELIMITER, out);
    fputc(DELIMITER,sockFile);
    //fflush(out);
    fflush(sockFile);
    
    return msgSize;
    
}

void handleTCPConnection(int sockfd)
{
    printf("server: waiting for connections...\n");

        int new_fd;

        while(1) 
        
        { 
        printf("back at the start\n");
        struct sockaddr_in clntAddr; 
        unsigned int clntAddrLen = sizeof(clntAddr); 
        struct sockaddr_in echoClntAddr; /* Client address */
        unsigned int clntLen; /* Length of client address data structure */
        
        // if (shutdownFlag == 1)
        // {
        //     shutdown(sockfd,SHUT_RDWR);
        //     close(sockfd);

        //     shutdownSuccess = 1;
        //     printf("Shutdown success %d", shutdownSuccess);
        //     break;
        //     //shutdownFlag = 0;
        // }

        new_fd = accept(sockfd, (struct sockaddr *) &echoClntAddr,&clntLen);
        PrintSocketAddress((struct sockaddr *) &echoClntAddr, stdout,1);
        printf("newfd = %d\n",new_fd);

        if (new_fd == -1) {
                perror("accept");
                continue;
            }
        
        FILE *channel = fdopen(new_fd, "a+");
        FILE *sockFilePtr = fopen("/var/tmp/aesdsocketdata","a+");
        int sockFid = fileno(sockFilePtr);
        FILE *sockFile = fdopen(sockFid,"a+");

        if (channel == NULL)
        {
            perror("fdopen() failed");
        }

        int mSize;
        //uint8_t inBuf[MAX_WIRE_SIZE];

        char* inBuf = (char*)malloc(MAX_WIRE_SIZE*sizeof(char));
        char* outBuf = NULL;

        while ((mSize = GetNextMsg(channel, &inBuf, MAX_WIRE_SIZE,&outBuf)) > 0) 
        {

            
            printf("Received message (%d bytes)\n", mSize);
            //uint8_t outBuf[MAX_WIRE_SIZE];
            if (PutMsg(inBuf, mSize, channel,sockFile,&outBuf) < 0) 
                {   
                fputs("Error framing/outputting message\n", stderr);
                break;
                } 
            else {
                printf("Processed output.\n");
                
                //(v.isInquiry ? "inquiry" : "vote"), v.candidate, v.count);
                }
            //     ssize_t numBytesSent = send(new_fd, inBuf, mSize, 0);

            // if (numBytesSent < 0)
            // {
            //     perror("send() failed");
            // }
            // else if (numBytesSent != mSize)
            // {
            //     perror("send() sent unexpected number of bytes");
            // }
            // int bytes_read;
            // while ((bytes_read = read(sockFilePtr, outBuf, sizeof(outBuf))) > 0)    
            //     {
            //         if (send(new_fd,outBuf,bytes_read,0) < 0)
            //         {
            //             perror("send");
            //             break;
            //         }
            //     }
            FILE *fp;
            long file_size;
            char *buffer;
            
            fp = fopen("/var/tmp/aesdsocketdata", "r");
            fseek(fp, 0, SEEK_END);
            file_size = ftell(fp);
            rewind(fp); //move the control to the file's beginning
    
    // Allocate memory for the char array
            buffer = (char*) malloc(file_size + 1);
            fread(buffer, file_size, 1, fp);
            buffer[file_size] = '\0';

            if(send(new_fd,buffer,file_size,0) < 0)
            {
                perror("Send");
            }

            
            fflush(channel);
                } 
                
                puts("Client finished");
                fclose(channel);
                PrintSocketAddress((struct sockaddr *) &echoClntAddr, stdout,2);
                // free(inBuf); free(outBuf);
                
        }
    


}



int main(int argc, char *argv[]) 
{
    char daemonMode = 0;

    if(argv[1]!=NULL)
    {
    if (strcmp(argv[1],"-d") == 0)
    {
        daemonMode = 1;
    }
}
    if (argc != 2) 
    {
        printf("Insufficient arguments\n");
    }

    //char *filename = "/var/tmp/aesdsocketdata";

    if (remove(filename) == 0)
        printf("The file %s was deleted.", filename);
    else
        perror("could not delete file");

    char *service = "9000";

    int servSock; /* Socket descriptor for server */
    int clntSock; /* Socket descriptor for client */
    struct sockaddr_in echoServAddr; /* Local address */
    struct sockaddr_in echoClntAddr; /* Client address */
    memset(&echoClntAddr,0,sizeof(echoClntAddr));
    unsigned short servPort; /* Server port */
    unsigned int clntLen = 0; /* Length of client address data structure */
    
    struct sigaction handler;
    handler.sa_handler = catchSignal; 

    if (sigfillset(&handler.sa_mask) < 0) // Block everything in handler
        perror("sigfillset() failed");
    handler.sa_flags = 0;

    if (sigaction(SIGINT, &handler, 0) < 0)
        perror("sigaction() failed for SIGINT");
    if (sigaction(SIGTERM, &handler, 0) < 0)
        perror("sigaction() failed for SIGKILL");

    struct addrinfo hints, *servinfo, *p;
    int rv;

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET; // use AF_INET6 to force IPv6
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
    hints.ai_flags = AI_PASSIVE;

    if ((rv = getaddrinfo(NULL, "9000", &hints, &servinfo)) != 0) 
    {
    fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
    exit(1);
    }
    //struct addrinfo *addrList;
    for (struct addrinfo *addr = servinfo; addr != NULL; addr = addr->ai_next) {
        PrintSocketAddress(addr->ai_addr, stdout,0);
        fputc('\n', stdout);
        }

    int sockfd = -1;
    for(p = servinfo; p != NULL; p = p->ai_next) {
		if ((sockfd = socket(p->ai_family, p->ai_socktype,
				p->ai_protocol)) < 0) {
			printf("server: socket");
            return -1;
			//continue;
		}
        // else
        // {
        //     printf("socket error In this condition");
        //     return -1;
        // }

		// if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes,
		// 		sizeof(int)) == -1) {
		// 	perror("setsockopt");
		// 	exit(1);
		// }

		// if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
		// 	close(sockfd);
		// 	perror("server: bind");
		// 	continue;
		// }

        // printf("server bind done\n");

        // if (listen(sockfd, MAXPENDING) < 0)
        // {
        // perror("listen failed!");
        //  }   

        // printf("server listen done\n");

        //  break;

        

       

        if((bind(sockfd, p->ai_addr, p->ai_addrlen) == 0) && (listen(sockfd, MAXPENDING) == 0) )
        {
            struct sockaddr_storage localAddr;
            socklen_t addrSize = sizeof(localAddr);
            if (getsockname(sockfd, (struct sockaddr *) &localAddr, &addrSize) < 0)
            //perror("getsockname() failed");
            printf("getsockname() failed");
            fputs("Binding to ", stdout);
            PrintSocketAddress((struct sockaddr *) &localAddr, stdout,0);
            fputc('\n', stdout);
            break;
            // Bind and list successful
        }
        else
        {
            printf("bind error In this condition");
            return -1;
        }
        }
        sockNum = sockfd;

        //handleTCPConnection(sockfd)
	    //}

        if (daemonMode)
        {
            
                // pid_t processID = fork();
                // if (processID == 0)
                // {
                //     printf("In the child process\n");
                //     close(sockfd);
                    
                    
                // }
                switch (fork())
                {
                    case 0:
                        break;

                    default:
                        close(sockfd);
                        printf("exiting..");
                        return 0;

                    case -1:
                        perror("fork");
                        return 3;
                }

                    
                
                    
        }
            
            
    

        freeaddrinfo(servinfo);
    




        // servPort = atoi(argv[1]); 
        // if ((servSock = socket(AF_UNSPEC, SOCK_STREAM, IPPROTO_TCP)) < 0)
        // {
        //     perror("server: socket");
        // }

        // struct sockaddr_in servAddr; 
        // memset(&servAddr, 0, sizeof(servAddr));

        // servAddr.sin_family = AF_UNSPEC;
        // servAddr.sin_addr.s_addr = htonl(INADDR_ANY);
        // servAddr.sin_port = htons(servPort);

        // if (bind(servSock, (struct sockaddr*) &servAddr, sizeof(servAddr)) < 0)
        // {
        //     perror("bind: socket");
        // }

        // if (listen(servSock, MAXPENDING) < 0)
        // {
        //     perror("listen failed!");
        // }


        printf("server: waiting for connections...\n");

        int new_fd = -1;

        while(1) 
        
        { 
            printf("back at the start\n");
        //struct sockaddr_in clntAddr; 
        //unsigned int clntAddrLen = NULL;
        char addrBuffer[INET6_ADDRSTRLEN];

        // if (shutdownFlag == 1)
        // {
        //     shutdown(sockfd,SHUT_RDWR);
        //     close(sockfd);

        //     shutdownSuccess = 1;
        //     printf("Shutdown success %d", shutdownSuccess);
        //     return 1;
        //     //shutdownFlag = 0;
        // }

        new_fd = accept(sockfd, (struct sockaddr *) &echoClntAddr,&clntLen);
        PrintSocketAddress((struct sockaddr *) &echoClntAddr, stdout,1);

        

        //syslog(LOG_INFO,"Accepted Connection from %s",inet_ntop(echoClntAddr))
        //printf("newfd = %d\n",new_fd);

        if (new_fd == -1) {
                perror("accept");
                continue;
            }
        
        FILE *channel = fdopen(new_fd, "a+");
        FILE *sockFilePtr = fopen("/var/tmp/aesdsocketdata","a+");
        int sockFid = fileno(sockFilePtr);
        FILE *sockFile = fdopen(sockFid,"a+");

        if (channel == NULL)
        {
            perror("fdopen() failed");
        }

        int mSize = 0;
        //uint8_t inBuf[MAX_WIRE_SIZE];

        //uint8_t* inBuf = (uint8_t*)malloc(MAX_WIRE_SIZE*sizeof(uint8_t));
        //char* inBuf = (char*)malloc(MAX_WIRE_SIZE*sizeof(char));
        // char* inBuf = NULL;
         char* outBuf = NULL;
        char *buffer = NULL;
        char* inBuf = (char*)malloc(MAX_WIRE_SIZE*sizeof(char));
        //inBuf = (char*)malloc(MAX_WIRE_SIZE*sizeof(char));
        //char* outBuf = (char*)malloc(MAX_WIRE_SIZE*sizeof(char));
        if (inBuf == NULL)
        {
            printf("buffer nulll");
            return -1;
        }

        // if (outBuf == NULL)
        // {
        //     printf("output buffer nulll");
        //     return -1;
        // }

        while ((mSize = GetNextMsg(channel, &inBuf, MAX_WIRE_SIZE,&outBuf)) > 0) 
        {

            
            if (outBuf == NULL)
            {
                printf("output buffer nulll");
                return -1;
            }     //printf("Received message (%d bytes)\n", mSize);
            //printf("input buffer content = %s\n",inBuf);
            //printf("output buffer content = %s\n",outBuf);
            //free(outBuf);
            //outBuf = NULL;
            //uint8_t outBuf[MAX_WIRE_SIZE];
            if (PutMsg(inBuf, mSize, channel,sockFile,&outBuf) < 0) 
                {   
                fputs("Error framing/outputting message\n", stderr);
                break;
                } 
            else {
                printf("Processed output.\n");
                
                //(v.isInquiry ? "inquiry" : "vote"), v.candidate, v.count);
                }
                free(outBuf); 
                
                outBuf = NULL;

            //     ssize_t numBytesSent = send(new_fd, inBuf, mSize, 0);

            // if (numBytesSent < 0)
            // {
            //     perror("send() failed");
            // }
            // else if (numBytesSent != mSize)
            // {
            //     perror("send() sent unexpected number of bytes");
            // }
            // int bytes_read;
            // while ((bytes_read = read(sockFilePtr, outBuf, sizeof(outBuf))) > 0)    
            //     {
            //         if (send(new_fd,outBuf,bytes_read,0) < 0)
            //         {
            //             perror("send");
            //             break;
            //         }
            //     }
            FILE *fp;
            long file_size;
            
            
            fp = fopen("/var/tmp/aesdsocketdata", "r");
            fseek(fp, 0, SEEK_END);
            file_size = ftell(fp);
            rewind(fp); //move the control to the file's beginning
    
    // Allocate memory for the char array
            buffer = (char*) malloc(file_size + 1);
            if(buffer == NULL)
            {
                printf("file buffer null");
                return -1;
            }
            fread(buffer, file_size, 1, fp);
            buffer[file_size] = '\0';

            if(send(new_fd,buffer,file_size,0) < 0)
            {
                perror("Send");
            }

            
            fflush(channel);
            free(buffer);
                buffer = NULL;

                
                
                // free(inBuf); 
                // free(outBuf); 
                // free(buffer);
                // buffer = NULL;
                // inBuf = NULL;
                // outBuf = NULL;
                //inBuf = NULL; outBuf = NULL; 
                //inBuf = malloc(MAX_WIRE_SIZE*sizeof(char));
                } 
                //free(inBuf); 
                
                // inBuf = NULL; outBuf = NULL; 

                //free(buffer);
                //buffer = NULL;

                //free(outBuf); 
                
                //outBuf = NULL;
                 
                puts("Client finished");
                fclose(channel);
                PrintSocketAddress((struct sockaddr *) &echoClntAddr, stdout,2);
                
                // if(inBuf!=NULL)
                // {
                //     free(inBuf);

                //     inBuf = malloc(MAX_WIRE_SIZE*sizeof(char));
                // }
                // if(outBuf != NULL)
                // {
                // free(outBuf);
                // outBuf = NULL;
                // }
        }

        
    }

    
       



    // for (;;) 
    // {
    // struct sockaddr_in clntAddr; 
    // unsigned int clntAddrLen = sizeof(clntAddr);
    // if ((clntSock = accept(servSock, (struct sockaddr *) &echoClntAddr,&clntLen)) < 0)
    // {
    //     perror("accept failed!");
    // }

    // printf("Handling client %s\n", inet_ntoa(echoClntAddr.sin_addr));

//     char echoBuffer[RCVBUFSIZE];
//     int recvMsgSize;

//     if ((recvMsgSize = recv(clntSock, echoBuffer, RCVBUFSIZE, 0)) < O)
//     {
//         perror("receive failed!");
//     }

//     while (recvMsgSize > O)
//     {
//         if (send(clntSock, echoBuffer, recvMsgSize, O) != recvMsgSize)
//         DieWithError("send() failed");

//         if ((recvMsgSize = recv(clntSock, echoBuffer, RCVBUFSIZE, 0)) < O)
// DieWithError("recv() failed") ;
//     }




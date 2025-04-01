#include "threading.h"
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

// Optional: use these functions to add debug or error prints to your application
#define DEBUG_LOG(msg,...)
//#define DEBUG_LOG(msg,...) printf("threading: " msg "\n" , ##__VA_ARGS__)
#define ERROR_LOG(msg,...) printf("threading ERROR: " msg "\n" , ##__VA_ARGS__)

void* threadfunc(void* thread_param)
{

    // TODO: wait, obtain mutex, wait, release mutex as described by thread_data structure
    // hint: use a cast like the one below to obtain thread arguments from your parameter
    //struct thread_data* thread_func_args = (struct thread_data *) thread_param;
    printf("entered in thread func\n");
    
    int e; int flag = 0;
    struct thread_data* thread_Params;
    thread_Params = (struct thread_data*) thread_param;
    printf("sleep time %d\n",(thread_Params->waitObtainMs));
    usleep(thread_Params->waitObtainMs*1000);
    //printf("Sleeping\n");
    e = pthread_mutex_lock((thread_Params)->mutex);
    printf("locked mutex\n");
    if(e!=0)
    {
        flag = -1;
    }
    usleep(thread_Params->waitReleaseMs*1000);
    printf("release mutex delay finished\n");
    e = pthread_mutex_unlock((thread_Params)->mutex);
    printf("released mutex\n");
    if(e!=0)
    {
        flag = -1;
    }
    if(flag == -1)
    {
    thread_Params->thread_complete_success=false;
    }
    else
    {
        thread_Params->thread_complete_success=true;
    }
    

    return thread_param;
}


bool start_thread_obtaining_mutex(pthread_t *thread, pthread_mutex_t *mutex,int wait_to_obtain_ms, int wait_to_release_ms)
{
    /**
     * TODO: allocate memory for thread_data, setup mutex and wait arguments, pass thread_data to created thread
     * using threadfunc() as entry point.
     *
     * return true if successful.
     *
     * See implementation details in threading.h file comment block
     */
    printf("entered here\n");
    struct thread_data* thread_param;
    thread_param = malloc(sizeof(struct thread_data));
    if (thread_param == NULL) {
        printf("malloc failed\n");
        perror("malloc() failed");
        return false;
    }
    thread_param->mutex = mutex;
    thread_param->waitObtainMs = wait_to_obtain_ms;
    thread_param->waitReleaseMs = wait_to_release_ms;
    
    thread_param->thread_complete_success=false;
    printf("sleep obtain ms =%d\n",(*thread_param).waitObtainMs);
    int s = pthread_create(thread,NULL,threadfunc,(void*)thread_param);
    printf("s %d\n",s);
    // sleep(1);
    //printf("[MAIN:%ld]: Thread allocated \n",pthread_self());
    // int *ptr_output_data;
    //pthread_join(*thread,NULL);
    //pthread_mutex_destroy(mutex);
    // printf("[MAIN:%ld]: Thread returns \n",pthread_self());
    //printf("%d\n",(struct thread_data*)s)
    if (s==0)
    {

        return true;

    }
   
    //thread_param->thread_complete_success=true;
    // if(thread_param->thread_complete_success)
    // {
    //     //free(thread_param);
    //     return true;
    // }
        
    

    //free(thread_param);
    return false;
}




#include "main.h"

#define PRIO 0
#define CHANNEL_NAME    "/main"

char buftemp[256] = {':',':','/', 0};

pthread_t thread = 0;
int status;
struct mq_attr rq;
struct mq_attr mq; 
mqd_t rlow = 0;
mqd_t flow = 0;

void kill_mq(void){
    if(rlow) {
        mq_close(rlow);
        mq_unlink(buftemp+2);
    }
    if(flow){
        mq_close(flow);
        //mq_unlink(CHANNEL_NAME);
    } 
}

void errExit(const char* str){
    perror(str);
    if(thread) pthread_cancel(thread);
    exit(EXIT_FAILURE);
}

void* receive(void* argv){
    char rcv[256];
    
    rq.mq_maxmsg = 10;
    rq.mq_msgsize = 256;
    
    rlow = mq_open((buftemp + 2) , O_CREAT | O_RDWR, 0666, &rq);

    if(rlow == (mqd_t) -1){
        kill_mq();
        errExit((buftemp + 2));
    }

    if(mq_getattr(rlow, &rq)){
        kill_mq();
        errExit("mq_getattr");
    }

    while(1){
        for(int i = 0; i < sizeof rcv; i++){
            rcv[i] = 0;
        }
        ssize_t sz = mq_receive(rlow, rcv, sizeof(rcv), PRIO);
        write(STDOUT_FILENO, rcv, sz);
        if(sz == (ssize_t) -1){
            kill_mq();
            errExit("mq_receive");
        }
    }
    return EXIT_SUCCESS;
}

int main(int argc, char* argv[]){
    mq.mq_maxmsg = 10;
    mq.mq_msgsize = 256;

    flow = mq_open(CHANNEL_NAME, O_CREAT | O_RDWR, 0666, &mq); //temporary name

    printf("enter nickname: ");

    fgets((buftemp + 3), 256 - 3, stdin);

    for(int i = 0; i < 256; i++){
        if(buftemp[i] == '\n') {
            buftemp[i] = 0;
            break;
        }
    }

    status = pthread_create(&thread, NULL, receive, NULL);

    if(flow == (mqd_t) -1){
        kill_mq();
        errExit("mq_open");
    }

    if(mq_getattr(flow, &mq)){
        kill_mq();
        errExit("mq_getattr");
    }
    
    if(mq_send(flow, buftemp, strlen(buftemp), PRIO) == -1){
            kill_mq();
            errExit("mq_send");
        }
    while(1){
        char buf_str[256];
        int iter = 0;
        while(iter < 256){
            buf_str[iter] = fgetc(stdin);
            if(buf_str[iter] == '\n') {
                iter++;
                break;
            }
            iter++;
        }
        if(!strncmp(buf_str, "quit", 4)){
            kill_mq();
            pthread_cancel(thread);
            exit(EXIT_SUCCESS);
        }
        if(mq_send(flow, buf_str, iter, PRIO) == -1){
            kill_mq();
            errExit("mq_send");
        }
    }

    return 0;
}
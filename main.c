#include "main.h"

#define PRIO 0
#define CHANNEL_NAME "/main"

struct thread_info {
    pthread_t   thread_id;
    int         thread_num;
    char        *argv_string;
};

struct mq_attr mq; 
    mqd_t flow;

void errExit(const char* str){
    perror(str);
    exit(EXIT_FAILURE);
}

void kill_mq(void){
    if(flow){
        mq_close(flow);
        mq_unlink(CHANNEL_NAME);
    } 
    // while(1){
    //     list *currnet;
    //     currnet = getAt(0);
    //     if(currnet){
    //         remove_at(0);
    //     }
    //     else
    //         break;
    // }
}

/*
    main service would receive from all clients
    for first connect client would send key "::" with his nickname
    not realise - service will start new thread with opening new named channel with his nickname
    all clients will send in "main" channel and will receive accumulate buffer
    in private named channel like "nickname".
*/

void repite(list *bk, char* buf, int size){
    struct timespec tm;
    tm.tv_sec = 5;
    tm.tv_nsec = 0;
    if(!bk->bk.fl){
        bk->bk.fl = mq_open(bk->bk.name, O_CREAT | O_RDWR, 0666, NULL);
        if(bk->bk.fl == (mqd_t) -1){
            kill_mq();
            errExit("mq_open");
        }
        if(mq_getattr(bk->bk.fl, &mq)){
            kill_mq();
            errExit("mq_getattr");
        }
    }
    if(mq_timedsend(bk->bk.fl, buf, size, PRIO, &tm) == -1){
        if(errno != ETIMEDOUT){
            kill_mq();
            errExit("mq_send");
        }
        else{
            printf("timeout to %s\n", bk->bk.name);
            int index = 0;
            while(1){
                if(bk == getAt(index)){
                    mq_close(bk->bk.fl);
                    mq_unlink(bk->bk.name);
                    printf("user %s was removed\n", bk->bk.name);
                    remove_at(index);
                    break;
                }
                index++;
            }
        }
    }
}

int main(int argc, char *argv[]){
    struct mq_attr mq; 
    mqd_t flow;
    char state; 
    char buf[STR_LEN];
    mq.mq_curmsgs = 0;
    mq.mq_maxmsg = 10;
    mq.mq_msgsize = 256;
    mq_unlink(CHANNEL_NAME);

    flow = mq_open(CHANNEL_NAME, O_CREAT | O_RDWR, 0666, &mq); //temporary name

    if(flow == (mqd_t) -1){
        kill_mq();
        errExit("mq_open");
    }

    if(mq_getattr(flow, &mq)){
        kill_mq();
        errExit("mq_getattr");
    }

    while(1){
        for(int i = 0; i < sizeof buf; i++){
            buf[i] = 0;
        }
        ssize_t sz = mq_receive(flow, buf, sizeof buf, PRIO);
        if(sz == (ssize_t) -1){
            kill_mq();
            errExit("mq_receive");
        }
        if(!strncmp(buf, "::", 2)) {
            printf("new user %s\n", buf + 3);
            list temp;
            memcpy(temp.bk.name, buf + 2, 254);
            temp.bk.fl = 0;
            pushBack(&temp.bk);
        }
        else{
            int index = 0;
            while(1){
                list *current = 0;
                current = getAt(index);
                if(current)
                    repite(current, buf, sz);
                else
                    break;
                index++;
            }
            write(STDOUT_FILENO, buf, sz);
        }
    }
    kill_mq();

    return 0;
}
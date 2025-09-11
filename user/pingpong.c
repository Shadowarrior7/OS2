#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int
main(int argc, char *argv[])
{
    unsigned char TheByte = 0x42;
    unsigned char TheOtherByte;
    
    int parent[2];
    int child[2];
    pipe(parent);
    pipe(child);

    int pid = fork();
    if(pid < 0){
        fprintf(2, "IMMENSE FAILURE! I AM SAD\n");
        exit(1);
    }
    else if(pid == 0){
        read(child[0], &TheOtherByte, 1);
        if (TheOtherByte != TheByte){
            fprintf(2, "child throws a tantrum\n");
            exit(1);
        }
        int pid2 = getpid();
        printf("%d: recieved ping\n", pid2);
        write(parent[1], &TheOtherByte, 1);
        exit(0);
    }
    else{
        write(child[1], &TheByte, 1);
        wait(0);
        read(parent[0], &TheOtherByte, 1);
        if (TheOtherByte != TheByte){
            fprintf(2, "dad gets milk\n");
            exit(1);
        }
        int pid3 = getpid();
        printf("%d: recieved pong\n", pid3);
        exit(0);
    }
    exit(0);
}
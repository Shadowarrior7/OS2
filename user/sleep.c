#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int
main(int argc, char *argv[])
{
    if(argc <= 1){
        fprintf(2, "usage: sleep [seconds]\n");
        exit(1);
    }
    else{
        int seconds = atoi(argv[1]);
        if(seconds < 0){
            fprintf(2, "sleep: invalid time %d\n", seconds);
            exit(1);
        }
        sleep(seconds);
    }
    exit(0);
}
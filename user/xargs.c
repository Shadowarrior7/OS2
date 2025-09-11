#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/param.h"

#define MAXLINE 512

int
main(int argc, char *argv[])
{
  char buf[MAXLINE];
  char *arguments[MAXARG];
  int pid;
  char *tempBuf;
  int i;
  
  if(argc < 2) {
    fprintf(2, "usage: xargs command [args...]\n");
    exit(1);
  }
  
  for(i = 1; i < argc; i++) {
    arguments[i-1] = argv[i];
  }
  
  while(1) {
    tempBuf = buf;
    int fd;
    while((fd = read(0, tempBuf, 1)) > 0) {
      if(*tempBuf == '\n') {
        *tempBuf = '\0'; 
        break;
      }
      tempBuf++;
      if(tempBuf - buf >= MAXLINE - 1) {
        fprintf(2, "xargs: line too long\n");
        exit(1);
      }
    }
    
    if(fd <= 0) {
      break;
    }
    
    if(buf[0] == '\0') {
      continue;
    }
    
    int xargc = argc - 1;
    tempBuf = buf;
    
    while(*tempBuf != '\0' && xargc < MAXARG - 1) {
      while(*tempBuf == ' ' || *tempBuf == '\t') {
        tempBuf++;
      }
      
      if(*tempBuf == '\0') {
        break;
      }
      
      arguments[xargc++] = tempBuf;
      
      while(*tempBuf != '\0' && *tempBuf != ' ' && *tempBuf != '\t') {
        tempBuf++;
      }
    
      if(*tempBuf != '\0') {
        *tempBuf = '\0';
        tempBuf++;
      }
    }

    arguments[xargc] = 0;
    
    pid = fork();
    if(pid == 0) {
      exec(arguments[0], arguments);
      fprintf(2, "xargs: exec %s failed\n", arguments[0]);
      exit(1);
    } else if(pid > 0) {
      wait(0);
    } else {
      fprintf(2, "xargs: fork failed\n");
      exit(1);
    }
  }
  
  exit(0);
}

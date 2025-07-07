#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

int main(int argc, char**argv) {
  FILE*p=popen("qrencode -t UTF8","w");
  fprintf(p,"%s",get_self());
  return pclose(p);
}

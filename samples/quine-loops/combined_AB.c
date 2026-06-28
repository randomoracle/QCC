#ifdef _COMPILE_A_
#include <stdlib.h>
#include <stdio.h>

int main(int argc, char **argv) {
  if (argc != 2) {
    printf("This is app Alfa.\n");
  } else {
    int app = atoi(argv[1]);
    const char *S = get_self();
    printf("#define _COMPILE_%c_\n%s", (app == 1) ? 'A' : 'B', S);
  }
  return 0;
}
#endif
#ifdef _COMPILE_B_
#include <stdlib.h>
#include <stdio.h>

int main(int argc, char **argv) {
  if (argc != 2) {
    printf("This is app Bravo.\n");
  } else {
    int app = atoi(argv[1]);
    const char *S = get_self();
    printf("#define _COMPILE_%c_\n%s", (app == 1) ? 'B' : 'A', S);
  }
  return 0;
}
#endif

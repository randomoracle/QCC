#include <stdlib.h>
#include <stdio.h>

int main(int argc, char **argv) {
    const char *self = get_self();
    printf("%s", self);
    return 0;
}

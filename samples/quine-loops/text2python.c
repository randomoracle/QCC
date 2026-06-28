
/*
 * Given an ASCII string S, return a newly malloc()'d C string containing a
 * Python program whose output is exactly S.
 *
 * The program emitted is:
 *
 *     import sys
 *     sys.stdout.write("<escaped S>")
 *
 * sys.stdout.write (rather than print) is used so that the output is exactly
 * S, with no extra trailing newline.
 *
 * Returns NULL if s is NULL or if allocation fails.  The caller owns the
 * returned buffer and must free() it.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *python_printer(const char *s) {
    if (s == NULL)
        return NULL;

    const char *prefix = "import sys\nsys.stdout.write(\"";
    const char *suffix = "\")\n";

    /* Each input byte expands to at most 4 output bytes (e.g. "\xHH"). */
    size_t len = strlen(s);
    size_t cap = strlen(prefix) + len * 4 + strlen(suffix) + 1;

    char *out = malloc(cap);
    if (out == NULL)
        return NULL;

    char *p = out;
    p += sprintf(p, "%s", prefix);

    for (const unsigned char *c = (const unsigned char *)s; *c; c++) {
        switch (*c) {
        case '\\': *p++ = '\\'; *p++ = '\\'; break;
        case '"':  *p++ = '\\'; *p++ = '"';  break;
        case '\n': *p++ = '\\'; *p++ = 'n';  break;
        case '\r': *p++ = '\\'; *p++ = 'r';  break;
        case '\t': *p++ = '\\'; *p++ = 't';  break;
        default:
            if (*c >= 0x20 && *c < 0x7f) {
                /* Printable ASCII: emit as-is. */
                *p++ = (char)*c;
            } else {
                /* Everything else: \xHH hex escape. */
                p += sprintf(p, "\\x%02x", *c);
            }
            break;
        }
    }

    p += sprintf(p, "%s", suffix);
    *p = '\0';

    return out;
}

int main(int argc, char **argv) {
  if (argc == 1) {
    fprintf(stderr, "usage: %s <string>\n", argv[0]);
    return +1;
  }
  char *prog = python_printer(argv[1]);
  if (prog == NULL) {
    fprintf(stderr, "error: failed to allocate memory\n");
    return +1;
  }
  printf("%s", prog);
  free(prog);
  return 0;
}

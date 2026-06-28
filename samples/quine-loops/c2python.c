#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *python_printer(const char *s) {
  if (s == nullptr) return nullptr;

  static const char prefix[] = "import sys\nsys.stdout.write(\"";
  static const char suffix[] = "\")\n";

  size_t len = strlen(s);
  size_t cap = strlen(prefix) + (len * 4) + strlen(suffix) + 1;

  char *out = malloc(cap);
  if (out == nullptr) return nullptr;

  char *p = out;
  p += sprintf(p, "%s", prefix);

  for (const unsigned char *c = (const unsigned char *)s; *c; c++) {
    switch (*c) {
    case '\\':
    case '"':
    case '\n':
    case '\r': 
    case '\t':
      *p++ = '\\';
      *p++ = *c;
      break;
    default:
      if (*c >= 0x20 && *c < 0x7f)
        *p++ = (char)*c;
      else
        p += sprintf(p, "\\x%02x", *c);
      break;
    }
  }

  p += sprintf(p, "%s", suffix);
  return out;
}

int main(int argc, char **argv) {
  const char *self = get_self();
  char *python_version = python_printer(self);
  printf("%s", python_version);
  free(python_version);
  return 0;
}

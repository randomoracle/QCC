#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

static const char *kUsage =
"Usage: qcc <source file>\n";

static const char kWarning[] =
"/* Warning: Automatically generated code; do not modify */\n";

static const char kPrologueHeaders[] =
"#include <stdlib.h>\n"
"#include <stdio.h>\n"
"#include <string.h>\n"
"\n";

static const char kProloguePrototype[] =
"const char *get_self();\n"
"\n";

static const char kHexDecodeHelper[] =
"size_t hex_decode(const char *input, char **output) {\n"
"  size_t l = strlen(input);\n"
"  char *decoded = *output = malloc(l / 2 + 1);\n"
"  while (*input) {\n"
"    unsigned char byte;\n"
"    sscanf(input, \"%2hhx\", &byte);\n"
"    *decoded++ = byte;\n"
"    input += 2;\n"
"  }\n"
"  *decoded = 0;\n"
"  return decoded - *output;\n"
"}\n";

static const char kSelfRef[] =
"const char *get_self() {\n"
"  static char *self;\n"
"  if (self == NULL) {\n"
"    char *decoded_prologue, *decoded_selfref;\n"
"    size_t c_prologue = hex_decode(prologue, &decoded_prologue);\n"
"    size_t c_selfref = hex_decode(selfref, &decoded_selfref);\n"
"    self = malloc(c_prologue + c_selfref + strlen(prologue) + strlen(selfref) + 0x1000);\n"
"    sprintf(self, \"%sconst char prologue[] = %c%s%c;\\nconst char selfref[] = %c%s%c;\\n\\n%s\",\n"
"            decoded_prologue, 34, prologue, 34, 34, selfref, 34, decoded_selfref);\n"
"    free(decoded_prologue), free(decoded_selfref);\n"
"  }\n"
"  return self;\n"
"}\n";

static const char *source;

void output_hex(FILE *fp, const char *text) {
  while (*text)
    fprintf(fp, "%02x", *text++);
}

void synchronized_write(FILE *fp, const char *text) {
  printf("%s", text);
  output_hex(fp, text);
}

int quine_source(FILE *fp) {
  static const size_t kSpace = 0x1000;
  char *segment = malloc(kSpace);

  FILE *prologue = tmpfile();
  if (prologue == NULL) {
    fprintf(stderr, "error creating temporary file: %d\n", errno);
    return __COUNTER__ + 1;
  }
  
  synchronized_write(prologue, kWarning);
  synchronized_write(prologue, kPrologueHeaders);
  synchronized_write(prologue, kProloguePrototype);

  while (fgets(segment, kSpace, fp))
    synchronized_write(prologue, segment);

  synchronized_write(prologue, "\n");
  synchronized_write(prologue, kHexDecodeHelper);
  synchronized_write(prologue, "\n");

  rewind(prologue);

  printf("const char prologue[] = \"");
  while (fgets(segment, kSpace, prologue))
    printf("%s", segment);
  printf("\";\n");

  printf("const char selfref[] = \"");
  output_hex(stdout, kSelfRef);
  printf("\";\n\n");

  printf("%s", kSelfRef);

  free(segment);
  fclose(prologue);
  return 0;
}

int main(int argc, char **argv) {
  int ecode = 0;

  if (argc != 2) {
    printf("%s", kUsage);
    return argc == 1;
  }

  source = argv[1];
  FILE *fp = fopen(source, "r");
  if (fp == NULL) {
    fprintf(stderr, "error opening source file: %s\n", source);
    ecode = errno;
  } else {
    ecode = quine_source(fp);
    fclose(fp);
  }

  return ecode;
}

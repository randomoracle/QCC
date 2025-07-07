/*
 * This file is part of the QCC project.
 *
 * QCC is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * QCC is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with QCC. If not, see <https://www.gnu.org/licenses/>.
 */

#include <stdlib.h>
#include <stdbool.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>

static const char *kUsage =
"Usage: qcc [options] <source file>\n"
"Options:\n"
"  -i  Suppress #include directives\n"
"  -p  Suppress function prototype\n"
"  -w  Suppress warning against modification\n"
"  -d  Define macro for conditional compilation\n"
#ifdef _QUINE_
"  -q  Output own source code\n"
#endif
 ;

static const char kWarning[] =
"/* Warning: Automatically generated code; do not modify */\n";

static const char kPreprocessorMacro[] = "#define _QUINE_\n";

static const char kPrologueHeaders[] =
"#include <stdlib.h>\n"
"#include <stdio.h>\n"
"#include <string.h>\n"
"\n";

static const char kProloguePrototype[] =
"const char *get_self();\n"
"\n";

static const char kSelfRef[] =
"size_t hex2bin(const char*hex,char**bin) {\n"
"  char*dst=*bin=malloc(strlen(hex)/2+1);\n"
"  while(*hex){sscanf(hex,\"%2hhx\",dst++),hex+=2;}\n"
"  return *dst=0,(dst-*bin);\n"
"}\n"
"\n"
"const char*get_self() {\n"
"  static char*self;\n"
"  if (!self) {\n"
"    char*decP,*decSR;\n"
"    size_t cP=hex2bin(prologue,&decP),cSR=hex2bin(selfref,&decSR);\n"
"    self=malloc(cP+cSR+strlen(prologue)+strlen(selfref)+1000);\n"
"    sprintf(self,\"%sconst char*prologue=%c%s%c;\\nconst char*selfref=%c%s%c;\\n\\n%s\",decP,34,prologue,34,34,selfref,34,decSR);\n"
"    free(decP),free(decSR);\n"
"  }\n"
"  return self;\n"
"}\n";

static bool
#ifdef _QUINE_
  kOutputSelf = false,
#endif
  kSuppressWarning = false,
  kSuppressInclude = false,
  kSuppressPrototype = false,
  kDefineMacro = false;

static const char *source;

int parse_arguments(int argc, char **argv) {
  if (argc == 1) return 0;

  for (int i = 1; i < argc; i++) {
    const char *argument = argv[i];
    if (argument[0] != '-') {
      if (source == NULL) {
        source = argv[i];
        continue;
      } else {
        fprintf(stderr, "Too many arguments\n");
        return __COUNTER__ + 1;
      }
    }

    if (argument[1] == '\0') {
      fprintf(stderr, "Empty option: %s\n", argument);
      return __COUNTER__ + 1;
    }
    for (int j = 1; argument[j]; j++) {
      switch (argv[i][j]) {
        case 'i':
          kSuppressInclude = true;
          break;
        case 'p':
          kSuppressPrototype = true;
          break;
        case 'w':
          kSuppressWarning = true;
          break;
        case 'd':
          kDefineMacro = true;
          break;
#ifdef _QUINE_
        case 'q':
          kOutputSelf = true;
          break;
#endif
        default:
          fprintf(stderr, "Unknown option: %s\n", argv[i]);
          return __COUNTER__ + 1;
      }
    }
  }

  return 0;
}

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

  if (!kSuppressWarning)    synchronized_write(prologue, kWarning);
  if (!kSuppressInclude)    synchronized_write(prologue, kPrologueHeaders);
  if (!kSuppressPrototype)  synchronized_write(prologue, kProloguePrototype);
  if (kDefineMacro)         synchronized_write(prologue, kPreprocessorMacro);

  while (fgets(segment, kSpace, fp))
    synchronized_write(prologue, segment);

  synchronized_write(prologue, "\n");

  rewind(prologue);

  printf("const char*prologue=\"");
  while (fgets(segment, kSpace, prologue))
    printf("%s", segment);
  printf("\";\n");

  printf("const char*selfref=\"");
  output_hex(stdout, kSelfRef);
  printf("\";\n\n");

  printf("%s", kSelfRef);

  free(segment);
  fclose(prologue);
  return 0;
}

int main(int argc, char **argv) {
  int ecode = parse_arguments(argc, argv);

  if (ecode) {
    return ecode;
#ifdef _QUINE_
  } else if (kOutputSelf) {
    const char *self = get_self();
    printf("%s", self);
#endif
  } else if (source == NULL) {
    printf("%s", kUsage);
  } else {
    FILE *fp = fopen(source, "r");
    if (fp == NULL) {
      fprintf(stderr, "error opening source file: %s\n", source);
      ecode = errno;
    } else {
      ecode = quine_source(fp);
      fclose(fp);
    }
  }

  return ecode;
}

#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "cstring.h"

void
equal(char *a, char *b) {
    printf("%s %s\n",a,b);
  if (strcmp(a, b) == 0) {
    printf("\n");
    printf("  expected: '%s'\n", a);
    printf("    actual: '%s'\n", b);
    printf("\n");
    exit(1);
  }
}

void
test_cstr_new() {
  cstring *buf = cstr_new();
  assert(64 == cstr_size(buf));
  assert(0 == cstr_length(buf));
  cstr_free(buf);
}

void
test_cstr_new_with_size() {
  cstring *buf = cstr_new_with_size(1024);
  assert(1024 == cstr_size(buf));
  assert(0 == cstr_length(buf));
  cstr_free(buf);
}

void
test_cstr_append() {
  cstring *buf = cstr_new();
  printf("--%d\n",cstr_append(buf, "Hello"));
  printf("--%d\n",cstr_append(buf, " World"));
  printf("--%d\n",cstr_length(buf));
  equal("Hello World", cstr_string(buf));
  cstr_free(buf);
}

void
test_cstr_append_n() {
  cstring *buf = cstr_new();
  assert(0 == cstr_append_n(buf, "subway", 3));
  assert(0 == cstr_append_n(buf, "marines", 6));
  assert(strlen("submarine") == cstr_length(buf));
  equal("submarine", cstr_string(buf));
  cstr_free(buf);
}

void
test_cstr_append__grow() {
  cstring *buf = cstr_new_with_size(10);
  assert(0 == cstr_append(buf, "Hello"));
  assert(0 == cstr_append(buf, " tobi"));
  assert(0 == cstr_append(buf, " was"));
  assert(0 == cstr_append(buf, " here"));

  char *str = "Hello tobi was here";
  equal(str, cstr_string(buf));
  assert(1024 == cstr_size(buf));
  assert(strlen(str) == cstr_length(buf));
  cstr_free(buf);
}

void
test_cstr_prepend() {
  cstring *buf = cstr_new();
  assert(0 == cstr_append(buf, " World"));
  assert(0 == cstr_prepend(buf, "Hello"));
  assert(strlen("Hello World") == cstr_length(buf));
  equal("Hello World", cstr_string(buf));
  cstr_free(buf);
}

void
test_cstr_slice() {
  cstring *buf = cstr_new();
  cstr_append(buf, "Tobi Ferret");

  cstring *a = cstr_slice(buf, 2, 8);
  equal("Tobi Ferret", cstr_string(buf));
  equal("bi Fer", cstr_string(a));

  cstr_free(buf);
  cstr_free(a);
}

void
test_cstr_slice__range_error() {
  cstring *buf = cstr_new_with_copy("Tobi Ferret");
  cstring *a = cstr_slice(buf, 10, 2);
  assert(NULL == a);
  cstr_free(buf);
}

void
test_cstr_slice__end() {
  cstring *buf = cstr_new_with_copy("Tobi Ferret");

  cstring *a = cstr_slice(buf, 5, -1);
  equal("Tobi Ferret", cstr_string(buf));
  equal("Ferret", cstr_string(a));

  cstring *b = cstr_slice(buf, 5, -3);
  equal("Ferr", cstr_string(b));

  cstring *c = cstr_slice(buf, 8, -1);
  equal("ret", cstr_string(c));

  cstr_free(buf);
  cstr_free(a);
  cstr_free(b);
  cstr_free(c);
}

void
test_cstr_slice__end_overflow() {
  cstring *buf = cstr_new_with_copy("Tobi Ferret");
  cstring *a = cstr_slice(buf, 5, 1000);
  equal("Tobi Ferret", cstr_string(buf));
  equal("Ferret", cstr_string(a));
  cstr_free(a);
  cstr_free(buf);
}

void
test_cstr_equals() {
  cstring *a = cstr_new_with_copy("Hello");
  cstring *b = cstr_new_with_copy("Hello");

  assert(1 == cstr_equals(a, b));

  cstr_append(b, " World");
  assert(0 == cstr_equals(a, b));

  cstr_free(a);
  cstr_free(b);
}

void test_cstr_formatting() {
  cstring *buf = cstr_new();
  int result = cstr_appendf(buf, "%d %s", 3, "cow");
  assert(0 == result);
  equal("3 cow", cstr_string(buf));
  result = cstr_appendf(buf, " - 0x%08X", 0xdeadbeef);
  assert(0 == result);
  equal("3 cow - 0xDEADBEEF", cstr_string(buf));
  cstr_free(buf);
}

void
test_cstr_indexof() {
  cstring *buf = cstr_new_with_copy("Tobi is a ferret");

  ssize_t i = cstr_indexof(buf, "is");
  assert(5 == i);

  i = cstr_indexof(buf, "a");
  assert(8 == i);

  i = cstr_indexof(buf, "something");
  assert(-1 == i);

  cstr_free(buf);
}

void
test_cstr_fill() {
  cstring *buf = cstr_new_with_copy("Hello");
  assert(5 == cstr_length(buf));

  cstr_fill(buf, 0);
  assert(0 == cstr_length(buf));
  cstr_free(buf);
}

void
test_cstr_clear() {
  cstring *buf = cstr_new_with_copy("Hello");
  assert(5 == cstr_length(buf));

  cstr_clear(buf);
  assert(0 == cstr_length(buf));
  cstr_free(buf);
}

void
test_cstr_trim() {
  cstring *buf = cstr_new_with_copy("  Hello\n\n ");
  cstr_trim(buf);
  equal("Hello", cstr_string(buf));
  cstr_free(buf);

  buf = cstr_new_with_copy("  Hello\n\n ");
  cstr_trim_left(buf);
  equal("Hello\n\n ", cstr_string(buf));
  cstr_free(buf);

  buf = cstr_new_with_copy("  Hello\n\n ");
  cstr_trim_right(buf);
  equal("  Hello", cstr_string(buf));
  cstr_free(buf);
}

void
test_cstr_compact() {
  cstring *buf = cstr_new_with_copy("  Hello\n\n ");
  cstr_trim(buf);
  assert(5 == cstr_length(buf));
  assert(10 == cstr_size(buf));

  ssize_t removed = cstr_compact(buf);
  assert(5 == removed);
  assert(5 == cstr_length(buf));
  assert(5 == cstr_size(buf));
  equal("Hello", cstr_string(buf));

  cstr_free(buf);
}

int
main(){
  //test_cstr_new();
  //test_cstr_new_with_size();
  test_cstr_append();
#if 0
  test_cstr_append__grow();
  test_cstr_append_n();
  test_cstr_prepend();
  test_cstr_slice();
  test_cstr_slice__range_error();
  test_cstr_slice__end();
  test_cstr_slice__end_overflow();
  test_cstr_equals();
  test_cstr_formatting();
  test_cstr_indexof();
  test_cstr_fill();
  test_cstr_clear();
  test_cstr_trim();
  test_cstr_compact();
  printf("\n  \e[32m\u2713 \e[90mok\e[0m\n\n");
#endif
  return 0;
}

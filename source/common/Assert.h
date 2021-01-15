//
// Created by vinhi on 01.12.2020.
//

#include <cstdio>

#define ASSERT_EQUAL(x, y, label)          \
  if (x != y) {                            \
    printf("ASSERTION FAILED: %s", label); \
    exit(-1);                              \
  }

#define ASSERT_NOT_EQUAL(x, y, label)      \
  if (x == y) {                            \
    printf("ASSERTION FAILED: %s", label); \
    exit(-1);                              \
  }

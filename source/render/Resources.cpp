#include "Resources.h"

#include <cstdio>

namespace Imperium::Render {
template <>
void CPUBuffer<float>::Display() {
  for (int i = 0; i < nbElements; i++) {
    printf("%f, ", data[i]);
  }
  printf("\n");
}

template <>
void CPUBuffer<int>::Display() {
  for (int i = 0; i < nbElements; i++) {
    printf("%d, ", data[i]);
  }
  printf("\n");
}

template <>
void CPUBuffer<unsigned int>::Display() {
  for (int i = 0; i < nbElements; i++) {
    printf("%d, ", data[i]);
  }
  printf("\n");
}
}  // namespace Imperium::Render
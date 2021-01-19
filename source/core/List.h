//
// Created by vinhig on 19.01.2021.
//

#pragma once

#include <cstdlib>

#include "core/Option.h"

namespace Imperium::Core {
template <typename T>
class List {
 private:
  T* _data{nullptr};
  int _size{0};

 public:
  List() = default;
  List(int size) : _size(size) { _data = (T*)malloc(sizeof(T) * size); }
  ~List() {
    delete[] _data;
  };
  T* Data() const { return _data; }

  Core::Option<T*> operator[](int i) {
    if (i >= _size) {
      return Core::Option<T*>();
    } else {
      return Core::Option<T*>(&_data[i]);
    }
  }
};
}  // namespace Imperium::Core
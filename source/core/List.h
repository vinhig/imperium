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
  /**
   * Allocates enough space for n objects.
   */
  explicit List(int n) : _size(n) { _data = new T[n]; }
  ~List() { delete[] _data; };
  T* Data() const { return _data; }

  T* Get(int i) { return this->operator[](i); }
  void Set(int i, T value) {
    if (i < _size) {
      _data[i] = value;
    }
  }

  Core::Option<T*> operator[](int i) {
    if (i >= _size) {
      return Core::Option<T*>();
    } else {
      return Core::Option<T*>(&_data[i]);
    }
  }
};
}  // namespace Imperium::Core
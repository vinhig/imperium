//
// Created by vinhig on 19.01.2021.
//

#pragma once

namespace Imperium::Core {
template <typename L, typename R>
class Pair {
 private:
  L _left;
  R _right;

 public:
  Pair() : _left(), _right(){};
  Pair(L left, R right) : _left(left), _right(right){};
  ~Pair() = default;
  L Left() { return _left; };
  R Right() { return _right; };

  void SetLeft(L left) { _left = left; };
  void SetRight(R right) { _right = right; };
};
}  // namespace Imperium::Core
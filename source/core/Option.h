//
// Created by vinhig on 19.01.2021.
//

#pragma once

namespace Imperium::Core {
template <typename T>
class Option {
 private:
  bool _hasValue{false};
  T _value;

 public:
  /**
   * Create Option without any value.
   */
  Option() : _value() {}
  /**
   * Create Option by assigning a value.
   */
  explicit Option(T value) : _value(value) { _hasValue = true; }

  bool HasValue() const { return _hasValue; };
  void SetValue(T value) { _value = value; };

  T Value() { return _value; };
};
}  // namespace Imperium::Core
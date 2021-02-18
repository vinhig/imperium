#pragma once

//
// Created by vinhig on 19.01.2021.
//

namespace Imperium::Core {
template <typename T>
class Option {
 private:
  bool _hasValue{false};
  T _value;

 public:
  /**
   * Create Option without any value. Underlying value (_value and Value()) is
   * completely UB. Be sure to always check HasValue().
   */
  Option() : _value() {}

  /**
   * Create Option by assigning a value.
   * @param value Value to assign.
   */
  explicit Option(T value) : _value(value) { _hasValue = true; }

  /**
   * Check if value has been assigned. Always check this value before using
   * underlying value.
   * @return Whether the value of option has been set.
   */
  [[nodiscard]] bool HasValue() const { return _hasValue; };

  /**
   * Assign value (omg most useful comment ever written by mankind). Option
   * now have value.
   * @param value Value to assign.
   */
  void SetValue(T value) {
    _hasValue = true;
    _value = value;
  };

  /**
   * Forget value assigned, but don't delete it. If SetValue(value) isn't called
   * Value() doesn't change even if Forget() is called.
   */
  void Forget() { _hasValue = false; }

  /**
   * Get assigned value. UB if HasValue() is false.
   * @return Assigned value.
   */
  T Value() { return _value; };
};
}  // namespace Imperium::Core
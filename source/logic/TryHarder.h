//
// Created by vinhi on 24/10/2020.
//

#pragma once

#include <deque>

#include "Actions.h"
/**
 * Create workers. Manager logical jobs by registering and distributing them.
 */
class TryHarder {
 private:
  std::deque<LogicCall> _jobs;
 public:
  /**
   * Create a TryHarder with maximum number of workers.
   */
  TryHarder();

  /**
   * Create a TryHarder with given number of workers. Must be greater or equal
   * to 1.
   * @param nbWorker
   */
  explicit TryHarder(int nbWorker);
  ~TryHarder() = default;

  void Register(LogicCall call);
  void Do();

  /**
   * Current status of workers.
   * @return True if there isn't any jobs left, false otherwise.
   */
  bool Done();

  /**
   * Wait until job list is empty.
   */
  void Wait();
};
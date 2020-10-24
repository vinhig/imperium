//
// Created by vinhi on 24/10/2020.
//

#include "TryHarder.h"
TryHarder::TryHarder() = default;

TryHarder::TryHarder(int nbWorker) {}

void TryHarder::Register(LogicCall call) { _jobs.push_back(call); }

void TryHarder::Do() {
  for (const auto& call : _jobs) {
    call.call();
  }
}

bool TryHarder::Done() { return _jobs.empty(); }

void TryHarder::Wait() {}

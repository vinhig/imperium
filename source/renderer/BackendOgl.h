//
// Created by vincent on 2/10/20.
//

#pragma once

#include "Backend.h"

#include <cstdint>
#include <cstdio>

class BackendOgl : public Backend {
public:
    BackendOgl() = default;
    ~BackendOgl() = default;

    uint32_t CreateBuffer(void *data, size_t size) override;

    void Initialize() override;
    void Dispose() override;
};
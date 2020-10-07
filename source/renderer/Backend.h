//
// Created by vincent on 2/10/20.
//

#pragma once

#include <cstdint>
#include <cstdio>

class Backend {
public:
    /**
     * Create some fucking good buffer of data.
     * @param data Data to upload on GPU
     * @param size Size of data (in bytes)
     * @return GPU address of buffer
     */
    virtual uint32_t CreateBuffer(void *data, size_t size) = 0;

    /**
     * Initialize backend for rendering. Create a window on desktop platform.
     */
    virtual void Initialize() = 0;

    /**
     * Delete every resources.
     */
    virtual void Dispose() = 0;
};

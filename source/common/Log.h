//
// Created by vinhi on 13.10.2020.
//

#pragma once

#include <cstdio>

#if _DEBUG
#define LOG_DEBUG(x) printf("[DEBUG]: %s\n", x);
#else
#define LOG_DEBUG(x)
#endif

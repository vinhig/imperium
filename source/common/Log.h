//
// Created by vinhi on 13/10/2020.
//

#pragma once

#if _DEBUG
#define LOG_DEBUG(x) std::cout << "[DEBUG]: " << x << std::endl;
#else
#define LOG_DEBUG(x)
#endif

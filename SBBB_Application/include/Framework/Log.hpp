#pragma once
#include <stdio.h>
#include "FrameworkConstants.hpp"
#include <iostream>

// A bit messy but obvious
#ifdef SBBB_DEBUG

// handy disableable specifications
#ifdef LOADLOGGING_ENABLED
#define LOAD_LOG(expr) std::cout << expr << std::endl
#else
#define LOAD_LOG(expr)
#endif

#ifdef DELETELOGGING_ENABLED
#define DELETE_LOG(expr) std::cout << expr << std::endl
#else
#define DELETE_LOG(expr)
#endif

#ifdef GLGENLOGGING_ENABLED
#define GLGEN_LOG(expr) std::cout << expr << std::endl
#else
#define GLGEN_LOG(expr)
#endif

// general log macro without a toggle
#define LOG(expr) std::cout << expr << std::endl
#define CONDITIONAL_LOG(cond, expr) if (cond) std::cout << expr << std::endl

#else

#define LOG(expr)
#define CONDITIONAL_LOG(cond, expr)
#define LOAD_LOG(expr)
#define DELETE_LOG(expr)
#define GLGEN_LOG(expr)

#endif
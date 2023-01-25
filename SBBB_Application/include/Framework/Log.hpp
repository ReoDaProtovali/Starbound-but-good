#pragma once
#include <stdio.h>
#include "FrameworkConstants.hpp"
#include <iostream>

// A bit messy but obvious
#ifdef SBBB_DEBUG

// handy disableable specifications
#ifdef LOADLOGGING_ENABLED
#define LOAD_LOG(expr) std::cout << expr << "\n"
#else
#define LOAD_LOG(expr)
#endif

#ifdef DELETELOGGING_ENABLED
#define DELETE_LOG(expr) std::cout << expr << "\n"
#else
#define DELETE_LOG(expr)
#endif

#ifdef GLGENLOGGING_ENABLED
#define GLGEN_LOG(expr) std::cout << expr << "\n"
#else
#define GLGEN_LOG(expr)
#endif

// general log macro without a toggle
#define LOG(expr) std::cout << expr << std::endl
#define CONDITIONAL_LOG(cond, expr) if (cond) std::cout << expr << '\n'
#define ERROR_LOG(expr) std::cout << "\nERROR: " << expr << "\n\n";
#define WARNING_LOG(expr) std::cout << "WARNING: " << expr << '\n';


#else

#define LOG(expr)
#define CONDITIONAL_LOG(cond, expr)
#define ERROR_LOG(expr)
#define WARNING_LOG(expr)
#define LOAD_LOG(expr)
#define DELETE_LOG(expr)
#define GLGEN_LOG(expr)

#endif

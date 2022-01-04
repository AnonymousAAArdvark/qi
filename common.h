//
// Created by Andrew Yang on 8/29/21.
//

#ifndef QI_COMMON_H
#define QI_COMMON_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#define NAN_BOXING
#define DEBUG_PRINT_CODE
#define DEBUG_TRACE_EXECUTION

#define DEBUG_STRESS_GC
#define DEBUG_LOG_GC

#define UINT8_COUNT (UINT8_MAX + 1)

#endif //QI_COMMON_H

//#undef DEBUG_PRINT_CODE
#undef DEBUG_TRACE_EXECUTION
#undef DEBUG_STRESS_GC
#undef DEBUG_LOG_GC
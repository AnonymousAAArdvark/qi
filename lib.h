//
// Created by Andrew Yang on 9/17/21.
//

#ifndef QI_STDLIB_H
#define QI_STDLIB_H

#include "common.h"
#include "object.h"
#include "memory.h"
#include "vm.h"

bool clockNative(int argCount, Value* args);
bool sqrtNative(int argCount, Value* args);
bool powNative(int argCount, Value* args);
bool minNative(int argCount, Value* args);
bool maxNative(int argCount, Value* args);
bool roundNative(int argCount, Value* args);

#endif //QI_STDLIB_H

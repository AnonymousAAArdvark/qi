//
// Created by Andrew Yang on 8/30/21.
//

#ifndef QI_COMPILER_H
#define QI_COMPILER_H

#include "object.h"
#include "vm.h"

ObjFunction* compile(const char* source);
void markCompilerRoots();

#endif //QI_COMPILER_H

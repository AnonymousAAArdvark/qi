//
// Created by Andrew Yang on 8/29/21.
//

#ifndef QI_DEBUG_H
#define QI_DEBUG_H

#include "chunk.h"

void disassembleChunk(Chunk* chunk, const wchar_t* name);
int disassembleInstruction(Chunk* chunk, int offset);

#endif //QI_DEBUG_H

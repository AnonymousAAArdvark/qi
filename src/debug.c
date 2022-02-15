//
// Created by Andrew Yang on 8/29/21.
//

#include <stdio.h>

#include "debug.h"
#include "object.h"
#include "value.h"

void disassembleChunk(Chunk* chunk, const wchar_t* name) {
    wprintf(L"== %ls == \n", name);

    for (int offset = 0; offset < chunk->count;) {
        offset = disassembleInstruction(chunk, offset);
    }
}

static int constantInstruction(const wchar_t* name, Chunk* chunk, int offset) {
    uint8_t constant = chunk->code[offset + 1];
    wprintf(L"%-16s %4d '", name, constant);
    printValue(chunk->constants.values[constant]);
    wprintf(L"'\n");
    return offset + 2;
}

static int invokeInstruction(const wchar_t* name, Chunk* chunk, int offset) {
    uint8_t constant = chunk->code[offset + 1];
    uint8_t argCount = chunk->code[offset + 2];
    wprintf(L"%-16s (%d args) %4d '", name, argCount, constant);
    printValue(chunk->constants.values[constant]);
    wprintf(L"'\n");
    return offset + 3;
}

static int simpleInstruction(const wchar_t* name, int offset) {
    wprintf(L"%ls\n", name);
    return offset + 1;
}

static int byteInstruction(const wchar_t* name, Chunk* chunk, int offset) {
    uint8_t slot = chunk->code[offset + 1];
    wprintf(L"%-16s %4d\n", name, slot);
    return offset + 2;
}

static int jumpInstruction(const wchar_t* name, int sign, Chunk* chunk, int offset) {
    uint16_t jump = (uint16_t)(chunk->code[offset + 1] << 8);
    jump |= chunk->code[offset + 2];
    wprintf(L"%-16s %4d -> %d\n", name, offset, offset + 3 + sign * jump);
    return offset + 3;
}

int disassembleInstruction(Chunk* chunk, int offset) {
    wprintf(L"%04d ", offset);
    if (offset > 0 && chunk->lines[offset] == chunk->lines[offset - 1]) {
        wprintf(L"   | ");
    } else {
        wprintf(L"%4d ", chunk->lines[offset]);
    }

    uint8_t instruction = chunk->code[offset];
    switch (instruction) {
        case OP_CONSTANT:
            return constantInstruction(L"OP_CONSTANT", chunk, offset);
        case OP_NIL:
            return simpleInstruction(L"OP_NIL", offset);
        case OP_TRUE:
            return simpleInstruction(L"OP_TRUE", offset);
        case OP_FALSE:
            return simpleInstruction(L"OP_FALSE", offset);
        case OP_POP:
            return simpleInstruction(L"OP_POP", offset);
        case OP_GET_GLOBAL:
            return constantInstruction(L"OP_GET_GLOBAL", chunk, offset);
        case OP_DEFINE_GLOBAL:
            return constantInstruction(L"OP_DEFINE_GLOBAL", chunk, offset);
        case OP_GET_LOCAL:
            return byteInstruction(L"OP_GET_LOCAL", chunk, offset);
        case OP_SET_LOCAL:
            return byteInstruction(L"OP_SET_LOCAL", chunk, offset);
        case OP_SET_GLOBAL:
            return constantInstruction(L"OP_SET_GLOBAL", chunk, offset);
        case OP_GET_UPVALUE:
            return byteInstruction(L"OP_GET_UPVALUE", chunk, offset);
        case OP_SET_UPVALUE:
            return byteInstruction(L"OP_SET_UPVALUE", chunk, offset);
        case OP_GET_PROPERTY:
            return constantInstruction(L"OP_GET_PROPERTY", chunk, offset);
        case OP_SET_PROPERTY:
            return constantInstruction(L"OP_SET_PROPERTY", chunk, offset);
        case OP_GET_SUPER:
            return constantInstruction(L"OP_GET_SUPER", chunk, offset);
        case OP_BUILD_LIST:
            return byteInstruction(L"OP_BUILD_LIST", chunk, offset);
        case OP_INDEX_SUBSCR:
            return simpleInstruction(L"OP_INDEX_SUBSCR", offset);
        case OP_STORE_SUBSCR:
            return simpleInstruction(L"OP_STORE_SUBSCR", offset);
        case OP_EQUAL:
            return simpleInstruction(L"OP_EQUAL", offset);
        case OP_GREATER:
            return simpleInstruction(L"OP_GREATER", offset);
        case OP_LESS:
            return simpleInstruction(L"OP_LESS", offset);
        case OP_ADD:
            return simpleInstruction(L"OP_ADD", offset);
        case OP_SUBTRACT:
            return simpleInstruction(L"OP_SUBTRACT", offset);
        case OP_INCREMENT:
            return simpleInstruction(L"OP_INCREMENT", offset);
        case OP_DECREMENT:
            return simpleInstruction(L"OP_DECREMENT", offset);
        case OP_MULTIPLY:
            return simpleInstruction(L"OP_MULTIPLY", offset);
        case OP_DIVIDE:
            return simpleInstruction(L"OP_DIVIDE", offset);
        case OP_MODULO:
            return simpleInstruction(L"OP_MODULO", offset);
        case OP_NOT:
            return simpleInstruction(L"OP_NOT", offset);
        case OP_NEGATE:
            return simpleInstruction(L"OP_NEGATE", offset);
        case OP_END:
            return simpleInstruction(L"OP_END", offset);
        case OP_DUP:
            return simpleInstruction(L"OP_DUP", offset);
        case OP_DOUBLE_DUP:
            return simpleInstruction(L"OP_DOUBLE_DUP", offset);
        case OP_JUMP:
            return jumpInstruction(L"OP_JUMP", 1, chunk, offset);
        case OP_JUMP_IF_FALSE:
            return jumpInstruction(L"OP_JUMP_IF_FALSE", 1, chunk, offset);
        case OP_LOOP:
            return jumpInstruction(L"OP_LOOP", -1, chunk, offset);
        case OP_CALL:
            return byteInstruction(L"OP_CALL", chunk, offset);
        case OP_INVOKE:
            return invokeInstruction(L"OP_INVOKE", chunk, offset);
        case OP_SUPER_INVOKE:
            return invokeInstruction(L"OP_SUPER_INVOKE", chunk, offset);
        case OP_CLOSURE: {
            offset++;
            uint8_t constant = chunk->code[offset++];
            wprintf(L"%-16s %4d ", L"OP_CLOSURE", constant);
            printValue(chunk->constants.values[constant]);
            wprintf(L"\n");

            ObjFunction* function = AS_FUNCTION(chunk->constants.values[constant]);
            for (int j = 0; j < function->upvalueCount; j++) {
                int isLocal = chunk->code[offset++];
                int index = chunk->code[offset++];
                wprintf(L"%04d      |                     %ls %d\n", offset - 2, isLocal ? L"local" : L"upvalue", index);
            }
            return offset;
        }
        case OP_CLOSE_UPVALUE:
            return simpleInstruction(L"OP_CLOSE_UPVALUE", offset);
        case OP_RETURN:
            return simpleInstruction(L"OP_RETURN", offset);
        case OP_CLASS:
            return constantInstruction(L"OP_CLASS", chunk, offset);
        case OP_INHERIT:
            return simpleInstruction(L"OP_INHERIT", offset);
        case OP_METHOD:
            return constantInstruction(L"OP_METHOD", chunk, offset);
        default:
            wprintf(L"Unknown opcode %d\n", instruction);
            return offset + 1;
    }
}
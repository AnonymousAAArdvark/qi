//
// Created by Andrew Yang on 8/29/21.
//

#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <ctype.h>

#include "common.h"
#include "compiler.h"
#include "debug.h"
#include "object.h"
#include "memory.h"
#include "vm.h"
#include "lib.h"

VM vm;

static void resetStack() {
    vm.stackTop = vm.stack;
    vm.frameCount = 0;
    vm.openUpvalues = NULL;
}

static void runtimeError(const char* format, ...) {
    va_list args;
    va_start(args, format);
    vfprintf(stderr, format, args);
    va_end(args);
    fputs("\n", stderr);

    for (int i = vm.frameCount - 1; i >= 0; i--) {
        CallFrame* frame = &vm.frames[i];
        ObjFunction* function = frame->closure->function;
        size_t instruction = frame->ip - function->chunk.code - 1;
        fprintf(stderr, "[line %d] in ", function->chunk.lines[instruction]);
        if (function->name == NULL) {
            fprintf(stderr, "script\n");
        } else {
            fprintf(stderr, "%s()\n", function->name->chars);
        }
    }

    resetStack();
}

static void defineNative(const char* name, NativeFn function, int arity) {
    push(OBJ_VAL(copyString(name, (int)strlen(name))));
    push(OBJ_VAL(newNative(function, arity)));
    tableSet(&vm.globals, AS_STRING(vm.stack[0]), vm.stack[1]);
    pop();
    pop();
}

void initVM() {
    resetStack();
    vm.objects = NULL;
    vm.bytesAllocated = 0;
    vm.nextGC = 1024 * 1024;

    vm.grayCount = 0;
    vm.grayCapacity = 0;
    vm.grayStack = NULL;

    initTable(&vm.globals);
    initTable(&vm.strings);

    vm.initString = NULL;
    vm.initString = copyString("init", 4);
    vm.markValue = true;

    defineNative("clock", clockNative, 0);
    defineNative("sqrt", sqrtNative, 1);
    defineNative("pow", powNative, 2);
    defineNative("min", minNative, 2);
    defineNative("max", maxNative, 2);
    defineNative("round", roundNative, -1);
    defineNative("ston", stonNative, 1);
    defineNative("nots", notsNative, 1);
    defineNative("type", typeNative, 1);
}

void freeVM() {
    freeTable(&vm.globals);
    freeTable(&vm.strings);
    vm.initString = NULL;
    freeObjects();
}

void push(Value value) {
    *vm.stackTop = value;
    vm.stackTop++;
}

Value pop() {
    vm.stackTop--;
    return *vm.stackTop;
}

static Value peek(int distance) {
    return vm.stackTop[-1 - distance];
}

static bool call(ObjClosure* closure, int argCount) {
    if (argCount != closure->function->arity) {
        runtimeError("Expected %d arguments but got %d.", closure->function->arity, argCount);
        return false;
    }

    if (vm.frameCount == FRAMES_MAX) {
        runtimeError("Stack overflow.");
        return false;
    }

    CallFrame* frame = &vm.frames[vm.frameCount++];
    frame->closure = closure;
    frame->ip = closure->function->chunk.code;
    frame->slots = vm.stackTop - argCount - 1;
    return true;
}

static bool callValue(Value callee, int argCount) {
    if (IS_OBJ(callee)) {
        switch (OBJ_TYPE(callee)) {
            case OBJ_BOUND_METHOD: {
                ObjBoundMethod* bound = AS_BOUND_METHOD(callee);
                vm.stackTop[-argCount - 1] = bound->receiver;
                return call(bound->method, argCount);
            }
            case OBJ_CLASS: {
                ObjClass* klass = AS_CLASS(callee);
                vm.stackTop[-argCount - 1] = OBJ_VAL(newInstance(klass));
                Value initializer;
                if (tableGet(&klass->methods, vm.initString, &initializer)) {
                    return call(AS_CLOSURE(initializer), argCount);
                } else if (argCount != 0) {
                    runtimeError("Expected 0 arguments but got %d.", argCount);
                    return false;
                }
                return true;
            }
            case OBJ_CLOSURE:
                return call(AS_CLOSURE(callee), argCount);
            case OBJ_NATIVE: {
                ObjNative* native = AS_NATIVE(callee);
                if (native->arity != -1 && argCount != native->arity) {
                    runtimeError("Expected %d arguments but got %d.", native->arity, argCount);
                    return false;
                }
                if (native->function(argCount, vm.stackTop - argCount)) {
                    vm.stackTop -= argCount;
                    return true;
                } else {
                    runtimeError(AS_STRING(vm.stackTop[-argCount - 1])->chars);
                    return false;
                }
            }
            default:
                break; // Non-callable object type.
        }
    }
    runtimeError("Can only call functions and classes.");
    return false;
}

static bool invokeFromClass(ObjClass* klass, ObjString* name, int argCount, CallFrame* frame, uint8_t* ip) {
    Value method;
    if (!tableGet(&klass->methods, name, &method)) {
        frame->ip = ip;
        runtimeError("Undefined property '%s'.", name->chars);
        return false;
    }
    return call(AS_CLOSURE(method), argCount);
}

static bool invokeInstance(const Value* receiver, ObjString* name, int argCount, CallFrame* frame, uint8_t* ip) {
    ObjInstance* instance = AS_INSTANCE(*receiver);

    Value value;
    if (tableGet(&instance->fields, name, &value)) {
        vm.stackTop[-argCount - 1] = value;
        return callValue(value, argCount);
    }

    return invokeFromClass(instance->klass, name, argCount, frame, ip);
}

static bool invokeString(const Value* receiver, ObjString* name, int argCount, CallFrame* frame, uint8_t* ip) {
    if (strcmp(name->chars, "length") == 0) {
        // Returns the length of the string
        vm.stackTop -= argCount + 1;
        push(NUMBER_VAL(AS_STRING(*receiver)->length));
        return true;
    } else if (strcmp(name->chars, "upper") == 0) {
        // Returns a string where all characters are in upper case.
        ObjString* str = AS_STRING(*receiver);

        char* chars = ALLOCATE(char, str->length + 1);
        memcpy(chars, str->chars, str->length);
        chars[str->length + 1] = '\0';
        char* c = chars;
        while (*c) {
            *c = (char)toupper(*c);
            c++;
        }
        ObjString* result = takeString(chars, str->length + 1);

        vm.stackTop -= argCount + 1;
        push(OBJ_VAL(result));
        return true;
    } else if (strcmp(name->chars, "lower") == 0) {
        // Returns a string where all characters are in lower case.
        ObjString* str = AS_STRING(*receiver);

        char* chars = ALLOCATE(char, str->length + 1);
        memcpy(chars, str->chars, str->length);
        chars[str->length + 1] = '\0';
        char* c = chars;
        while (*c) {
            *c = (char)tolower(*c);
            c++;
        }
        ObjString* result = takeString(chars, str->length + 1);

        vm.stackTop -= argCount + 1;
        push(OBJ_VAL(result));
        return true;
    } else if (strcmp(name->chars, "substring") == 0) {
        // Returns a part of a string between given indexes
        if (argCount != 2) {
            frame->ip = ip;
            runtimeError("Expected 2 arguments but got %d.", argCount);
            return false;
        } else if (!IS_NUMBER(peek(argCount - 1))) {
            frame->ip = ip;
            runtimeError("Argument 1 (begin) must be of type 'number', not '%s'.", getType(vm.stackTop[-argCount]));
            return false;
        } else if (!IS_NUMBER(peek(argCount - 2))) {
            frame->ip = ip;
            runtimeError("Argument 2 (end) must be of type 'number', not '%s'.", getType(vm.stackTop[-argCount]));
            return false;
        }

        ObjString* str = AS_STRING(*receiver);
        int begin = AS_NUMBER(peek(argCount - 1));
        int end = AS_NUMBER(peek(argCount - 2));
        if (begin < 0) begin = str->length + begin;
        if (end < 0) end = str->length + end;

        if (!isValidStringIndex(str, begin)) {
            frame->ip = ip;
            runtimeError("Argument 1 is not a valid index");
            return false;
        } else if (!isValidStringIndex(str, end - 1)) { // Ending index is exclusive
            frame->ip = ip;
            runtimeError("Argument 2 is not a valid index");
            return false;
        } else if (end < begin) {
            frame->ip = ip;
            runtimeError("End index cannot be before begin index.");
            return false;
        }

        char* chars = ALLOCATE(char, end - begin + 1);
        memcpy( chars, &str->chars[begin], end - begin );
        chars[end - begin] = '\0';
        ObjString* result = takeString(chars, end - begin + 1);

        vm.stackTop -= argCount + 1;
        push(OBJ_VAL(result));
        return true;
    }
    frame->ip = ip;
    runtimeError("Undefined property %s.", name->chars);
    return false;
}

static bool invokeList(const Value* receiver, ObjString* name, int argCount, CallFrame* frame, uint8_t* ip) {
    if (strcmp(name->chars, "push") == 0) {
        // Push a value to the end of a list increasing the list's length by 1
        if (argCount != 1) {
            frame->ip = ip;
            runtimeError("Expected 1 argument but got %d.", argCount);
            return false;
        }
        ObjList *list = AS_LIST(*receiver);
        Value item = peek(argCount - 1);
        insertToList(list, item, list->count);
        vm.stackTop -= argCount + 1;
        push(NIL_VAL);
        return true;
    } else if (strcmp(name->chars, "pop") == 0) {
        // Pop a value from the end of a list decreasing the list's length by 1
        if (argCount != 0) {
            frame->ip = ip;
            runtimeError("Expected 0 arguments but got %d.", argCount);
            return false;
        }

        ObjList *list = AS_LIST(*receiver);

        if (!isValidListIndex(list, list->count - 1)) {
            frame->ip = ip;
            runtimeError("Cannot pop from an empty list.");
            return false;
        }

        deleteFromList(list, list->count - 1);
        vm.stackTop -= argCount + 1;
        push(NIL_VAL);
        return true;
    } else if (strcmp(name->chars, "insert") == 0) {
        // Insert a value to the specified index of a list increasing the list's length by 1
        if (argCount != 2) {
            frame->ip = ip;
            runtimeError("Expected 2 arguments but got %d.", argCount);
            return false;
        } else if (!IS_NUMBER(peek(argCount - 1))) {
            frame->ip = ip;
            runtimeError("Argument 1 (index) must be of type 'number', not '%s'.", getType(vm.stackTop[-argCount]));
            return false;
        }

        ObjList *list = AS_LIST(*receiver);
        int index = AS_NUMBER(peek(argCount - 1));
        if (index < 0) index = list->count + index;
        Value item = peek(argCount - 2);

        if (!isValidListIndex(list, index)) {
            frame->ip = ip;
            runtimeError("Argument 1 is not a valid index");
            return false;
        }

        insertToList(list, item, index);
        vm.stackTop -= argCount + 1;
        push(NIL_VAL);
        return true;
    } else if (strcmp(name->chars, "delete") == 0) {
        // Delete an item from a list at the given index.
        if (argCount != 1) {
            frame->ip = ip;
            runtimeError("Expected 1 argument but got %d.", argCount);
            return false;
        } else if (!IS_NUMBER(peek(argCount - 1))) {
            frame->ip = ip;
            runtimeError("Argument 1 (index) must be of type 'number', not '%s'.", getType(vm.stackTop[-argCount]));
            return false;
        }

        ObjList* list = AS_LIST(*receiver);
        int index = AS_NUMBER(peek(argCount - 1));
        if (index < 0) index = list->count + index;

        if (!isValidListIndex(list, index)) {
            frame->ip = ip;
            runtimeError("Argument 1 is not a valid index");
            return false;
        }

        deleteFromList(list, index);
        vm.stackTop -= argCount + 1;
        push(NIL_VAL);
        return true;
    } else if (strcmp(name->chars, "length") == 0) {
        // Returns the length of the list
        vm.stackTop -= argCount + 1;
        push(NUMBER_VAL(AS_LIST(*receiver)->count));
        return true;
    }

    frame->ip = ip;
    runtimeError("Undefined property %s.", name->chars);
    return false;
}

static bool invoke(ObjString* name, int argCount, CallFrame* frame, uint8_t* ip) {
    Value receiver = peek(argCount);

    if (IS_INSTANCE(receiver)) {
        return invokeInstance(&receiver, name, argCount, frame, ip);
    } else if (IS_STRING(receiver)) {
        return invokeString(&receiver, name, argCount, frame, ip);
    } else if (IS_LIST(receiver)) {
        return invokeList(&receiver, name, argCount, frame, ip);
    }

    frame->ip = ip;
    runtimeError("Only instances, strings, and lists have methods");
    return false;
}

static bool bindMethod(ObjClass* klass, ObjString* name, CallFrame* frame, uint8_t* ip) {
    Value method;
    if (!tableGet(&klass->methods, name, &method)) {
        frame->ip = ip;
        runtimeError("Undefined property '%s'.", name->chars);
        return false;
    }

    ObjBoundMethod* bound = newBoundMethod(peek(0), AS_CLOSURE(method));
    pop();
    push(OBJ_VAL(bound));
    return true;
}

static ObjUpvalue* captureUpvalue(Value* local) {
    ObjUpvalue* prevUpvalue = NULL;
    ObjUpvalue* upvalue = vm.openUpvalues;
    while (upvalue != NULL && upvalue->location > local) {
        prevUpvalue = upvalue;
        upvalue = upvalue->next;
    }

    if (upvalue != NULL && upvalue->location == local) {
        return upvalue;
    }

    ObjUpvalue* createdUpvalue = newUpvalue(local);
    createdUpvalue->next = upvalue;

    if (prevUpvalue == NULL) {
        vm.openUpvalues = createdUpvalue;
    } else {
        prevUpvalue->next = createdUpvalue;
    }
    return createdUpvalue;
}

static void closeUpvalues(const Value* last) {
    while (vm.openUpvalues != NULL && vm.openUpvalues->location >= last) {
        ObjUpvalue* upvalue = vm.openUpvalues;
        upvalue->closed = *upvalue->location;
        upvalue->location = &upvalue->closed;
        vm.openUpvalues = upvalue->next;
    }
}

static void defineMethod(ObjString* name) {
    Value method = peek(0);
    ObjClass* klass = AS_CLASS(peek(1));
    tableSet(&klass->methods, name, method);
    pop();
}

static bool isFalsey(Value value) {
    return IS_NIL(value) || (IS_BOOL(value) && !AS_BOOL(value));
}

static void concatenate() {
    ObjString* b = AS_STRING(peek(0));
    ObjString* a = AS_STRING(peek(1));

    int length = a->length + b->length;
    char* chars = ALLOCATE(char, length + 1);
    memcpy(chars, a->chars, a->length);
    memcpy(chars + a->length, b->chars, b->length);
    chars[length] = '\0';

    ObjString* result = takeString(chars, length);
    pop();
    pop();
    push(OBJ_VAL(result));
}

static InterpretResult run() {
    CallFrame* frame = &vm.frames[vm.frameCount - 1];
    register uint8_t* ip = frame->ip;

#define READ_BYTE() (*ip++)

#define READ_SHORT() \
    (ip += 2, (uint16_t)((ip[-2] << 8) | ip[-1]))

#define READ_CONSTANT() \
    (frame->closure->function->chunk.constants.values[READ_BYTE()])

#define READ_STRING() AS_STRING(READ_CONSTANT())
#define BINARY_OP(valueType, op) \
    do { \
      if (!IS_NUMBER(peek(0)) || !IS_NUMBER(peek(1))) { \
        printValue(peek(0));     \
        printValue(peek(1));\
        frame->ip = ip; \
        runtimeError("Operands must be numbers."); \
        return INTERPRET_RUNTIME_ERROR; \
      } \
      double b = AS_NUMBER(pop()); \
      double a = AS_NUMBER(pop()); \
      push(valueType(a op b)); \
    } while (false)

    for(;;) {
#ifdef DEBUG_TRACE_EXECUTION
        printf("          ");
        for (Value *slot = vm.stack; slot < vm.stackTop; slot++) {
            printf("[ ");
            printValue(*slot);
            printf(" ]");
        }
        printf("\n");
        disassembleInstruction(&frame->closure->function->chunk,
                               (int) (frame->ip - frame->closure->function->chunk.code));
#endif
        switch (READ_BYTE()) {
            case OP_CONSTANT: {
                Value constant = READ_CONSTANT();
                push(constant);
                break;
            }
            case OP_NIL:
                push(NIL_VAL);
                break;
            case OP_TRUE:
                push(BOOL_VAL(true));
                break;
            case OP_FALSE:
                push(BOOL_VAL(false));
                break;
            case OP_POP:
                pop();
                break;
            case OP_SET_LOCAL: {
                uint8_t slot = READ_BYTE();
                frame->slots[slot] = peek(0);
                break;
            }
            case OP_GET_LOCAL: {
                uint8_t slot = READ_BYTE();
                push(frame->slots[slot]);
                break;
            }
            case OP_GET_GLOBAL: {
                ObjString *name = READ_STRING();
                Value value;
                if (!tableGet(&vm.globals, name, &value)) {
                    frame->ip = ip;
                    runtimeError("Undefined variable '%s'.", name->chars);
                    return INTERPRET_RUNTIME_ERROR;
                }
                push(value);
                break;
            }
            case OP_DEFINE_GLOBAL: {
                ObjString *name = READ_STRING();
                tableSet(&vm.globals, name, peek(0));
                pop();
                break;
            }
            case OP_SET_GLOBAL: {
                ObjString *name = READ_STRING();
                if (tableSet(&vm.globals, name, peek(0))) {
                    tableDelete(&vm.globals, name);
                    frame->ip = ip;
                    runtimeError("Undefined variable '%s'.", name->chars);
                    return INTERPRET_RUNTIME_ERROR;
                }
                break;
            }
            case OP_GET_UPVALUE: {
                uint8_t slot = READ_BYTE();
                push(*frame->closure->upvalues[slot]->location);
                break;
            }
            case OP_SET_UPVALUE: {
                uint8_t slot = READ_BYTE();
                *frame->closure->upvalues[slot]->location = peek(0);
                break;
            }
            case OP_GET_PROPERTY: {
                if (!IS_INSTANCE(peek(0))) {
                    frame->ip = ip;
                    runtimeError("Only instances have properties.");
                    return INTERPRET_RUNTIME_ERROR;
                }
                ObjInstance *instance = AS_INSTANCE(peek(0));
                ObjString *name = READ_STRING();

                Value value;
                if (tableGet(&instance->fields, name, &value)) {
                    pop(); // Instance.
                    push(value);
                    break;
                }

                if (!bindMethod(instance->klass, name, frame, ip)) {
                    return INTERPRET_RUNTIME_ERROR;
                }
                break;
            }
            case OP_SET_PROPERTY: {
                if (!IS_INSTANCE(peek(1))) {
                    frame->ip = ip;
                    runtimeError("Only instances have fields.");
                    return INTERPRET_RUNTIME_ERROR;
                }

                ObjInstance *instance = AS_INSTANCE(peek(1));
                tableSet(&instance->fields, READ_STRING(), peek(0));
                Value value = pop();
                pop();
                push(value);
                break;
            }
            case OP_GET_SUPER: {
                ObjString *name = READ_STRING();
                ObjClass *superclass = AS_CLASS(pop());

                if (!bindMethod(superclass, name, frame, ip)) {
                    return INTERPRET_RUNTIME_ERROR;
                }
                break;
            }
            case OP_EQUAL: {
                Value b = pop();
                Value a = pop();
                push(BOOL_VAL(valuesEqual(a, b)));
                break;
            }
            case OP_GREATER:
                BINARY_OP(BOOL_VAL, >);
                break;
            case OP_LESS:
                BINARY_OP(BOOL_VAL, <);
                break;
            case OP_ADD:
                if (IS_STRING(peek(0)) && IS_STRING(peek(1))) {
                    concatenate();
                } else if (IS_NUMBER(peek(0)) && IS_NUMBER(peek(1))) {
                    double b = AS_NUMBER(pop());
                    double a = AS_NUMBER(pop());
                    push(NUMBER_VAL(a + b));
                } else {
                    frame->ip = ip;
                    runtimeError("Operands must be two numbers or two strings.");
                    return INTERPRET_RUNTIME_ERROR;
                }
                break;
            case OP_SUBTRACT:
                BINARY_OP(NUMBER_VAL, -);
                break;
            case OP_MULTIPLY:
                BINARY_OP(NUMBER_VAL, *);
                break;
            case OP_DIVIDE:
                BINARY_OP(NUMBER_VAL, /);
                break;
            case OP_NOT:
                push(BOOL_VAL(isFalsey(pop())));
                break;
            case OP_NEGATE:
                if (!IS_NUMBER(peek(0))) {
                    frame->ip = ip;
                    runtimeError("Operand must be a number.");
                    return INTERPRET_RUNTIME_ERROR;
                }
                push(NUMBER_VAL(-AS_NUMBER(pop())));
                break;
            case OP_PRINT: {
                printValue(pop());
                printf("\n");
                break;
            }
            case OP_JUMP: {
                uint16_t offset = READ_SHORT();
                ip += offset;
                break;
            }
            case OP_JUMP_IF_FALSE: {
                uint16_t offset = READ_SHORT();
                if (isFalsey(peek(0))) ip += offset;
                break;
            }
            case OP_LOOP: {
                uint16_t offset = READ_SHORT();
                ip -= offset;
                break;
            }
            case OP_CALL: {
                int argCount = READ_BYTE();
                frame->ip = ip;
                if (!callValue(peek(argCount), argCount)) {
                    return INTERPRET_RUNTIME_ERROR;
                }
                frame = &vm.frames[vm.frameCount - 1];
                ip = frame->ip;
                break;
            }
            case OP_INVOKE: {
                ObjString *method = READ_STRING();
                int argCount = READ_BYTE();
                frame->ip = ip;
                if (!invoke(method, argCount, frame, ip)) {
                    return INTERPRET_RUNTIME_ERROR;
                }
                frame = &vm.frames[vm.frameCount - 1];
                ip = frame->ip;
                break;
            }
            case OP_SUPER_INVOKE: {
                ObjString *method = READ_STRING();
                int argCount = READ_BYTE();
                frame->ip = ip;
                ObjClass *superclass = AS_CLASS(pop());
                if (!invokeFromClass(superclass, method, argCount, frame, ip)) {
                    return INTERPRET_RUNTIME_ERROR;
                }
                frame = &vm.frames[vm.frameCount - 1];
                ip = frame->ip;
                break;
            }
            case OP_CLOSURE: {
                ObjFunction *function = AS_FUNCTION(READ_CONSTANT());
                ObjClosure *closure = newClosure(function);
                push(OBJ_VAL(closure));
                for (int i = 0; i < closure->upvalueCount; i++) {
                    uint8_t isLocal = READ_BYTE();
                    uint8_t index = READ_BYTE();
                    if (isLocal) {
                        closure->upvalues[i] = captureUpvalue(frame->slots + index);
                    } else {
                        closure->upvalues[i] = frame->closure->upvalues[index];
                    }
                }
                break;
            }
            case OP_CLOSE_UPVALUE:
                closeUpvalues(vm.stackTop - 1);
                pop();
                break;
            case OP_RETURN: {
                Value result = pop();
                closeUpvalues(frame->slots);
                vm.frameCount--;
                if (vm.frameCount == 0) {
                    pop();
                    return INTERPRET_OK;
                }

                vm.stackTop = frame->slots;
                push(result);
                frame = &vm.frames[vm.frameCount - 1];
                ip = frame->ip;
                break;
            }
            case OP_CLASS:
                push(OBJ_VAL(newClass(READ_STRING())));
                break;
            case OP_INHERIT: {
                Value superclass = peek(1);
                if (!IS_CLASS(superclass)) {
                    frame->ip = ip;
                    runtimeError("Superclass must be a class.");
                    return INTERPRET_RUNTIME_ERROR;
                }
                ObjClass *subclass = AS_CLASS(peek(0));
                tableAddAll(&AS_CLASS(superclass)->methods, &subclass->methods);
                pop(); // Subclass.
                break;
            }
            case OP_METHOD:
                defineMethod(READ_STRING());
                break;
            case OP_DUP: push(peek(0)); break;
            case OP_BUILD_LIST: {
                // Stack before: [item1, item2, ..., itemN] and after: [list]
                ObjList* list = newList();
                uint8_t itemCount = READ_BYTE();

                // Add items to list
                push(OBJ_VAL(list)); // So list isn't sweeped by GC in insertToList
                for (int i = itemCount; i > 0; i--) {
                    insertToList(list, peek(i), list->count);
                }
                pop();

                // Pop items from stack
                while (itemCount-- > 0) {
                    pop();
                }

                push(OBJ_VAL(list));
                break;
            }
            case OP_INDEX_SUBSCR: {
                // Stack before: [list, index] and after: [index(list, index)]
                Value index = pop();
                Value list = pop();
                Value result;

                if (!IS_LIST(list)) {
                    runtimeError("Invalid type to index into.");
                    return INTERPRET_RUNTIME_ERROR;
                }
                ObjList* objList = AS_LIST(list);

                if (!IS_NUMBER(index)) {
                    runtimeError("List index is not a number.");
                    return INTERPRET_RUNTIME_ERROR;
                }
                int numIndex = AS_NUMBER(index);
                if (numIndex < 0) numIndex = objList->count + numIndex;

                if (!isValidListIndex(objList, numIndex)) {
                    runtimeError("List index out of range.");
                    return INTERPRET_RUNTIME_ERROR;
                }

                result = indexFromList(objList, numIndex);
                push(result);
                break;
            }
            case OP_STORE_SUBSCR: {
                // Stack before: [list, index, item] and after: [item]
                Value item = pop();
                Value index = pop();
                Value list = pop();

                if (!IS_LIST(list)) {
                    runtimeError("Cannot store value in a non-list.");
                    return INTERPRET_RUNTIME_ERROR;
                }
                ObjList* objList = AS_LIST(list);

                if (!IS_NUMBER(index)) {
                    runtimeError("List index is not a number.");
                    return INTERPRET_RUNTIME_ERROR;
                }
                int numIndex = AS_NUMBER(index);
                if (numIndex < 0) numIndex = objList->count + numIndex;

                if (!isValidListIndex(objList, numIndex)) {
                    runtimeError("Invalid list index.");
                    return INTERPRET_RUNTIME_ERROR;
                }

                storeToList(objList, numIndex, item);
                push(item);
                break;
            }
        }
    }

#undef READ_BYTE
#undef READ_SHORT
#undef READ_CONSTANT
#undef READ_STRING
#undef BINARY_OP
}

InterpretResult interpret(const char* source) {
    ObjFunction* function = compile(source);
    if (function == NULL) return INTERPRET_COMPILE_ERROR;

    push(OBJ_VAL(function));
    ObjClosure* closure = newClosure(function);
    pop();
    push(OBJ_VAL(closure));
    call(closure, 0);

    return run();
}
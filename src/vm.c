//
// Created by Andrew Yang on 8/29/21.
//

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <ctype.h>
#include <math.h>

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

static void runtimeError(const wchar_t* format, ...) {
    va_list args;
    va_start(args, format);
    vfwprintf(stderr, format, args);
    va_end(args);
    fputs("\n", stderr);

    for (int i = vm.frameCount - 1; i >= 0; i--) {
        CallFrame* frame = &vm.frames[i];
        ObjFunction* function = frame->closure->function;
        size_t instruction = frame->ip - function->chunk.code - 1;
        fwprintf(stderr, L"【行 %d】在 ", function->chunk.lines[instruction]);
        if (function->name == NULL) {
            fwprintf(stderr, L"脚本\n");
        } else {
            fwprintf(stderr, L"%ls（）\n", function->name->chars);
        }
    }

    resetStack();
}

static void defineNative(const wchar_t* name, NativeFn function, int arity) {
    push(OBJ_VAL(copyString(name, (int)wcslen(name))));
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
    vm.initString = copyString(L"初始化", 3);
    vm.markValue = true;

    defineNative(L"打印", printNative, 1);
    defineNative(L"打印行", printlnNative, 1);
    defineNative(L"扫描", scanNative, 0);
    defineNative(L"时钟", clockNative, 0);
    defineNative(L"平方根", sqrtNative, 1);
    defineNative(L"次方", powNative, 2);
    defineNative(L"最小", minNative, 2);
    defineNative(L"最大", maxNative, 2);
    defineNative(L"四舍五入", roundNative, -1);
    defineNative(L"串到数", stonNative, 1);
    defineNative(L"数到串", ntosNative, 1);
    defineNative(L"型", typeofNative, 1);
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
        runtimeError(L"需要 %d 个参数，但得到 %d。", closure->function->arity, argCount);
        return false;
    }

    if (vm.frameCount == FRAMES_MAX) {
        runtimeError(L"堆栈溢出。");
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
                    runtimeError(L"需要 0 个参数，但得到 %d。", argCount);
                    return false;
                }
                return true;
            }
            case OBJ_CLOSURE:
                return call(AS_CLOSURE(callee), argCount);
            case OBJ_NATIVE: {
                ObjNative* native = AS_NATIVE(callee);
                if (native->arity != -1 && argCount != native->arity) {
                    runtimeError(L"需要 %d 个参数，但得到 %d。", native->arity, argCount);
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
    runtimeError(L"只能调用功能和类。");
    return false;
}

static bool invokeFromClass(ObjClass* klass, ObjString* name, int argCount, CallFrame* frame, uint8_t* ip) {
    Value method;
    if (!tableGet(&klass->methods, name, &method)) {
        frame->ip = ip;
        runtimeError(L"未定义的属性 %ls", name->chars);
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
    if (wcscmp(name->chars, L"长度") == 0) {
        // Returns the length of the string
        vm.stackTop -= argCount + 1;
        push(NUMBER_VAL(AS_STRING(*receiver)->length));
        return true;
    } else if (wcscmp(name->chars, L"大写") == 0) {
        // Returns a string where all characters are in upper case.
        ObjString* str = AS_STRING(*receiver);

        wchar_t* chars = ALLOCATE(wchar_t, str->length + 1);
        wcscpy(chars, str->chars);
        chars[str->length + 1] = L'\0';
        wchar_t* c = chars;
        while (*c) {
            *c = towupper(*c);
            c++;
        }
        ObjString* result = takeString(chars, str->length + 1);

        vm.stackTop -= argCount + 1;
        push(OBJ_VAL(result));
        return true;
    } else if (wcscmp(name->chars, L"小写") == 0) {
        // Returns a string where all characters are in lower case.
        ObjString* str = AS_STRING(*receiver);

        wchar_t* chars = ALLOCATE(wchar_t, str->length + 1);
        wcscpy(chars, str->chars);
        chars[str->length + 1] = L'\0';
        wchar_t* c = chars;
        while (*c) {
            *c = towlower(*c);
            c++;
        }
        ObjString* result = takeString(chars, str->length + 1);

        vm.stackTop -= argCount + 1;
        push(OBJ_VAL(result));
        return true;
    } else if (wcscmp(name->chars, L"子串") == 0) {
        // Returns a part of a string between given indexes
        if (argCount != 2) {
            frame->ip = ip;
            runtimeError(L"Expected 2 arguments but got %d.", argCount);
            return false;
        } else if (!IS_NUMBER(peek(argCount - 1))) {
            frame->ip = ip;
            runtimeError(L"Argument 1 (begin) must be of type 'number', not '%ls'.", getType(vm.stackTop[-argCount]));
            return false;
        } else if (!IS_NUMBER(peek(argCount - 2))) {
            frame->ip = ip;
            runtimeError(L"Argument 2 (end) must be of type 'number', not '%ls'.", getType(vm.stackTop[-argCount]));
            return false;
        }

        ObjString* str = AS_STRING(*receiver);
        int begin = AS_NUMBER(peek(argCount - 1));
        int end = AS_NUMBER(peek(argCount - 2));
        if (begin < 0) begin = str->length + begin;
        if (end < 0) end = str->length + end;

        if (!isValidStringIndex(str, begin)) {
            frame->ip = ip;
            runtimeError(L"Argument 1 is not a valid index");
            return false;
        } else if (!isValidStringIndex(str, end - 1)) { // Ending index is exclusive
            frame->ip = ip;
            runtimeError(L"Argument 2 is not a valid index");
            return false;
        } else if (end < begin) {
            frame->ip = ip;
            runtimeError(L"End index cannot be before begin index.");
            return false;
        }

        wchar_t* chars = ALLOCATE(wchar_t, end - begin + 1);
        memcpy( chars, &str->chars[begin], (end - begin) * sizeof(wchar_t) );
        chars[end - begin] = L'\0';
        ObjString* result = takeString(chars, end - begin + 1);

        vm.stackTop -= argCount + 1;
        push(OBJ_VAL(result));
        return true;
    }
    frame->ip = ip;
    runtimeError(L"Undefined property '%ls'.", name->chars);
    return false;
}

static bool invokeList(const Value* receiver, ObjString* name, int argCount, CallFrame* frame, uint8_t* ip) {
    if (wcscmp(name->chars, L"推") == 0) {
        // Push a value to the end of a list increasing the list's length by 1
        if (argCount != 1) {
            frame->ip = ip;
            runtimeError(L"Expected 1 argument but got %d.", argCount);
            return false;
        }
        ObjList *list = AS_LIST(*receiver);
        Value item = peek(argCount - 1);
        insertToList(list, item, list->count);
        vm.stackTop -= argCount + 1;
        push(NIL_VAL);
        return true;
    } else if (wcscmp(name->chars, L"弹") == 0) {
        // Pop a value from the end of a list decreasing the list's length by 1
        if (argCount != 0) {
            frame->ip = ip;
            runtimeError(L"Expected 0 arguments but got %d.", argCount);
            return false;
        }

        ObjList *list = AS_LIST(*receiver);

        if (!isValidListIndex(list, list->count - 1)) {
            frame->ip = ip;
            runtimeError(L"Cannot pop from an empty list.");
            return false;
        }

        deleteFromList(list, list->count - 1);
        vm.stackTop -= argCount + 1;
        push(NIL_VAL);
        return true;
    } else if (wcscmp(name->chars, L"插") == 0) {
        // Insert a value to the specified index of a list increasing the list's length by 1
        if (argCount != 2) {
            frame->ip = ip;
            runtimeError(L"Expected 2 arguments but got %d.", argCount);
            return false;
        } else if (!IS_NUMBER(peek(argCount - 1))) {
            frame->ip = ip;
            runtimeError(L"Argument 1 (index) must be of type 'number', not '%ls'.", getType(vm.stackTop[-argCount]));
            return false;
        }

        ObjList *list = AS_LIST(*receiver);
        int index = AS_NUMBER(peek(argCount - 1));
        if (index < 0) index = list->count + index;
        Value item = peek(argCount - 2);

        if (!isValidListIndex(list, index)) {
            frame->ip = ip;
            runtimeError(L"Argument 1 is not a valid index");
            return false;
        }

        insertToList(list, item, index);
        vm.stackTop -= argCount + 1;
        push(NIL_VAL);
        return true;
    } else if (wcscmp(name->chars, L"删") == 0) {
        // Delete an item from a list at the given index.
        if (argCount != 1) {
            frame->ip = ip;
            runtimeError(L"Expected 1 argument but got %d.", argCount);
            return false;
        } else if (!IS_NUMBER(peek(argCount - 1))) {
            frame->ip = ip;
            runtimeError(L"Argument 1 (index) must be of type 'number', not '%ls'.", getType(vm.stackTop[-argCount]));
            return false;
        }

        ObjList* list = AS_LIST(*receiver);
        int index = AS_NUMBER(peek(argCount - 1));
        if (index < 0) index = list->count + index;

        if (!isValidListIndex(list, index)) {
            frame->ip = ip;
            runtimeError(L"Argument 1 is not a valid index");
            return false;
        }

        deleteFromList(list, index);
        vm.stackTop -= argCount + 1;
        push(NIL_VAL);
        return true;
    } else if (wcscmp(name->chars, L"长度") == 0) {
        // Returns the length of the list
        vm.stackTop -= argCount + 1;
        push(NUMBER_VAL(AS_LIST(*receiver)->count));
        return true;
    }

    frame->ip = ip;
    runtimeError(L"Undefined property '%ls'.", name->chars);
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
    runtimeError(L"Only instances, strings, and lists have methods");
    return false;
}

static bool bindMethod(ObjClass* klass, ObjString* name, CallFrame* frame, uint8_t* ip) {
    Value method;
    if (!tableGet(&klass->methods, name, &method)) {
        frame->ip = ip;
        runtimeError(L"Undefined property '%ls'.", name->chars);
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

static ObjString* concatenate(ObjString* a, ObjString* b) {
    int length = a->length + b->length;
    wchar_t* chars = ALLOCATE(wchar_t, length + 1);
    memcpy(chars, a->chars, a->length * sizeof(wchar_t));
    memcpy(chars + a->length, b->chars, b->length * sizeof(wchar_t));
    chars[length] = L'\0';

    ObjString* result = takeString(chars, length);
    return result;
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
        frame->ip = ip; \
        runtimeError(L"Operands must be numbers."); \
        return INTERPRET_RUNTIME_ERROR; \
      } \
      double b = AS_NUMBER(pop()); \
      double a = AS_NUMBER(pop()); \
      push(valueType(a op b)); \
    } while (false)
#define BINARY_FUNC_OP(valueType, op) \
    do { \
      if (!IS_NUMBER(peek(0)) || !IS_NUMBER(peek(1))) { \
        frame->ip = ip; \
        runtimeError(L"Operands must be numbers."); \
        return INTERPRET_RUNTIME_ERROR; \
      } \
      double b = AS_NUMBER(pop()); \
      double a = AS_NUMBER(pop());    \
      push(valueType(op(a, b))); \
    } while (false)

    for(;;) {
#ifdef DEBUG_TRACE_EXECUTION
        wprintf(L"          ");
        for (Value *slot = vm.stack; slot < vm.stackTop; slot++) {
            wprintf(L"[ ");
            printValue(*slot);
            wprintf(L" ]");
        }
        wprintf(L"\n");
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
                    runtimeError(L"Undefined 变量 '%ls'.", name->chars);
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
                    runtimeError(L"Undefined 变量 '%ls'.", name->chars);
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
                    runtimeError(L"Only instances have properties.");
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
                    runtimeError(L"Only instances have fields.");
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
                    ObjString* b = AS_STRING(peek(0));
                    ObjString* a = AS_STRING(peek(1));
                    ObjString* result = concatenate(a, b);
                    pop();
                    pop();
                    push(OBJ_VAL(result));
                } else if (IS_NUMBER(peek(0)) && IS_NUMBER(peek(1))) {
                    double b = AS_NUMBER(pop());
                    double a = AS_NUMBER(pop());
                    push(NUMBER_VAL(a + b));
                } else {
                    frame->ip = ip;
                    runtimeError(L"Operands must be two numbers or two strings.");
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
            case OP_MODULO:
                BINARY_FUNC_OP(NUMBER_VAL, fmod);
                break;
            case OP_NOT:
                push(BOOL_VAL(isFalsey(pop())));
                break;
            case OP_NEGATE:
                if (!IS_NUMBER(peek(0))) {
                    frame->ip = ip;
                    runtimeError(L"Operand must be a number.");
                    return INTERPRET_RUNTIME_ERROR;
                }
                push(NUMBER_VAL(-AS_NUMBER(pop())));
                break;
            case OP_INCREMENT: {
                if (!IS_NUMBER(peek(0))) {
                    frame->ip = ip;
                    runtimeError(L"Operand must be a number.");
                    return INTERPRET_RUNTIME_ERROR;
                }
                push(NUMBER_VAL(AS_NUMBER(pop()) + 1));
                break;
            }
            case OP_DECREMENT: {
                if (!IS_NUMBER(peek(0))) {
                    frame->ip = ip;
                    runtimeError(L"Operand must be a number.");
                    return INTERPRET_RUNTIME_ERROR;
                }
                push(NUMBER_VAL(AS_NUMBER(pop()) - 1));
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
                    runtimeError(L"超类 must be a 类.");
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
            case OP_DOUBLE_DUP: push(peek(1)); push(peek(1)); break;
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
                Value obj = pop();

                if (IS_STRING(obj)) {
                    ObjString *objString = AS_STRING(obj);

                    if (!IS_NUMBER(index)) {
                        frame->ip = ip;
                        runtimeError(L"String index is not a number.");
                        return INTERPRET_RUNTIME_ERROR;
                    }
                    int numIndex = AS_NUMBER(index);
                    if (numIndex < 0) numIndex = objString->length + numIndex;

                    if (!isValidStringIndex(objString, numIndex)) {
                        frame->ip = ip;
                        runtimeError(L"String index out of range.");
                        return INTERPRET_RUNTIME_ERROR;
                    }
                    wchar_t* result = ALLOCATE(wchar_t, 1);
                    result[0] = indexFromString(objString, numIndex);
                    push(OBJ_VAL(takeString(result, 1)));
                    break;
                } else if (IS_LIST(obj)) {
                    ObjList *objList = AS_LIST(obj);

                    if (!IS_NUMBER(index)) {
                        frame->ip = ip;
                        runtimeError(L"List index is not a number.");
                        return INTERPRET_RUNTIME_ERROR;
                    }
                    int numIndex = AS_NUMBER(index);
                    if (numIndex < 0) numIndex = objList->count + numIndex;

                    if (!isValidListIndex(objList, numIndex)) {
                        frame->ip = ip;
                        runtimeError(L"List index out of range.");
                        return INTERPRET_RUNTIME_ERROR;
                    }

                    Value result = indexFromList(objList, numIndex);
                    push(result);
                    break;
                }

                frame->ip = ip;
                runtimeError(L"Invalid type to index into.");
                return INTERPRET_RUNTIME_ERROR;
            }
            case OP_STORE_SUBSCR: {
                // Stack before: [list, index, item] and after: [item]
                Value item = pop();
                Value index = pop();
                Value obj = pop();

                if (IS_STRING(obj)) {
                    ObjString* objString = AS_STRING(obj);

                    if (!IS_NUMBER(index)) {
                        frame->ip = ip;
                        runtimeError(L"String index is not a number.");
                        return INTERPRET_RUNTIME_ERROR;
                    } else if (!IS_STRING(item)) {
                        frame->ip = ip;
                        runtimeError(L"Only characters can be stored in strings.");
                        return INTERPRET_RUNTIME_ERROR;
                    }

                    ObjString* itemString = AS_STRING(item);
                    int numIndex = AS_NUMBER(index);
                    if (numIndex < 0) numIndex = objString->length + numIndex;

                    if (!isValidStringIndex(objString, numIndex)) {
                        frame->ip = ip;
                        runtimeError(L"Invalid string index.");
                        return INTERPRET_RUNTIME_ERROR;
                    } else if (wcslen(itemString->chars) != 1) {
                        frame->ip = ip;
                        runtimeError(
                                L"Expected string of length 1 but got length %d.", wcslen(itemString->chars));
                        return INTERPRET_RUNTIME_ERROR;
                    }

                    storeToString(objString, numIndex, itemString->chars[0]);
                    push(item);
                    break;
                } else if (IS_LIST(obj)) {
                    ObjList *objList = AS_LIST(obj);

                    if (!IS_NUMBER(index)) {
                        frame->ip = ip;
                        runtimeError(L"List index is not a number.");
                        return INTERPRET_RUNTIME_ERROR;
                    }
                    int numIndex = AS_NUMBER(index);
                    if (numIndex < 0) numIndex = objList->count + numIndex;

                    if (!isValidListIndex(objList, numIndex)) {
                        frame->ip = ip;
                        runtimeError(L"Invalid list index.");
                        return INTERPRET_RUNTIME_ERROR;
                    }

                    storeToList(objList, numIndex, item);
                    push(item);
                    break;
                }

                frame->ip = ip;
                runtimeError(L"Cannot store value: 变量 is not a string or list.");
                return INTERPRET_RUNTIME_ERROR;
            }
        }
    }

#undef READ_BYTE
#undef READ_SHORT
#undef READ_CONSTANT
#undef READ_STRING
#undef BINARY_FUNC_OP
#undef BINARY_OP
}

InterpretResult interpret(const char* source) {
    wchar_t* wsource = ALLOCATE(wchar_t, strlen(source) + 1);
    mbstowcs(wsource, source, strlen(source) + 1);
    ObjFunction* function = compile(wsource);
    if (function == NULL) return INTERPRET_COMPILE_ERROR;

    push(OBJ_VAL(function));
    ObjClosure* closure = newClosure(function);
    pop();
    push(OBJ_VAL(closure));
    call(closure, 0);

    return run();
}
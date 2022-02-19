//
// Created by Andrew Yang on 9/2/21.
//

#include <stdio.h>
#include <string.h>

#include "memory.h"
#include "object.h"
#include "table.h"
#include "value.h"
#include "vm.h"

#define ALLOCATE_OBJ(type, objectType) \
    (type*)allocateObject(sizeof(type), objectType)

static Obj* allocateObject(size_t size, ObjType type) {
    Obj* object = (Obj*)reallocate(NULL, 0, size);
    object->type = type;
    object->isMarked = false;
    object->next = vm.objects;
    vm.objects = object;

#ifdef DEBUG_LOG_GC
    wprintf(L"%p allocate %zu for %d\n", (void*)object, size, type);
#endif

    return object;
}

ObjBoundMethod* newBoundMethod(Value reciever, ObjClosure* method) {
    ObjBoundMethod* bound = ALLOCATE_OBJ(ObjBoundMethod, OBJ_BOUND_METHOD);
    bound->receiver = reciever;
    bound->method = method;
    return bound;
}

ObjClass* newClass(ObjString* name) {
    ObjClass* klass = ALLOCATE_OBJ(ObjClass, OBJ_CLASS);
    klass->name = name;
    initTable(&klass->methods);
    return klass;
}

ObjClosure* newClosure(ObjFunction* function) {
    ObjUpvalue** upvalues = ALLOCATE(ObjUpvalue*, function->upvalueCount);
    for (int i = 0; i < function->upvalueCount; i++) {
        upvalues[i] = NULL;
    }

    ObjClosure* closure = ALLOCATE_OBJ(ObjClosure, OBJ_CLOSURE);
    closure->function = function;
    closure->upvalues = upvalues;
    closure->upvalueCount = function->upvalueCount;
    return closure;
}

ObjFunction* newFunction() {
    ObjFunction* function = ALLOCATE_OBJ(ObjFunction, OBJ_FUNCTION);
    function->arity = 0;
    function->upvalueCount = 0;
    function->name = NULL;
    initChunk(&function->chunk);
    return function;
}

ObjInstance* newInstance(ObjClass* klass) {
    ObjInstance* instance = ALLOCATE_OBJ(ObjInstance, OBJ_INSTANCE);
    instance->klass = klass;
    initTable(&instance->fields);
    return instance;
}

ObjNative* newNative(NativeFn function, int arity) {
    ObjNative* native = ALLOCATE_OBJ(ObjNative, OBJ_NATIVE);
    native->function = function;
    native->arity = arity;
    return native;
}

static ObjString* allocateString(wchar_t* chars, int length, uint32_t hash) {
    ObjString* string = ALLOCATE_OBJ(ObjString, OBJ_STRING);
    string->length = length;
    string->chars = chars;
    string->hash = hash;

    push(OBJ_VAL(string));
    tableSet(&vm.strings, string, NIL_VAL);
    pop();

    return string;
}

static uint32_t hashString(const wchar_t* key, int length) {
    uint32_t hash = 2166136261u;
    for (int i = 0; i < length; ++i) {
        hash ^= (uint8_t)key[i];
        hash *= 16777619;
    }
    return hash;
}

ObjString* takeString(wchar_t* chars, int length) {
    uint32_t hash = hashString(chars, length);
    ObjString* interned = tableFindString(&vm.strings, chars, length, hash);
    if (interned != NULL) {
        FREE_ARRAY(wchar_t, chars, length + 1);
        return interned;
    }
    return allocateString(chars, length, hash);
}

ObjString* copyString(const wchar_t* chars, int length) {
    uint32_t hash = hashString(chars, length);
    ObjString* interned = tableFindString(&vm.strings, chars, length, hash);
    if (interned != NULL) return interned;
    wchar_t* heapChars = ALLOCATE(wchar_t, length + 1);
    memcpy(heapChars, chars, length * sizeof(wchar_t));
    heapChars[length] = L'\0';
    return allocateString(heapChars, length, hash);
}

void storeToString(ObjString* string, int index, wchar_t value) {
    string->chars[index] = value;
}

wchar_t indexFromString(ObjString* string, int index) {
    return string->chars[index];
}

bool isValidStringIndex(ObjString* string, int index) {
    if (index < 0 || index > string->length - 1) {
        return false;
    }
    return true;
}

ObjUpvalue* newUpvalue(Value* slot) {
    ObjUpvalue* upvalue = ALLOCATE_OBJ(ObjUpvalue, OBJ_UPVALUE);
    upvalue->closed = NIL_VAL;
    upvalue->location = slot;
    upvalue->next = NULL;
    return upvalue;
}

static void printfunction(ObjFunction* function) {
    if (function->name == NULL) {
        wprintf(L"<script>");
        return;
    }
    wprintf(L"<fn %ls>", function->name->chars);
}

static void printList(ObjList* list) {
    wprintf(L"【");
    for (int i = 0; i < list->count; i++) {
        printValue(list->items[i]);
        if (i < list->count - 1) {
            wprintf(L"，");
        }
    }
    wprintf(L"】");
}

ObjList* newList() {
    ObjList* list = ALLOCATE_OBJ(ObjList, OBJ_LIST);
    list->items = NULL;
    list->count = 0;
    list->capacity = 0;
    return list;
}

void insertToList(ObjList* list, Value value, int index) {
    // Grow the array if necessary
    if (list->capacity < list->count + 1) {
        int oldCapacity = list->capacity;
        list->capacity = GROW_CAPACITY(oldCapacity);
        list->items = GROW_ARRAY(Value, list->items, oldCapacity, list->capacity);
    }
    for (int i = list->count; i > index; i--) {
        list->items[i] = list->items[i - 1];
    }
    list->items[index] = value;
    list->count++;
}

void storeToList(ObjList* list, int index, Value value) {
    list->items[index] = value;
}

Value indexFromList(ObjList* list, int index) {
    return list->items[index];
}

void deleteFromList(ObjList* list, int index) {
    for (int i = index; i < list->count - 1; i++) {
        list->items[i] = list->items[i+1];
    }
    list->items[list->count - 1] = NIL_VAL;
    list->count--;
}

bool isValidListIndex(ObjList* list, int index) {
    if (index < 0 || index > list->count - 1) {
        return false;
    }
    return true;
}

void printObject(Value value) {
    switch (OBJ_TYPE(value)) {
        case OBJ_BOUND_METHOD:
            printfunction(AS_BOUND_METHOD(value)->method->function);
            break;
        case OBJ_CLASS:
            wprintf(L"%ls", AS_CLASS(value)->name->chars);
            break;
        case OBJ_CLOSURE:
            printfunction(AS_CLOSURE(value)->function);
            break;
        case OBJ_FUNCTION:
            printfunction(AS_FUNCTION(value));
            break;
        case OBJ_INSTANCE:
            wprintf(L"%ls instance", AS_INSTANCE(value)->klass->name->chars);
            break;
        case OBJ_NATIVE:
            wprintf(L"<native fn>");
            break;
        case OBJ_STRING:
            wprintf(L"%ls", AS_WCSTRING(value));
            break;
        case OBJ_UPVALUE:
            wprintf(L"upvalue");
            break;
        case OBJ_LIST:
            printList(AS_LIST(value));
            break;
    }
}

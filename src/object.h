//
// Created by Andrew Yang on 9/2/21.
//

#ifndef QI_OBJECT_H
#define QI_OBJECT_H

#include "common.h"
#include "chunk.h"
#include "table.h"
#include "value.h"

#define OBJ_TYPE(value)        (AS_OBJ(value)->type)

#define IS_BOUND_METHOD(value) isObjType(value, OBJ_BOUND_METHOD);
#define IS_CLASS(value)        isObjType(value, OBJ_CLASS)
#define IS_CLOSURE(value)      isObjType(value, OBJ_CLOSURE)
#define IS_FUNCTION(value)     isObjType(value, OBJ_FUNCTION)
#define IS_INSTANCE(value)     isObjType(value, OBJ_INSTANCE)
#define IS_NATIVE(value)       isObjType(value, OBJ_NATIVE)
#define IS_STRING(value)       isObjType(value, OBJ_STRING)
#define IS_LIST(value)         isObjType(value, OBJ_LIST)

#define AS_BOUND_METHOD(value) ((ObjBoundMethod*)AS_OBJ(value))
#define AS_CLASS(value)        ((ObjClass*)AS_OBJ(value))
#define AS_CLOSURE(value)      ((ObjClosure*)AS_OBJ(value))
#define AS_NATIVE(value)       ((ObjNative*)AS_OBJ(value))
#define AS_FUNCTION(value)     ((ObjFunction*)AS_OBJ(value))
#define AS_INSTANCE(value)     ((ObjInstance*)AS_OBJ(value))
#define AS_STRING(value)       ((ObjString*)AS_OBJ(value))
#define AS_WCSTRING(value)     (((ObjString*)AS_OBJ(value))->chars)
#define AS_LIST(value)         ((ObjList*)AS_OBJ(value))

typedef enum {
    OBJ_BOUND_METHOD,
    OBJ_CLASS,
    OBJ_CLOSURE,
    OBJ_FUNCTION,
    OBJ_INSTANCE,
    OBJ_NATIVE,
    OBJ_STRING,
    OBJ_UPVALUE,
    OBJ_LIST
} ObjType;

struct Obj {
    ObjType type;
    bool isMarked;
    struct Obj* next;
};

typedef struct {
    Obj obj;
    int arity;
    int upvalueCount;
    Chunk chunk;
    ObjString* name;
} ObjFunction;

typedef bool (*NativeFn)(int argCount, Value* args);

typedef struct {
    Obj obj;
    int arity;
    NativeFn function;
} ObjNative;

struct ObjString {
    Obj obj;
    int length;
    wchar_t* chars;
    uint32_t hash;
};

typedef struct ObjUpvalue {
    Obj obj;
    Value* location;
    Value closed;
    struct ObjUpvalue* next;
} ObjUpvalue;

typedef struct {
    Obj obj;
    ObjFunction* function;
    ObjUpvalue** upvalues;
    int upvalueCount;
} ObjClosure;

typedef struct {
    Obj obj;
    ObjString* name;
    Table methods;
} ObjClass;

typedef struct {
    Obj obj;
    ObjClass* klass;
    Table fields;
    bool isStatic;
} ObjInstance;

typedef struct {
    Obj obj;
    Value receiver;
    ObjClosure* method;
    ObjNative* native;
} ObjBoundMethod;

typedef struct {
    Obj obj;
    int count;
    int capacity;
    Value* items;
} ObjList;

ObjBoundMethod* newBoundMethod(Value receiver, ObjClosure* method);
ObjBoundMethod* newBoundNative(Value reciever, ObjNative* native);
ObjClass* newClass(ObjString* name);
ObjClosure* newClosure(ObjFunction* function);
ObjFunction* newFunction();
ObjInstance* newInstance(ObjClass* klass, bool isStatic);
ObjNative* newNative(NativeFn function, int arity);
ObjString* takeString(wchar_t* chars, int length);
ObjString* copyString(const wchar_t* chars, int length);
ObjString* handleEscapeSequences(ObjString* string);
void storeToString(ObjString* string, int index, wchar_t value);
wchar_t indexFromString(ObjString* string, int index);
bool isValidStringIndex(ObjString* string, int index);
ObjUpvalue* newUpvalue(Value* slot);
ObjList* newList();
void insertToList(ObjList* list, Value value, int index);
void storeToList(ObjList* list, int index, Value value);
Value indexFromList(ObjList* list, int index);
void deleteFromList(ObjList* list, int index);
bool sortList(ObjList* list, int low, int high, ObjClosure* pred);
bool isValidListIndex(ObjList* list, int index);
void printObject(Value value);

static inline bool isObjType(Value value, ObjType type) {
    return IS_OBJ(value) && AS_OBJ(value)->type == type;
}

#endif //QI_OBJECT_H

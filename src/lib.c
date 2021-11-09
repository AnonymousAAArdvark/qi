//
// Created by Andrew Yang on 9/17/21.
//

#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <math.h>
#include <stdlib.h>

#include "lib.h"

static bool nativeError(Value* args, wchar_t* msg, ...) {
    va_list list;
    wchar_t error[100];
    va_start(list, msg);
    vswprintf(error, sizeof(error), msg, list);
    args[-1] = OBJ_VAL(copyString(error, 100));
    va_end(list);
    return false;
}

wchar_t* getType(Value value) {
    if (IS_BOOL(value)) return L"布尔";
    else if (IS_NUMBER(value)) return L"数字";
    else if (IS_NIL(value)) return L"空";
    else if (IS_OBJ(value)) {
        switch (AS_OBJ(value)->type) {
            case OBJ_NATIVE: return L"本机功能";
            case OBJ_BOUND_METHOD: return L"绑定方法";
            case OBJ_INSTANCE: return L"实例";
            case OBJ_FUNCTION: return L"功能";
            case OBJ_STRING: return L"字符串";
            case OBJ_LIST: return L"列表";
            case OBJ_UPVALUE: return L"升值";
            case OBJ_CLOSURE: return L"关闭";
            case OBJ_CLASS: return L"类";
        }
    }
    // Unreachable.
    return L"未知";
}

bool printNative(int argCount, Value* args) {
    printValue(args[0]);
    args[-1] = NIL_VAL;
    return true;
}

bool printlnNative(int argCount, Value* args) {
    printValue(args[0]);
    wprintf(L"\n");
    args[-1] = NIL_VAL;
    return true;
}

bool scanNative(int argCount, Value* args) {
    char input[100];
    while (fgets(input, 100, stdin) == NULL) {}
    input[ strlen(input)-1] = '\0';
    wchar_t* winput = ALLOCATE(wchar_t, strlen(input));
    mbstowcs(winput, input, strlen(input));
    args[-1] = OBJ_VAL(copyString(winput, wcslen(winput)));
    return true;
}

bool clockNative(int argCount, Value* args) {
    args[-1] = NUMBER_VAL((double)clock() / CLOCKS_PER_SEC);
    return true;
}

bool sqrtNative(int argCount, Value* args) {
    if (!IS_NUMBER(args[0])) {
        return nativeError(args,
                           L"参数1（输入）的类型必须是「数字」，而不是「%ls」。", getType(args[0]));
    }
    args[-1] = NUMBER_VAL(sqrt(AS_NUMBER(args[0])));
    return true;
}

bool powNative(int argCount, Value* args) {
    if (!IS_NUMBER(args[0])) {
        return nativeError(args,
                           L"参数1（基数）的类型必须是「数字」，而不是「%ls」。", getType(args[0]));
    }
    if (!IS_NUMBER(args[1])) {
        return nativeError(args,
                           L"参数2（次方）的类型必须是「数字」，而不是「%ls」。", getType(args[1]));
    }
    args[-1] = NUMBER_VAL(pow(AS_NUMBER(args[0]), AS_NUMBER(args[1])));
    return true;
}

bool minNative(int argCount, Value* args) {
    if (!IS_NUMBER(args[0])) {
        return nativeError(args,
                           L"参数1的类型必须是「数字」，而不是「%ls」。", getType(args[0]));
    }
    if (!IS_NUMBER(args[1])) {
        return nativeError(args,
                           L"参数2的类型必须是「数字」，而不是「%ls」。", getType(args[1]));
    }
    double a = AS_NUMBER(args[0]);
    double b = AS_NUMBER(args[1]);
    args[-1] = NUMBER_VAL(a < b ? a : b);
    return true;
}

bool maxNative(int argCount, Value* args) {
    if (!IS_NUMBER(args[0])) {
        return nativeError(args,
                           L"参数1的类型必须是「数字」，而不是「%ls」。", getType(args[0]));
    }
    if (!IS_NUMBER(args[1])) {
        return nativeError(args,
                           L"参数2的类型必须是「数字」，而不是「%ls」。", getType(args[1]));
    }
    double a = AS_NUMBER(args[0]);
    double b = AS_NUMBER(args[1]);
    args[-1] = NUMBER_VAL(a > b ? a : b);
    return true;
}

bool roundNative(int argCount, Value* args) {
    if (argCount < 1 || argCount > 2) {
        return nativeError(args, L"需要 1 到 2 个参数，但得到%d。", argCount);
    }
    if (!IS_NUMBER(args[0])) {
        return nativeError(args,
                           L"参数1（输入）的类型必须是「数字」，而不是「%ls」。", getType(args[0]));
    }
    if (argCount == 2 && !IS_NUMBER(args[1])) {
        return nativeError(args,
                           L"参数2（精度）的类型必须是「数字」，而不是「%ls」。", getType(args[1]));
    }
    double shift =  pow(10.0, AS_NUMBER(args[1]));
    args[-1] = NUMBER_VAL(round(AS_NUMBER(args[0]) * shift) / shift);
    return true;
}

bool stonNative(int argCount, Value* args) {
    if (!IS_STRING(args[0])) {
        return nativeError(args,
                           L"参数1（输入）的类型必须是「字符串」，而不是「%ls」。", getType(args[0]));
    }
    args[-1] = NUMBER_VAL(wcstod(AS_WCSTRING(args[0]), NULL));
    return true;
}

bool ntosNative(int argCount, Value* args) {
    if (!IS_NUMBER(args[0])) {
        return nativeError(args,
                           L"参数1（输入）的类型必须是「数字」，而不是「%ls」。", getType(args[0]));
    }
    wchar_t str[100];
    swprintf(str, sizeof(str), L"%g", AS_NUMBER(args[0]));
    args[-1] = OBJ_VAL(copyString(str, wcslen(str)));
    return true;
}

bool typeofNative(int argCount, Value* args) {
    wchar_t* type = getType(args[0]);
    args[-1] = OBJ_VAL(copyString(type, wcslen(type)));
    return true;
}

//
// Created by Andrew Yang on 9/17/21.
//

#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <float.h>
#include <time.h>
#include <math.h>
#include <stdlib.h>

#include "core_module.h"

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
        switch (OBJ_TYPE(value)) {
            case OBJ_BOUND_METHOD: return AS_BOUND_METHOD(value)->method ? L"绑定方法" : L"静态方法";
            case OBJ_NATIVE: return L"静态方法";
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

bool typeofNative(int argCount, Value* args) {
    wchar_t* type = getType(args[0]);
    args[-1] = OBJ_VAL(copyString(type, wcslen(type)));
    return true;
}

bool sqrtNative(int argCount, Value* args) {
    if (!IS_NUMBER(args[0])) {
        return nativeError(args,
                           L"参数 1（输入）的类型必须是「数字」，而不是「%ls」。", getType(args[0]));
    }
    args[-1] = NUMBER_VAL(sqrt(AS_NUMBER(args[0])));
    return true;
}

bool powNative(int argCount, Value* args) {
    if (!IS_NUMBER(args[0])) {
        return nativeError(args,
                           L"参数 1（基数）的类型必须是「数字」，而不是「%ls」。", getType(args[0]));
    }
    if (!IS_NUMBER(args[1])) {
        return nativeError(args,
                           L"参数 2（次方）的类型必须是「数字」，而不是「%ls」。", getType(args[1]));
    }
    args[-1] = NUMBER_VAL(pow(AS_NUMBER(args[0]), AS_NUMBER(args[1])));
    return true;
}

bool minNative(int argCount, Value* args) {
    if (!IS_NUMBER(args[0])) {
        return nativeError(args,
                           L"参数 1 的类型必须是「数字」，而不是「%ls」。", getType(args[0]));
    }
    if (!IS_NUMBER(args[1])) {
        return nativeError(args,
                           L"参数 2 的类型必须是「数字」，而不是「%ls」。", getType(args[1]));
    }
    double a = AS_NUMBER(args[0]);
    double b = AS_NUMBER(args[1]);
    args[-1] = NUMBER_VAL(a < b ? a : b);
    return true;
}

bool maxNative(int argCount, Value* args) {
    double max = DBL_MIN;
    for (int i = 0; i < argCount; i++) {
        if (!IS_NUMBER(args[i])) {
            return nativeError(args,
                               L"参数 %d 的类型必须是「数字」，而不是「%ls」。", i + 1, getType(args[0]));
        }
        double a = AS_NUMBER(args[i]);
        max = a > max ? a : max;
    }
    args[-1] = NUMBER_VAL(max);
    return true;
}

bool roundNative(int argCount, Value* args) {
    if (argCount < 1 || argCount > 2) {
        return nativeError(args, L"需要 1 到 2 个参数，但得到%d。", argCount);
    }
    if (!IS_NUMBER(args[0])) {
        return nativeError(args,
                           L"参数 1（输入）的类型必须是「数字」，而不是「%ls」。", getType(args[0]));
    }
    if (argCount == 2 && !IS_NUMBER(args[1])) {
        return nativeError(args,
                           L"参数 2（精度）的类型必须是「数字」，而不是「%ls」。", getType(args[1]));
    }
    double shift =  pow(10.0, AS_NUMBER(args[1]));
    args[-1] = NUMBER_VAL(round(AS_NUMBER(args[0]) * shift) / shift);
    return true;
}

bool ntosNative(int argCount, Value* args) {
    if (!IS_NUMBER(args[0])) {
        return nativeError(args,
                           L"参数 1（输入）的类型必须是「数字」，而不是「%ls」。", getType(args[0]));
    }
    wchar_t str[100];
    swprintf(str, sizeof(str), L"%g", AS_NUMBER(args[0]));
    args[-1] = OBJ_VAL(copyString(str, wcslen(str)));
    return true;
}

bool logNative(int argCount, Value* args) {
    if (argCount < 1 || argCount > 2) {
        return nativeError(args, L"需要 1 到 2 个参数，但得到%d。", argCount);
    }
    if (!IS_NUMBER(args[0])) {
        return nativeError(args,
                           L"参数 1（输入）的类型必须是「数字」，而不是「%ls」。", getType(args[0]));
    }
    if (argCount == 2 && !IS_NUMBER(args[1])) {
        return nativeError(args,
                           L"参数 2（精度）的类型必须是「数字」，而不是「%ls」。", getType(args[1]));
    }
    double base = argCount == 1 ? M_E : AS_NUMBER(args[1]);
    args[-1] = NUMBER_VAL(log(args[0]) / log(base));
    return true;
}

bool sinNative(int argCount, Value* args) {
    if (!IS_NUMBER(args[0])) {
        return nativeError(args,
                           L"参数 1（输入）的类型必须是「数字」，而不是「%ls」。", getType(args[0]));
    }
    args[-1] = NUMBER_VAL(sin(args[0]));
    return true;
}

bool cosNative(int argCount, Value* args) {
    if (!IS_NUMBER(args[0])) {
        return nativeError(args,
                           L"参数 1（输入）的类型必须是「数字」，而不是「%ls」。", getType(args[0]));
    }
    args[-1] = NUMBER_VAL(cos(args[0]));
    return true;
}

bool tanNative(int argCount, Value* args) {
    if (!IS_NUMBER(args[0])) {
        return nativeError(args,
                           L"参数 1（输入）的类型必须是「数字」，而不是「%ls」。", getType(args[0]));
    }
    args[-1] = NUMBER_VAL(tan(args[0]));
    return true;
}

bool asinNative(int argCount, Value* args) {
    if (!IS_NUMBER(args[0])) {
        return nativeError(args,
                           L"参数 1（输入）的类型必须是「数字」，而不是「%ls」。", getType(args[0]));
    }
    args[-1] = NUMBER_VAL(asin(args[0]));
    return true;
}

bool acosNative(int argCount, Value* args) {
    if (!IS_NUMBER(args[0])) {
        return nativeError(args,
                           L"参数 1（输入）的类型必须是「数字」，而不是「%ls」。", getType(args[0]));
    }
    args[-1] = NUMBER_VAL(acos(args[0]));
    return true;
}

bool atanNative(int argCount, Value* args) {
    if (!IS_NUMBER(args[0])) {
        return nativeError(args,
                           L"参数 1（输入）的类型必须是「数字」，而不是「%ls」。", getType(args[0]));
    }
    args[-1] = NUMBER_VAL(atan(args[0]));
    return true;
}

bool ceilNative(int argCount, Value* args) {
    if (!IS_NUMBER(args[0])) {
        return nativeError(args,
                           L"参数 1（输入）的类型必须是「数字」，而不是「%ls」。", getType(args[0]));
    }
    args[-1] = NUMBER_VAL(ceil(args[0]));
    return true;
}

bool floorNative(int argCount, Value* args) {
    if (!IS_NUMBER(args[0])) {
        return nativeError(args,
                           L"参数 1（输入）的类型必须是「数字」，而不是「%ls」。", getType(args[0]));
    }
    args[-1] = NUMBER_VAL(floor(args[0]));
    return true;
}

bool randNative(int argCount, Value* args) {
    if (argCount < 0 || argCount > 2) {
        return nativeError(args, L"需要 0 到 2 个参数，但得到%d。", argCount);
    }
    if (argCount == 1 && !IS_NUMBER(args[0])) {
        return nativeError(args,
                           L"参数 1（输入）的类型必须是「数字」，而不是「%ls」。", getType(args[0]));
    }
    if (argCount == 2 && !IS_NUMBER(args[1])) {
        return nativeError(args,
                           L"参数 2（精度）的类型必须是「数字」，而不是「%ls」。", getType(args[1]));
    }
    int min, max;
    if (argCount == 0) {
        min = 0;
        max = 1;
    }
    else if (argCount == 1) {
        min = 0;
        max = (int)round(AS_NUMBER(args[0]));
    }
    else {
        min = (int)round(AS_NUMBER(args[0]));
        max = (int)round(AS_NUMBER(args[1]));
    }

    args[-1] = NUMBER_VAL(rand() % max + 1 - min) + min;
    return true;
}

bool stonNative(int argCount, Value* args) {
    if (!IS_STRING(args[0])) {
        return nativeError(args,
                           L"参数 1（输入）的类型必须是「字符串」，而不是「%ls」。", getType(args[0]));
    }
    args[-1] = NUMBER_VAL(wcstod(AS_WCSTRING(args[0]), NULL));
    return true;
}

void initCoreClass() {
    // System Core Class
    ObjClass* systemClass = newClass(copyString(L"系统", 2));
    defineNative(L"打印", printNative, 1, systemClass);
    defineNative(L"打印行", printlnNative, 1, systemClass);
    defineNative(L"扫描", scanNative, 0, systemClass);
    defineNative(L"时钟", clockNative, 0, systemClass);
    defineNative(L"型", typeofNative, 1, systemClass);
    defineNative(L"最大", maxNative, -1, systemClass);
    ObjInstance* systemInstance = newInstance(systemClass, true);
    defineNativeInstance(L"系统", systemInstance);

    // Number Core Class
    ObjClass* numberClass = newClass(copyString(L"数字", 2));
    defineNative(L"平方根", sqrtNative, 1, numberClass);
    defineNative(L"次方", powNative, 2, numberClass);
    defineNative(L"最小", minNative, 2, numberClass);
    defineNative(L"最大", maxNative, -1, numberClass);
    defineNative(L"四舍五入", roundNative, -1, numberClass);
    defineNative(L"数到串", ntosNative, 1, numberClass);
    defineNative(L"对数", logNative, -1, numberClass);
    defineNative(L"对数", logNative, -1, numberClass);
    defineNative(L"正弦", sinNative, 1, numberClass);
    defineNative(L"余弦", cosNative, 1, numberClass);
    defineNative(L"正切", tanNative, 1, numberClass);
    defineNative(L"反正弦", asinNative, 1, numberClass);
    defineNative(L"反余弦", acosNative, 1, numberClass);
    defineNative(L"反正切", atanNative, 1, numberClass);
    defineNative(L"上限", ceilNative, 1, numberClass);
    defineNative(L"下限", floorNative, 1, numberClass);
    defineNative(L"随机", randNative, -1, numberClass);
    ObjInstance* numberInstance = newInstance(numberClass, true);
    defineProperty(L"圆周率", NUMBER_VAL(M_PI), numberInstance);
    defineProperty(L"欧拉数", NUMBER_VAL(M_E), numberInstance);
    defineProperty(L"无穷大", NUMBER_VAL(INFINITY), numberInstance);
    defineProperty(L"不数字", NUMBER_VAL(NAN), numberInstance);
    defineProperty(L"最大值", NUMBER_VAL(DBL_MAX), numberInstance);
    defineProperty(L"最小值", NUMBER_VAL(DBL_MIN), numberInstance);
    defineProperty(L"最大安全", NUMBER_VAL(9007199254740991), numberInstance);
    defineProperty(L"最小安全", NUMBER_VAL(-9007199254740991), numberInstance);
    defineNativeInstance(L"数字", numberInstance);

    // String Core Class
    ObjClass* stringClass = newClass(copyString(L"字符串", 3));
    defineNative(L"串到数", stonNative, 1, stringClass);
    ObjInstance* stringInstance = newInstance(stringClass, true);
    defineNativeInstance(L"字符串", stringInstance);
}

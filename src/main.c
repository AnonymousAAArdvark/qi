#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <locale.h>

#include "common.h"
#include "vm.h"

static void repl() {
    char line[1024];
    for (;;) {
        wprintf(L"》");

        if (!fgets(line, sizeof(line), stdin)) {
            wprintf(L"\n");
            break;
        }

        interpret(line);
    }
}

static char* readFile(const char* path) {
    FILE* file = fopen(path, "rb");

    if (file == NULL) {
        fwprintf(stderr, L"无法打开文件「%s」。\n", path);
        exit(74);
    }
    fseek(file, 0L, SEEK_END);
    size_t fileSize = ftell(file);
    rewind(file);

    char* buffer = (char*)malloc(fileSize + 1);
    if (buffer == NULL) {
        fwprintf(stderr, L"没有足够的内存来读取「%s」。\n", path);
        exit(74);
    }

    size_t bytesRead = fread(buffer, sizeof(char), fileSize, file);
    if (bytesRead < fileSize) {
        fwprintf(stderr, L"无法读取文件「%s」。\n", path);
        exit(74);
    }

    buffer[bytesRead] = L'\0';

    fclose(file);
    return buffer;
}

static void runFile(const char* path) {
    char* source = readFile(path);
    InterpretResult result = interpret(source);
    free(source);

    if (result == INTERPRET_COMPILE_ERROR) exit(65);
    if (result == INTERPRET_RUNTIME_ERROR) exit(70);
}

int main(int argc, const char* argv[]) {
    setlocale(LC_ALL, "");

    initVM();

    if (argc == 1) {
        repl();
    } else if (argc == 2) {
        runFile(argv[1]);
    } else {
        fwprintf(stderr, L"用法：qi【文件路径】\n");
        exit(64);
    }

    freeVM();
    return 0;
}

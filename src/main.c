#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <locale.h>

#include "common.h"
#include "chunk.h"
#include "debug.h"
#include "vm.h"

static void repl() {
    char line[1024];
    for (;;) {
        wprintf(L"> ");

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
        fwprintf(stderr, L"Could not open file \"%ls\".\n", path);
        exit(74);
    }
    fseek(file, 0L, SEEK_END);
    size_t fileSize = ftell(file);
    rewind(file);

    char* buffer = (char*)malloc(fileSize + 1);
    if (buffer == NULL) {
        fwprintf(stderr, L"Not enough memory to read \"%ls\".\n", path);
        exit(74);
    }

    size_t bytesRead = fread(buffer, sizeof(char), fileSize, file);
    if (bytesRead < fileSize) {
        fwprintf(stderr, L"Could not read file \"%ls\".\n", path);
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
//    int i;
//    wchar_t chinese[] = L"我不是中国人。";
//    for(i = 0; chinese[i]; ++i)
//    {
//        if(chinese[i] == L'不')
//            wprintf(L"%lc", chinese[i]);
//        if(chinese[i] == L'\u4E0D')
//            wprintf(L"also found\n");
//    }
//
    initVM();

    if (argc == 1) {
        repl();
    } else if (argc == 2) {
        runFile(argv[1]);
    } else {
        fwprintf(stderr, L"Usage: qi [path]\n");
        exit(64);
    }

    freeVM();
    return 0;
}

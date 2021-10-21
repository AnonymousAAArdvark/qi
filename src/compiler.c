//
// Created by Andrew Yang on 8/30/21.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "common.h"
#include "compiler.h"
#include "memory.h"
#include "scanner.h"

#ifdef DEBUG_PRINT_CODE
#include "debug.h"
#endif

typedef struct {
    Token current;
    Token previous;
    bool hadError;
    bool panicMode;
} Parser;

typedef enum {
    PREC_NONE,
    PREC_ASSIGNMENT,  // =
    PREC_OR,          // or
    PREC_AND,         // and
    PREC_EQUALITY,    // == !=
    PREC_COMPARISON,  // < > <= >=
    PREC_TERM,        // + -
    PREC_FACTOR,      // * /
    PREC_UNARY,       // ! -
    PREC_CALL,        // . ()
    PREC_SUBSCRIPT,   // [
} Precedence;

typedef void (*ParseFn)(bool canAssign);

typedef struct {
    ParseFn prefix;
    ParseFn infix;
    Precedence precedence;
} ParseRule;

typedef struct {
    Token name;
    int depth;
    bool isCaptured;
} Local;

typedef struct {
    uint8_t index;
    bool isLocal;
} Upvalue;

typedef enum {
    TYPE_FUNCTION,
    TYPE_INITIALIZER,
    TYPE_METHOD,
    TYPE_SCRIPT
} FunctionType;

typedef struct Compiler {
    struct Compiler* enclosing;
    ObjFunction* function;
    FunctionType type;

    Local locals[UINT8_COUNT];
    int localCount;
    Upvalue upvalues[UINT8_COUNT];
    int scopeDepth;
} Compiler;

typedef struct ClassCompiler {
    struct ClassCompiler* enclosing;
    bool hasSuperclass;
} ClassCompiler;

Parser parser;
Compiler* current = NULL;
ClassCompiler* currentClass = NULL;

int innermostLoopStart = -1;
int innermostLoopScopeDepth = 0;

static Chunk* currentChunk() {
    return &current->function->chunk;
}

static void errorAt(Token* token, const wchar_t* message) {
    if (parser.panicMode) return;
    parser.panicMode = true;
    fwprintf(stderr, L"[line %d] Error", token->line);

    if (token->type == TOKEN_EOF) {
        fwprintf(stderr, L" at end");
    } else if (token->type == TOKEN_ERROR) {
        // Nothing.
    } else {
        fwprintf(stderr, L" at '%.*ls'", token->length, token->start);
    }

    fwprintf(stderr, L": %ls\n", message);
    parser.hadError = true;
}

static void error(const wchar_t* message) {
    errorAt(&parser.previous, message);
}

static void errorAtCurrent(const wchar_t* message) {
    errorAt(&parser.current, message);
}

static void advance() {
    parser.previous = parser.current;

    for (;;) {
        parser.current = scanToken();
        if (parser.current.type != TOKEN_ERROR) break;

        errorAtCurrent(parser.current.start);
    }
}

static void consume(TokenType type, const wchar_t* message) {
    if (parser.current.type == type) {
        advance();
        return;
    }

    errorAtCurrent(message);
}

static bool check(TokenType type) {
    return parser.current.type == type;
}

static bool match(TokenType type) {
    if (!check(type)) return false;
    advance();
    return true;
}

static void emitByte(uint8_t byte) {
    writeChunk(currentChunk(), byte, parser.previous.line);
}

static void emitBytes(uint8_t byte1, uint8_t byte2) {
    emitByte(byte1);
    emitByte(byte2);
}

static void emitLoop(int loopStart) {
    emitByte(OP_LOOP);

    int offset = currentChunk()->count - loopStart + 2;
    if (offset > UINT16_MAX) error(L"Loop body too large.");

    emitByte((offset >> 8) & 0xff);
    emitByte(offset & 0xff);
}

static int emitJump(uint8_t instruction) {
    emitByte(instruction);
    emitByte(0xff);
    emitByte(0xff);
    return currentChunk()->count - 2;
}

static void emitReturn() {
    if (current->type == TYPE_INITIALIZER) {
        emitBytes(OP_GET_LOCAL, 0);
    } else {
        emitByte(OP_NIL);
    }

    emitByte(OP_RETURN);
}

static uint8_t makeConstant(Value value) {
    int constant = addConstant(currentChunk(), value);
    if (constant > UINT8_MAX) {
        error(L"Too many constants in one chunk.");
        return 0;
    }

    return (uint8_t)constant;
}

static void emitConstant(Value value) {
    emitBytes(OP_CONSTANT, makeConstant(value));
}

static void patchJump(int offset) {
    // -2 to adjust for the bytecode for the jump offset itself.
    int jump = currentChunk()->count - offset - 2;

    if (jump > UINT16_MAX) {
        error(L"Too much code to jump over.");
    }

    currentChunk()->code[offset] = (jump >> 8) & 0xff;
    currentChunk()->code[offset + 1] = jump & 0xff;
}

static void initCompiler(Compiler* compiler, FunctionType type) {
    compiler->enclosing = current;
    compiler->function = NULL;
    compiler->type = type;
    compiler->localCount = 0;
    compiler->scopeDepth = 0;
    compiler->function = newFunction();
    current = compiler;
    if (type != TYPE_SCRIPT) {
        current->function->name = copyString(parser.previous.start,
                                             parser.previous.length);
    }

    Local* local = &current->locals[current->localCount++];
    local->depth = 0;
    local->isCaptured = false;
    if (type != TYPE_FUNCTION) {
        local->name.start = L"这";
        local->name.length = 1;
    } else {
        local->name.start = L"";
        local->name.length = 0;
    }
}

static ObjFunction* endCompiler() {
    emitReturn();
    ObjFunction* function = current->function;

#ifdef DEBUG_PRINT_CODE
    if (!parser.hadError) {
        disassembleChunk(currentChunk(), function->name != NULL
            ? function->name->chars : L"<script>");
    }
#endif

    current = current->enclosing;
    return function;
}

static void beginScope() {
    current->scopeDepth++;
}

static void endScope() {
    current->scopeDepth--;

    while (current->localCount > 0 &&
           current->locals[current->localCount - 1].depth >
           current->scopeDepth) {
        if (current->locals[current->localCount - 1].isCaptured) {
            emitByte(OP_CLOSE_UPVALUE);
        } else {
            emitByte(OP_POP);
        }
        current->localCount--;
    }
}

static void expression();
static void statement();
static void declaration();
static ParseRule* getRule(TokenType type);
static void parsePrecedence(Precedence precedence);

static uint8_t identifierConstant(Token* name) {
    return makeConstant(OBJ_VAL(copyString(name->start, name->length)));
}

static bool identifiersEqual(Token* a, Token* b) {
    if (a->length != b->length) return false;
    return memcmp(a->start, b->start, a->length * sizeof(wchar_t)) == 0;
}

static int resolveLocal(Compiler* compiler, Token* name) {
    for (int i = compiler->localCount - 1; i >= 0; i--) {
        Local* local = &compiler->locals[i];
        if (identifiersEqual(name, &local->name)) {
            if (local->depth == -1) {
                error(L"Can't read local 变量 in its own initializer.");
            }
            return i;
        }
    }

    return -1;
}

static int addUpvalue(Compiler* compiler, uint8_t index, bool isLocal) {
    int upvalueCount = compiler->function->upvalueCount;

    for (int i = 0; i < upvalueCount; i++) {
        Upvalue* upvalue = &compiler->upvalues[i];
        if (upvalue->index == index && upvalue->isLocal == isLocal) {
            return i;
        }
    }

    if (upvalueCount == UINT8_COUNT) {
        error(L"Too many closure 变量 in function.");
        return 0;
    }

    compiler->upvalues[upvalueCount].isLocal = isLocal;
    compiler->upvalues[upvalueCount].index = index;
    return compiler->function->upvalueCount++;
}

static int resolveUpvalue(Compiler* compiler, Token* name) {
    if (compiler->enclosing == NULL) return -1;

    int local = resolveLocal(compiler->enclosing, name);
    if (local != -1) {
        compiler->enclosing->locals[local].isCaptured = true;
        return addUpvalue(compiler, (uint8_t)local, true);
    }

    int upvalue = resolveUpvalue(compiler->enclosing, name);
    if (upvalue != -1) {
        return addUpvalue(compiler, (uint8_t)upvalue, false);
    }

    return -1;
}

static void addLocal(Token name) {
    if (current->localCount == UINT8_COUNT) {
        error(L"Too many local 变量 in function.");
        return;
    }

    Local* local = &current->locals[current->localCount++];
    local->name = name;
    local->depth = -1;
    local->isCaptured = false;
}

static void declareVariable() {
    if (current->scopeDepth == 0) return;

    Token* name = &parser.previous;
    for (int i = current->localCount - 1; i >= 0; i--) {
        Local* local = &current->locals[i];
        if (local->depth != -1 && local->depth < current->scopeDepth) {
            break;
        }

        if (identifiersEqual(name, &local->name)) {
            error(L"Already a 变量 with this name in this scope.");
        }
    }

    addLocal(*name);
}

static uint8_t parseVariable(const wchar_t* errorMessage) {
    consume(TOKEN_IDENTIFIER, errorMessage);

    declareVariable();
    if (current->scopeDepth > 0) return 0;

    return identifierConstant(&parser.previous);
}

static void markInitialized() {
    if (current->scopeDepth == 0) return;
    current->locals[current->localCount - 1].depth = current->scopeDepth;
}

static void defineVariable(uint8_t global) {
    if (current->scopeDepth > 0) {
        markInitialized();
        return;
    }

    emitBytes(OP_DEFINE_GLOBAL, global);
}

static uint8_t argumentList() {
    uint8_t argCount = 0;
    if (!check(TOKEN_RIGHT_PAREN)) {
        do {
            expression();
            if (argCount == 255) {
                error(L"Can't have more than 255 arguments.");
            }
            argCount++;
        } while (match(TOKEN_COMMA));
    }
    consume(TOKEN_RIGHT_PAREN, L"Expect ')' after arguments.");
    return argCount;
}

static void and_(bool canAssign) {
    int endJump = emitJump(OP_JUMP_IF_FALSE);

    emitByte(OP_POP);
    parsePrecedence(PREC_AND);

    patchJump(endJump);
}

static void binary(bool canAssign) {
    TokenType operatorType = parser.previous.type;
    ParseRule* rule = getRule(operatorType);
    parsePrecedence((Precedence)(rule->precedence + 1));

    switch (operatorType) {
        case TOKEN_BANG_EQUAL:    emitBytes(OP_EQUAL, OP_NOT); break;
        case TOKEN_EQUAL_EQUAL:   emitByte(OP_EQUAL); break;
        case TOKEN_GREATER:       emitByte(OP_GREATER); break;
        case TOKEN_GREATER_EQUAL: emitBytes(OP_LESS, OP_NOT); break;
        case TOKEN_LESS:          emitByte(OP_LESS); break;
        case TOKEN_LESS_EQUAL:    emitBytes(OP_GREATER, OP_NOT); break;
        case TOKEN_PLUS:          emitByte(OP_ADD); break;
        case TOKEN_MINUS:         emitByte(OP_SUBTRACT); break;
        case TOKEN_STAR:          emitByte(OP_MULTIPLY); break;
        case TOKEN_SLASH:         emitByte(OP_DIVIDE); break;
        case TOKEN_PERCENT:       emitByte(OP_MODULO); break;
        default: return; // Unreachable.
    }
}

static void postfix(bool canAssign) {
    TokenType operatorType = parser.previous.type;

    switch(parser.previous.type) {
        case TOKEN_PLUS_PLUS:
        case TOKEN_MINUS_MINUS: {
            uint8_t op1 = -1, op2 = -1;
            if (currentChunk()->count - 2 >= 0) {
                op1 = currentChunk()->code[currentChunk()->count - 2];
                op2 = currentChunk()->code[currentChunk()->count - 1];
            }
            if (op1 == OP_GET_PROPERTY) {
                emitByte(op2);
                currentChunk()->code[currentChunk()->count - 2] = OP_GET_PROPERTY;
                currentChunk()->code[currentChunk()->count - 3] = OP_DUP;
                emitByte(operatorType == TOKEN_PLUS_PLUS ? OP_INCREMENT : OP_DECREMENT);
                emitBytes(OP_SET_PROPERTY, op2);
                emitByte(operatorType == TOKEN_PLUS_PLUS ? OP_DECREMENT : OP_INCREMENT);
                break;
            } else if (op1 == OP_GET_GLOBAL || op1 == OP_GET_LOCAL || op1 == OP_GET_UPVALUE) {
                emitByte(operatorType == TOKEN_PLUS_PLUS ? OP_INCREMENT : OP_DECREMENT);
                emitBytes(op1 == OP_GET_GLOBAL ? OP_SET_GLOBAL
                                               : op1 == OP_GET_LOCAL ? OP_SET_LOCAL : OP_SET_UPVALUE, op2);
                emitByte(operatorType == TOKEN_PLUS_PLUS ? OP_DECREMENT : OP_INCREMENT);
                break;
            } else if (op2 == OP_INDEX_SUBSCR) {
                emitByte(op2);
                currentChunk()->code[currentChunk()->count - 2] = OP_DOUBLE_DUP;
                emitByte(operatorType == TOKEN_PLUS_PLUS ? OP_INCREMENT : OP_DECREMENT);
                emitByte(OP_STORE_SUBSCR);
                emitByte(operatorType == TOKEN_PLUS_PLUS ? OP_DECREMENT : OP_INCREMENT);
                break;
            }
            error(L"Expression is not assignable.");
            break;
        }
        default: return; // Unreachable.
    }
}

static void call(bool canAssign) {
    uint8_t argCount = argumentList();
    emitBytes(OP_CALL, argCount);
}

static void dot(bool canAssign) {
    consume(TOKEN_IDENTIFIER, L"Expect property name after '.'.");
    uint8_t name = identifierConstant(&parser.previous);
    if (canAssign && match(TOKEN_EQUAL)) {
        expression();
        emitBytes(OP_SET_PROPERTY, name);
    } else if (canAssign && (match(TOKEN_PLUS_EQUAL) || match(TOKEN_MINUS_EQUAL))) {
        TokenType type = parser.previous.type;
        emitByte(OP_DUP);
        emitBytes(OP_GET_PROPERTY, name);
        expression();
        emitByte(type == TOKEN_PLUS_EQUAL ? OP_ADD : OP_SUBTRACT);
        emitBytes(OP_SET_PROPERTY, name);
    } else if (match(TOKEN_LEFT_PAREN)) {
        uint8_t argCount = argumentList();
        emitBytes(OP_INVOKE, name);
        emitByte(argCount);
    } else {
        emitBytes(OP_GET_PROPERTY, name);
    }
}

static void literal(bool canAssign) {
    switch (parser.previous.type) {
        case TOKEN_FALSE: emitByte(OP_FALSE); break;
        case TOKEN_NIL: emitByte(OP_NIL); break;
        case TOKEN_TRUE: emitByte(OP_TRUE); break;
        default: return; // Unreachable.
    }
}

static void grouping(bool canAssign) {
    expression();
    consume(TOKEN_RIGHT_PAREN, L"Expect ')' after expression.");
}

static void number(bool canAssign) {
    double value = wcstod(parser.previous.start, NULL);
    emitConstant(NUMBER_VAL(value));
}

static void or_(bool canAssign) {
    int elseJump = emitJump(OP_JUMP_IF_FALSE);
    int endJump = emitJump(OP_JUMP);

    patchJump(elseJump);
    emitByte(OP_POP);

    parsePrecedence(PREC_OR);
    patchJump(endJump);
}

static void string(bool canAssign) {
    emitConstant(OBJ_VAL(copyString(parser.previous.start + 1,
                                    parser.previous.length - 2)));
}

static void list(bool canAssign) {
    int itemCount = 0;
    if (!check(TOKEN_RIGHT_BRACKET)) {
        do {
            if (check(TOKEN_RIGHT_BRACKET)) {
                // Trailing comma case
                break;
            }

            parsePrecedence(PREC_OR);

            if (itemCount == UINT8_COUNT) {
                error(L"Cannot have more than 256 items in a list literal.");
            }
            itemCount++;
        } while (match(TOKEN_COMMA));
    }

    consume(TOKEN_RIGHT_BRACKET, L"Expect ']' after list literal.");

    emitByte(OP_BUILD_LIST);
    emitByte(itemCount);
}

static void subscript(bool canAssign) {
    parsePrecedence(PREC_OR);
    consume(TOKEN_RIGHT_BRACKET, L"Expect ']' after index.");

    if (canAssign && match(TOKEN_EQUAL)) {
        expression();
        emitByte(OP_STORE_SUBSCR);
    } else if (canAssign && (match(TOKEN_PLUS_EQUAL) || match(TOKEN_MINUS_EQUAL))) {
        TokenType type = parser.previous.type;
        emitByte(OP_DOUBLE_DUP);
        emitByte(OP_INDEX_SUBSCR);
        expression();
        emitByte(type == TOKEN_PLUS_EQUAL ? OP_ADD : OP_SUBTRACT);
        emitByte(OP_STORE_SUBSCR);
    } else {
        emitByte(OP_INDEX_SUBSCR);
    }
}

static void namedVariable(Token name, bool canAssign) {
    uint8_t getOp, setOp;
    int arg = resolveLocal(current, &name);
    if (arg != -1) {
        getOp = OP_GET_LOCAL;
        setOp = OP_SET_LOCAL;
    } else if ((arg = resolveUpvalue(current, &name)) != -1) {
        getOp = OP_GET_UPVALUE;
        setOp = OP_SET_UPVALUE;
    } else {
        arg = identifierConstant(&name);
        getOp = OP_GET_GLOBAL;
        setOp = OP_SET_GLOBAL;
    }

    if (canAssign && match(TOKEN_EQUAL)) {
        expression();
        emitBytes(setOp, (uint8_t) arg);
    } else if (canAssign && (match(TOKEN_PLUS_EQUAL) || match(TOKEN_MINUS_EQUAL))) {
        TokenType type = parser.previous.type;
        emitBytes(getOp, (uint8_t) arg);
        expression();
        emitByte(type == TOKEN_PLUS_EQUAL ? OP_ADD : OP_SUBTRACT);
        emitBytes(setOp, (uint8_t) arg);
    } else {
        emitBytes(getOp, (uint8_t)arg);
    }
}

static void variable(bool canAssign) {
    namedVariable(parser.previous, canAssign);
}

static Token syntheticToken(const wchar_t* text) {
    Token token;
    token.start = text;
    token.length = (int)wcslen(text);
    return token;
}

static void super_(bool canAssign) {
    if (currentClass == NULL) {
        error(L"Can't use '超' outside of a 类.");
    } else if (!currentClass->hasSuperclass) {
        error(L"Can't use '超' in a 类 with no 超类.");
    }

    consume(TOKEN_DOT, L"Expect '.' after '超'.");
    consume(TOKEN_IDENTIFIER, L"Expect 超类 method name.");
    uint8_t name = identifierConstant(&parser.previous);

    namedVariable(syntheticToken(L"这"), false);
    if (match(TOKEN_LEFT_PAREN)) {
        uint8_t argCount = argumentList();
        namedVariable(syntheticToken(L"超"), false);
        emitBytes(OP_SUPER_INVOKE, name);
        emitByte(argCount);
    } else {
        namedVariable(syntheticToken(L"超"), false);
        emitBytes(OP_GET_SUPER, name);
    }
}

static void this_(bool canAssign) {
    if (currentClass == NULL) {
        error(L"Can't use '这' outside of a 类.");
        return;
    }

    variable(false);
}

static void unary(bool canAssign) {
    TokenType operatorType = parser.previous.type;

    // Compile the operand.
    parsePrecedence(PREC_UNARY);

    // Emit the operator instruction.
    switch (operatorType) {
        case TOKEN_BANG: emitByte(OP_NOT); break;
        case TOKEN_MINUS: emitByte(OP_NEGATE); break;
        case TOKEN_PLUS_PLUS:
        case TOKEN_MINUS_MINUS: {
            uint8_t op1 = -1, op2 = -1;
            if (currentChunk()->count - 2 >= 0) {
                op1 = currentChunk()->code[currentChunk()->count - 2];
                op2 = currentChunk()->code[currentChunk()->count - 1];
            }
            if (op1 == OP_GET_PROPERTY) {
                emitByte(op2);
                currentChunk()->code[currentChunk()->count - 2] = OP_GET_PROPERTY;
                currentChunk()->code[currentChunk()->count - 3] = OP_DUP;
                emitByte(operatorType == TOKEN_PLUS_PLUS ? OP_INCREMENT : OP_DECREMENT);
                emitBytes(OP_SET_PROPERTY, op2);
                break;
            } else if (op1 == OP_GET_GLOBAL || op1 == OP_GET_LOCAL || op1 == OP_GET_UPVALUE) {
                emitByte(operatorType == TOKEN_PLUS_PLUS ? OP_INCREMENT : OP_DECREMENT);
                emitBytes(op1 == OP_GET_GLOBAL ? OP_SET_GLOBAL
                                               : op1 == OP_GET_LOCAL ? OP_SET_LOCAL : OP_SET_UPVALUE, op2);
                break;
            } else if (op2 == OP_INDEX_SUBSCR) {
                emitByte(op2);
                currentChunk()->code[currentChunk()->count - 2] = OP_DOUBLE_DUP;
                emitByte(operatorType == TOKEN_PLUS_PLUS ? OP_INCREMENT : OP_DECREMENT);
                emitByte(OP_STORE_SUBSCR);
                break;
            }
            error(L"Expression is not assignable.");
            break;
        }
        default: return; // Unreachable.
    }
}

ParseRule rules[] = {
        [TOKEN_LEFT_PAREN]    = {grouping, call,   PREC_CALL},
        [TOKEN_RIGHT_PAREN]   = {NULL,     NULL,   PREC_NONE},
        [TOKEN_LEFT_BRACE]    = {NULL,     NULL,   PREC_NONE},
        [TOKEN_RIGHT_BRACE]   = {NULL,     NULL,   PREC_NONE},
        [TOKEN_COMMA]         = {NULL,     NULL,   PREC_NONE},
        [TOKEN_DOT]           = {NULL,     dot,   PREC_CALL},
        [TOKEN_MINUS]         = {unary,    binary, PREC_TERM},
        [TOKEN_PLUS]          = {NULL,     binary, PREC_TERM},
        [TOKEN_MINUS_MINUS]   = {unary,    postfix,   PREC_CALL},
        [TOKEN_PLUS_PLUS]   = {unary,    postfix,   PREC_CALL},
        [TOKEN_SEMICOLON]     = {NULL,     NULL,   PREC_NONE},
        [TOKEN_SLASH]         = {NULL,     binary, PREC_FACTOR},
        [TOKEN_STAR]          = {NULL,     binary, PREC_FACTOR},
        [TOKEN_PERCENT]        = {NULL,     binary, PREC_FACTOR},
        [TOKEN_BANG]          = {unary,     NULL,   PREC_NONE},
        [TOKEN_BANG_EQUAL]    = {NULL,     binary,   PREC_EQUALITY},
        [TOKEN_EQUAL]         = {NULL,     NULL,   PREC_NONE},
        [TOKEN_PLUS_EQUAL]    = {NULL,     NULL,   PREC_NONE},
        [TOKEN_MINUS_EQUAL]   = {NULL,     NULL,   PREC_NONE},
        [TOKEN_EQUAL_EQUAL]   = {NULL,     binary, PREC_EQUALITY},
        [TOKEN_GREATER]       = {NULL,     binary, PREC_COMPARISON},
        [TOKEN_GREATER_EQUAL] = {NULL,     binary, PREC_COMPARISON},
        [TOKEN_LESS]          = {NULL,     binary, PREC_COMPARISON},
        [TOKEN_LESS_EQUAL]    = {NULL,     binary, PREC_COMPARISON},
        [TOKEN_IDENTIFIER]    = {variable,     NULL,   PREC_NONE},
        [TOKEN_STRING]        = {string,     NULL,   PREC_NONE},
        [TOKEN_LEFT_BRACKET]  = {list,     subscript, PREC_SUBSCRIPT},
        [TOKEN_RIGHT_BRACKET] = {NULL,     NULL,   PREC_NONE},
        [TOKEN_NUMBER]        = {number,   NULL,   PREC_NONE},
        [TOKEN_AND]           = {NULL,     and_,   PREC_AND},
        [TOKEN_CLASS]         = {NULL,     NULL,   PREC_NONE},
        [TOKEN_ELSE]          = {NULL,     NULL,   PREC_NONE},
        [TOKEN_FALSE]         = {literal,     NULL,   PREC_NONE},
        [TOKEN_FOR]           = {NULL,     NULL,   PREC_NONE},
        [TOKEN_FUN]           = {NULL,     NULL,   PREC_NONE},
        [TOKEN_IF]            = {NULL,     NULL,   PREC_NONE},
        [TOKEN_NIL]           = {literal,     NULL,   PREC_NONE},
        [TOKEN_OR]            = {NULL,     or_,   PREC_OR},
        [TOKEN_RETURN]        = {NULL,     NULL,   PREC_NONE},
        [TOKEN_SUPER]         = {super_,     NULL,   PREC_NONE},
        [TOKEN_THIS]          = {this_,     NULL,   PREC_NONE},
        [TOKEN_TRUE]          = {literal,     NULL,   PREC_NONE},
        [TOKEN_VAR]           = {NULL,     NULL,   PREC_NONE},
        [TOKEN_WHILE]         = {NULL,     NULL,   PREC_NONE},
        [TOKEN_ERROR]         = {NULL,     NULL,   PREC_NONE},
        [TOKEN_EOF]           = {NULL,     NULL,   PREC_NONE},
};

static void parsePrecedence(Precedence precedence) {
    advance();
    ParseFn prefixRule = getRule(parser.previous.type)->prefix;
    if (prefixRule == NULL) {
        error(L"Expect expression.");
        return;
    }

    bool canAssign = precedence <= PREC_ASSIGNMENT;
    prefixRule(canAssign);

    while (precedence <= getRule(parser.current.type)->precedence) {
        advance();
        ParseFn infixRule = getRule(parser.previous.type)->infix;
        infixRule(canAssign);
    }

    if (canAssign && (match(TOKEN_EQUAL) || match(TOKEN_PLUS_EQUAL) || match(TOKEN_PLUS_EQUAL))) {
        error(L"Invalid assignment target.");
    }
}

static ParseRule* getRule(TokenType type) {
    return &rules[type];
}

static int getByteCountForArguments(int ip) {
    OpCode instruction = (OpCode)current->function->chunk.code[ip];
    switch (instruction) {
        case OP_NIL:
        case OP_TRUE:
        case OP_FALSE:
        case OP_POP:
        case OP_EQUAL:
        case OP_GREATER:
        case OP_LESS:
        case OP_ADD:
        case OP_SUBTRACT:
        case OP_MULTIPLY:
        case OP_DIVIDE:
        case OP_MODULO:
        case OP_NOT:
        case OP_NEGATE:
        case OP_CLOSE_UPVALUE:
        case OP_RETURN:
        case OP_INHERIT:
        case OP_DUP:
        case OP_END:
            return 0;

        case OP_CONSTANT:
        case OP_GET_GLOBAL:
        case OP_DEFINE_GLOBAL:
        case OP_SET_GLOBAL:
        case OP_GET_PROPERTY:
        case OP_SET_PROPERTY:
        case OP_GET_SUPER:
        case OP_CLASS:
        case OP_METHOD:
        case OP_GET_LOCAL:
        case OP_SET_LOCAL:
        case OP_GET_UPVALUE:
        case OP_SET_UPVALUE:
        case OP_CALL:
            return 1;

        case OP_INVOKE:
        case OP_SUPER_INVOKE:
        case OP_JUMP_IF_FALSE:
        case OP_JUMP:
        case OP_LOOP:
            return 2;

        case OP_CLOSURE:
            return 2 + (current->function->upvalueCount);

        default:
            // Unreachable.
            return 0;
    }
}

static void expression() {
    parsePrecedence(PREC_ASSIGNMENT);
}

static void block() {
    while (!check(TOKEN_RIGHT_BRACE) && !check(TOKEN_EOF)) {
        declaration();
    }

    consume(TOKEN_RIGHT_BRACE, L"Expect '}' after block.");
}

static void function(FunctionType type) {
    Compiler compiler;
    initCompiler(&compiler, type);
    beginScope();

    consume(TOKEN_LEFT_PAREN, L"Expect '(' after function name.");
    if (!check(TOKEN_RIGHT_PAREN)) {
        do {
            current->function->arity++;
            if (current->function->arity > 255) {
                errorAtCurrent(L"Can't have more than 255 parameters.");
            }
            uint8_t constant = parseVariable(L"Expect parameter name.");
            defineVariable(constant);
        } while (match(TOKEN_COMMA));
    }
    consume(TOKEN_RIGHT_PAREN, L"Expect ')' after parameters.");
    consume(TOKEN_LEFT_BRACE, L"Expect '{' before function body.");
    block();

    ObjFunction* function = endCompiler();
    emitBytes(OP_CLOSURE, makeConstant(OBJ_VAL(function)));

    for (int i = 0; i < function->upvalueCount; i++) {
        emitByte(compiler.upvalues[i].isLocal ? 1 : 0);
        emitByte(compiler.upvalues[i].index);
    }
}

static void method() {
    consume(TOKEN_IDENTIFIER, L"Expect method name.");
    uint8_t constant = identifierConstant(&parser.previous);

    FunctionType type = TYPE_METHOD;
    if (parser.previous.length == 4 && memcmp(parser.previous.start, L"init", 4 * sizeof(wchar_t)) == 0) {
        type = TYPE_INITIALIZER;
    }

    function(type);
    emitBytes(OP_METHOD, constant);
}

static void classDeclaration() {
    consume(TOKEN_IDENTIFIER, L"Expect 类 name.");
    Token className = parser.previous;
    uint8_t  nameConstant = identifierConstant(&parser.previous);
    declareVariable();

    emitBytes(OP_CLASS, nameConstant);
    defineVariable(nameConstant);

    ClassCompiler classCompiler;
    classCompiler.hasSuperclass = false;
    classCompiler.enclosing = currentClass;
    currentClass = &classCompiler;

    if (match(TOKEN_COLON)) {
        consume(TOKEN_IDENTIFIER, L"Expect 超类 name.");
        variable(false);

        if (identifiersEqual(&className, &parser.previous)) {
            error(L"A 类 can't inherit from itself.");
        }

        beginScope();
        addLocal(syntheticToken(L"超"));
        defineVariable(0);

        namedVariable(className, false);
        emitByte(OP_INHERIT);
        classCompiler.hasSuperclass = true;
    }

    namedVariable(className, false);
    consume(TOKEN_LEFT_BRACE, L"Expect '{' before 类 body.");
    while (!check(TOKEN_RIGHT_BRACE) && !check(TOKEN_EOF)) {
        method();
    }
    consume(TOKEN_RIGHT_BRACE, L"Expect '}' after 类 body.");
    emitByte(OP_POP);

    if (classCompiler.hasSuperclass) {
        endScope();
    }

    currentClass = currentClass->enclosing;
}

static void funDeclaration() {
    uint8_t global = parseVariable(L"Expect function name.");
    markInitialized();
    function(TYPE_FUNCTION);
    defineVariable(global);
}

static void varDeclaration() {
    uint8_t global = parseVariable(L"Expect 变量 name.");

    if (match(TOKEN_EQUAL)) {
        expression();
    } else {
        emitByte(OP_NIL);
    }
    consume(TOKEN_SEMICOLON, L"Expect '；' after 变量 declaration.");

    defineVariable(global);
}

static void expressionStatement() {
    expression();
    consume(TOKEN_SEMICOLON, L"Expect '；' after expression.");
    emitByte(OP_POP);
}

static void forStatement() {
    beginScope();

    consume(TOKEN_LEFT_PAREN, L"Expect '(' after 'for'.");
    if (match(TOKEN_VAR)) {
        varDeclaration();
    } else if (match(TOKEN_SEMICOLON)) {
        // No initializer.
    } else {
        expressionStatement();
    }

    int surroundingLoopStart = innermostLoopStart;
    int surroundingLoopScopeDepth = innermostLoopScopeDepth;
    innermostLoopStart = currentChunk()->count;
    innermostLoopScopeDepth = current->scopeDepth;

    int exitJump = -1;
    if (!match(TOKEN_SEMICOLON)) {
        expression();
        consume(TOKEN_SEMICOLON, L"Expect '；' after loop condition.");

        // Jump out of the loop if the condition is false.
        exitJump = emitJump(OP_JUMP_IF_FALSE);
        emitByte(OP_POP); // Condition.
    }

    if (!match(TOKEN_RIGHT_PAREN)) {
        int bodyJump = emitJump(OP_JUMP);

        int incrementStart = currentChunk()->count;
        expression();
        emitByte(OP_POP);
        consume(TOKEN_RIGHT_PAREN, L"Expect ')' after for clauses.");

        emitLoop(innermostLoopStart);
        innermostLoopStart = incrementStart;
        patchJump(bodyJump);
    }

    int loopBody = current->function->chunk.count;
    statement();

    emitLoop(innermostLoopStart);

    if (exitJump != -1) {
        patchJump(exitJump);
        emitByte(OP_POP); // Condition.
    }

    int i = loopBody;
    while (i < current->function->chunk.count) {
        if (current->function->chunk.code[i] == OP_END) {
            current->function->chunk.code[i] = OP_JUMP;
            patchJump(i + 1);
            i += 3;
        } else {
            i += 1 + getByteCountForArguments(i);
        }
    }

    innermostLoopStart = surroundingLoopStart;
    innermostLoopScopeDepth = surroundingLoopScopeDepth;

    endScope();
}

static void ifStatement() {
    consume(TOKEN_LEFT_PAREN, L"Expect '(' after 'if'.");
    expression();
    consume(TOKEN_RIGHT_PAREN, L"Expect ')' after condition.");

    int thenJump = emitJump(OP_JUMP_IF_FALSE);
    emitByte(OP_POP);
    statement();

    int elseJump = emitJump(OP_JUMP);

    patchJump(thenJump);
    emitByte(OP_POP);

    if (match(TOKEN_ELSE)) statement();
    patchJump(elseJump);
}

static void returnStatement() {
    if (current->type == TYPE_SCRIPT) {
        error(L"Can't 返回 from top-level code.");
    }

    if (match(TOKEN_SEMICOLON)) {
        emitReturn();
    } else {
        if (current->type == TYPE_INITIALIZER) {
            error(L"Can't 返回 a value from an initializer.");
        }

        expression();
        consume(TOKEN_SEMICOLON, L"Expect '；' after return value.");
        emitByte(OP_RETURN);
    }
}

static void whileStatement() {
    int surroundingLoopStart = innermostLoopStart;
    int surroundingLoopScopeDepth = innermostLoopScopeDepth;
    innermostLoopStart = currentChunk()->count;
    innermostLoopScopeDepth = current->scopeDepth;

    consume(TOKEN_LEFT_PAREN, L"Expect '(' after 'while'.");
    expression();
    consume(TOKEN_RIGHT_PAREN, L"Expect ')' after condition.");

    int exitJump = emitJump(OP_JUMP_IF_FALSE);
    emitByte(OP_POP);
    int loopBody = current->function->chunk.count;
    statement();
    emitLoop(innermostLoopStart);

    patchJump(exitJump);
    emitByte(OP_POP);

    innermostLoopStart = surroundingLoopStart;
    innermostLoopScopeDepth = surroundingLoopScopeDepth;

    int i = loopBody;
    while (i < current->function->chunk.count) {
        if (current->function->chunk.code[i] == OP_END) {
            current->function->chunk.code[i] = OP_JUMP;
            patchJump(i + 1);
            i += 3;
        } else {
            i += 1 + getByteCountForArguments(i);
        }
    }
}

#define MAX_CASES 256

static void switchStatement() {
    consume(TOKEN_LEFT_PAREN, L"Expect '(' after 'switch'.");
    expression();
    consume(TOKEN_RIGHT_PAREN, L"Expect ')' after value.");
    consume(TOKEN_LEFT_BRACE, L"Expect '{' before switch cases.");

    int state = 0; // 0: before all cases, 1: before default, 2: after default.
    int caseEnds[MAX_CASES];
    int caseCount = 0;
    int previousCaseSkip = -1;

    while (!match(TOKEN_RIGHT_BRACE) && !check(TOKEN_EOF)) {
        if (match(TOKEN_CASE) || match(TOKEN_DEFAULT)) {
            TokenType caseType = parser.previous.type;

            if (state == 2) {
                error(L"Can't have another case or default after the default case.");
            }

            if (state == 1) {
                // At the end of the previous case, jump over the others.
                caseEnds[caseCount++] = emitJump(OP_JUMP);

                // Patch its condition to jump to the next case (this one).
                patchJump(previousCaseSkip);
                emitByte(OP_POP);
            }

            if (caseType == TOKEN_CASE) {
                state = 1;

                // See if the case is equal to the value.
                emitByte(OP_DUP);
                expression();

                consume(TOKEN_COLON, L"Expect ':' after case value.");

                emitByte(OP_EQUAL);
                previousCaseSkip = emitJump(OP_JUMP_IF_FALSE);

                // Pop the comparison result.
                emitByte(OP_POP);
            } else {
                state = 2;
                consume(TOKEN_COLON, L"Expect ':' after default.");
                previousCaseSkip = -1;
            }
        } else {
            // Otherwise, it's a statement inside the current case.
            if (state == 0) {
                error(L"Can't have statements before any case.");
            }
            statement();
        }
    }

    // If we ended without a default case, patch its condition jump.
    if (state == 1) {
        patchJump(previousCaseSkip);
        emitByte(OP_POP);
    }

    // Patch all the case jumps to the end.
    for (int i = 0; i < caseCount; i++) {
        patchJump(caseEnds[i]);
    }

    emitByte(OP_POP); // The switch value.
}

static void continueStatement() {
    if (innermostLoopStart == -1) {
        error(L"Can't use '继续' outside of a loop.");
    }

    consume(TOKEN_SEMICOLON, L"Expect '；' after '继续'.");

    // Discard any locals created inside the loop.
    for (int i = current->localCount - 1; i >= 0 && current->locals[i].depth > innermostLoopScopeDepth; i--) {
        emitByte(OP_POP);
    }

    // Jump to top of current innermost loop.
    emitLoop(innermostLoopStart);
}

static void breakStatement() {
    if (innermostLoopStart == -1) {
        error(L"Can't use '打断' outside of a loop.");
    }

    consume(TOKEN_SEMICOLON, L"Expect '；' after '打断'.");

    // Discard any locals created inside the loop.
    for (int i = current->localCount - 1; i >= 0 && current->locals[i].depth > innermostLoopScopeDepth; i--) {
        emitByte(OP_POP);
    }

    // Emit a placeholder instruction for the jump to the end of the body. When
    // we're done compiling the loop body and know where the end is, we'll
    // replace these with `CODE_JUMP` instructions with appropriate offsets.
    // We use `OP_END` here because that can't occur in the middle of
    // bytecode.
    emitJump(OP_END);
}

static void synchronize() {
    parser.panicMode = false;

    while (parser.current.type != TOKEN_EOF) {
        if (parser.previous.type == TOKEN_SEMICOLON) return;
        switch (parser.current.type) {
            case TOKEN_CLASS:
            case TOKEN_FUN:
            case TOKEN_VAR:
            case TOKEN_FOR:
            case TOKEN_IF:
            case TOKEN_SWITCH:
            case TOKEN_WHILE:
            case TOKEN_RETURN:
                return;

            default:
                ; // Do nothing.
        }

        advance();
    }
}

static void declaration() {
    if (match(TOKEN_CLASS)) {
        classDeclaration();
    } else if (match(TOKEN_FUN)) {
        funDeclaration();
    } else if (match(TOKEN_VAR)) {
        varDeclaration();
    } else {
        statement();
    }

    if (parser.panicMode) synchronize();
}

static void statement() {
    if (match(TOKEN_FOR)) {
        forStatement();
    } else if (match(TOKEN_IF)) {
        ifStatement();
    } else if (match(TOKEN_RETURN)) {
        returnStatement();
    } else if (match(TOKEN_WHILE)) {
        whileStatement();
    } else if (match(TOKEN_SWITCH)) {
        switchStatement();
    } else if (match(TOKEN_CONTINUE)) {
        continueStatement();
    } else if (match(TOKEN_BREAK)) {
        breakStatement();
    } else if (match(TOKEN_LEFT_BRACE)) {
        beginScope();
        block();
        endScope();
    }
    else {
        expressionStatement();
    }
}

ObjFunction* compile(const wchar_t* source) {
    initScanner(source);
    Compiler compiler;
    initCompiler(&compiler, TYPE_SCRIPT);

    parser.hadError = false;
    parser.panicMode = false;

    advance();

    while (!match(TOKEN_EOF)) {
        declaration();
    }

    ObjFunction* function = endCompiler();
    return parser.hadError ? NULL : function;
}

void markCompilerRoots() {
    Compiler* compiler = current;
    while (compiler != NULL) {
        markObject((Obj*)compiler->function);
        compiler = compiler->enclosing;
    }
}
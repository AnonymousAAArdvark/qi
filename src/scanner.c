//
// Created by Andrew Yang on 8/30/21.
//

#include <stdio.h>
#include <string.h>

#include "common.h"
#include "scanner.h"

typedef struct {
    const wchar_t* start;
    const wchar_t* current;
    int line;
} Scanner;

Scanner scanner;

void initScanner(const wchar_t* source) {
    scanner.start = source;
    scanner.current = source;
    scanner.line = 1;
}

static bool isAtEnd() {
    return *scanner.current == L'\0';
}

static wchar_t advance() {
    scanner.current++;
    return scanner.current[-1];
}

static wchar_t peek() {
    return *scanner.current;
}

static wchar_t peekNext() {
    if (isAtEnd()) return L'\0';
    return scanner.current[1];
}

static bool match(wchar_t expected) {
    if (isAtEnd()) return false;
    if (*scanner.current != expected) return false;
    scanner.current++;
    return true;
}

static Token makeToken(TokenType type) {
    Token token;
    token.type = type;
    token.start = scanner.start;
    token.length = (int)(scanner.current - scanner.start);
    token.line = scanner.line;
    return token;
}

static Token errorToken(const wchar_t* message) {
    Token token;
    token.type = TOKEN_ERROR;
    token.start = message;
    token.length = (int)wcslen(message);
    token.line = scanner.line;
    return token;
}

static void skipWhitespace() {
    for (;;) {
        wchar_t c = peek();
        switch (c) {
            case L' ':
            case L'\r':
            case L'\t':
                advance();
                break;
            case L'\n':
                scanner.line++;
                advance();
                break;
            case L'/':
                if (peekNext() == L'/') {
                    // A comment goes until the end of the line.
                    while (peek() != L'\n' && !isAtEnd()) advance();
                } else {
                    return;
                }
                break;
            default:
                return;
        }
    }
}

static TokenType checkKeyword(int start, int length, const wchar_t* rest, TokenType type) {
    if (scanner.current - scanner.start == start + length
    && memcmp(scanner.start + start, rest, length * sizeof(wchar_t)) == 0) {
        return type;
    }

    return TOKEN_IDENTIFIER;
}

static TokenType identifierType() {
    switch (scanner.start[0]) {
        case L'b': return checkKeyword(1, 4, L"reak", TOKEN_BREAK);
        case L'c':
            if (scanner.current - scanner.start > 1) {
                switch (scanner.start[1]) {
                    case L'l': return checkKeyword(2, 3, L"ass", TOKEN_CLASS);
                    case L'o': return checkKeyword(2, 6, L"ntinue", TOKEN_CONTINUE);
                    case L'a': return checkKeyword(2, 2, L"se", TOKEN_CASE);
                }
            }
        case L'd': return checkKeyword(1, 6, L"efault", TOKEN_DEFAULT);
        case L'e': return checkKeyword(1, 3, L"lse", TOKEN_ELSE);
        case L'f':
            if (scanner.current - scanner.start > 1) {
                switch (scanner.start[1]) {
                    case L'a': return checkKeyword(2, 3, L"lse", TOKEN_FALSE);
                    case L'o': return checkKeyword(2, 1, L"r", TOKEN_FOR);
                    case L'u': return checkKeyword(2, 1, L"n", TOKEN_FUN);
                }
            }
            break;
        case L'i': return checkKeyword(1, 1, L"f", TOKEN_IF);
        case L'n': return checkKeyword(1, 2, L"il", TOKEN_NIL);
        case L'r': return checkKeyword(1, 5, L"eturn", TOKEN_RETURN);
        case L's':
            if (scanner.current - scanner.start > 1) {
                switch (scanner.start[1]) {
                    case L'u': return checkKeyword(2, 3, L"per", TOKEN_SUPER);
                    case L'w': return checkKeyword(2, 4, L"itch", TOKEN_SWITCH);
                }
            }
        case L't':
            if (scanner.current - scanner.start > 1) {
                switch (scanner.start[1]) {
                    case L'h': return checkKeyword(2, 2, L"is", TOKEN_THIS);
                    case L'r': return checkKeyword(2, 2, L"ue", TOKEN_TRUE);
                }
            }
            break;
        case L'v': return checkKeyword(1, 2, L"ar", TOKEN_VAR);
        case L'w': return checkKeyword(1, 4, L"hile", TOKEN_WHILE);
    }

    return TOKEN_IDENTIFIER;
}

static Token identifier() {
    while (iswalpha(peek()) || iswdigit(peek()) || peek() == L'_') advance();
    return makeToken(identifierType());
}

static Token number() {
    while (iswdigit(peek())) advance();

    // Look for a fractional part.
    if (peek() == L'.' && iswdigit(peekNext())) {
        // Consume the ".".
        advance();

        while (iswdigit(peek())) advance();
    }

    return makeToken(TOKEN_NUMBER);
}

static Token string() {
    while (peek() != L'"' && !isAtEnd()) {
        if (peek() == L'\n') scanner.line++;
        advance();
    }

    if (isAtEnd()) return errorToken(L"Unterminated string.");

    // The closing quote.
    advance();
    return makeToken(TOKEN_STRING);
}

Token scanToken() {
    skipWhitespace();
    scanner.start = scanner.current;

    if (isAtEnd()) return makeToken(TOKEN_EOF);

    wchar_t c = advance();
    if (iswalpha(c) || c == L'_') return identifier();
    if (iswdigit(c)) return number();

    switch (c) {
        case L'(': return makeToken(TOKEN_LEFT_PAREN);
        case L')': return makeToken(TOKEN_RIGHT_PAREN);
        case L'{': return makeToken(TOKEN_LEFT_BRACE);
        case L'}': return makeToken(TOKEN_RIGHT_BRACE);
        case L';': return makeToken(TOKEN_SEMICOLON);
        case L',': return makeToken(TOKEN_COMMA);
        case L'.': return makeToken(TOKEN_DOT);
        case L'-':
            return makeToken(match(L'=') ? TOKEN_MINUS_EQUAL
            : match(L'-') ? TOKEN_MINUS_MINUS : TOKEN_MINUS);
        case L'+':
            return makeToken(match(L'=') ? TOKEN_PLUS_EQUAL
            : match(L'+') ? TOKEN_PLUS_PLUS : TOKEN_PLUS);
        case L'/': return makeToken(TOKEN_SLASH);
        case L'*': return makeToken(TOKEN_STAR);
        case L'%': return makeToken(TOKEN_PERCENT);
        case L':': return makeToken(TOKEN_COLON);
        case L'!':
            return makeToken(match(L'=') ? TOKEN_BANG_EQUAL : TOKEN_BANG);
        case L'=':
            return makeToken(match(L'=') ? TOKEN_EQUAL_EQUAL : TOKEN_EQUAL);
        case L'<':
            return makeToken(match(L'=') ? TOKEN_LESS_EQUAL : TOKEN_LESS);
        case L'>':
            return makeToken(match(L'=') ? TOKEN_GREATER_EQUAL : TOKEN_GREATER);
        case L'"': return string();
        case L'[': return makeToken(TOKEN_LEFT_BRACKET);
        case L']': return makeToken(TOKEN_RIGHT_BRACKET);
        case L'&': if (match(L'&')) return makeToken(TOKEN_AND);
        case L'|': if (match(L'|')) return makeToken(TOKEN_OR);
    }

    return errorToken(L"Unexpected character.");
}

Scanner scanner;

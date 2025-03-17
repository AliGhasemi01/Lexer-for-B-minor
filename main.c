#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

#define MAX_LEXEME_LEN 100
#define NUM_KEYWORDS 15
#define MAX_SYMBOLS 100

typedef enum {
    TOKEN_ID,       
    TOKEN_KEYWORD,
    TOKEN_INTEGER,
    TOKEN_STRING,
    TOKEN_OPERATOR,
    TOKEN_DELIMITER,
    TOKEN_ERROR,
    TOKEN_EOF       
} TokenType;

typedef struct {
    char lexeme[MAX_LEXEME_LEN];
    TokenType type;
    int line_no;
    int column_no;
} Token;

typedef struct {
    char name[50];
    int id;
} Symbol;

const char *keywords[NUM_KEYWORDS] = { "array", "boolean", "char", "else", "false", "for",
                                              "function", "if", "integer", "print", "return",
                                              "string", "true", "void", "while" };

const char operators[] = "+-!*/%^<>=|&";
const char delimiters[] = "()[]{},;:'\"";

FILE *sourceFile;
int line_no = 1, column_no = 0;

void initLexer(FILE *file) {
    sourceFile = file;
}

int isKeyword(const char *str) {
    for (int i = 0; i < NUM_KEYWORDS; i++) {
        if (strcmp(str, keywords[i]) == 0)
            return 1;
    }
    return 0;
}


Symbol symbolTable[MAX_SYMBOLS];
int symbolCount = 0;
int addSymbol(char *name) {
    for (int i = 0; i < symbolCount; i++) {
        if (strcmp(symbolTable[i].name, name) == 0) {
            return symbolTable[i].id;
        }
    }

    if (symbolCount < MAX_SYMBOLS) {
        strcpy(symbolTable[symbolCount].name, name);
        symbolTable[symbolCount].id = 100 + symbolCount;  // مقداردهی یکتا
        return symbolTable[symbolCount++].id;
    } else {
        printf("Symbol table full!\n");
        return -1;
    }
}

Token getNextToken() {
    Token token;
    char c;
    int i = 0;
    memset(token.lexeme, 0, MAX_LEXEME_LEN);

    while ((c = fgetc(sourceFile)) != EOF) {
        column_no++;

        if (c == ' ' || c == '\t') continue;
        if (c == '\n') {
            line_no++;
            column_no = 0;
            continue;
        }

        int start_column = column_no;


        if (c == '/') {
            char next = fgetc(sourceFile);
            
            if (next == '*') {
                column_no++;
                while ((c = fgetc(sourceFile)) != EOF) {
                    column_no++;
                    if (c == '\n') {
                        line_no++;
                        column_no = 0;
                    }
                    if (c == '*' && (next = fgetc(sourceFile)) == '/') {
                        column_no++;
                        break;
                    }
                }
                continue;
            } else if (next == '/') {
                column_no++;
                while ((c = fgetc(sourceFile)) != EOF && c != '\n') {
                    column_no++;
                }
                line_no++;
                column_no = 0;
                continue;
            } else {
                ungetc(next, sourceFile);
            }
        }

        

        if (isalpha(c) || c == '_') {
            token.lexeme[i++] = c;
            while (isalnum(c = fgetc(sourceFile)) || c == '_') {
                token.lexeme[i++] = c;
                column_no++;
            }
            ungetc(c, sourceFile);
            token.lexeme[i] = '\0';

            token.type = isKeyword(token.lexeme) ? TOKEN_KEYWORD : TOKEN_ID;
            token.line_no = line_no;
            token.column_no = token.column_no = start_column;
            return token;
        }

        if (isdigit(c)) {
            token.lexeme[i++] = c;
            while (isdigit(c = fgetc(sourceFile))) {
                token.lexeme[i++] = c;
                column_no++;
            }
            ungetc(c, sourceFile);
            token.lexeme[i] = '\0';
            token.type = TOKEN_INTEGER;
            token.line_no = line_no;
            token.column_no = start_column;
            return token;
        }


        if (c == '"') {
            token.lexeme[i++] = c;
            while ((c = fgetc(sourceFile)) != '"' && c != EOF) {
                token.lexeme[i++] = c;
                column_no++;
            }
            token.lexeme[i++] = '"';
            token.lexeme[i] = '\0';

            token.type = TOKEN_STRING;
            token.line_no = line_no;
            token.column_no = start_column;
            column_no++;
            return token;
        }

        if (strchr(operators, c)) {
            token.lexeme[0] = c;
            token.lexeme[1] = '\0';
            token.type = TOKEN_OPERATOR;
            token.line_no = line_no;
            token.column_no = start_column;
            return token;
        }

        if (strchr(delimiters, c)) {
            token.lexeme[0] = c;
            token.lexeme[1] = '\0';
            token.type = TOKEN_DELIMITER;
            token.line_no = line_no;
            token.column_no = start_column;
            return token;
        }
        

        printf("Lexical error at line %d, column %d: Invalid character '%c'\n", line_no, column_no, c);
        token.lexeme[0] = c;
        token.lexeme[1] = '\0';
        token.type = TOKEN_ERROR;
        token.line_no = line_no;
        token.column_no = start_column;
        return token;
    }

    token.type = TOKEN_EOF;
    return token;
}

void printToken(Token token) {
    char value[20] = "";

    if (token.type == TOKEN_ERROR) {
        return;
    }

    if (token.type == TOKEN_ID) {
        sprintf(value, "%d", addSymbol(token.lexeme));
    } else if (token.type == TOKEN_ERROR) {
        strcpy(value, "ERROR");
    }
    else {
        strcpy(value, token.lexeme);
;    }

    printf("%-15s %-15s %-15s %d:%d\n", 
           token.lexeme, 

           (token.type == TOKEN_ID) ? "ID" :
           (token.type == TOKEN_KEYWORD) ? "KEYWORD" :
           (token.type == TOKEN_INTEGER) ? "INTEGER" :
           (token.type == TOKEN_STRING) ? "STRING" :
           (token.type == TOKEN_OPERATOR) ? "OPERATOR" :
           (token.type == TOKEN_DELIMITER) ? "DELIMITER" : "ERROR",

           value, token.line_no, token.column_no);
}


int main() {
    FILE *file = fopen("input.txt", "r");
    if (!file) {
        printf("Error opening input file\n");
        return 1;
    }

    initLexer(file);
    Token token;
    
    printf("%-15s %-15s %-15s %-15s\n", "token", "token_type", "token_value", "token_loc");
    printf("----------------------------------------------------------\n");
    
    while ((token = getNextToken()).type != TOKEN_EOF) {
        printToken(token);
    }

    fclose(file);
    return 0;
}


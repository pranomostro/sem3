#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>

#include "list.h"

typedef enum parserState {dQuote, quote, dollar, none} parserState;

void p(char* s) {
    puts(s);
}

bool parse(char* str) {
    int len = strlen(str);
    
    parserState state = none;
    bool backslash = false;

    // char* buf = malloc(len * sizeof(char));

    list_t* paramList = list_init();
    if (paramList == NULL) {
        perror("Cannot allocate list\n");
        exit(-1);
    }

    char buf[len];
    int i = 0;

    for (char* c = str; *c != '\0'; c++) {
        char cur = *c;
        switch (state) {
            case dQuote:
                if (cur == '"') {
                    state = none;
                    continue;
                }

                buf[i++] = cur;

                break;
            case quote:
                if (cur == '\'') {
                    state = none;
                    continue;
                }

                buf[i++] = cur;

                break;
            case dollar:
                break;
            case none: 
                if (backslash) {
                    buf[i++] = cur;
                    backslash = false;
                    continue;
                }
                switch (cur) {
                    case '"':
                        state = dQuote;
                        break;
                    case '\'':
                        state = quote;
                        break;
                    case '$':
                        state = dollar;
                        break;
                    case '\\':
                        backslash = true;
                        break;
                    case ' ': // Finish one param
                        if (i == 0) {
                            // buf is empty
                            continue;
                        }
                        // Put param into list
                        buf[i] = '\0';
                        char* param = strdup(buf);
                        list_append(paramList, param);

                        // Reset
                        i = 0;
                        break;
                    default:
                        buf[i++] = cur;
                        break;
                }
                break;
            default:
                perror("Unexpected state\n");
                exit(-1);
        }
    }

    switch (state) {
        case dQuote:
            perror("Unclosed \"\n");
            exit(-1);
        case quote:
            perror("Unclosed '\n");
            exit(-1);
        case dollar:
            //perror("Unused $\n");
            //exit(-1);
        case none:
            if (backslash) {
                perror("Unused \\\n");
                exit(-1);
            }
            // Finish one param
            if (i != 0) {
                // buf not empty
                // Put param into list
                buf[i] = '\0';
                char* param = strdup(buf);
                list_append(paramList, param);
            }
            break;
        default:
            perror("Unexpected state\n");
            exit(-1);
    }

    list_print(paramList, p);

    return strcmp(paramList->first->data, "exit") != 0;
}
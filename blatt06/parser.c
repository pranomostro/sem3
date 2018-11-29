#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#include "list.h"

typedef enum parserState {dQuote, quote, dollar, none} parserState;

void p(char* s) {
    puts(s);
}

bool parse(char* str, char** envp) {
    int len = strlen(str);
    if (len == 0) {
        return true;
    }

    parserState restore = none;
    parserState state = none;
    bool backslash = false;

    list_t* paramList = list_init();
    if (paramList == NULL) {
        perror("Cannot allocate list\n");
        exit(-1);
    }

    char* buf = malloc(len * sizeof(char));
    int bufLen = len;
    int i = 0;
    
    char envBuf[len];
    int ei = 0;

    for (char* c = str; *c != '\0'; c++) {
        // printf("%x %c %d\n", (unsigned int) c, *c, state);
        char cur = *c;

        if (backslash) {
            buf[i++] = cur;
            backslash = false;
            continue;
        }

        if (cur == '\\') {
            backslash = true;
            continue;
        }

        switch (state) {
            case dQuote:
                if (cur == '"') {
                    state = none;
                    continue;
                } else if (cur == '$') {
                    restore = state;
                    state = dollar;
                    continue;
                }

                buf[i++] = cur;

                break;
            case quote:
                if (cur == '\'') {
                    state = none;
                    continue;
                } else if (cur == '$') {
                    restore = state;
                    state = dollar;
                    continue;
                }

                buf[i++] = cur;

                break;
            case dollar:
                if (!(isupper(cur)|| isdigit(cur) || cur == '_')) {
                    envBuf[ei] = '\0';
                    char* envVar = getenv(envBuf);
                    
                    // Expand buf when too small
                    int envLen = strlen(envVar);
                    if (envLen + i + 1>= bufLen) {
                        buf = realloc(buf, envLen + i + 1);
                    }

                    // Copy into buf
                    strncpy(&buf[i], envVar, envLen);
                    i += envLen;
                    
                    // Reset
                    ei = 0;
                    // Reevaluate current character
                    c--;
                    state = restore;
                    continue;
                }

                envBuf[ei++] = cur;
                break;
            case none: 
                switch (cur) {
                    case '"':
                        state = dQuote;
                        break;
                    case '\'':
                        state = quote;
                        break;
                    case '$':
                        restore = state;
                        state = dollar;
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
        case dollar: {
            envBuf[ei] = '\0';
            char* envVar = getenv(envBuf);      
            
            // Expand buf when too small
            int envLen = strlen(envVar);
            if (envLen + i + 1 >= bufLen) {
                buf = realloc(buf, envLen + i + 1);
            }
            
            // Copy into buf
            strcpy(&buf[i], envVar);
            // hexDump("buf", buf, envLen+i+1);
            char* param = strdup(buf);
            list_append(paramList, param);
            break;
        }
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

    free(buf);
    list_print(paramList, p);

    if (paramList->first == NULL) {
        return true;
    }

    bool ret = strcmp(paramList->first->data, "exit") != 0;
    // Free paramList
    while (paramList->first != NULL) {
        free(paramList->first->data);
        list_remove(paramList, paramList->first);
    }

    list_finit(paramList);

    free(paramList);

    return ret;
}
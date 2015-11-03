/****************************************************************************
 *                              ArtraCFD                                    *
 *                          <By Huangrui Mo>                                *
 * Copyright (C) 2014-2018 Huangrui Mo <huangrui.mo@gmail.com>              *
 * This file is part of ArtraCFD.                                           *
 * ArtraCFD is free software: you can redistribute it and/or modify it      *
 * under the terms of the GNU General Public License as published by        *
 * the Free Software Foundation, either version 3 of the License, or        *
 * (at your option) any later version.                                      *
 ****************************************************************************/
/****************************************************************************
 * Required Header Files
 ****************************************************************************/
#include "commons.h"
#include <stdio.h> /* standard library for input and output */
#include <stdlib.h> /* dynamic memory allocation and exit */
#include <string.h> /* manipulating strings */
/****************************************************************************
 * General functions
 ****************************************************************************/
int CommandLineProcessor(char *lineCommand)
{
    char *scanner = lineCommand; /* copy the address to scanner */
    char *receiver = lineCommand; /* copy the address to receiver */
    /* check whether is a NULL command pointer */
    if (NULL == lineCommand) {
        fprintf(stderr, "warning: process a NULL command line pointer\n");
        return 0;
    }
    /* check whether is a empty command line */
    if ('\0' == lineCommand[0]) {
        return 0;
    }
    /* then get rid of before tabs and spaces */
    while ((' ' == scanner[0]) || ('\t' == scanner[0])) {
        scanner[0] = ' '; /* replace tab with space */
        ++scanner; /* update scanner */
    }
    while ('\0' != scanner[0]) { /* until reach the end of original command */
        if (('\r' == scanner[0]) || ('\n' == scanner[0])) {
            ++scanner; /* scan the next character */
            continue; /* go to next loop */
        }
        if ('#' == scanner[0]) {
            break; /* no more scan */
        }
        if ((' ' == scanner[0]) || ('\t' == scanner[0])) { /* a space or tab */
            if (' ' != scanner[-1]) { /* only check space; tabs are all replaced */
                /* now its a first space or tab between words */
                scanner[0] = ' '; /* replace tab with space */
                receiver[0] = ' '; /* receive a space */
                ++receiver; /* update the receiver address */
                ++scanner; /* scan the next character */
                continue;
            }
            /* otherwise, do not receive */
            scanner[0] = ' '; /* replace tab with space */
            ++scanner; /* scan the next character */
            continue;
        }
        /* now its a normal character */
        receiver[0] = scanner[0]; /* receive the value of current scanner */
        ++receiver; /* update the receiver address */
        ++scanner; /* scan the next character */
    }
    /* if no useful information in command, receiver didn't receive anything */
    if (receiver == lineCommand) {
        receiver[0] = '\0';
        return 0;
    }
    /* if received something and also ended with a space */
    if (' ' == receiver[-1]) {
        receiver[-1] = '\0';
        return 0;
    }
    receiver[0] = '\0'; /* add string terminator */
    return 0;
}
void FatalError(const char *statement)
{
    fprintf(stderr, "error: %s\n", statement);
    exit(EXIT_FAILURE); /* indicate failure */
}
int ShowInformation(const char *statement)
{
    if (0 == strcmp(statement, "Session End")) {
        fprintf(stdout, "\n**********************************************************\n\n");
        return 0;
    }
    fprintf(stdout, "%s\n", statement);
    return 0;
}
/*
 * Assign linear array storage to a pointer with a specific datatype.
 * Note:
 *  - in C, don't need to cast the return value of malloc. The pointer to
 *    void returned by malloc is automatically converted to the correct type.
 *    However, if compile with a C++ compiler, a cast is needed.
 *  - The sizeof operator is used to determine the amount of space a designated
 *    datatype would occupy in memory. To use sizeof, the keyword "sizeof" is
 *    followed by a type name or an expression (which may be merely a variable
 *    name). If a type name is used, it must always be enclosed in parentheses,
 *    whereas expressions can be specified with or without parentheses. 
 *  - The sizeof is a unary operator (not a function!), sizeof gives the size
 *    in units of chars.
 *  - When sizeof is applied to the name of a static array (not allocated
 *    through malloc), the result is the size in bytes (in unit of chars) of the 
 *    whole array, that is, number of elements times the sizeof an array element.
 *    This is one of the few exceptions to the rule that the name of an array is
 *    converted to a pointer to the first element of the array, and is possible
 *    just because the actual array size is fixed and known at compile time, 
 *    when sizeof operator is evaluated.
 *  - When returning a pointer from a function, do not return a pointer that
 *    points to a value that is local to the function or that is a pointer
 *    to a function argument. Pointers to local variables become invalid
 *    when the function exits. In a function, the value returned points to
 *    a static variable or returning a pointer to dynamically allocated 
 *    memory can both be valid.
 */
void *AssignStorage(const int idxMax, const char *dataType)
{
    void *pointer = NULL;
    if (0 == strcmp(dataType, "Real")) {
        pointer = malloc(idxMax * sizeof(Real));
    }
    if (0 == strcmp(dataType, "double")) {
        pointer = malloc(idxMax * sizeof(double));
    }
    if (0 == strcmp(dataType, "float")) {
        pointer = malloc(idxMax * sizeof(float));
    }
    if (0 == strcmp(dataType, "char")) {
        pointer = malloc(idxMax * sizeof(char));
    }
    if (0 == strcmp(dataType, "int")) {
        pointer = malloc(idxMax * sizeof(int));
    }
    if (NULL == pointer) {
        FatalError("memory allocation failed");
    }
    return pointer;
}
/*
 * There are a number of rules you should follow in de-allocating memory.
 * - Prevent access to de-allocated memory. This can be done by setting the
 *   pointer to null after de-allocating, which requires pass the reference
 *   of the target pointer.
 * - De-allocate memory in the reverse order it was allocated. This makes sure
 *   that any dependencies between the allocated memory will not result in 
 *   "dangling pointers". So if one allocated data structure has a pointer to 
 *   another allocated data structure, the second should be de-allocated first.
 * - For a temporary memory block, de-allocate the block before leaving the
 *   routine. If the de-allocation is not done before the routine ends, access
 *   to the memory is lost.
 */
int RetrieveStorage(void *pointer)
{
    if (NULL != pointer) {
        free(pointer);
    }
    return 0;
}
/* a good practice: end file with a newline */


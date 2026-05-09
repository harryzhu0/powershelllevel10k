#ifndef PWSH10K_PARSE_H
#define PWSH10K_PARSE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

typedef struct {
    char *name;
    char *value;
} Variable;

typedef struct {
    char *label;   
    char *target;  
} Choice;

typedef struct {
    char   *name;      
    char   *text;      
    Choice *choices;   
    int     nchoices;
} Step;

typedef struct {
    Step *steps;
    int   nsteps;

    Variable *vars;
    int nvars;
} Program;

typedef struct {
    int errcode; // if this is 0 then that means that the program ran successfully, probably because you have to set it yourself
    Program prog;
    char *err;
} Return;

Return parse();

#endif
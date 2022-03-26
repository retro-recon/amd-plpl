/*
October 24, 1987: OPT2DEF.H contains all the variable declarations
and definitions for the Logic Optimizer V2.2

C note:
  a variable definition allocates space; a declaration
  uses the variable that is defined elsewhere
*/
#define vms        0    /* for vax vms systems */
#define unix        1    /* for generic unix systems */
#define comp_inv_c86    0    /* for IBM-PC computer innovations C86 */
#define microsoft    0    /* for IBM-PC microsoft C V4.0 */
/*
#if vms
#include stdio
#include ctype
#include math
#else
#if unix
#include <stdio.h>
#include <ctype.h>
#include <math.h>
#else
#if comp_inv_c86
#include "stdio.h"
#else
#if microsoft
#include "stdio.h"
#include "ctype.h"
#endif
#endif
#endif
#endif
*/
#include "stdio.h"

#ifdef DEF_MAIN
#define STORAGE
#else
#define STORAGE extern
#endif

#define GOOD_EXIT 0  /* used in exit() statements */
#define BAD_EXIT  1
#define NO  0
#define YES 1
#define STX 2
#define ETX 3

#define VALID 0
#define FATAL 2
#define WARNING 3
#define NOTICE  4
#define CONTINUE 5
#define STOP 6
#define LOW  7
#define HIGH 8
#define NO_FILE   15

#define MAX_NAME   29
#define LONGNAME   80
#define NULL_VALUE -4000

#define THE_END     1
#define VARIABLE    2
#define NUMBER      3
#define KEYWORD     4
#define WHITE_SPACE 5
#define PUNCT_MARK  6
#define NOT_A_KEYWORD 7

#define NO_BASE     -1
#define BINARY       0
#define OCTAL        1
#define DECIMAL      2
#define HEXADECIMAL  3
#define NOT_A_NUMBER 4

#define START_EQ '*'
#define END_EQ   '@'

extern char *malloc();
extern int free();
extern char *strcat();
extern char *strcpy();
extern char *strncat();
extern char *strncpy();
extern int fgetc();
extern char *fgets();
extern double pow();
extern FILE *fopen();
extern int fclose();

/*
void free();
void exit();
*/

#ifndef OPT_FILE
extern FILE *exist();
#endif
/* - - - - - - - - */
/* VARTYPE is used to hold the logic equation; it will be a linked list
   with each record holding 2 pointers: NEXTVAR and NEXT_MINTERM. NEXTVAR
   points to the next variable in the minterm and NEXT_MINTERM points to
   the next product term in this function.

   Once the function has been parsed, this is compressed into the
   array format VARRAY, which will be an array of integers and a
   pointer to the next product term. This was done to simplify the
   optimization process.
*/

STORAGE
struct vartype {
  int a_var;
  struct vartype *nextvar;
  struct vartype *next_minterm;
};
STORAGE
struct vartype *tmplist,*varlist;

STORAGE
struct varray {
  int *variables;
  struct varray *nextPT;
};
STORAGE
struct varray *mainlist;

STORAGE
char optsource[LONGNAME],optdest[LONGNAME],
     icon[MAX_NAME],main_msg[LONGNAME],chr;
STORAGE
char source_line[256], token[MAX_NAME], last_token[MAX_NAME];

STORAGE int screen_position, token_type, number_type, line_number,
   the_keyword, lexi_i, end_of_file, end_found, errors,
   get_new_token, debug_flag, suppress_msg, var_number;

STORAGE FILE *input_file;
STORAGE FILE *out_file;

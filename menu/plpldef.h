/* August 8, 1986 : PLPLDEF.H contains variable definitions and declarations
   for the PLPL Rev 2.2 menu program

Portability considerations: When transferring to UNIX or VMS,
the following routines must be changed in:
  Source file OPR1.C
   - CLEAR()  = clears the screen
   - RUN_OS() = runs OS commands */

#ifdef vms
#include stdio
#include ctype
#else
#ifdef unix
#include <stdio.h>
#include <ctype.h>
#include <curses.h>
#else
#include "stdio.h"
#ifdef MENU_MAIN
#include "screen.h" /* contains screen and cursor routines for PC */
#endif              /* for MENU_MAIN */
#endif   /* for unix */
#endif   /* for vms  */

#ifdef MENU_MAIN
#define STORAGE
#else
#define STORAGE extern
#endif

#define NAMELNG 80  /* maximum name length */
#define MAXLNG  80  /* maximum string length */
#define MAXARG  20  /* maximum number of arguments for a module */
#define VERYLONG 256
#define NO  0
#define YES 1
#define STOP 2

#define OPTN_FILE "$plplopt.txt"
#define HELP_FILE "$plplhlp.txt"

/* For the big model : all library functions
   returning pointers need to be defined as follows */
extern char *malloc();  /* needed for CI C86 compiler big model version */
extern char *realloc(); /* needed for CI C86 compiler big model version */
extern int free();      /* needed for CI C86 compiler big model version */
extern double pow();
extern FILE *fopen();
extern int fclose();
extern char *strcpy();
extern char *strncpy();
extern char *strcat();
extern char *strncat();
extern int strcmp();
extern int fgetc();
extern char *fgets();

STORAGE
char chr,long_string[VERYLONG],src_name[NAMELNG+1],tmp1_name[NAMELNG+1],
     tmp2_name[NAMELNG+1],tmp3_name[NAMELNG+1], arg_array[MAXARG][MAXLNG],
     arg_statement[MAXARG][MAXLNG],arg_buffer[MAXARG][MAXLNG];
STORAGE
FILE *ptr;

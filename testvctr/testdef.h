/* August 29, 1986
 TESTDEF.H : Contains the data structure definition and declaration files
               for the manual test vector generation (TESTV) program.
   C users note: a variable definition allocates space; a declaration
                 uses the variable that is defined elsewhere */
#ifdef vms
#include stdio
#include ctype
#include math
#else
#ifdef unix
#include <stdio.h>
#include <ctype.h>
#include <math.h>
#else
#include "stdio.h"
#endif
#endif

#ifdef DEF_MAIN
#define STORAGE
#else
#define STORAGE extern
#endif

#define GOOD_EXIT 0
#define BAD_EXIT  1

#define NO    0
#define YES   1
#define VALID 0
#define FATAL 2
#define WARNING 3
#define NOTICE  4
#define CONTINUE 5
#define STOP 6
#define NEXT 7
#define RIGHT 8
#define LEFT 9
#define COMPLETE 10
#define RBRACKET 11 /* right bracket */
#define LBRACKET 12 /* left bracket */
#define PAIR 13

#define PROGRAMMABLE 0    /* define polarity */
#define LOW  1
#define HIGH 2

#define HIGH_FDBK 0    /* indicates feedback source */
#define LOW_FDBK 1
#define NO_FDBK 2
#define CORRECT_FDBK 3  /* see MAP.23 for details */

/* pin types */
#define INPUT   0
#define OUTPUT  1
#define IO      2
#define BREG    3  /* buried register */
#define VCC     4
#define GND     5
#define CLOCK   6
#define CLK_INPUT 7
#define CONTROL 8

#define OPERATOR  9
#define SPEC_OPERATOR 10
#define NC     11
#define VCTR   12
#define SCALR  13
#define MACRO  14
#define PT     15
#define FUSE   16
#define EXIST  17
#define NOT_EXIST 18
#define FUNCTION  19
#define IN  20
#define OUT 21
#define I_O  22

#define MAX_NAME   29
#define NULL_VALUE -4000
#define INPUT_ARRAY_SIZE 100  /* see MAP.23 for details */

#define THE_END     1
#define VARIABLE    2
#define NUMBER      3
#define KEYWORD     4
#define WHITE_SPACE 5
#define PUNCT_MARK  6
#define NOT_A_KEYWORD 7
#define DEVICE 8
#define PIN    9
#define DEFINE 10
#define TEST_VECTORS 11
#define BEGIN 12
#define END 13

#define NO_BASE     -1
#define BINARY       0
#define OCTAL        1
#define DECIMAL      2
#define HEXADECIMAL  3
#define NOT_A_NUMBER 4

#define STX 2
#define ETX 3

#define CONFIG_FILE "$___plpl.cfg"

#define OPR_OPRND  "put an operator between operands"
#define MEM_ERR_MSG "not enough memory to create device link map"

#define TYP0_INSERT 0  /* used by LIST_INSERT routine to determine which */
#define TYP1_INSERT 1  /* list type to insert an item into */

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
struct type0  /* used to hold the pin numbers */
  { char fn_name[MAX_NAME];
    int fuse_numbers;
    struct type0 *typ0_next;
  };

STORAGE
struct type1
  { int pin_num;
    char operator;
    struct type1 *backptr;
    struct type1 *typ1_next;
  };
STORAGE
struct type1 *typ1_entry, *typ1_temp, *vctr_list,
       *vctr2, *vctr_temp, *pin_vctr;

STORAGE
struct type2
  { int func_fuse;
    char opt_name[MAX_NAME];
    int selected;
    int addr_func_fuse;
    int count_state;
    struct type2 *spec_options;
    struct type2 *typ2_next;
  };
STORAGE
struct type2 *typ2_entry;

STORAGE
struct type3
  /* definition structure for pins in a PLD package */
  { int pin_nmbr;
    int pin_type;     /* INPUT, OUTPUT, I/O, VCC or GND */
    char pname[MAX_NAME];
    int pname_type;   /* VCTR or SCALR */
    int true;
    int complement;
    int num_pterms; /* max number of PTs */
    int fdbkpath;  /* indicates positive or negative feedback source */
    int PTaddr;    /* first PT address */
    int polarity;  /* polarity type */
    int tristate;  /* tristate term available : see MAP.23 */
    int tri_1st;   /* 1st tristate address line */
    int preset;    /* preset term available */
    int pre_1st;   /* 1st preset term address */
    int reset;     /* reset term available */
    int reset_1st; /* 1st reset term address */
    struct type2 *spec_function;
    struct type3 *next_pin;
  };
STORAGE
struct type3 *pins;

#ifndef DEF_MAIN
extern struct type3 *find_pin();
#endif

STORAGE
char spec_name[MAX_NAME],testfile[MAX_NAME],
     icon[MAX_NAME],part_name[MAX_NAME], chr;
STORAGE
char source_line[256], token[MAX_NAME], last_token[MAX_NAME];

STORAGE int screen_position, token_type, number_type, line_number,
   the_keyword, lexi_i, end_of_file, end_found, errors,
   get_new_token, num_pins, physical_pins, input_array, dflt_test;

STORAGE FILE *input_file;
STORAGE FILE *out_file;
STORAGE FILE *part_file;

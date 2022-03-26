/* GLSDEC.H : Contains the data structure declaration files
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
#define BAD_EXIT 1

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
#define HIGH 14
#define LOW  15

#define INPUT   0
#define OUTPUT  1
#define I_O     2
#define VIRTUAL 3  /* virtual pin */
#define GND     4
#define VCC     5
#define OPERATOR  6
#define SPEC_OPERATOR 7
#define NC      8
#define VCTR    9
#define SCALR   10
#define MACRO 11
#define MAX_NAME   29
#define LONGNAME   80
#define NULL_VALUE -4000

#define THE_END     1
#define VARIABLE    2
#define NUMBER      3
#define KEYWORD     4
#define WHITE_SPACE 5
#define PUNCT_MARK  6

#define NO_BASE     -1
#define BINARY       0
#define OCTAL        1
#define DECIMAL      2
#define HEXADECIMAL  3
#define NOT_A_NUMBER 4
#define EXIST        5
#define NOT_EXIST    6
#define FUNCTION     7
#define SKIP   8
#define VALUE 9
#define PARSE 10

#define NOT_A_KEYWORD  0
#define DEVICE   1
#define PIN      2
#define DEFINE   3
#define BEGIN    4
#define END      5
#define CASE     6
#define IF       7
#define THEN     8
#define ELSE     9

#define END_STRING "-2"
#define OPR_OPRND  "put operator between operands"
#define INVAL_STAT "invalid logic expression/statement"

#define TYP0_INSERT 0  /* used by LIST_INSERT routine to determine which */
#define TYP1_INSERT 1  /* list type to insert an item into */
#define TMP_FILE  "$tmp.$$$"
#define FNC_FILE  "$fnctmp.$$$"
#define NAME_FILE "$___part"

STORAGE  /* used by the fix function */
struct fixtype
 { int pin;
   char function_name[40];
   struct fixtype *next;
 };
STORAGE
struct fixtype *known_pins,*current_ptr,*fname,*curnt_fname;

/* For the big model : all library functions
   returning pointers need to be defined as follows */
extern char *malloc();  /* needed for CI C86 compiler big model version */
extern char *realloc(); /* needed for CI C86 compiler big model version */
extern int free();      /* needed for CI C86 compiler big model version */
extern double pow();
extern FILE *fopen();
extern int fclose();

STORAGE struct type0
   /* data structure TYPE0 : used for special functions; will
      contain the function name and the corresponding product
      terms or fuse numbers */
  { char fn_name[MAX_NAME];  /* Function name max 20 characters long */
    int fuse_numbers;        /* Used for fuse numbers */
    struct type1 *pterms;
    struct type0 *typ0_next;
  };
STORAGE struct type0 *typ0_entry, *typ0_temp, *macro_list;
STORAGE struct type1
   /* data structure TYPE1 : contains fields for the eq tree:
      either a pin number, an operator ('*' or '+'), and a right
      and left pointer pointing to subtrees */
  { int pin_num;
    char operator;
    struct type1 *rtptr;
    struct type1 *ltptr;
    struct type1 *backptr;
    struct type1 *typ1_next;
  };
STORAGE struct type1
  *typ1_entry, *typ1_temp, *cond_list, *vctr_list, *macro_temp,
  *vctr_temp, *vctr2, *caselist, *oprtr, *oprnd, *tmp_oprnd, *pin_vctr,
  *func_list;
STORAGE struct type2
  /* definition structure for pins in a PLD package */
  { int pin_nmbr;
    int pin_type;     /* INPUT, OUTPUT, I/O, VCC or GND */
    char pname[MAX_NAME];
    int pname_type;   /* VCTR or SCALR */
    struct type1 *pt_list;   /* product term list */
    struct type0 *special;   /* special functions */
    struct type1 *oprtr_stk; /* use linked list as operator stack */
    struct type1 *oprnd_stk; /* use linked list as operand stack */
    struct type2 *typ2_next;
  };
STORAGE struct type2 *pins;

#ifndef DEF_MAIN
extern struct type2 *find_pin();
#endif

STORAGE char spec_name[LONGNAME], int_file[LONGNAME],
   icon[LONGNAME], part_name[LONGNAME], chr;
STORAGE char source_line[256], token[MAX_NAME], last_token[MAX_NAME];

STORAGE int screen_position, token_type, number_type, line_number,
   the_keyword, lexi_i, end_of_file, end_found, errors, debug_flag,
   get_new_token, level, first_PT, fnc_first, show_parentheses,write_part;

STORAGE FILE *input_file;
STORAGE FILE *out_file;
STORAGE FILE *fnc_file;  /* function file */
/* - - - - - - - - - - - */

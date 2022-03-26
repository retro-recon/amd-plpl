/*
June 10, 1987: SIMDEF.H contains all the variable declarations
and definitions for the Logic Simulator V2.2

C note:
  a variable definition allocates space; a declaration
  uses the variable that is defined elsewhere
*/
#define vms        0    /* for vax vms systems */
#define unix        1    /* for generic unix systems */
#define comp_inv_c86    0    /* for IBM-PC computer innovations C86 */
#define microsoft    0    /* for IBM-PC microsoft C V4.0 */

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
/*
#include "stdio.h"
*/
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
#define NEXT 7
#define RIGHT 8
#define LEFT 9
#define COMPLETE 10
#define RBRACKET 11 /* right bracket */
#define LBRACKET 12 /* left bracket */
#define PAIR 13
#define BAD_JEDEC 14
#define NO_FILE   15

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
#define INIT_PT 400

#define BEGIN_DEF 24
#define END_DEF   25
#define EQ        26
#define NODE      27
#define EQTYPE    28
#define VAL       29
#define RESET     30
#define SET       31
#define PRELOAD   32
#define FF_TYPE   33
#define STORAGE_TYPE 34
#define ENABLE    35
#define MEM0_INPUT  36
#define MEM1_INPUT  37
#define MEM_OUTPUT  38
#define OUT_ELEMENT 49
#define MISC_FNC    50
#define OBSERVE     51

#define D_FF       0
#define JK_FF      1
#define SR_FF      2
#define T_FF       3

#define REG_CELL   0
#define LATCH_CELL 1

#define PREVIOUS 0
#define CURRENT  1
#define FUTURE   2

#define MAX_NAME   29
#define LONGNAME   80
#define NULL_VALUE -4000
#define INPUT_ARRAY_SIZE 100

#define EQNMBR 'N'
#define Q_OUTPUT '>'
#define NO_EQ_CHR 'X'

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

#define OPR_OPRND  "put an operator between operands"
#define MEM_ERR_MSG "not enough memory to create device link map"
#define SEARCH_FOR_STAR "Looking for *"
#define USE_CONSTANT "use index/constant value"
#define USE_DELIMITER "Use ;"
#define USE_COLON "Use :"
#define LIST_HEAD "!!!!"
#define NAME_FILE   "$___part"
#define CONFIG_FILE "$___plpl.cfg"
#define MAP_FILE    "$___map.jed"

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

#ifndef DEF_MAIN
extern FILE *exist();
extern FILE *partsearch();
#endif

STORAGE
struct pair_name_no {
    char devpin_name[MAX_NAME];
        int dev_no;
    struct pair_name_no *pair_next;
};
STORAGE
struct pair_name_no *name_pair_list;

STORAGE
struct node1class {
    char node_item;
    struct node1class *node1next;
};

STORAGE
struct dfn_class {
    char class1_name[MAX_NAME];
    struct node1class *list_val;
    struct dfn_class *nxt_dfn;
};
STORAGE
struct dfn_class *dfn_list;

STORAGE
struct node2class {
    char *symbol;
    int pin_no;
        struct node2class *FWD; /* forward ptr */
        struct node2class *RWD; /* back ptr */
    struct node2class *LEFT_0;
    struct node2class *RIGHT_1;
};

STORAGE   /* stores the equation format structure */
struct EQFORMAT {
    char EQNAME[MAX_NAME];
    int EQWIDTH,EQ_OFFSET;  /* width of equation and an offset value */
    struct EQ_FRM *EQFRM;
    struct EQFORMAT *MORE_EQ;
};
STORAGE
struct EQFORMAT *eq_frm_list;

STORAGE
struct EQ_FRM {
    char EQ_opr;
    int EQ_pin;
        char a_PT_name[MAX_NAME];
    struct EQ_FRM *nxt_EQ;
};
STORAGE
struct EQ_FRM *pt_EQFRM;

STORAGE
struct PT_element {
    char term_name[MAX_NAME];
    unsigned int pinstates;  /* bit 0 = previous state
                    1 = current state
                    2 = future state  */
    struct node2class *pin_eq;
    struct PT_element *next_PT;
};
STORAGE
struct PT_element *product_terms;

/*
Lower nibble specifies STORE_TYPE:
  0 = D    2 = JK
  1 = T    3 = SR
For storage element types 2 and 3, MEM1_ELEMENT
is used for the K or R input term

Upper nibble specifies REG (0) of LATCH (1)
*/
STORAGE
struct mem_element {
    unsigned int store_type;
    struct PT_element *PTreset;
    struct PT_element *PTset;
    struct PT_element *PTpreload;
    struct PT_element *PTobserve;
    struct PT_element *PTclock;
    struct PT_element *PTenable;
    struct PT_element *PTmem0_input;
    struct PT_element *PTmem1_input;
    unsigned int mem_output;  /* stores the output states */
};

STORAGE
struct pin_element {
    int vctr_element;  /* pin/vector element in the part */
    struct dfn_class *poss_values;
    unsigned int pin_status;
    struct mem_element reg_latch;
    struct node2class *out_element;
    struct PT_element *misc_fnc;  /* miscellaneous functions */
    struct pin_element *nxt_tst_pin;
};
STORAGE
struct pin_element *test_pin_elements;

STORAGE
struct nmbr_list {
    int x_pin;
    struct dfn_class *list_features;
    struct nmbr_list *nxt_pin;
};
STORAGE
struct nmbr_list *pin_arch_list;

#ifndef FILE_PIN_SET
extern struct node2class *s_prse_pin();
extern struct node2class *fuse_eval();
extern struct node2class *s_get_name();
extern struct node2class *PTname_eval();
extern struct node2class *new_ND2();
extern struct node2class *nd_eq_parse();
extern struct PT_element *get_PT();
#endif

#ifndef IN_DBSIM
extern struct pin_element *sim_find_pin();
#endif

STORAGE
/* graphics characters to use when drawing the waveforms */
char VECTOR_MARK,SPACE,SPC_VCTR,VERT_BAR,LOW_LEFT,TOP_RIGHT,
     LOW_RIGHT,TOP_LEFT,DOUBLE_VERT,LEFT_LOW_OUT,RIGHT_TOP_OUT,
     RIGHT_LOW_OUT,LEFT_TOP_OUT,IN_FLOAT,OUT_HI_Z,NO_TEST,
     DONT_CARE,CROSS,RIGHT_CROSS,LEFT_CROSS,C_CLOCK,K_CLOCK;
/* - - - - - - - - */
STORAGE
char jmapfile[LONGNAME],db_sim[LONGNAME],
     icon[MAX_NAME],simfile[LONGNAME],eq_file[LONGNAME],
     names_file[LONGNAME],wavefile[LONGNAME],calcfile[LONGNAME],
     vctr_file[LONGNAME],main_msg[LONGNAME],chr;
STORAGE
char source_line[256], token[MAX_NAME], last_token[MAX_NAME];

STORAGE int screen_position, token_type, number_type, line_number,
   the_keyword, lexi_i, end_of_file, end_found, errors,trace,
   get_new_token, num_pins, physical_pins, input_array, dflt_test,
   dflt_names,debug_flag,suppress_msg,accurate;

STORAGE FILE *input_file;
STORAGE FILE *out_file;

/* File pointers
M29FILE   = full JEDEC file from JEDEC file
JEDECFILE = user-supplied JEDEC file
WAVEPTR   = wave pattern file
CALCVCTR  = calculated vectors from user supplied test vector file
*/
STORAGE FILE *m29file,*jedecfile,*waveptr,*calcvctr,*MAP_EQ_file;
STORAGE char *jedec_map;
STORAGE unsigned fuse_count;

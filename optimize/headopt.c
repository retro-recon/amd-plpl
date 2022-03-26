/* ==> Logic Optimizer V2.2 (OPT2)
Oct. 24, 1987 HEADOPT.C contains the MAIN() function. The
logic optimizer will process a file written in the PLPL
intermediate format. Logic equations are logically reduced
one output at a time (may be modified to handle PLAs), and the
number of variables is limited only by the amount of system memory.

This optimizer will now be able to handle equations such as:
  F = -1*2 + 2*4 + 2*-3 + -1*3*-4; where _1*2 is redundant.

The previous optimizer did not handle this case.

OPT2 uses a 2-stage proces: Quine-McCluskey and then a "2nd order"
logic differentiation" step.
*/
#define DEF_MAIN 1
#include "opt2def.h" /* data definition/declaration module */
/*  -   -   -   -   -   -   */
inform(msg,status) /* inform the user of a file error and then exit */
char msg[];
int status;
{
fprintf(stderr,"\n[Message] %s",msg);
switch(status) {
  case NO_FILE   : fprintf(stderr,"\nRestart program\n");
                   exit(BAD_EXIT);
  case STOP      : exit(BAD_EXIT);
  default        : break;
} /* SWITCH */
} /* INFORM */
/*  -   -   -   -   -   -   */
read_command_line(argc,argv)
int argc;
char *argv[];
{ char *ch;

input_file = NULL;
out_file = NULL;
debug_flag = NO;
suppress_msg = NO;
mainlist = NULL;
/* list of all variables in the logic equation being minimized */
varlist = NULL;
tmplist = NULL;
var_number = 0;
while (--argc > 0 && (*++argv)[0] == '-')
  { for (ch = argv[0] + 1; *ch != '\0'; ch++)
      { if (islower(*ch))
      *ch = toupper(*ch);
    switch (*ch)
     { case 'I': argc--;
            if (argc <= 0)
              inform("missing input file\n",STOP);
            strcpy(optsource,*++argv);
            input_file = exist(optsource,"r");
            break;
      case 'O': argc--;
            if (argc <= 0)
              inform("missing output file\n",STOP);
            strcpy(optdest,*++argv);
            out_file = exist(optdest,"w");
                    break;
          case 'Q': suppress_msg = YES;
                    break;
          case 'B': --argc;
                    if (argc <= 0)
                      inform("Debug Value 200",STOP);
                    debug_flag = atoi(*++argv);
                    break;
      default : fprintf(stderr,"optimize:[%c]?\n", *ch);
            exit(BAD_EXIT);
            break;
     } /* SWITCH */
     } /* FOR */
  } /* WHILE */
if (input_file == NULL)
 { inform("Using the Logic Optimizer:",CONTINUE);
   inform("optimize -i <intermediate file>",CONTINUE);
   inform("Options:",CONTINUE);
   inform("\t-o <optimize result file>",CONTINUE);
   inform("\t-q (quiet mode)\n",STOP);
 }
} /* end of READ_COMMAND_LINE */
/*  -   -   -   -   -   -   */
get_index(nmbr_chr)
/* get an index (positive or negative) and return
   the number through NMBR_CHR */
char nmbr_chr[];
{ int i,p;
nmbr_chr[0] = '\0';
lex(icon);
i = HIGH;
if (icon[0] == '-') {
  lex(icon);
  i = LOW;
}
switch(token_type)
 { case NUMBER   : convert('d',icon,nmbr_chr);
                   p = (int)atoi(nmbr_chr);
                   if (i == LOW)
                     return(-p);
                   return(p);
   default       : err(FATAL,"Use numbers",token_type,PUNCT_MARK,NUMBER);
                   return(NULL_VALUE);
 } /* SWITCH */
} /* GET_INDEX */
/*  -   -   -   -   -   -   */
/* Initialize global variables. If ACTION = YES, then
   do list_initialize also; if NO, then skip it. The NO case
   is done when PLD_sw_WRITE() is performed
*/
var_init()
{
end_found = NO;
errors = 0;
get_new_token = YES;
end_of_file = NO;
get_line();  /* read in a source line */
} /* VAR_INIT */
/*  -   -   -   -   -   -   */
/* close all files before ending the simulation and
   free all space used by the linked lists */
close_files()
{
if (input_file != NULL) {
  fclose(input_file);
  input_file = NULL;
}
if (out_file != NULL) {
  fclose(out_file);
  out_file = NULL;
}
} /* CLOSE_FILES */
/*  -   -   -   -   -   -   */
main(argc,argv)
int argc;
char *argv[];
{
fprintf(stderr,"Logic Optimizer Ver 2.21 Dec 1987\n");
read_command_line(argc,argv);
optimize();
close_files();
exit(GOOD_EXIT);
} /* MAIN */

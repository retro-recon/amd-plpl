/* ==> Test Vector Generator (TESTV)
   August 29, 1986: HEADTEST.C contains the MAIN() function. The header
   section containing the part name is read and the device data base is
   accessed. Pin definition and macro definition statements are read
   before looking for the keyword TEST_VECTORS.
*/
#define DEF_MAIN 1   /* var in TESTDEF.H will be defined through this module */
#include "testdef.h" /* data definition/declaration module */
/*  -   -   -   -   -   -   */
struct type3 *find_pin(x_pin)
int x_pin;
{ struct type3 *runner;
  char buf[MAX_NAME];

for (runner = pins; runner != NULL; runner = runner->next_pin)
 { if (runner->pin_nmbr == x_pin)
     return(runner);
 }
sprintf(buf,"Pin [%d] not found\n",x_pin);
fprintf(stderr,"See SRC line:\n%s",source_line);
inform(buf,STOP);
} /* FIND_PIN */
/*  -   -   -   -   -   -   */
inform(msg,status) /* inform the user of a file error and then exit */
char msg[];
int status;
{
fprintf(stderr,"\n[Message] %s",msg);
if (status == STOP)
  exit(BAD_EXIT);
} /* INFORM */
/*  -   -   -   -   -   -   */
FILE *exist(src_name)
/*
check if the file SRC_NAME exists, and if so, query the user to delete,
append to it, or specify a new name. This function will return a file
pointer.
*/
char src_name[];
{ FILE *e_ptr;
  char answer;

if (src_name[0] == '\0')
  file_error("a NULL file",BAD_EXIT);
e_ptr = fopen(src_name,"r");
if (e_ptr == NULL)  /* file does not yet exist */
 { e_ptr = fopen(src_name,"w");
   if (e_ptr == NULL)
     file_error(src_name,BAD_EXIT);
   return(e_ptr);
 }
for (;;)
 { fprintf(stderr,
   "\n[%s] exists\nDelete it, append to it, or specify a new file (d/a/n)?: ",
           src_name);
   if (e_ptr != NULL)
    { fclose(e_ptr);
      e_ptr = NULL;
    }
   answer = getchar();
   if (isalpha(answer)) {
     if (isupper(answer)) {
       answer = tolower(answer);
     }
   }
   getchar();  /* get the <CR> */
   switch(answer)
    { case 'd'
        : e_ptr = fopen(src_name,"w");
          if (e_ptr == NULL)
            file_error(src_name,BAD_EXIT);
          fprintf(stderr,"[%s] deleted\n",src_name);
          return(e_ptr);
      case 'a'
        : e_ptr = fopen(src_name,"a");
          if (e_ptr == NULL)
            file_error(src_name,BAD_EXIT);
          fprintf(stderr,"Will append to [%s]\n",src_name);
          return(e_ptr);
      case 'n'
        : fprintf(stderr,"New file name: ");
          scanf("%s",src_name);
          getchar();  /* get the <CR> */
          return(exist(src_name));
      case '\n'
        : break;
      default
        : while (getchar() != '\n')
           ;
          break;
    } /* SWITCH */
 } /* FOR */
} /* EXIST */
/*  -   -   -   -   -   -   */
read_command_line(argc,argv)
int argc;
char *argv[];
{ char *ch;
input_file = NULL;
out_file = NULL;
testfile[0] = '\0';
dflt_test = 0;
while (--argc > 0 && (*++argv)[0] == '-')
  { for (ch = argv[0] + 1; *ch != '\0'; ch++)
      { if (islower(*ch))
      *ch = toupper(*ch);
    switch (*ch)
     { case 'I' : argc--;
             if (argc > 0)
            { input_file = fopen(*++argv,"r");
              if (input_file == NULL)
                file_error(*argv,BAD_EXIT);
            }
             else
            inform("missing input filename\n",STOP);
             break;
      case 'O' : argc--;
             if (argc > 0)
            { strcpy(testfile,*++argv);
              out_file = exist(testfile);
              if (out_file == NULL)
                file_error(*argv,BAD_EXIT);
            }
             else
            inform("missing test vector output filename\n",STOP);
                     break;
          case 'X': dflt_test = 1;
                    break;
      default : fprintf(stderr,"testv: [%c] ?\n",*ch);
            exit(BAD_EXIT);
            break;
     } /* SWITCH */
     } /* FOR */
  } /* WHILE */
if (input_file == NULL)
 { inform("TESTV usage:",CONTINUE);
   inform("testv -i <input file>",CONTINUE);
   inform("\tother options:",CONTINUE);
   inform("\t-o <vector output file>",CONTINUE);
   inform("\t-x (set X/don't care value to 1)",CONTINUE);
   inform("\tAny errors are written to <vector output file>\n",STOP);
 }
} /* end of GET_FILE_NAME */
/*  -   -   -   -   -   -   */
file_error(xxname,exit_status)
char *xxname;
int exit_status;
{ fprintf(stderr,"\ntestv: cannot open [%s]\n",xxname);
  exit(exit_status);
} /* FILE_ERROR */
/*  -   -   -   -   -   -   */
pin_architecture(p_name,ptype,the_pins)
/* define the pin architecture; e.g., REG, COMB, POLARITY and feedback */
char p_name[];
int ptype;
struct type1 *the_pins;
{ struct type1 *tmp_ptr;

lex(icon);
if (icon[0] != '(')
 { get_new_token = NO;
   return;
 }
for (lex(icon);
     icon[0] != ')' && (the_keyword != BEGIN || the_keyword != DEFINE);
     lex(icon)) /* read until ')' symbol or until BEGIN */
 { for (tmp_ptr = the_pins->typ1_next;
        tmp_ptr != NULL;
        tmp_ptr = tmp_ptr->typ1_next)
    { if (fuse_feature_check(abs(tmp_ptr->pin_num),icon) == NO)
        output_pin_check(icon,abs(tmp_ptr->pin_num));
    } /* FOR tmp_ptr */
 } /* FOR */
if (the_keyword == BEGIN || the_keyword == DEFINE)
  err(WARNING,"expecting ')' to close PIN definition section",
      token_type,KEYWORD,KEYWORD);
} /* PIN_ARCHITECTURE */
/*  -   -   -   -   -   -   */
name_redef_check(name_to_check)
  /* check if the NAME_TO_CHECK has been used already in the pin array */
char name_to_check[];
{ struct type3 *i;
for (i=pins; i != NULL; i = i->next_pin)
 { if (!strcmp(i->pname,name_to_check))
     err(FATAL,"redefinition of pin name",0,0,0);
 } /* FOR */
} /* NAME_REDEF_CHECK */
/*  -   -   -   -   -   -   */
put_in_pin_array(pa_name,a_pin,a_type)
/* store pin name and polarity (HIGH/LOW) */
char pa_name[];
int a_pin,a_type;
{ struct type3 *ptr_list;

a_pin = abs(a_pin);
ptr_list = (struct type3 *)find_pin(a_pin);
if (pa_name[0] != '\0')
 { if (strcmp(ptr_list->pname,"$@"))
     err(FATAL,"redefinition of pin",token_type,WHITE_SPACE,WHITE_SPACE);
   else
    { name_redef_check(pa_name);  /* check for pin name redefinition */
      strcpy(ptr_list->pname,pa_name);
      ptr_list->pname_type = a_type;
      switch(ptr_list->pin_type)
       { case OUTPUT:
         case IO:
         case BREG: if ((ptr_list->polarity != a_type) &&
                        (ptr_list->polarity != PROGRAMMABLE))
                      err(NOTICE,"fixed polarity",1,0,0);
                    break;
         default: break;
       } /* SWITCH */
      switch(a_type)
       { case HIGH: case LOW: break;
         case PROGRAMMABLE:
         default: err(FATAL,"polarity must be HIGH or LOW",0,0,0);
       } /* SWITCH */
      ptr_list->polarity = a_type;
    }
 } /* IF */
} /* PUT_IN_PIN_ARRAY */
/*  -   -   -   -   -   -   */
vector_parse(vpname,pin_pol)  /* variable name */
char vpname[];
int pin_pol; /* pin polarity */
{ int count_a,count_b,pin_r1,pin_r2,dir,dir_pin,count;
  char chr_number[MAX_NAME],vname[MAX_NAME],valbuf[MAX_NAME];
   /* PIN_R1/2 are the pin ranges */

count_a = get_index(valbuf);
lex(icon);
switch(icon[0])
 { case ']' : count_b = count_a;
              break;
   case ':' : count_b = get_index(valbuf);
              lex(icon);
              if (icon[0] != ']')
                err(FATAL,"close vector specification with ']'",
                    token_type,PUNCT_MARK,NUMBER);
              break;
   default  : err(FATAL,"put ':' or ']' here",token_type,VARIABLE,NUMBER);
              return;
 } /* SWITCH */
if (count_a >= count_b)
   dir = -1;
else
   dir = 1;
lex(icon);
if (icon[0] != '=')
  err(FATAL,"put '=' between pin vector and number",
      token_type,NUMBER,PUNCT_MARK);
for (; count_a != count_b + dir ;)
 { count = get_index(valbuf);
   switch(icon[0])
    { case ';' : err(FATAL,"vector range-pin mismatch",token_type,NUMBER,
                     WHITE_SPACE);
                 get_new_token = NO;
                 return;
    } /* SWITCH */
   if (count > 80)
     err(FATAL,"unknown pin",0,0,0);
   lex(icon);
   switch(icon[0])
    { case '(' :
      case '/' :
      case ';' : get_new_token = NO;
      case ',' : strcpy(vname,vpname);
                 strcat(vname,"[");
                 sprintf(chr_number,"%d",count_a);
                 strcat(vname,chr_number);
                 strcat(vname,"]");
                 push(vctr2,'#',count,(struct type1 *)NULL);
                 put_in_pin_array(vname,count,pin_pol);
                 count_a = count_a + dir;
                 break;
      case ':' : pin_r1 = count;
                 pin_r2 = get_index(valbuf);
                 if (pin_r2 > 80)
                    err(FATAL,"unknown pin",0,0,0);
                 if (pin_r1 >= pin_r2)
                    dir_pin = -1;
                 else
                    dir_pin = 1;
                 for (; pin_r1 != pin_r2 + dir_pin ;pin_r1 = pin_r1 + dir_pin)
                   { strcpy(vname,vpname);
                     strcat(vname,"[");
                     if (!(count_a != count_b + dir))
                       { err(FATAL,"vector-pin mismatch",
                             token_type,VARIABLE,WHITE_SPACE);
                         break;
                       }
                     sprintf(chr_number,"%d",count_a);
                     strcat(vname,chr_number);
                     strcat(vname,"]");
                     push(vctr2,'#',pin_r1,(struct type1 *)NULL);
                     put_in_pin_array(vname,pin_r1,pin_pol);
                     count_a = count_a + dir;
                   } /* FOR */
                 break;
      default  : if (token_type != VARIABLE && token_type != NUMBER)
                   err(FATAL,"use ',' or ':' to specify pin range",
                       token_type,NUMBER,VARIABLE);
                 else
                  { get_new_token = NO;
                    strcpy(vname,vpname);
                    strcat(vname,"[");
                    sprintf(chr_number,"%d",count_a);
                    strcat(vname,chr_number);
                    strcat(vname,"]");
                    push(vctr2,'#',count,(struct type1 *)NULL);
                    put_in_pin_array(vname,count,pin_pol);
                    count_a = count_a + dir;
                  }
                 break;
    } /* SWITCH */
  lex(icon);             /* +1 */
  if (icon[0] != ',')
    get_new_token = NO;
 } /* FOR count_a */
pin_architecture(vname,VCTR,vctr2);  /* LEX(ICON) in +1 provides icon */
clean_list(vctr2->typ1_next);
vctr2->typ1_next = NULL;
} /* VECTOR_PARSE */
/*  -   -   -   -   -   -   */
scalar_parse(sname,pin_pol) /* scalar name */
char sname[];
int pin_pol;
{ int x;
  char tmpbuf[MAX_NAME];
if (icon[0] != '=')
  err(FATAL,"put '=' between pin name and number",
      token_type,NUMBER,PUNCT_MARK);
lex(icon);
if (token_type != NUMBER)
  err(FATAL,"assign a pin number",token_type,VARIABLE,PUNCT_MARK);
else
 { convert('d',icon,tmpbuf);
   x = (int)atoi(tmpbuf);
 }
push(vctr2,'#',x,(struct type1 *)NULL);
put_in_pin_array(sname,x,pin_pol);
pin_architecture(sname,SCALR,vctr2);
clean_list(vctr2->typ1_next);
vctr2->typ1_next = NULL;
} /* SCALAR_PARSE */
/*  -   -   -   -   -   -   */
parse_pin()
{ char var_name[MAX_NAME];
  int polarity;  /* indicates if vector/pin is active HIGH or LOW */
lex(icon);
if (strcmp(icon,"pin"))
  err(FATAL,"pins must be defined",token_type,VARIABLE,PUNCT_MARK);
for (lex(icon), var_name[0] = '\0', polarity = HIGH;
     icon[0] != ';';
     var_name[0] = '\0',lex(icon), polarity = HIGH)
 { if (icon[0] == '/')
    { polarity = LOW;
      lex(icon);
    }
   if (token_type != VARIABLE)
      err(FATAL,"need a variable name",token_type,NUMBER,VARIABLE);
   else
      { strncpy(var_name,icon,MAX_NAME-1);
        var_name[MAX_NAME-1] = '\0';
      }
   lex(icon);
   if (icon[0] == '[')
     vector_parse(var_name,polarity);
   else
     scalar_parse(var_name,polarity);
 } /* FOR */
} /* PARSE_PIN */
/*  -   -   -   -   -   -   */
name_check(name0,nbuf,name_type)
 /* check if NAME0 is in the PINS array;
    if NAME0 exists, then return the int value FUSE_NUMBERS
    thru NBUF */
char name0[];
char nbuf[];
int *name_type;
{ struct type3 *i;

*name_type = NULL_VALUE;
strcpy(nbuf,name0);
for (i = pins; i != NULL ; i = i->next_pin)  /* check PIN list */
 { if (!strcmp(i->pname,name0))
    { if (i->pname_type == HIGH)
        sprintf(nbuf,"%d",i->pin_nmbr);
      else
        sprintf(nbuf,"%d",-(i->pin_nmbr));
      *name_type = PIN;
      return(EXIST);
    }
 } /* FOR */
return(NOT_EXIST);
} /* NAME_CHECK */
/*  -   -   -   -   -   -   */
get_index(nmbr_chr)
/* get an index and return the number through NMBR_CHR */
char nmbr_chr[];
{ char tmp[MAX_NAME];
  int nmbr_type;
nmbr_chr[0] = '\0';
lex(icon);
switch(token_type)
 { case NUMBER   : convert('d',icon,nmbr_chr);
                   return((int)atoi(nmbr_chr));
   case VARIABLE : switch(name_check(icon,tmp,&nmbr_type))
                    { case NOT_EXIST
                            : err(FATAL,"index does not exist",
                                  token_type,PUNCT_MARK,NUMBER);
                              break;
                      case EXIST
                            : err(FATAL,"this is a pin number",
                                  token_type,NUMBER,PUNCT_MARK);
                              convert('d',tmp,nmbr_chr);
                              return((int)atoi(nmbr_chr));
                              break;
                    } /* SWITCH */
                   break;
   default       : err(FATAL,"need an index/number or constant value",
                       token_type,PUNCT_MARK,NUMBER);
                   return(NULL_VALUE);
 } /* SWITCH */
return(NULL_VALUE);
} /* GET_INDEX */
/*  -   -   -   -   -   -   */
expand_vctr(vname,range_a,range_b)
/* expand the vector specification for VNAME from range RANGE_A
   to RANGE_B and put the expanded vector into the list VCTR2 */
char vname[];
int range_a, range_b;
{ int dir;  /* DIR = direction of vector range */
  int x_a,x_b,tempval,info_type;
  char buf1[MAX_NAME],buf2[MAX_NAME];

if (range_a >= range_b)
  dir = -1;
else
  dir = 1;
x_a = range_a;
x_b = range_b;
for (strcpy(buf1,vname); x_a != x_b + dir; strcpy(buf1,vname))
 { sprintf(buf2,"%d",x_a);
   strcat(buf1,buf2);
   strcat(buf1,"]");
   switch(name_check(buf1,buf2,&info_type))
    { case EXIST : if (info_type == MACRO)
                     err(FATAL,"not a pin name",
                         token_type,PUNCT_MARK,WHITE_SPACE);
                   else
                    { tempval = (int)atoi(buf2);
                      push(vctr2,'#',tempval,(struct type1 *)NULL);
                    }
                   break;
      default    : err(FATAL,
                       "vector range mismatch; or put a pin number here",
                       token_type,KEYWORD,KEYWORD);
                   break;
    } /* SWITCH */
   x_a = x_a + dir;
 } /* FOR */
lex(icon);
if (icon[0] != ']')
  err(FATAL,"use ']' to close vector specification",
      token_type,VARIABLE,NUMBER);
} /* EXPAND_VCTR */
/*  -   -   -   -   -   -   */
check_name(name,buf,val)
  /* check if var NAME is vector/scalar in PIN array
     or if it is a number; return the variable name/number in BUF */
char name[],buf[],val[];
{ char tmp2[MAX_NAME],tmp3[MAX_NAME];
  int status,index_a,index_b,temp_int;

strcpy(buf,name);
switch(token_type)
 { case NUMBER :
   case VARIABLE
               : break;
   default     : err(FATAL,"use vector/scalar name or number",
                     token_type,PUNCT_MARK,NUMBER);
                 return(NULL_VALUE);
 } /* SWITCH */
if (token_type == NUMBER)
 { convert('d',icon,val);
   return(NUMBER);
 }
lex(icon);
switch(icon[0])
 { case '['
      : strcat(buf,"[");
        index_a = get_index(tmp2);
        if (index_a == NULL_VALUE)
          err(FATAL,"invalid vector range",
              token_type,VARIABLE,NUMBER);
        lex(icon);
        switch(icon[0])
         { case ']' : strcat(buf,tmp2);
                      strcat(buf,"]");
                      break;
           case ':' : index_b = get_index(tmp3);
                      if (index_b == NULL_VALUE)
                        err(FATAL,"invalid vector range",
                            token_type,VARIABLE,NUMBER);
                      expand_vctr(buf,index_a,index_b);
                      return(VCTR);
           default  : err(FATAL,
                          "expecting ']'",token_type,PUNCT_MARK,VARIABLE);
         } /* SWITCH */
        break;
    default
       : get_new_token = NO;
         break;
 } /* SWITCH icon[0] */
status = name_check(buf,val,&temp_int);
return(status);
} /* CHECK_NAME */
/*  -   -   -   -   -   -   */
parse_part(part)  /* find the partname and then read the database */
char part[];
{ int i,b,prev_status;
  char tmp_src[256];

part_file = fopen(part,"r");
if (part_file == NULL)
  return(NO);
b = lexi_i;
i = line_number;  /* see MAP.23 for explanation */
line_number = 0;
prev_status = get_new_token;
get_new_token = YES;
strcpy(tmp_src,source_line);
read_part();
get_new_token = prev_status;
line_number = i;
lexi_i = b;
strcpy(source_line,tmp_src);
} /* PARSE_PART */
/*  -   -   -   -   -   -   */
findpart(a_part)
/* find the configuration file CONFIG_FILE to get the database location */
char a_part[];
{ FILE *findptr;
  char findbuf[80];
findptr = fopen(CONFIG_FILE,"r");
if (findptr == NULL)
  return;
fscanf(findptr,"%s",findbuf);
strcat(findbuf,a_part);
strcpy(a_part,findbuf);
fclose(findptr);
findptr = NULL;
} /* FINDPART */
/*  -   -   -   -   -   -   */
parse_header() /* find the part type and read the associated data base */
{ char partbuf[80],tmp1string[80],tmp2string[80];
  FILE *pathptr;
  int newpath;
lex(icon); /* get a token */
if (strcmp(icon,"device"))
  err(FATAL,"Looking for keyword DEVICE",token_type,VARIABLE,PUNCT_MARK);
strcpy(spec_name,"no_title");
lex(icon);
if (token_type == VARIABLE)
 { strncpy(spec_name,icon,MAX_NAME-1);
   spec_name[MAX_NAME-1] = '\0';
   lex(icon);
 }
if (icon[0] != '(')
  err(FATAL,"'(' expected",token_type,VARIABLE,NUMBER);
lex(icon);
strcpy(part_name,icon);
strcpy(partbuf,icon);
findpart(partbuf);
for (newpath = NO; parse_part(partbuf) == NO;)
 { fprintf(stderr,"Part [%s] not found; specify path: ",partbuf);
   scanf("%s",tmp1string);
   strcpy(tmp2string,tmp1string);
   strcat(tmp1string,part_name);
   strcpy(partbuf,tmp1string);
   newpath = YES;
 } /* FOR */
if (newpath == YES)
 { pathptr = fopen(CONFIG_FILE,"w"); /* rewrite the cfg file */
   if (pathptr != NULL)
    { fprintf(pathptr,"%s",tmp2string);
      fclose(pathptr);
      pathptr = NULL;
    }
 } /* IF */
lex(icon);
if (icon[0] != ')')
  err(FATAL,"')' expected",token_type,KEYWORD,PUNCT_MARK);
if (part_file != NULL)
 { fclose(part_file);
   part_file = NULL;
 }
fprintf(stderr,"\nReading design file [%s]\n",spec_name);
parse_pin();
} /* PARSE_HEADER */
/*  -   -   -   -   -   -   */
var_init() /* initialize certain global variables */
{
list_initialize(); /* in VCTRFUNC.C module */
spec_name[0] = '\0';
end_found = NO;
line_number = 0;
errors = 0;
get_new_token = YES;
end_of_file = NO;
get_line();  /* read in a source line */
} /* VAR_INIT */
/*  -   -   -   -   -   -   */
main(argc,argv)
int argc;
char *argv[];
{
fprintf(stderr,"JEDEC Test Vector Generator TESTV Ver 2.2 Oct 1987\n");
read_command_line(argc,argv);
var_init(); /* initialize data structures: lists, arrays, etc. */
parse_header();
testvector_parse();
} /* MAIN */

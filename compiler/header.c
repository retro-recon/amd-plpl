/* ==> Generic Logic Software (GLS)
   April 23, 1986 : HEADER.C contains the MAIN() function. The header
   section containing the part name is read, and the device data base
   is accessed. Pin definition and macro definition statements are read
   before compiling the statements
*/
#define DEF_MAIN 1   /* var in GLSDEF.H will be defined through this module */
#include "glsdef.h"  /* data definition/declaration module */
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
/* find the pin PIN_X in the list PINS and return a pointer to it */
struct type2 *find_pin(pin_x)
int pin_x;
{ struct type2 *tmp;

for (tmp = pins; tmp != NULL; tmp = tmp->typ2_next)
 { if (tmp->pin_nmbr == pin_x)
     return(tmp);
 } /* FOR */
fprintf(stderr,"Pin [%d] not found\n",pin_x);
fprintf(stderr,"Src line [%d]:\n%s",line_number,source_line);
exit(BAD_EXIT);
} /* FIND_PIN */
/*  -   -   -   -   -   -   */
FILE *exist(src_name)
/* check if the file SRC_NAME exists, and if so, query the user to delete or
  specify a new name. This function will return a file pointer */
char src_name[];
{ FILE *e_ptr;
  char answer;

if (src_name[0] == '\0')
  file_error("NULL file",BAD_EXIT);
e_ptr = fopen(src_name,"r");
if (e_ptr == NULL)  /* file does not yet exist */
 { e_ptr = fopen(src_name,"w");
   if (e_ptr == NULL)
     file_error(src_name,BAD_EXIT);
   return(e_ptr);
 }
for (;;)
 { fprintf(stderr,
  "\n[%s] exists\nDelete file, append to it, or specify a new file (d/a/n)?: ",
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
          fprintf(stderr,"[%s] deleted",src_name);
          return(e_ptr);
      case 'a'
        : e_ptr = fopen(src_name,"a");
          if (e_ptr == NULL)
            file_error(src_name,BAD_EXIT);
          fprintf(stderr,"Will append to [%s]",src_name);
          return(e_ptr);
      case 'n'
        : fprintf(stderr,"New file name: ");
          scanf("%s",src_name);
          getchar();  /* get the <CR> */
          return(exist(src_name));
    } /* SWITCH */
 } /* FOR */
} /* EXIST */
/*  -   -   -   -   -   -   */
read_command_line(argc,argv)
int argc;
char *argv[];
{ char *ch;
debug_flag = NO;
show_parentheses = NO;
write_part = NO;
input_file = NULL;
out_file = NULL;
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
            inform("missing input file\n",STOP);
             break;
      case 'O' : argc--;
             if (argc > 0)
            { strcpy(int_file,*++argv);
              out_file = exist(int_file);
              if (out_file == NULL)
                file_error(*argv,BAD_EXIT);
                          fclose(out_file);
                          out_file = fopen(TMP_FILE,"w");
              if (out_file == NULL)
                file_error(TMP_FILE,BAD_EXIT);
            }
             else
            inform("missing output file\n",STOP);
                     break;
      case 'B' : debug_flag = YES;
             break;
      case 'P' : write_part = YES;
             break;
          case 'X' : show_parentheses = YES;
                     break;
          case 'Z' : fprintf(stderr,"\nVersion 2.201\n");
                     break;
      default : fprintf(stderr,"\nplc:[%c] ?\n",*ch);
            exit(BAD_EXIT);
            break;
     } /* SWITCH */
     } /* FOR */
  } /* WHILE */
if (input_file == NULL)
 { inform("PLC usage:",CONTINUE);
   inform("plc -i <input file> -o <output file>",CONTINUE);
   inform("\tother option: -b  (show source line being compiled)",CONTINUE);
   inform("\tErrors are written to [$tmp.$$$]\n",STOP);
 }
} /* end of GET_FILE_NAME */
/*  -   -   -   -   -   -   */
file_error(xxname,exit_status)
char *xxname;
int exit_status;
{ fprintf(stderr,"\nplc: file error [%s]\n",xxname);
  exit(exit_status);
} /* FILE_ERROR */
/*  -   -   -   -   -   -   */
pin_architecture(p_name,ptype,the_pins)
/* define the pin architecture; e.g., REG, COMB, POLARITY and feedback */
char p_name[];
int ptype;
struct type1 *the_pins;
{ struct type1 *tmp_ptr;
  char buf[MAX_NAME];
  struct type2 *tmpx;

lex(icon);
if (icon[0] != '(')
 { get_new_token = NO;
   return;
 }
for (lex(icon);
     icon[0] != ')' && (the_keyword != BEGIN || the_keyword != DEFINE);
     lex(icon)) /* read until the ')' symbol or until BEGIN */
 { for (tmp_ptr = the_pins->typ1_next;
        tmp_ptr != NULL;
        tmp_ptr = tmp_ptr->typ1_next)
    { buf[0] = '!';
      buf[1] = '\0';  /* STRCAT will attach ICON from this char position */
      strcat(buf,icon);
      tmpx = (struct type2 *)find_pin(abs(tmp_ptr->pin_num));
      insert(tmpx->special,(struct type1 *)NULL,(struct type1 *)NULL,
             buf,NULL_VALUE,'%',HIGH);
    } /* FOR tmp_ptr */
 } /* FOR */
if (the_keyword == BEGIN || the_keyword == DEFINE)
  err(WARNING,"use ) to close PIN definition",
      token_type,KEYWORD,KEYWORD);
} /* PIN_ARCHITECTURE */
/*  -   -   -   -   -   -   */
name_redef_check(name_to_check)
  /* check if the NAME_TO_CHECK has been used already in the pin array */
char name_to_check[];
{ struct type2 *i;
for (i=pins; i!= NULL; i = i->typ2_next)
 { if (!strcmp(i->pname,name_to_check))
     err(FATAL,"redefinition of pin name",0,0,0);
 } /* FOR */
} /* NAME_REDEF_CHECK */
/*  -   -   -   -   -   -   */
put_in_pin_array(pa_name,a_pin,a_type)
/* store pin name and polarity (HIGH/LOW) */
char pa_name[];
int a_pin,a_type;
{ struct type2 *tmpx1;
if (pa_name[0] != '\0')
 { for (tmpx1 = pins; tmpx1 != NULL; tmpx1 = tmpx1->typ2_next)
    { if (tmpx1->pin_nmbr == a_pin)
        err(FATAL,"pin redefinition",token_type,WHITE_SPACE,WHITE_SPACE);
    } /* FOR */
   name_redef_check(pa_name);  /* check for pin name redefinition */
   new_pin(a_pin);
   tmpx1 = (struct type2 *)find_pin(a_pin);
   strcpy(tmpx1->pname,pa_name);
   tmpx1->pname_type = a_type;
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
                err(FATAL,"close vector specification with ]",
                    token_type,PUNCT_MARK,NUMBER);
              break;
   default  : err(FATAL,"use : or ]",token_type,VARIABLE,NUMBER);
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
                 if (pin_r2 > 36)
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
                   err(FATAL,"use , or : to specify pin range",
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
clean_list((struct type0 *)NULL,vctr2->typ1_next);
vctr2->typ1_next = NULL;
} /* VECTOR_PARSE */
/*  -   -   -   -   -   -   */
scalar_parse(sname,pin_pol) /* scalar name */
char sname[];
int pin_pol;
{ int x;
  char tmpbuf[MAX_NAME];
if (icon[0] != '=')
  err(FATAL,"put = between pin name and number",
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
clean_list((struct type0 *)NULL,vctr2->typ1_next);
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
 /* check if NAME0 is in the PINS array or in the MACRO_LIST;
    if NAME0 exists, then return the int value FUSE_NUMBERS
    in MACRO_LIST thru NBUF */
char name0[];
char nbuf[];
int *name_type;
{ struct type2 *i;
*name_type = NULL_VALUE;
strcpy(nbuf,name0);
for (i = pins ; i != NULL ; i = i->typ2_next)  /* check PIN array */
 { if (!strcmp(i->pname,name0))
    { if (i->pname_type == HIGH)
        sprintf(nbuf,"%d",i->pin_nmbr);
      else
        sprintf(nbuf,"%d",-(i->pin_nmbr));
      *name_type = PIN;
      return(EXIST);
    }
 } /* FOR */
for (typ0_temp = macro_list; typ0_temp != NULL;  /* check MACRO list */
     typ0_temp = typ0_temp->typ0_next)
 { if (!strcmp(typ0_temp->fn_name,name0))
     { *name_type = MACRO;
       if (typ0_temp->fuse_numbers != NULL_VALUE)
         sprintf(nbuf,"%d",typ0_temp->fuse_numbers);
       if (typ0_temp->pterms != NULL)
         return(MACRO);
       else
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
                            : err(FATAL,"nonexistent index",
                                  token_type,PUNCT_MARK,NUMBER);
                              break;
                      case MACRO
                            : if (typ0_temp->pterms != NULL)
                               { if (typ0_temp->pterms->rtptr != NULL ||
                                     typ0_temp->pterms->ltptr != NULL ||
                                     typ0_temp->pterms->operator != '$')
                                   err(FATAL,"macro logic expr",
                                       token_type,NUMBER,PUNCT_MARK);
                                 else
                                  { sprintf(nmbr_chr,"%d",
                                        typ0_temp->pterms->pin_num);
                                    return(typ0_temp->pterms->pin_num);
                                  }
                               } /* IF */
                              else
                               { convert('d',tmp,nmbr_chr);
                                 return((int)atoi(nmbr_chr));
                               }
                              break;
                      case EXIST
                            : err(FATAL,"pin number",
                                  token_type,NUMBER,PUNCT_MARK);
                              convert('d',tmp,nmbr_chr);
                              return((int)atoi(nmbr_chr));
                    } /* SWITCH */
                   break;
   default       : err(FATAL,"use index/number/constant",
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
      default    : err(FATAL,"vector range mismatch; or put pin number",
                       token_type,KEYWORD,KEYWORD);
                   break;
    } /* SWITCH */
   x_a = x_a + dir;
 } /* FOR */
lex(icon);
if (icon[0] != ']')
  err(FATAL,"use ] to close vector specification",
      token_type,VARIABLE,NUMBER);
} /* EXPAND_VCTR */
/*  -   -   -   -   -   -   */
check_name(name,buf,val)
  /* check if var NAME is vector/scalar in PIN array or MACRO_LIST
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
          err(FATAL,"invalid vector range",token_type,VARIABLE,NUMBER);
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
                          "expecting ]",token_type,PUNCT_MARK,VARIABLE);
         } /* SWITCH */
        break;
    case '('
       : if (name_check(buf,val,&temp_int) != NOT_EXIST)
          { err(FATAL,"pin/macro name cannot be used as function",
                token_type,VARIABLE,KEYWORD);
            return(NULL_VALUE);
          }
         get_new_token = NO; /* use the '(' icon again */
         parse_case(func_list,FUNCTION);
         return(FUNCTION);
    default
       : get_new_token = NO;
         break;
 } /* SWITCH icon[0] */
status = name_check(buf,val,&temp_int);
return(status);
} /* CHECK_NAME */
/*  -   -   -   -   -   -   */
parse_part(part)
/* this section can be expanded by the user; e.g., read the part to make
   the compiler do more device-feature checking */
char part[];
{
strcpy(part_name,part);
lex(icon);
if (icon[0] != ')')
  err(FATAL,") expected",token_type,KEYWORD,PUNCT_MARK);
} /* PARSE_PART */
/*  -   -   -   -   -   -   */
/* write this part name to the file NAME_FILE; this will
   be used by other programs in the PLPL V2.2 package
*/
send_part(a_part_name)
char a_part_name[];
{ FILE *tmp_file;

tmp_file = fopen(NAME_FILE,"w");
if (tmp_file == NULL)
  err(FATAL,"Cannot open name file",0,0,0);
fprintf(tmp_file,"%s",a_part_name);
fclose(tmp_file);
tmp_file = NULL;
} /* SEND_PART */
/*  -   -   -   -   -   -   */
parse_header() /* find the part type and read the associated data base */
{
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
  err(FATAL,"( expected",token_type,VARIABLE,NUMBER);
lex(icon);
if (write_part == YES)
  send_part(icon);
parse_part(icon);
fprintf(stderr,"\nCompiling PLD file : [%s]\n",spec_name);
parse_pin();
parse_define();
} /* PARSE_HEADER */
/*  -   -   -   -   -   -   */
var_init() /* initialize certain global variables */
{
list_initialize(); /* in EQ module */
spec_name[0] = '\0';
end_found = NO;
line_number = 0;
errors = 0;
get_new_token = YES;
end_of_file = NO;
level = 0;
get_line();  /* read in a source line */
} /* VAR_INIT */
/*  -   -   -   -   -   -   */
main(argc,argv)
int argc;
char *argv[];
{
fprintf(stderr,"\nLogic Compiler PLC Ver 2.2 Oct 1987");
read_command_line(argc,argv);
var_init(); /* initialize data structures: lists, arrays, etc. */
parse_header();
parse_main();
/* fix the output file and convert to an intermediate file format */
if (int_file[0] != '\0')
  fix(TMP_FILE,int_file);
} /* MAIN */

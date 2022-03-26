/* [May 15, 1986] : STATEMNT.C contains the functions and routines that
   parse the high level logic expressions. Expressions presently supported
   include IF-THEN-ELSE and CASE
Portability Considerations:
  VAX/VMS will need the include statement [#include math]
  and VAX/UNIX will need [#include <math.h>] to handle the function POW(x,y)
  This is in the include file GLSDEF.H */
#include "glsdef.h"
/*   -   -   -   -   -   */
function(func_name)
char func_name[];
{ struct type1 *ptr,*func_ptr;
  char buf[MAX_NAME];
  struct type2 *ptr_2;

buf[0] = '@';
buf[1] = '\0';
strcat(buf,func_name);
lex(icon);
switch(icon[0])
 { case ';'
     : get_new_token = NO;
       push(oprnd,'$',1,(struct type1 *)NULL);  /* push the constant 1 */
       func_ptr = oprnd->typ1_next;
       oprnd->typ1_next = NULL;
       for (ptr = func_list->typ1_next;
            ptr != NULL;
            ptr = ptr->typ1_next)
        { if (ptr->operator == '#')
           { copytree(func_ptr,typ1_entry,'t');
             oprnd->typ1_next = typ1_entry;
             typ1_entry->backptr = oprnd;
             ptr_2 = (struct type2 *)find_pin(abs(ptr->pin_num));
             add_cond(ptr_2->special,(struct type1 *)NULL,buf,
                      VCTR,abs(ptr->pin_num));
           }
        } /* FOR */
       oprnd->typ1_next = func_ptr;
       clean_list((struct type0 *)NULL,oprnd->typ1_next);
       oprnd->typ1_next = NULL;
       out_logic_equations();
       free_space();
       break;
   case ':'
     :
   case '='
     : get_new_token = NO;
       get_equal_sign();
       logic_expression(buf,(struct type0 *)NULL,(struct type1 *)NULL,
                        HIGH,FUNCTION,func_list);
       oprnd->typ1_next = NULL;
       break;
   default
     : err(FATAL,"put ';' or assign expression to function",
           token_type,KEYWORD,PUNCT_MARK);
       break;
 } /* SWITCH */
lex(icon);
if (icon[0] != ';')
  err(FATAL,"terminate logic expression with ';'",
      token_type,VARIABLE,KEYWORD);
clean_list((struct type0 *)NULL,func_list->typ1_next);
func_list->typ1_next = NULL;
} /* FUNCTION */
/*   -   -   -   -   -   */
equation(signal,high)  /* parse the equation */
char signal[];
int high;
{ char buf1[MAX_NAME],buf2[MAX_NAME],outname[MAX_NAME];
  int outpin,typ_pin,t;
  struct type1 *eq_ptr;
  struct type2 *ptr_2a;

outpin = 0;
typ_pin = PIN;
outname[0] = '\0';  /* initialize the output signal name to NULL */
switch(check_name(signal,buf1,buf2))
 { case EXIST
     : strcpy(outname,buf1);
       outpin = (int)atoi(buf2);
       break;
   case FUNCTION
     : function(buf1);
       return;
   case VCTR
     : pin_vctr->typ1_next = vctr2->typ1_next;
       vctr2->typ1_next->backptr = pin_vctr;
       vctr2->typ1_next = NULL;
       if (high == NO)
        { for (eq_ptr = pin_vctr->typ1_next;
               eq_ptr != NULL;
               eq_ptr = eq_ptr->typ1_next)
           { if (eq_ptr->pin_num == NULL_VALUE)
               inform("EQ_PTR ptr to vctr list root\n",STOP);
             t = abs(eq_ptr->pin_num);
             ptr_2a = (struct type2 *)find_pin(t);
             push(ptr_2a->oprtr_stk,'/',OPERATOR,(struct type1 *)NULL);
             push(ptr_2a->oprtr_stk,'(',OPERATOR,(struct type1 *)NULL);
           } /* FOR */
          clean_list((struct type0 *)NULL,oprtr->typ1_next);
          oprtr->typ1_next = NULL;
        } /* IF */
       strcpy(outname,buf1);
       typ_pin = VCTR;
       break;
   case NOT_EXIST
     : err(FATAL,"undefined variable",token_type,PUNCT_MARK,KEYWORD);
       break;
   case MACRO
     : if (typ0_temp->pterms->rtptr != NULL &&
           typ0_temp->pterms->ltptr != NULL &&
           typ0_temp->pterms->operator != '#')
          err(FATAL,"cannot use macro on left side of statement",
              token_type,PUNCT_MARK,NUMBER);
       else
        { ptr_2a = (struct type2 *)find_pin(abs(typ0_temp->pterms->pin_num));
          strcpy(outname,ptr_2a->pname);
          outpin = typ0_temp->pterms->pin_num;
        }
       break;
   case NUMBER
     : err(FATAL,"cannot assign expressions to numbers",token_type,
           PUNCT_MARK,VARIABLE);
       break;
   default
     : inform("EQUATION error\n",STOP);
       break;
 } /* SWITCH */
get_equal_sign();
if (outpin == 0)
  ptr_2a = pins;
else
  ptr_2a = (struct type2 *)find_pin(abs(outpin));
logic_expression(outname,(struct type0 *)NULL,ptr_2a->pt_list,
                 high,typ_pin,pin_vctr);
oprnd->typ1_next = NULL;
lex(icon);
if (icon[0] != ';')
  err(FATAL,"terminate logic expression with ';'",
      token_type,VARIABLE,KEYWORD);
clean_list((struct type0 *)NULL,pin_vctr->typ1_next);
pin_vctr->typ1_next = NULL;
} /* EQUATION */
/*   -   -   -   -   -   */
eval_cond_logic(eval_list,target_list)
 /* evaluate the condition logic expression and
    add this to the condition list COND_LIST */
struct type1 *eval_list, *target_list;
{ struct type1 *ptr1;

for (ptr1 = eval_list; ptr1 != NULL; ptr1 = ptr1->typ1_next)
 { switch(ptr1->operator)
     { case '/' :
       case '(' :
       case '*' :
       case '%' :
       case '+' : if (ptr1->rtptr == NULL || ptr1->ltptr == NULL)
                    push(oprtr,ptr1->operator,OPERATOR,(struct type1 *)NULL);
                  else
                    { copytree(ptr1,typ1_entry,'t');
                      push(oprnd,'#',NULL_VALUE,typ1_entry);
                      eval(PAIR);
                    } /* ELSE */
                  break;
       case ',' : push(oprtr,'*',OPERATOR,(struct type1 *)NULL);
                  break;
       case ')' : eval(LBRACKET);
                  break;
       case '#' : push(oprnd,'#',ptr1->pin_num,(struct type1 *)NULL);
                  eval(PAIR);
                  break;
       case '?' : break; /* skip the list head */
       default  : inform("EVALCONDLOG Error\n",STOP);
                  break;
     } /* SWITCH */
 } /* FOR */
insert((struct type0 *)NULL,target_list,oprnd->typ1_next,
       "CONDITION",NULL_VALUE,'#',NEXT);
oprnd->typ1_next = NULL;
} /* EVAL_COND_LOGIC */
/*   -   -   -   -   -   */
cond_eval(alist)
 /* evaluate condition and insert into COND_LIST: See GLS.TXT for details */
struct type1 *alist;
{ struct type1 *aptr,*point1,*point2;
  int eq_sym,dimension,k,next_item;
  double x,y;
  char buf[MAX_NAME];

for (next_item = YES,aptr = alist,eq_sym = NO; aptr != NULL;next_item = YES)
 { switch(aptr->operator)
    { case '(' :
      case ')' :
      case '?' : break;
      case '$' : if (eq_sym == YES)
                  { for (dimension = 0,point1 = aptr->backptr,point2 = aptr;
                         point1->backptr != NULL && point1->operator != '(';
                         point1 = point1->backptr)
                     { switch(point1->operator)
                        { case '#'   /* if pin number */
                             : ++dimension;
                               break;
                          case '+'
                             : err(FATAL,
                                   "do not use '+' when comparing numbers",
                                   token_type,KEYWORD,KEYWORD);
                               break;
                          case '*'
                             : if (point1->rtptr != NULL ||
                                   point1->ltptr != NULL)
                                 err(FATAL,
                                     "cannot compare macros with constants",
                                     token_type,KEYWORD,KEYWORD);
                               break;
                          case '$'
                             : err(FATAL,
                                   "cannot test constants with constants",
                                   token_type,KEYWORD,KEYWORD);
                               break;
                        } /* SWITCH */
                     } /* FOR */
                    if (dimension > 16)
                      err(FATAL,"compare 16 or fewer variables only",
                          token_type,KEYWORD,KEYWORD);
                    x = 2;
                    y = (double)dimension; /* cast the int as a double */
                    if (pow(x,y) <= (double)(aptr->pin_num))
                      err(FATAL,"number cannot be represented by variables",
                          token_type,KEYWORD,KEYWORD);
                    else
                     { sprintf(buf,"%d",aptr->pin_num);
                       convert('b',buf,buf);
                       point1 = aptr;
                       for (k = 0; k < dimension; ++k)
                        { for (point1 = point1->backptr;
                               point1->operator != '?';
                               point1 = point1->backptr)
                            { if (point1->operator == '#')
                                { if (buf[strlen(buf)-1-k] == '0')
                                    point1->pin_num = -(point1->pin_num);
                                  break;
                                } /* IF */
                            } /* FOR */
                        } /* FOR */
                       eq_sym = NO;
                       point2->backptr->backptr->typ1_next = point2->typ1_next;
                       aptr = point2->typ1_next;
                       next_item = NO;
                       point2->typ1_next->backptr = point2->backptr->backptr;
                       point2->typ1_next = NULL;
                       point2->backptr->backptr = NULL;
                       clean_list((struct type0 *)NULL,point2);
                     } /* ELSE */
                  } /* IF */
                 break; /* CASE '$' */
      case '#' : if (eq_sym == YES)
                   err(FATAL,"cannot compare variables with variables",
                       token_type,KEYWORD,KEYWORD);
                 break;
      case '/' :
      case '%' :
      case '*' :
      case '+' :
      case ',' : break;
      case '=' : eq_sym = YES;
                 break;
      default  : inform("COND_EVAL Error\n",STOP);
    } /* SWITCH */
  if (next_item == YES)
    aptr = aptr->typ1_next;
 } /* FOR */
eval_cond_logic(alist,cond_list);
} /* COND_EVAL */
/*   -   -   -   -   -   */
condition_parse()  /* parse conditions: see GLS.TXT for details */
{ int val,last_item;
  char tmpbuf[MAX_NAME],buf[MAX_NAME],last_icon;
  struct type1 *a_ptr, *b_ptr;

last_item = NO; /* start parsing the condition */
last_icon = ' ';
for (lex(icon);the_keyword != THEN && icon[0] != ';';lex(icon))
 { switch(token_type)
    { case VARIABLE
         : if (last_item == VARIABLE || last_item == NUMBER)
             err(FATAL,OPR_OPRND,token_type,VARIABLE,KEYWORD);
           else
             last_item = VARIABLE;
           switch(check_name(icon,tmpbuf,buf))
            { case EXIST
                 : convert('d',buf,tmpbuf);
                   val = (int)atoi(tmpbuf);
                   push(vctr_temp,'#',val,(struct type1 *)NULL);
                   break;
              case NOT_EXIST
                 : err(FATAL,"undefined variable",token_type,PUNCT_MARK,
                       KEYWORD);
                   break;
              case MACRO
                 : copytree(typ0_temp->pterms,typ1_entry,'t');
                   push(vctr_temp,MACRO,NULL_VALUE,typ1_entry);
                   break;
              case VCTR
                 : for (a_ptr = vctr_temp;
                        a_ptr->typ1_next != NULL;
                        a_ptr = a_ptr->typ1_next)
                     ;
                   for (b_ptr = vctr2->typ1_next; /* see GLS.TXT for details */
                        b_ptr != NULL;
                        b_ptr = b_ptr->typ1_next)
                    { if (last_icon == '/' && (b_ptr != vctr2->typ1_next))
                        push(vctr_temp,'/',OPERATOR,(struct type1 *)NULL);
                      push(vctr_temp,'#',b_ptr->pin_num,(struct type1 *)NULL);
                      if (b_ptr->typ1_next != NULL)
                        push(vctr_temp,'*',OPERATOR,(struct type1 *)NULL);
                    } /* FOR */
                   clean_list((struct type0 *)NULL,vctr2->typ1_next);
                   vctr2->typ1_next = NULL;
                   break;
              default
                 : inform("VARIABLE error\n",STOP);
                   break;
            } /* SWITCH */
           break;
      case NUMBER
         : if (last_item == VARIABLE || last_item == NUMBER ||
              (last_item == PUNCT_MARK && last_icon == ')'))
             err(FATAL,OPR_OPRND,token_type,VARIABLE,PUNCT_MARK);
           else
             last_item = NUMBER;
           convert('d',icon,buf);
           val = (int)atoi(buf);
           push(vctr_temp,'$',val,(struct type1 *)NULL);
           break;
      case PUNCT_MARK
         : switch(icon[0])
            { case '(' :
              case '/' : if (last_item == VARIABLE || last_item == NUMBER ||
                            (last_item == PUNCT_MARK && last_icon == ')'))
                           err(FATAL,OPR_OPRND,token_type,KEYWORD,VARIABLE);
                         break;
              case ')' : if (last_item == PUNCT_MARK && last_icon != ')')
                           err(FATAL,"missing a variable",
                               token_type,KEYWORD,NUMBER);
                         break;
              case ',' :
              case '*' :
              case '%' :
              case '+' :
              case '=' : if (last_item == PUNCT_MARK && last_icon != ')')
                           err(FATAL,"put operand between operators",
                               token_type,VARIABLE,NUMBER);
                         break;
              default  : err(FATAL,"use only +,*,=,/,(, or )",
                             token_type,VARIABLE,KEYWORD);
                         break;
            } /* SWITCH */
           push(vctr_temp,icon[0],OPERATOR,(struct type1 *)NULL);
           last_item = PUNCT_MARK;
           last_icon = icon[0];
           break;
      default
         : err(FATAL,"use +,*,/, variables and numbers",
               token_type,KEYWORD,PUNCT_MARK);
    } /* SWITCH */
 } /* FOR */
if (the_keyword != THEN)
  err(FATAL,"use keyword THEN",token_type,NUMBER,VARIABLE);
if (last_icon != ')')
  err(FATAL,"close condition with )",token_type,KEYWORD,VARIABLE);
/* if there were no errors up to this point then evaluate the condition
   and put it into the COND_LIST; else continue parsing the file */
if (errors == 0)  /* do not evaluate if there were parsing errors */
  cond_eval(vctr_temp);
if (vctr_temp != NULL)
 { clean_list((struct type0 *)NULL,vctr_temp->typ1_next);
   vctr_temp->typ1_next = NULL;
 }
} /* CONDITION_PARSE */
/*   -   -   -   -   -   */
invert_condition()
     /* invert last condition in the IF part for the ELSE section */
{ struct type1 *ptr;
if (cond_list->typ1_next == NULL)
  return;
for (ptr = cond_list->typ1_next;  /* see GLS.TXT for details */
     ptr->typ1_next != NULL;
     ptr = ptr->typ1_next)
 ;
if (oprnd->typ1_next != NULL)
  inform("OPRND not NULL: INV_COND\n",STOP);
oprnd->typ1_next = ptr;  /* put this last IF condition into the OPRND stack */
ptr->backptr->typ1_next = NULL;
ptr->backptr = oprnd;
push(oprtr,'/',OPERATOR,(struct type1 *)NULL);
eval(PAIR); /* complement the condition */
insert((struct type0 *)NULL,cond_list,oprnd->typ1_next,
       "CONDITION",NULL_VALUE,'#',NEXT);
oprnd->typ1_next = NULL;
} /* INVERT_CONDITION */
/*   -   -   -   -   -   */
remove_last(item_list) /* remove the last item in this list */
struct type1 *item_list;
{ struct type1 *ptr;

for (ptr = item_list; ptr->typ1_next != NULL; ptr = ptr->typ1_next)
  ;
/* find the end of the ITEM_LIST to remove the last condition expr since
   the compiler is leaving this expression block already */
if (ptr->operator == '?')
  inform("COND_LIST already NULL\n",STOP);
ptr->backptr->typ1_next = NULL;
ptr->backptr = NULL;
clean_list((struct type0 *)NULL,ptr);  /* free the space used by this expr */
} /* REMOVE_LAST */
/*   -   -   -   -   -   */
parse_if() /* parse the IF-THEN-ELSE statements */
{
condition_parse();
if (the_keyword != THEN)
  err(WARNING,"put keyword THEN",token_type,VARIABLE,KEYWORD);
statement();
lex(icon);
if (the_keyword == ELSE)
 { invert_condition();
   statement();
 }
else
 get_new_token = NO;
/* remove the IF condition expr since the compiler
   is leaving this IF-THEN-ELSE block already */
remove_last(cond_list);
} /* PARSE_IF */
/*   -   -   -   -   -   */
bgn_end(new_level)
  /* just as long as the level does not become less than the level when
     this block was entered, stay in the FOR loop */
int new_level;
{
for (;level >= new_level;)
  statement();
} /* BGN_END */
/*   -   -   -   -   -   */
statement()
{ int act_high;

act_high = YES;
lex(icon);
switch(token_type)
 { case VARIABLE
      : equation(icon,act_high);
        return;
   case KEYWORD
      : switch(the_keyword)
         { case IF   : parse_if();
                       return;
           case CASE : parse_case(vctr_list,NO);
                       return;
           case BEGIN: bgn_end(++level);  /* parse a BEGIN-END block */
                       return;
           case END  : --level;
                       if (level == 0)  /* end of main BEGIN-END block */
                         return;
                       lex(icon);
                       if (icon[0] != ';')
                         err(FATAL,"terminate BEGIN-END block with ';'",
                             token_type,VARIABLE,KEYWORD);
                       return;
           default   : err(FATAL,"invalid statement",
                           token_type,VARIABLE,PUNCT_MARK);
         } /* SWITCH */
        break;
   case PUNCT_MARK
      : if (icon[0] == '/')
         { push(oprtr,icon[0],OPERATOR,(struct type1 *)NULL);
           push(oprtr,'(',OPERATOR,(struct type1 *)NULL);
           act_high = NO;
           lex(icon);
           if (token_type != VARIABLE)
             err(FATAL,"assign logic expressions to variable",
                 token_type,PUNCT_MARK,PUNCT_MARK);
           equation(icon,act_high);
           return;
         }
        else
          err(FATAL,INVAL_STAT,token_type,VARIABLE,KEYWORD);
        break;
   default
      : err(FATAL,INVAL_STAT,token_type,PUNCT_MARK,WHITE_SPACE);
        break;
 } /* SWITCH */
} /* STATEMENT */
/*   -   -   -   -   -   */
clean_space_used()
{
if (input_file != NULL)
  fclose(input_file);
if (out_file != NULL)
  fclose(out_file);
if (fnc_file != NULL)
  fclose(fnc_file);
input_file = NULL;
out_file = NULL;
fnc_file = NULL;
} /* CLEAN_SPACE_USED */
/*   -   -   -   -   -   */
upstring(dest,src)
/* convert SRC to all uppercase letters and put into DEST */
char dest[],src[];
{ int i,m;
for (i = 0, m = strlen(src); i <= m; ++i)
 { if (isalpha(src[i]))
    { if (islower(src[i]))
        dest[i] = toupper(src[i]);
      else
        dest[i] = src[i];
    }
   else
     dest[i] = src[i];
 } /* FOR */
} /* UPSTRING */
/*   -   -   -   -   -   */
out_architecture(the_file) /* output the pin architecture to THE_FILE */
FILE *the_file;
{ struct type0 *ptr0;
  char buf[MAX_NAME];
  struct type2 *ptr_2b;

fprintf(the_file,"\n%s\n%s",spec_name,part_name);
for (ptr_2b = pins; ptr_2b != NULL; ptr_2b = ptr_2b->typ2_next)
  /* display the pin architecture */
 {
   if (ptr_2b->pin_nmbr != NULL_VALUE)
    { if (ptr_2b->pname_type == HIGH)  /* print pin name */
        fprintf(the_file,"\n%s",ptr_2b->pname);
      else
        fprintf(the_file,"\n/%s",ptr_2b->pname);
      fprintf(the_file," %d",ptr_2b->pin_nmbr);  /* print pin number */
      for (ptr0 = ptr_2b->special; /* print pin architecture */
           ptr0 != NULL;
           ptr0 = ptr0->typ0_next)
       { if (ptr0->fn_name[0] == '!')
          { strcpy(buf,ptr0->fn_name);
            buf[0] = ' ';  /* remove the '!' symbol */
            upstring(buf,buf); /* convert to uppercase */
            fprintf(the_file,"%s",buf);
          } /* IF */
       } /* FOR */
      fprintf(the_file," @");
   } /* IF */
 } /* FOR */
fprintf(the_file,"\n*\n"); /* separate pin arch and equations with '*' */
} /* OUT_ARCHITECTURE */
/*   -   -   -   -   -   */
out_to_user(a_file)
FILE *a_file;
/* send the logic equations to the user and any intermediate files */
{ struct type2 *i;
  struct type0 *ptr0;
  char buf[MAX_NAME];

for (i = pins; i != NULL; i = i->typ2_next)
 {
   if (i->pin_nmbr != NULL_VALUE)
    { if (i->pt_list->typ1_next != NULL)
       { fprintf(a_file," %d = ",i->pin_nmbr);
         traverse(a_file,(struct type0 *)NULL,i->pt_list->typ1_next);
         fprintf(a_file,";\n");
       }
      for (ptr0 = i->special;
           ptr0 != NULL;
           ptr0 = ptr0->typ0_next)
       { if (ptr0->fn_name[0] == '@')
           { fprintf(a_file,"%d",i->pin_nmbr);
             strcpy(buf,ptr0->fn_name);
             buf[0] = '.';
             fprintf(a_file,"%s = ",buf);
             traverse(a_file,(struct type0 *)NULL,ptr0->pterms);
             fprintf(a_file,";\n");
           } /* IF */
       } /* FOR */
   } /* IF */
 } /* FOR */
} /* OUT_TO_USER */
/*   -   -   -   -   -   */
out_logic_equations()
{
if (errors > 0)
  return;
if (debug_flag == YES) {
  fprintf(stderr,"\n=> Compiled Equation\n");
  out_to_user(stderr);
}
if (out_file != NULL)
  out_to_user(out_file);
} /* OUT_LOGIC_EQUATIONS */
/*   -   -   -   -   -   */
/* free space after compiling a statement; this should allow more programs
   to run because now a statement is compiled and written to a file instead
   of holding everything in the PT_LIST field. */
free_space()
{ struct type2 *i;
for (i = pins; i != NULL; i = i->typ2_next)
 { clean_list((struct type0 *)NULL,i->pt_list->typ1_next);
   i->pt_list->typ1_next = NULL;
   clean_list(i->special->typ0_next,(struct type1 *)NULL);
   i->special->typ0_next = NULL;
   clean_list((struct type0 *)NULL,i->oprnd_stk->typ1_next);
   i->oprnd_stk->typ1_next = NULL;
   clean_list((struct type0 *)NULL,i->oprtr_stk->typ1_next);
   i->oprtr_stk->typ1_next = NULL;
 } /* FOR */
} /* FREE_SPACE */
/*   -   -   -   -   -   */
parse_main() /* process the body of the program */
{
++level;  /* LEVEL keeps track of the BEGIN-END nesting level */
lex(icon);
if (the_keyword != BEGIN)
  err(WARNING,"use keyword BEGIN",token_type,VARIABLE,KEYWORD);

if (debug_flag == YES)
  out_architecture(stderr);
if (out_file != NULL)
  out_architecture(out_file);

for (;level > 0;)
  statement();

if (out_file != NULL)
  fprintf(out_file,"@\n");
lex(icon);
if (icon[0] != '.')
  err(FATAL,"unmatched BEGIN; or put '.' after last END",
      token_type,PUNCT_MARK,WHITE_SPACE);
clean_space_used();
fprintf(stderr,"\n[Compilation Errors: %d]\n",errors);
} /* PARSE_MAIN */

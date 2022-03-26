/* [May 26, 1986] : STATE2.C contains additional routines and functions for
   compiling statements. These include parsing the CASE structure */
#include "glsdef.h"
/*  -   -   -   -   -   */
generate_condition(constant,listx,dimension)
/* generate a condition by equating the CONSTANT to the conditions in LISTX */
int constant;
struct type1 *listx;
int dimension;
{ int k;
  char buf1[MAX_NAME];
  struct type1 *point1;

sprintf(buf1,"%d",constant);
convert('b',buf1,buf1);
for (point1 = listx; point1->typ1_next != NULL; point1 = point1->typ1_next)
  ;
for (k = 0; k < dimension; ++k)
 { for (point1 = point1->backptr;
        point1->operator != '(';
        point1 = point1->backptr)
    { if (point1->operator == '#')
       { if (buf1[strlen(buf1)-1-k] == '0')
            point1->pin_num = -(point1->pin_num);
         break;
       } /* IF */
    } /* FOR */
 } /* FOR */
eval_cond_logic(listx,tmp_oprnd);
} /* GENERATE_CONDITION */
/*  -   -   -   -   -   */
eval_range(r1,r2,thelist)
/* generate conditions by evaluating the range: equate all numbers in the
   range to variables in THELIST */
int r1,r2;
struct type1 *thelist;
{ int dimension,m,direction;
  double x,y;
  struct type1 *point1,*point2,*aptr,*argvptr;

if (errors > 0)
  return;
if (r1 != NULL_VALUE && r2 != NULL_VALUE)
  { if (r1 >= r2)
      direction = -1;
    else
      direction = 1;
    for (aptr = thelist; aptr->typ1_next != NULL; aptr = aptr->typ1_next)
     ; /* find the tail of THE_PTR list */
    for (dimension = 0,point1 = aptr->backptr;
         point1->backptr != NULL && point1->operator != '(';
         point1 = point1->backptr)
     { switch(point1->operator)
        { case '#'   /* if pin number */
            : ++dimension;
              break;
          case '/'
            : break;
          case '*'
            : if (point1->rtptr != NULL || point1->ltptr != NULL)
                err(FATAL,"cannot compare macros with constants",
                    token_type,KEYWORD,KEYWORD);
              break;
          default
            : inform("Error in EVAL_RANGE\n",STOP);
              break;
        } /* SWITCH */
     } /* FOR */
    if (dimension > 16)
      err(FATAL,"compare 16 or fewer variables only",
          token_type,KEYWORD,KEYWORD);
    x = 2;
    y = (double)dimension; /* cast the int as a double */
    if ((pow(x,y) <= (double)r1) || (pow(x,y) <= (double)r2))
     { err(FATAL,"number range cannot be represented by variables",
          token_type,KEYWORD,KEYWORD);
       return;
     }
    else
     { for ( m = r1; m != r2 + direction; m = m + direction)
        { for (point2 = point1, argvptr = NULL;
               point2 != NULL;
               point2 = point2->typ1_next)
           { typ1_entry = (struct type1 *)malloc(sizeof(struct type1));
             mem_check((struct type0 *)NULL,typ1_entry,(struct type2 *)NULL);
             typ1_entry->operator = point2->operator;
             typ1_entry->pin_num = point2->pin_num;
             typ1_entry->rtptr = NULL;
             typ1_entry->ltptr = NULL;
             typ1_entry->backptr = NULL;
             typ1_entry->typ1_next = NULL;
             if (argvptr == NULL)
               argvptr = typ1_entry;
             else
              { for (aptr = argvptr;
                     aptr->typ1_next != NULL;
                     aptr = aptr->typ1_next)
                  ;  /* goto the end of this list to attach TYP1_ENTRY */
                aptr->typ1_next = typ1_entry;
                typ1_entry->backptr = aptr;
              } /* ELSE */
           } /* FOR point2 */
          generate_condition(m,argvptr,dimension);
          clean_list((struct type0 *)NULL,argvptr);
          argvptr = NULL;
        } /* FOR */
     } /* ELSE */
  } /* IF */
else
  err(FATAL,"invalid range specification for CASE statement",
      token_type,KEYWORD,PUNCT_MARK);
} /* EVAL_RANGE */
/*  -   -   -   -   -   */
or_case(lst)
struct type1 *lst;
{ struct type1 *apoint;

if (errors > 0)
  return;
oprnd->typ1_next = lst->typ1_next;
lst->typ1_next->backptr = oprnd;
lst->typ1_next = NULL;
for (apoint = oprnd->typ1_next;
     apoint->typ1_next != NULL;
     apoint = apoint->typ1_next)
  push(oprtr,'+',OPERATOR,(struct type1 *)NULL);
eval(COMPLETE);
for (apoint = caselist;
     apoint->typ1_next != NULL;
     apoint = apoint->typ1_next)
  ;
apoint->typ1_next = oprnd->typ1_next;
if (oprnd->typ1_next != NULL)
  oprnd->typ1_next->backptr = apoint;
oprnd->typ1_next = NULL;
} /* OR_CASE */
/*  -   -   -   -   -   */
get_case_range(the_ptr)
/* compile the numeric ranges for the CASE conditions */
struct type1 *the_ptr;
{ int range1,range2;
  char buf1[MAX_NAME];

for (lex(icon); icon[0] != ')';lex(icon))
 { get_new_token = NO;  /* return the token for use by GET_INDEX() */
   range1 = get_index(buf1);
   lex(icon);
   switch(icon[0])
    { case ')' : get_new_token = NO;
                 range2 = range1;
                 eval_range(range1,range2,vctr_list);
                 break;
      case ',' : range2 = range1;
                 eval_range(range1,range2,vctr_list);
                 break;
      case ':' : range2 = get_index(buf1);
                 lex(icon);
                 switch(icon[0])
                  { case ')' : get_new_token = NO;
                               break;
                    case ',' : break;
                    default  : err(FATAL,
                                   "use ',' or ')' to terminate case range",
                                   token_type,VARIABLE,NUMBER);
                               break;
                  } /* SWITCH */
                 eval_range(range1,range2,vctr_list);
                 break;
      default  : err(FATAL,
                     "use ',' or ':' when expressing possible case values",
                     token_type,VARIABLE,PUNCT_MARK);
                 break;
    } /* CASE */
 } /* FOR */
or_case(tmp_oprnd);  /* or all the CASE conditions */
} /* GET_CASE_RANGE */
/*  -   -   -   -   -   */
eval_case_cond(case_level)  /* start evaluating the CASE conditions */
int case_level;
{
for (; level >= case_level;)
 { lex(icon);
   if (the_keyword == END)
    { --level;
      lex(icon);
      if (icon[0] != ';')
        err(FATAL,"terminate CASE block with ';'",token_type,
            KEYWORD,VARIABLE);
      break;
    }
   else
     get_new_token = NO;
   get_case_range(vctr_list);
   statement();
   remove_last(caselist);
 } /* FOR */
} /* EVAL_CASE_COND */
/*   -   -   -   -   -   */
parse_case(list_x,for_case_statement)
struct type1 *list_x;
int for_case_statement;
  /* parse case conditions: see GLS.TXT for details */
{ int val,last_item;
  char tmpbuf[MAX_NAME],buf[MAX_NAME],last_icon;
  struct type1 *a_ptr, *b_ptr;

last_item = NO; /* start parsing the condition */
last_icon = ' ';
lex(icon);
if (icon[0] != '(')
  err(FATAL,"enclose conditions within parenthesis",token_type,
      VARIABLE,VARIABLE);
else
 { if (for_case_statement != FUNCTION)
    push(list_x,'(',OPERATOR,(struct type1 *)NULL);
 }
for (lex(icon); icon[0] != ')' && icon[0] != ';'; lex(icon))
 { switch(token_type)
    { case VARIABLE
         : last_item = VARIABLE;
           switch(check_name(icon,tmpbuf,buf))
            { case EXIST
                 : convert('d',buf,tmpbuf);
                   val = (int)atoi(tmpbuf);
                   push(list_x,'#',val,(struct type1 *)NULL);
                   break;
              case NOT_EXIST
                 : err(FATAL,"undefined variable",token_type,PUNCT_MARK,
                       KEYWORD);
                   break;
              case MACRO
                 : if (typ0_temp->pterms->rtptr != NULL &&
                       typ0_temp->pterms->ltptr != NULL &&
                       typ0_temp->pterms->operator != '#')
                     err(FATAL,"macro logic expression",
                         token_type,KEYWORD,PUNCT_MARK);
                   else
                    { copytree(typ0_temp->pterms,typ1_entry,'t');
                      push(list_x,MACRO,NULL_VALUE,typ1_entry);
                    }
                   break;
              case VCTR
                 : for (a_ptr = list_x;
                        a_ptr->typ1_next != NULL;
                        a_ptr = a_ptr->typ1_next)
                     ;
                   for (b_ptr = vctr2->typ1_next; /* see GLS.TXT for details */
                        b_ptr != NULL;
                        b_ptr = b_ptr->typ1_next)
                    { if (last_icon == '/' && (b_ptr != vctr2->typ1_next))
                       { if (for_case_statement != FUNCTION)
                           push(list_x,'/',OPERATOR,(struct type1 *)NULL);
                                /* see GLS.TXT */
                       }
                      push(list_x,'#',b_ptr->pin_num,(struct type1 *)NULL);
                      if (b_ptr->typ1_next != NULL)
                       { if (for_case_statement != FUNCTION)
                           push(list_x,'*',OPERATOR,(struct type1 *)NULL);
                       }
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
         : last_item = NUMBER;
           err(FATAL,"use variables only for CASE/function conditions",
               token_type,VARIABLE,PUNCT_MARK);
           break;
      case PUNCT_MARK
         : switch(icon[0])
            { case '(' :
              case '/' : if (last_item == VARIABLE || last_item == NUMBER ||
                            (last_item == PUNCT_MARK && last_icon != ','))
                           err(FATAL,OPR_OPRND,token_type,KEYWORD,VARIABLE);
                         break;
              case ')' : if (last_item == PUNCT_MARK && last_icon != ')')
                           err(FATAL,"missing variable",
                               token_type,KEYWORD,NUMBER);
                         break;
              case '*' : err(FATAL,"use concatenation operator ','",
                             token_type,VARIABLE,KEYWORD);
              case ',' : break;
              case '+' :
              case '=' : err(FATAL,
                             "cannot assign values to CASE conditions",
                             token_type,VARIABLE,NUMBER);
                         break;
              default  : err(FATAL,"use only /,(,), or ',' in conditions",
                            token_type,VARIABLE,KEYWORD);
                         break;
            } /* SWITCH */
           if (for_case_statement != FUNCTION)
            { if (icon[0] == ',')
                push(list_x,'*',OPERATOR,(struct type1 *)NULL);
              else
                push(list_x,icon[0],OPERATOR,(struct type1 *)NULL);
            }
           last_item = PUNCT_MARK;
           last_icon = icon[0];
           break;
      default
         : err(FATAL,"use /,',', variables and numbers in condition",
               token_type,KEYWORD,PUNCT_MARK);
    } /* SWITCH */
 } /* FOR */
if (icon[0] != ')')
  err(FATAL,"looking for ')'",token_type,KEYWORD,VARIABLE);
/* if there were no errors up to this point then evaluate the condition
   and put it into the COND_LIST; else continue parsing the file */
else
 { if (for_case_statement != FUNCTION)
     push(list_x,')',OPERATOR,(struct type1 *)NULL);
 }
if (for_case_statement == FUNCTION)
  return;
lex(icon);
if (the_keyword != BEGIN)
  err(FATAL,"missing BEGIN",0,0,0);
eval_case_cond(++level);
for (a_ptr = list_x; a_ptr->typ1_next != NULL; a_ptr = a_ptr->typ1_next)
  ; /* look for the tail of this list */
for (; a_ptr->backptr->operator != ')' && a_ptr->backptr->operator != '?';
     a_ptr = a_ptr->backptr)
  ;
b_ptr = a_ptr->backptr;
clean_list((struct type0 *)NULL,b_ptr->typ1_next);
b_ptr->typ1_next = NULL;
} /* PARSE_CASE */
/*  -   -   -   -   -   */

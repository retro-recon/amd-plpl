/* April 29, 1986 : MACRO.C contains the routine for parsing and evaluating
   macro definitions */
#include "glsdef.h"
/*  -   -   -   -   -   -   */
#ifndef unix
abs(an_int)  /* return the absolute value of AN_INT */
int an_int;
{
if (an_int < 0)
  return(-an_int);
return(an_int);
} /* ABS */
#endif
/*  -   -   -   -   -   -   */
traverse(filedest,list0,list1)
FILE *filedest;
struct type0 *list0;
struct type1 *list1;
{ struct type0 *tmp0;
  struct type1 *tmp1;

if (list0 != NULL)
 { for (tmp0 = list0; tmp0 != NULL; tmp0 = tmp0->typ0_next)
    { if (filedest != NULL) {
        if (tmp0->fn_name[0] != '\0')
          fprintf(filedest,"\n[%s] = ",tmp0->fn_name);
        if (tmp0->pterms != NULL)
          traverse(filedest,(struct type0 *)NULL,tmp0->pterms);
      }
    }
 } /* LIST0 != NULL */
for (tmp1 = list1; tmp1 != NULL; tmp1 = tmp1->typ1_next)
 { if (debug_flag == YES && show_parentheses == YES)
    { if (filedest != NULL)
        fprintf(filedest,"(");
    }
   traverse(filedest,(struct type0 *)NULL,tmp1->rtptr);
   switch(tmp1->operator)
    { case '#' : if (tmp1->pin_num < 0)
                  { if (filedest != NULL)
                      fprintf(filedest,"/%d",-(tmp1->pin_num));
                  }
                 else
                  { if (filedest != NULL)
                      fprintf(filedest,"%d",tmp1->pin_num);
                  }
                 break;
      case '$' : if (filedest != NULL) /* number is a constant */
                   fprintf(filedest,"#%d",tmp1->pin_num);
                 break;
      case '*' : if (filedest != NULL)
                   fprintf(filedest,"%c",tmp1->operator);
                 break;
      case '+' : if (filedest != NULL)
                   fprintf(filedest," %c\n\t",tmp1->operator);
                 break;
      default  : fprintf(filedest,"[%d]<%c>",tmp1->pin_num,tmp1->operator);
     } /* SWITCH */
   traverse(filedest,(struct type0 *)NULL,tmp1->ltptr);
   if (debug_flag == YES && show_parentheses == YES)
    { if (filedest != NULL)
        fprintf(filedest,")");
    }
 } /* FOR */
} /* TRAVERSE */
/*  -   -   -   -   -   -   */
veval(eval_extent,a_vctr_list)
/* vector evaluation */
int eval_extent;
struct type1 *a_vctr_list;
{ struct type1 *temp,*bracket_test;
  int x;
  struct type2 *vptr1;

if (errors > 0)
  return;
if (a_vctr_list == NULL)
  inform("VEVAL ERROR: A_VCTR_LIST NULL\n",STOP);
if (a_vctr_list->typ1_next == NULL)
 { if (eval_extent == COMPLETE)
    { for (bracket_test = oprtr;
           bracket_test != NULL;
           bracket_test = bracket_test->typ1_next)
       { if (bracket_test->operator == '(')
           err(FATAL,"mismatched parentheses",0,0,0);
       }
     }
   eval(eval_extent);
   return;
 }
for (temp = a_vctr_list->typ1_next;
     temp != NULL;
     temp = temp->typ1_next)
 { if (temp->operator == '#')
    { x = abs(temp->pin_num);
      vptr1 = (struct type2 *)find_pin(x);
      oprnd->typ1_next = vptr1->oprnd_stk->typ1_next;
      oprtr->typ1_next = vptr1->oprtr_stk->typ1_next;
      if (vptr1->oprnd_stk->typ1_next != NULL)
        vptr1->oprnd_stk->typ1_next->backptr = oprnd;
      if (vptr1->oprtr_stk->typ1_next != NULL)
        vptr1->oprtr_stk->typ1_next->backptr = oprtr;
      vptr1->oprnd_stk->typ1_next = NULL;
      vptr1->oprtr_stk->typ1_next = NULL;
      if (eval_extent == COMPLETE)
       { for (bracket_test = oprtr;
              bracket_test != NULL;
              bracket_test = bracket_test->typ1_next)
         if (bracket_test->operator == '(')
           err(FATAL,"mismatched parentheses",0,0,0);
       }
      eval(eval_extent);
      vptr1->oprnd_stk->typ1_next = oprnd->typ1_next;
      vptr1->oprtr_stk->typ1_next = oprtr->typ1_next;
      if (oprnd->typ1_next != NULL)
        oprnd->typ1_next->backptr = vptr1->oprnd_stk;
      if (oprtr->typ1_next != NULL)
        oprtr->typ1_next->backptr = vptr1->oprtr_stk;
      oprnd->typ1_next = NULL;
      oprtr->typ1_next = NULL;
    } /* IF */
 } /* FOR */
} /* VEVAL */
/*  -   -   -   -   -   -   */
check_parenthesis(symbol,vlist)
/* use the ICON global variable to check if a
   parenthesis or '/' operator is in the expression */
int symbol;
struct type1 *vlist;
{ struct type1 *aptr;
  int x;
  struct type2 *chk1;

lex(icon);
switch(icon[0])
 { case '/' :
   case '(' : if (symbol == RBRACKET)
               { err(FATAL,"looking for variable/macro",
                     token_type,VARIABLE,PUNCT_MARK);
                 return;
               }
              if (vlist->typ1_next != NULL)
               { for (aptr = vlist->typ1_next;
                      aptr != NULL;
                     aptr = aptr->typ1_next)
                  { x = abs(aptr->pin_num);
                    chk1 = (struct type2 *)find_pin(x);
                    push(chk1->oprtr_stk,icon[0],OPERATOR,(struct type1
 *)NULL);
                  }
               }
              else
                push(oprtr,icon[0],OPERATOR,(struct type1 *)NULL);
              check_parenthesis(LBRACKET,vlist);
              break;
   case ')' : if (symbol == RBRACKET)
               { veval(LBRACKET,pin_vctr);
                 check_parenthesis(RBRACKET,vlist);
               }
              else
                err(FATAL,"mismatched parenthesis",
                    token_type,KEYWORD,VARIABLE);
              break;
   case '%' : /* XOR */
   case '*' :
   case '+' : if (symbol == RBRACKET)
               { if (vlist->typ1_next != NULL)
                  { for (aptr = vlist->typ1_next;
                         aptr != NULL;
                        aptr = aptr->typ1_next)
                     { x = abs(aptr->pin_num);
                       chk1 = (struct type2 *)find_pin(x);
                       push(chk1->oprtr_stk,icon[0],OPERATOR,
                                  (struct type1 *)NULL);
                     }
                  }
                 else
                   push(oprtr,icon[0],OPERATOR,(struct type1 *)NULL);
               }
              else
                err(FATAL,"put variable/macro here",
                    token_type,VARIABLE,NUMBER);
              break;
   case ';' : if (symbol != RBRACKET)
                err(FATAL,"unexpected ;",token_type,VARIABLE,KEYWORD);
              get_new_token = NO;
              break;
  } /* SWITCH */
} /* CHECK_PARENTHESIS */
/*  -   -   -   -   -   -   */
vctr_push(arg_const,arg_pin,arg_oprtr,arg_macro,arg_vctr,lst_vctr)
    /* push operands or operators into each operand
       or operator stack indexed by the LST_VCTR list */
char arg_const[];
int arg_pin;
char arg_oprtr;
struct type1 *arg_macro, *arg_vctr, *lst_vctr;
{ double x,y;
  int k,m,p;
  struct type1 *ptr1, *ptr2;
  struct type2 *vp1;

if (arg_const[0] != '\0')
 { x = 2;
   y = (double)(lst_length(lst_vctr));
   k = (int)y;
   if (y > 16)
    { err(FATAL,"compare 16 or fewer variables per pin vector",
          token_type,PUNCT_MARK,VARIABLE);
      return;
    }
   if (pow(x,y) <= (double)arg_pin)
     err(FATAL,"number cannot be represented by pin vector",
         token_type,PUNCT_MARK,VARIABLE);
   else
    { for (ptr1 = lst_vctr->typ1_next, m = 0;
           ptr1 != NULL;
           ptr1 = ptr1->typ1_next)
       { if (ptr1->operator == '#')
          { p = abs(ptr1->pin_num);
            vp1 = (struct type2 *)find_pin(p);
            switch(arg_const[strlen(arg_const)-k+m])
             { case '0': push(vp1->oprnd_stk,'$',0,(struct type1 *)NULL);
 break;
               case '1': push(vp1->oprnd_stk,'$',1,(struct type1 *)NULL);
 break;
               default: inform("incorrect VCTR_PUSH use\n",STOP);
                        break;
             } /* SWITCH */
            ++m;  /* see GLS.TXT for details */
          }
       } /* FOR */
    } /* ELSE pow(x,y) */
  return;
 } /* IF arg_const */
if (arg_pin != NULL_VALUE && arg_const[0] == '\0')
 { for (ptr1 = lst_vctr->typ1_next;
        ptr1 != NULL;
        ptr1 = ptr1->typ1_next)
    { if (ptr1->operator == '#')
       { p = abs(ptr1->pin_num);
         vp1 = (struct type2 *)find_pin(p);
         push(vp1->oprnd_stk,'#',arg_pin,(struct type1 *)NULL);
       }
    } /* FOR */
   return;
 } /* IF arg_pin */
switch(arg_oprtr)
 { case '+' :
   case '*' :
   case '%' :
   case '/' :
   case '(' : for (ptr1 = lst_vctr->typ1_next;
                   ptr1 != NULL;
                   ptr1 = ptr1->typ1_next)
                { if (ptr1->operator == '#')
                   { p = abs(ptr1->pin_num);
                     vp1 = (struct type2 *)find_pin(p);
                     push(vp1->oprtr_stk,arg_oprtr,OPERATOR,
                               (struct type1 *)NULL);
                   }
                }
              return;
 } /* SWITCH */
if (arg_macro != NULL)
 { for (ptr1 = lst_vctr->typ1_next;
        ptr1 != NULL;
        ptr1 = ptr1->typ1_next)
    { if (ptr1->operator == '#')
       { copytree(arg_macro,typ1_entry,'t');
         p = abs(ptr1->pin_num);
         vp1 = (struct type2 *)find_pin(p);
         push(vp1->oprnd_stk,'#',NULL_VALUE,typ1_entry);
       }
    } /* FOR */
   return;
 } /* IF arg_macro */
if (arg_vctr->typ1_next != NULL)
 { if (lst_length(arg_vctr) != lst_length(lst_vctr))
    { err(FATAL,"vector assignment mismatch",token_type,PUNCT_MARK,KEYWORD);
      return;
    }
   for (ptr1 = lst_vctr->typ1_next, ptr2 = arg_vctr->typ1_next;
        ptr1 != NULL && ptr2 != NULL;
        ptr1 = ptr1->typ1_next, ptr2 = ptr2->typ1_next)
    { if (ptr1->operator == '#')
       { p = abs(ptr1->pin_num);
         vp1 = (struct type2 *)find_pin(p);
         push(vp1->oprnd_stk,'#',ptr2->pin_num,(struct type1 *)NULL);
       }
    } /* FOR */
   return;
 } /* IF arg_vctr */
inform("VCTR_PUSH Fatal error\n",STOP);
} /* VCTR_PUSH */
/*  -   -   -   -   -   -   */
lst_length(anylist)
struct type1 *anylist;
{ int count;
  struct type1 *lstptr;
for (lstptr = anylist, count = 0; lstptr != NULL; lstptr = lstptr->typ1_next)
 { if (lstptr->operator == '#')
     ++count;
 }
return(count);
} /* LST_LENGTH */
/*  -   -   -   -   -   -   */
add_cond(a_list0,a_list1,signal_name,signal_type,pin_signal)
 /* add conditions to the oprnd stack before assigning
    the logic expression to the signal */
struct type0 *a_list0;
struct type1 *a_list1;
char signal_name[];
int signal_type,pin_signal;
{ struct type1 *add_ptr;

/* add all the IF conditions to this equation */
for (add_ptr = cond_list->typ1_next;
     add_ptr != NULL;
     add_ptr = add_ptr->typ1_next)
 { copytree(add_ptr,typ1_entry,'t');
   push(oprnd,'#',NULL_VALUE,typ1_entry);
   push(oprtr,'*',OPERATOR,(struct type1 *)NULL);  /* push the AND operator */
   eval(PAIR);
 } /* FOR */
    /* add all the CASE conditions to this equation */
if (caselist->typ1_next != NULL)
 { for (add_ptr = caselist->typ1_next;
        add_ptr != NULL;
        add_ptr = add_ptr->typ1_next)
    { copytree(add_ptr,typ1_entry,'t');
      push(oprnd,'#',NULL_VALUE,typ1_entry);
      push(oprtr,'*',OPERATOR,(struct type1 *)NULL);  /* push AND operator */
      eval(PAIR);
    } /* FOR */
 }
for (;DeMorgan(oprnd->typ1_next) != STOP;)  /* DeMorganize the equation */
  ;
if (signal_name[0] != '\0')
 { insert(a_list0,a_list1,oprnd->typ1_next,signal_name,pin_signal,'#',
          signal_type);
 }
oprnd->typ1_next = NULL;
} /* ADD_COND */
/*  -   -   -   -   -   -   */
logic_expression(signal,list0,list1,high,type_signal,pinv_list)
   /* parse the logic expression for SIGNAL; FIRST is first variable
      and attach the resulting logic expression onto LIST */
char signal[];
struct type0 *list0;
struct type1 *list1;
int high; /* tells if SIGNAL is active high or low */
int type_signal;
struct type1 *pinv_list;
{ char tmp1[MAX_NAME], tmp2[MAX_NAME], buf[MAX_NAME];
  int pnum,t;  /* pin number */
  struct type1 *ptr, *a_ptr;
  struct type2 *lptr1;

for (; icon[0] != ',' && icon[0] != ';' ;)
 { check_parenthesis(LBRACKET,pinv_list);
   switch(check_name(icon,tmp1,tmp2))
    { case EXIST
        : convert('d',tmp2,buf);
          pnum = (int)atoi(buf);
          switch(type_signal)
           { case VCTR
               :
             case FUNCTION
               : vctr_push("",pnum,'#',(struct type1 *)NULL,
                              (struct type1 *)NULL,pinv_list);
                 break;
             default
               : push(oprnd,'#',pnum,(struct type1 *)NULL);
                 break;
           }
          veval(PAIR,pinv_list);
          break;
      case NOT_EXIST
        : err(FATAL,"undefined variable/macro",token_type,PUNCT_MARK,NUMBER);
          break;
      case MACRO
        : a_ptr = NULL;
          copytree(typ0_temp->pterms,typ1_entry,'t');
          /* TYP0_TEMP points to the macro name in MACRO_LIST
             as a result of the CHECK_NAME call */
          switch(type_signal)
           { case VCTR
              :
             case FUNCTION
              : if (typ1_entry->operator == '$')
                 { sprintf(buf,"%d",typ1_entry->pin_num);
                   convert('b',buf,buf);
                   vctr_push(buf,typ1_entry->pin_num,'$',
                         (struct type1 *)NULL,(struct type1 *)NULL,pinv_list);
                 }
                else
                 { a_ptr = typ1_entry;
                   vctr_push("",NULL_VALUE,'#',a_ptr,
                        (struct type1 *)NULL,pinv_list);
                   clean_list((struct type0 *)NULL,a_ptr);
                 }
                break;
             default
              : push(oprnd,'#',NULL_VALUE,typ1_entry);
                break;
           } /* SWITCH */
          veval(PAIR,pinv_list);
          break;
      case NUMBER
        : convert('d',tmp1,buf);
          pnum = (int)atoi(buf);
          switch(type_signal)
           { case PIN
                : if (pnum > 1)
                    err(FATAL,
                        "number cannot be represented by scalar variable",
                        token_type,KEYWORD,PUNCT_MARK);
                  else
                   { push(oprnd,'$',pnum,(struct type1 *)NULL);
                     eval(PAIR);
                   }
                  break;
             case NO
                : push(oprnd,'$',pnum,(struct type1 *)NULL);
                  eval(PAIR);
                  break;
             case VCTR
                :
             case FUNCTION
                : convert('b',buf,buf);
                  vctr_push(buf,pnum,'$',(struct type1 *)NULL,
                         (struct type1 *)NULL,pinv_list);
                  veval(PAIR,pinv_list);
                  break;
             default
                : inform("Number error in LOGEXP\n",STOP);
           } /* SWITCH */
          break;
      case VCTR
        : switch(type_signal)
           { case PIN
               : err(FATAL,"cannot assign vector to scalar variable",
                     token_type,PUNCT_MARK,KEYWORD);
                 break;
             case VCTR
               :
             case FUNCTION
               : vctr_push("",NULL_VALUE,'#',(struct type1 *)NULL,
                          vctr2,pinv_list);
                 clean_list((struct type0 *)NULL,vctr2->typ1_next);
                 vctr2->typ1_next = NULL;
                 veval(PAIR,pinv_list);
                 break;
             default
               : inform("LOGEXP: case VCTR\n",STOP);
           } /* SWITCH */
          break;
      default
        : err(FATAL,"use vector/scalar variables or numbers",
              token_type,PUNCT_MARK,WHITE_SPACE);
          break;
    } /* SWITCH */
   lex(icon);
   if (token_type != PUNCT_MARK ||
       !(icon[0] == '+' || icon[0] == '*' || icon[0] == '%' ||
         icon[0] == ')' || icon[0] == ',' || icon[0] == ';'))
     err(FATAL,"use +, *, %, ), ',', or ';' only",token_type,VARIABLE,NUMBER);
   else
    { switch(icon[0])
       { case '*' :
         case '%' :
         case '+' : if (type_signal == VCTR || type_signal == FUNCTION)
                      vctr_push("",NULL_VALUE,icon[0],
                          (struct type1 *)NULL,(struct type1 *)NULL,pinv_list);
                    else
                      push(oprtr,icon[0],OPERATOR,(struct type1 *)NULL);
                    break;
         case ')' : veval(LBRACKET,pinv_list);
                    check_parenthesis(RBRACKET,pinv_list);
                    break;
         case ';' : get_new_token = NO;
                    break;
         case ',' : break;
         default  : inform("LOGEXP Error\n",STOP);
                    break;
       } /* SWITCH */
    } /* ELSE */
 } /* FOR */
if (high == NO) /* evaluate left bracket added in PARSE_DEFINE section */
  veval(LBRACKET,pinv_list);
veval(COMPLETE,pinv_list);

if (pinv_list->typ1_next == NULL)
  add_cond(list0,list1,signal,PIN,NULL_VALUE);
else
 { for (ptr = pinv_list->typ1_next;
        ptr != NULL;
        ptr = ptr->typ1_next)
    { if (ptr->operator == '#')
       { switch(type_signal)
          { case VCTR
              :
            case FUNCTION
              : t = abs(ptr->pin_num);
                lptr1 = (struct type2 *)find_pin(t);
                oprnd->typ1_next = lptr1->oprnd_stk->typ1_next;
                oprtr->typ1_next = lptr1->oprtr_stk->typ1_next;
                if (lptr1->oprnd_stk->typ1_next != NULL)
                  lptr1->oprnd_stk->typ1_next->backptr = oprnd;
                if (lptr1->oprtr_stk->typ1_next != NULL)
                  lptr1->oprtr_stk->typ1_next->backptr = oprtr;
                lptr1->oprnd_stk->typ1_next = NULL;
                lptr1->oprtr_stk->typ1_next = NULL;
                if (type_signal == VCTR)
                  add_cond(list0,lptr1->pt_list,signal,VCTR,ptr->pin_num);
                else
                  { lptr1 = (struct type2 *)find_pin(abs(ptr->pin_num));
                    add_cond(lptr1->special,(struct type1 *)NULL,signal,
                         FUNCTION,abs(ptr->pin_num));
                  }
                break;
            default
              : inform("LOGEXP: Add Cond\n",STOP);
          } /* SWITCH */
       } /* IF */
    } /* FOR */
 } /* ELSE */
out_logic_equations();
if (list0 == NULL)  /* free space only after reading define section */
  free_space();
} /* LOGIC_EXPRESSION */
/*  -   -   -   -   -   -   */
insert(list0,list1,sublist,fcname,value,oprtor,typ_signal)
struct type0 *list0;
struct type1 *list1,*sublist;
char fcname[];
int value;
char oprtor;
int typ_signal;
{ int func_defined;

if (list0 == NULL && list1 == NULL && typ_signal != VCTR)
   return;
if (list0 != NULL)
 { for (typ0_temp = list0,func_defined = NO; typ0_temp != NULL;
        typ0_temp = typ0_temp->typ0_next)
     { if (!strcmp(typ0_temp->fn_name,fcname))
         func_defined = YES;
     } /* FOR */
   if (func_defined == YES)
     { for (typ0_temp = list0;
            (typ0_temp != NULL) && strcmp(typ0_temp->fn_name,fcname);
            typ0_temp = typ0_temp->typ0_next)
          ;
       if (typ0_temp == NULL)
          inform("GLS Program Error\n",STOP);
       oprnd->typ1_next = typ0_temp->pterms;
       if (typ0_temp->pterms != NULL)
          typ0_temp->pterms->backptr = oprnd;
       else
        { typ0_temp->pterms = sublist;
          return;
        }
       typ0_temp->pterms = NULL;
       for (typ1_temp = oprnd; typ1_temp->typ1_next != NULL;
            typ1_temp = typ1_temp->typ1_next)
         ; /* find the end of this list */
       typ1_temp->typ1_next = sublist;
       if (sublist != NULL)
          sublist->backptr = typ1_temp;
       push(oprtr,'+',OPERATOR,(struct type1 *)NULL);
       eval(COMPLETE);
       typ0_temp->pterms = oprnd->typ1_next;
       oprnd->typ1_next->backptr = typ0_temp->pterms;
       oprnd->typ1_next = NULL;
       return;
     } /* IF func_defined == YES */
   for (typ0_temp = list0; typ0_temp->typ0_next != NULL;
        typ0_temp = typ0_temp->typ0_next)
     ; /* find the end of this list */
   typ0_entry = (struct type0 *)malloc(sizeof(struct type0));
   mem_check(typ0_entry,(struct type1 *)NULL,(struct type2 *)NULL);
   strcpy(typ0_entry->fn_name,"BVB");
   typ0_entry->fuse_numbers = NULL_VALUE;
   typ0_entry->pterms = NULL;
   typ0_entry->typ0_next = NULL;
   typ0_temp->typ0_next = typ0_entry;
   if (sublist != NULL)
     { typ0_entry->pterms = sublist;
       sublist->backptr = NULL;
     }
   if (fcname[0] != '\0')
     strcpy(typ0_entry->fn_name,fcname);
   if (value != NULL_VALUE)
     typ0_entry->fuse_numbers = value;
   return;
 } /* LIST0 != NULL */
if (list1 == NULL && (typ_signal != PIN || typ_signal != VCTR))
 { inform("LIST1 NULL in INSERT\n",STOP);
   return;
 }
if (list1 != NULL && (typ_signal == PIN || typ_signal == VCTR))
 { if (list1->typ1_next == NULL)
    { list1->typ1_next = sublist;
      if (sublist != NULL)
        sublist->backptr = list1;
      return;
    }
   oprnd->typ1_next = list1->typ1_next;
   if (list1->typ1_next != NULL)
     list1->typ1_next->backptr = oprnd;
   list1->typ1_next = NULL;
   for (typ1_temp = oprnd;
        typ1_temp->typ1_next != NULL;
        typ1_temp = typ1_temp->typ1_next)
    ;
   typ1_temp->typ1_next = sublist;
   if (sublist != NULL)
     sublist->backptr = typ1_temp;
   push(oprtr,'+',OPERATOR,(struct type1 *)NULL);
   eval(COMPLETE);

   list1->typ1_next = oprnd->typ1_next;
   if (oprnd->typ1_next != NULL)
     oprnd->typ1_next->backptr = list1;
   oprnd->typ1_next = NULL;
   return;
 }
if (list1 != NULL && (typ_signal != VCTR && typ_signal != PIN))
 { for (typ1_temp = list1; typ1_temp->typ1_next != NULL;
        typ1_temp = typ1_temp->typ1_next)
      ;
   typ1_temp->typ1_next = sublist;
   if (sublist != NULL)
     sublist->backptr = typ1_temp;
   return;
 } /* LIST1 != NULL */
inform("Error INSERT Routine\n",STOP);
} /* INSERT */
/*  -   -   -   -   -   -   */
convert(base,nmbr,pattern)
/* convert using BASE the number NMBR and put the result in PATTERN */
char base;  /* can take the values B,O,D,H for Bin, Octal, Dec, and Hex */
char nmbr[],pattern[];
{ int i,j,k,value,num_type;
  char buf[MAX_NAME];

/* convert everything to decimal first */

num_type = DECIMAL;
if (nmbr[0] == '#')  /* take out the radices */
 { switch(nmbr[1])
    { case 'b' :
      case 'B' : num_type = BINARY;
                 break;
      case 'o' :
      case 'O' : num_type = OCTAL;
                 break;
      case 'd' :
      case 'D' : num_type = DECIMAL;
                 break;
      case 'h' :
      case 'H' : num_type = HEXADECIMAL;
                 break;
      default  : err(FATAL,"unknown radix/base",0,0,0);
    } /* SWITCH */
   for (i=2, j=0; nmbr[i] != '\0'; ++i)
     buf[j++] = nmbr[i];
   buf[j] = '\0';
 }
else /* NMBR was decimal */
  strcpy(buf,nmbr);
switch(num_type)
 { case OCTAL       : sscanf(buf,"%o",&value); break;
   case HEXADECIMAL : sscanf(buf,"%x",&value); break;
   case DECIMAL     : sscanf(buf,"%d",&value); break;
   case BINARY
       : i = strlen(buf);
         for (j=i-1,k=1,value=0; j >= 0; --j)
          { if (j!= i-1)
              k = k*2;
            if (buf[j] == '1')
              value = value+k;
          }
         break;
   default : value = (int)atoi(nmbr);
             break;
 } /* SWITCH */
switch(base)
 { case 'b' :
   case 'B' : if (num_type == BINARY)
               { strcpy(pattern,buf);
                 return;
               }
              for (i=0 ; i<16 ; ++i)
               { if (((value>>i)&(0x1)) == 1)
                   buf[i] = '1';
                 else
                   buf[i] = '0';
               }
              buf[i] = '\0';
              j = strlen(buf)-1;  /* reverse the string */
              for (i = j,pattern[j+1] = '\0';i >= 0; --i)
                pattern[j-i] = buf[i];
              break;
   case 'd' :
   case 'D' : sprintf(pattern,"%d",value);
              break;
   case 'o' :
   case 'O' : sprintf(pattern,"%o",value);
              break;
   case 'h' :
   case 'H' : sprintf(pattern,"%x",value);
              break;
 } /* SWITCH */
} /* CONVERT */
/*  -   -   -   -   -   -   */
get_equal_sign()
/* look for equal sign; ':=' and '=' are treated as the same */
{
lex(icon);
switch(icon[0])
 { case ':'
     : lex(icon);
       break;
   case '='
     : break;
 } /* SWITCH */
if (icon[0] != '=')
  err(FATAL,"looking for '='",token_type,NUMBER,VARIABLE);
} /* GET_EQUAL_SIGN */
/*  -   -   -   -   -   -   */
parse_define()
{ char mname[MAX_NAME];  /* macro name */
  char mname_val[MAX_NAME]; /* macro name pin value in ASCII */
  int active_high;
lex(icon);
if (the_keyword != DEFINE)
 { get_new_token = NO;
   return;
 }
for (lex(icon),active_high = YES;icon[0] != ';';lex(icon),active_high = YES)
 { if (icon[0] == '/')  /* the macro is active low */
    { push(oprtr,icon[0],OPERATOR,(struct type1 *)NULL);
      push(oprtr,'(',OPERATOR,(struct type1 *)NULL);
      active_high = NO;
      lex(icon);
    }
   switch(check_name(icon,mname,mname_val))
    { case MACRO     :
      case EXIST     : err(FATAL,"name/macro already exists",token_type,
                           PUNCT_MARK,NUMBER);
                       break;
      case NOT_EXIST : break;
      default        : err(FATAL,"must be vector/scalar variable",
                       token_type,PUNCT_MARK,NUMBER);
    } /* SWITCH */
   get_equal_sign();
   logic_expression(mname,macro_list,(struct type1 *)NULL,
                    active_high,NO,pin_vctr);
          /* get the equation */
 } /* FOR */
if (debug_flag == YES) {
  fprintf(stderr,"\n=> Definitions");
  traverse(stderr,macro_list,(struct type1 *)NULL);
} /* IF */
} /* PARSE_DEFINE */
